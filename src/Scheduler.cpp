module Lattice.Scheduler;

import Lattice.Logger.ILogger;
import Lattice.Logger.ProgressLogger;
import Lattice.Logger.TextLogger;

using namespace Lattice;
using namespace Lattice::Logger;

Scheduler::Scheduler(Constructable, const std::shared_ptr<Object::DependencyGraph> &dependencyGraph, std::size_t maxJobs) :
    m_dependencyGraph(dependencyGraph),
    m_maxJobs(maxJobs)
{

}

auto Scheduler::Create(const std::shared_ptr<Object::DependencyGraph> &dependencyGraph, std::size_t maxJobs) -> std::shared_ptr<Scheduler> {
    std::shared_ptr<Scheduler> scheduler = std::make_shared<Scheduler>(Constructable{}, dependencyGraph, maxJobs);

    return scheduler;
}

auto Scheduler::ReloadJobs(std::deque<Job> &jobs, const std::shared_ptr<ProgressLogger> &progressLogger) -> bool {
    bool modified{false};
    std::list<std::shared_ptr<Object::DependencyGraph::DependencyNode>> ready = m_dependencyGraph->GetReady();
    std::shared_ptr<SchedulableProgress> schedulableProgress = progressLogger ? progressLogger->GetProgress() : nullptr;
    for (std::shared_ptr<Object::DependencyGraph::DependencyNode> depNode : ready) {
        std::string objectId = depNode->object->GetResolvedObject()->GetIdentifier();
        if (std::shared_ptr<Object::Capabilities::Schedulable> schedulable = depNode->object->GetResolvedObject()->GetCapability<Object::Capabilities::Schedulable>().value_or(nullptr); schedulable) {
            if (schedulable->IsComplete()) {
                depNode->status = Object::DependencyGraph::DependencyNode::Status::Finished;
                m_dependencyGraph->Update(depNode);
                if (schedulableProgress) {
                    schedulableProgress->SetObjectsDone(schedulableProgress->GetObjectsDone() + 1);
                }

                continue;
            }
            for (std::shared_ptr<Object::Capabilities::Schedulable::Step> step : schedulable->GetReadySteps()) {
                if (!std::ranges::any_of(jobs, [&step](const Job &job) -> bool {
                    return step.get() == job.step.get();
                })) {
                    jobs.push_back({step, depNode, step->GetState()});
                    if (schedulableProgress)
                        schedulableProgress->AddStep(objectId, {.Id = step->GetID(), .Description = step->GetDescription().GetFullDescription()});
                    modified = true;
                }
            }
        } else {
            // The scheduler doesn't know how to handle non-schedulable objects in the dependency graph.
            throw std::runtime_error("ERROR: Scheduler encountered a non-schedulable object.");
        }
    }

    if (schedulableProgress)
        schedulableProgress->ApplyChanges();

    return modified;
}

auto Scheduler::Start() -> std::optional<std::future<bool>> {
    if (m_schedulerThread.joinable())
        return {};

    std::shared_ptr<ILogger> logger = ILogger::GetDefault();
    std::shared_ptr<TextLogger> textLogger = std::dynamic_pointer_cast<TextLogger>(logger);
    std::shared_ptr<ProgressLogger> progressLogger = std::dynamic_pointer_cast<ProgressLogger>(logger);
    std::shared_ptr<SchedulableProgress> schedulableProgress = progressLogger ? progressLogger->GetProgress() : nullptr;

    std::packaged_task<bool(void)> scheduler([this, textLogger, progressLogger, schedulableProgress]() -> bool {
        std::mutex stepsMutex;
        std::condition_variable workAvailable;
        std::condition_variable workFinished;

        std::deque<Scheduler::Job> finishedJobs;
        std::deque<Scheduler::Job> jobs;

        auto workerFunction = [&stepsMutex, &workAvailable, &workFinished, &finishedJobs, &jobs, textLogger, schedulableProgress](const std::stop_token &stopToken) -> void {
            while (true) {
                // Prepare task
                Job job;
                {
                    std::unique_lock<std::mutex> lock(stepsMutex);
                    workAvailable.wait(lock, [&jobs, &stopToken]() -> bool {
                        return stopToken.stop_requested() || !jobs.empty();
                    });

                    if (stopToken.stop_requested()) {
                        return;
                    }

                    if (auto err = jobs.front().step->GetParent().lock()->UpdateStep(jobs.front().step, Object::Capabilities::Schedulable::Step::State::Running); !err && err.error() == Object::Capabilities::Schedulable::Step::State::Running) {
                        if (textLogger)
                            textLogger->Warn(std::format("WARN: Worker thread encountered already running job for {}.", jobs.front().node->object->GetResolvedObject()->GetIdentifier()));

                        continue;
                    } else if (!err) {
                        throw std::runtime_error("Failed to transition job to running.");
                    }

                    job = std::move(jobs.front());
                    jobs.pop_front();

                }
                std::string objectId = job.node->object->GetResolvedObject()->GetIdentifier(); 
                std::shared_ptr<Object::Capabilities::Schedulable> schedulable = job.node->object->GetResolvedObject()->GetCapability<Object::Capabilities::Schedulable>().value_or(nullptr);
                if (schedulableProgress && !schedulableProgress->ContainsObject(objectId) && schedulable) {
                    schedulableProgress->AddObject({.Id = objectId, .Steps = {}, .TotalSteps = schedulable->GetTotalSteps(), .CompletedSteps = schedulable->GetTotalSteps() - schedulable->GetRemainingSteps()});
                    schedulableProgress->AddStep(objectId, {.Id = job.step->GetID(), .Description = job.step->GetDescription().GetFullDescription()});
                    schedulableProgress->ApplyChanges();
                }

                // Run task
                job.result = job.step->Run();
                
                {
                    // Finish task
                    std::unique_lock<std::mutex> lock(stepsMutex);
                    finishedJobs.push_back(job);
                    workFinished.notify_one();
                }
            }
        };

        {
            std::lock_guard<std::mutex> lock(stepsMutex);

            if (schedulableProgress)
                schedulableProgress->SetTotalObjects(m_dependencyGraph->GetTotalObjects());

            if (!ReloadJobs(jobs, progressLogger)) {
                for (std::jthread &thread : m_workerThreads) {
                    thread.request_stop();
                }
                workAvailable.notify_all();
                
                throw std::runtime_error("No jobs available to run.");
            }

            for (std::size_t i = 0; i < m_maxJobs; i++) {
                m_workerThreads.emplace_back(workerFunction);
            }

        }

        workAvailable.notify_all();

        while (true) {
            std::deque<Job> finishedToProcess;

            {
                std::unique_lock<std::mutex> lock(stepsMutex);
                workFinished.wait_for(lock, std::chrono::milliseconds(10), [this, &finishedJobs]() -> bool {
                    return !finishedJobs.empty() || m_dependencyGraph->IsCompleted();
                });

                if (finishedJobs.empty()) {
                    ReloadJobs(jobs, progressLogger);
                    if (!jobs.empty())
                        workAvailable.notify_all();
                    continue;
                }

                finishedToProcess.swap(finishedJobs);
            }

            while (!finishedToProcess.empty()) {
                auto job = std::move(finishedToProcess.front());
                finishedToProcess.pop_front();

                if (job.result == Object::Capabilities::Schedulable::Step::State::Finished) {
                    auto schedulable = job.node->object->GetResolvedObject()->GetCapability<Object::Capabilities::Schedulable>().value_or(nullptr); 
                    std::string schedulableObjectId = job.node->object->GetResolvedObject()->GetIdentifier();
                    if (!schedulable)
                        throw std::runtime_error("ERROR: Scheduler encountered a non-schedulable object.");
                    {
                        std::unique_lock<std::mutex> lock(stepsMutex);

                        // First update dependents
                        if (auto err = schedulable->UpdateStep(job.step, job.result); !err)
                            throw std::runtime_error("Failed to process finished job and transition job to finished.");

                        // Then check if that made the schedulable completely finished
                        if (schedulable->IsComplete()) {
                            job.node->status = Object::DependencyGraph::DependencyNode::Status::Finished;
                            m_dependencyGraph->Update(job.node);
                            if (schedulableProgress && schedulableProgress->ContainsObject(schedulableObjectId)) {
                                schedulableProgress->RemoveObject(schedulableObjectId);
                                schedulableProgress->SetObjectsDone(schedulableProgress->GetObjectsDone() + 1);
                            }
                        }

                        if (schedulableProgress && schedulableProgress->ContainsObject(schedulableObjectId)) {
                            schedulableProgress->RemoveStep(schedulableObjectId, job.step->GetID());
                            schedulableProgress->IncrementCompletedSteps(schedulableObjectId);
                        }
                    }
                } else if (job.result == Object::Capabilities::Schedulable::Step::State::Failed) {
                    // Signal for all workers to stop
                    for (std::jthread &thread : m_workerThreads) {
                        thread.request_stop();
                    }
                    workAvailable.notify_all();
                    for (std::jthread &thread : m_workerThreads) {
                        if (thread.joinable())
                            thread.join();
                    }
                    // Update dependency graph for a failed node
                    {
                        std::unique_lock<std::mutex> lock(stepsMutex);
                        job.node->status = Object::DependencyGraph::DependencyNode::Status::Failed;
                        m_dependencyGraph->Update(job.node);
                    }

                    return false;
                } else {
                    throw std::runtime_error("Caught Step::State that shouldn't be possible in context.");
                }
            }

            {
                std::unique_lock<std::mutex> lock(stepsMutex);
                if (schedulableProgress)
                    schedulableProgress->ApplyChanges();
                ReloadJobs(jobs, progressLogger);
                if (!jobs.empty())
                    workAvailable.notify_all();

                if (m_dependencyGraph->IsCompleted() && jobs.empty() && finishedJobs.empty())  {
                    // Signal for all workers to stop
                    if (textLogger)
                        textLogger->Info("INFO: Build complete, signalling all workers to stop.");
                    for (std::jthread &thread : m_workerThreads) {
                        thread.request_stop();
                    }
                    lock.unlock();
                    workAvailable.notify_all();
                    for (std::jthread &thread : m_workerThreads) {
                        if (thread.joinable())
                            thread.join();
                    }
                    lock.lock();
                    return true;
                }
            }
        }

        return true;
    });

    std::future<bool> future = scheduler.get_future();

    m_schedulerThread = std::jthread(std::move(scheduler));

    return std::optional<std::future<bool>>{std::move(future)};
}
