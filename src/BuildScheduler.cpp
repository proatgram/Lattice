module Lattice.BuildScheduler;

import Lattice.Logger.ILogger;
import Lattice.Logger.ProgressLogger;
import Lattice.Logger.TextLogger;

using namespace Lattice;
using namespace Lattice::Logger;

BuildScheduler::BuildScheduler(Constructable, const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs) :
    m_buildGraph(buildGraph),
    m_maxJobs(maxJobs)
{

}

auto BuildScheduler::Create(const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs) -> std::shared_ptr<BuildScheduler> {
    std::shared_ptr<BuildScheduler> buildScheduler = std::make_shared<BuildScheduler>(Constructable{}, buildGraph, maxJobs);

    return buildScheduler;
}

auto BuildScheduler::ReloadJobs(std::deque<Job> &jobs, const std::shared_ptr<ProgressLogger> &progressLogger) -> bool {
    bool modified{false};
    std::list<std::shared_ptr<Object::BuildGraph::DependencyNode>> ready = m_buildGraph->GetReady();
    std::shared_ptr<BuildProgress> buildProgress = progressLogger ? progressLogger->GetProgress() : nullptr;
    for (std::shared_ptr<Object::BuildGraph::DependencyNode> depNode : ready) {
        std::string objectId = depNode->object->GetResolvedObject()->GetIdentifier();
        if (std::shared_ptr<Object::Capabilities::Buildable> buildable = depNode->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); buildable) {
            if (buildable->IsBuilt()) {
                depNode->status = Object::BuildGraph::DependencyNode::Status::Finished;
                m_buildGraph->Update(depNode);
                if (buildProgress) {
                    if (buildProgress->ContainsObject(objectId)) {
                        buildProgress->RemoveObject(objectId);
                        buildProgress->ApplyChanges();
                    }
                    buildProgress->SetObjectsDone(buildProgress->GetObjectsDone() + 1);
                }

                continue;
            }
            if (buildProgress && !buildProgress->ContainsObject(objectId)) {
                buildProgress->AddObject({.Id = objectId, .Steps = {}, .TotalSteps = buildable->GetTotalSteps(), .CompletedSteps = buildable->GetTotalSteps() - buildable->GetRemainingSteps()});
                buildProgress->ApplyChanges();
            }
            for (std::shared_ptr<Object::Capabilities::Buildable::BuildStep> buildStep : buildable->GetReadySteps()) {
                if (!std::ranges::any_of(jobs, [&buildStep](const Job &job) -> bool {
                    return buildStep.get() == job.step.get();
                })) {
                    jobs.push_back({buildStep, depNode, buildStep->GetState()});
                    if (buildProgress)
                        buildProgress->AddStep(objectId, {.Id = buildStep->GetID(), .Description = buildStep->GetDescription().GetFullDescription()});
                    modified = true;
                }
            }
        } else {
            // The scheduler doesn't know how to handle non-buildable objects in the build graph.
            // TODO: Figure out what to do with these. Probably add a capability that is `BuildGraphCapable`.
            
            if (auto textLogger = std::dynamic_pointer_cast<TextLogger>(progressLogger); textLogger)
                textLogger->Error("ERROR: Build Scheduler encountered a non-buildable object.");

            throw std::runtime_error("ERROR: Build Scheduler encountered a non-buildable object.");
        }
    }

    if (buildProgress)
        buildProgress->ApplyChanges();

    return modified;
}

auto BuildScheduler::Start() -> std::optional<std::future<bool>> {
    if (m_schedulerThread.joinable())
        return {};

    std::shared_ptr<ILogger> logger = ILogger::GetDefault();
    std::shared_ptr<TextLogger> textLogger = std::dynamic_pointer_cast<TextLogger>(logger);
    std::shared_ptr<ProgressLogger> progressLogger = std::dynamic_pointer_cast<ProgressLogger>(logger);
    std::shared_ptr<BuildProgress> buildProgress = progressLogger ? progressLogger->GetProgress() : nullptr;

    std::packaged_task<bool(void)> scheduler([this, textLogger, progressLogger, buildProgress]() -> bool {
        std::mutex stepsMutex;
        std::condition_variable workAvailable;
        std::condition_variable workFinished;

        std::deque<BuildScheduler::Job> finishedJobs;
        std::deque<BuildScheduler::Job> jobs;

        auto workerFunction = [&stepsMutex, &workAvailable, &workFinished, &finishedJobs, &jobs, textLogger](const std::stop_token &stopToken) -> void {
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

                    if (auto err = jobs.front().step->GetParent().lock()->UpdateBuiltStep(jobs.front().step, Object::Capabilities::Buildable::BuildStep::State::Running); !err && err.error() == Object::Capabilities::Buildable::BuildStep::State::Running) {
                        if (textLogger)
                            textLogger->Warn(std::format("WARN: Worker thread encountered already running job for {}.", jobs.front().node->object->GetResolvedObject()->GetIdentifier()));

                        continue;
                    } else if (!err) {
                        if (textLogger)
                            textLogger->Error("ERROR: Failed to transition job to running.");
                        throw std::runtime_error("Failed to transition job to running.");
                    }

                    job = std::move(jobs.front());
                    jobs.pop_front();
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

            if (buildProgress)
                buildProgress->SetTotalObjects(m_buildGraph->GetTotalObjects());

            if (!ReloadJobs(jobs, progressLogger)) {
                for (std::jthread &thread : m_workerThreads) {
                    thread.request_stop();
                    // TODO: Log
                }
                workAvailable.notify_all();
                
                if (textLogger)
                    textLogger->Error("ERROR: No jobs available to run.");
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
                    return !finishedJobs.empty() || m_buildGraph->IsCompleted();
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

                if (job.result == Object::Capabilities::Buildable::BuildStep::State::Finished) {
                    auto buildable = job.node->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); 
                    std::string buildableObjectId = job.node->object->GetResolvedObject()->GetIdentifier();
                    if (!buildable) {
                        if (textLogger)
                            textLogger->Error("ERROR: Build Scheduler encountered a non-buildable object.");

                        throw std::runtime_error("ERROR: Build Scheduler encountered a non-buildable object.");
                    }
                    {
                        std::unique_lock<std::mutex> lock(stepsMutex);

                        // First update dependents
                        if (auto err = buildable->UpdateBuiltStep(job.step, job.result); !err) {
                            if (textLogger)
                                textLogger->Error("ERROR: Failed to process finished job and transition job to finished.");
                            throw std::runtime_error("Failed to process finished job and transition job to finished.");
                        }

                        // Then check if that made the buildable completely build
                        if (buildable->IsBuilt()) {
                            job.node->status = Object::BuildGraph::DependencyNode::Status::Finished;
                            m_buildGraph->Update(job.node);
                            if (buildProgress && buildProgress->ContainsObject(buildableObjectId)) {
                                buildProgress->RemoveObject(buildableObjectId);
                                buildProgress->SetObjectsDone(buildProgress->GetObjectsDone() + 1);
                            }
                        }

                        if (buildProgress && buildProgress->ContainsObject(buildableObjectId)) {
                            buildProgress->RemoveStep(buildableObjectId, job.step->GetID());
                            buildProgress->GetObject(buildableObjectId)->get().CompletedSteps++;
                        }
                    }
                } else if (job.result == Object::Capabilities::Buildable::BuildStep::State::Failed) {
                    // Signal for all workers to stop
                    for (std::jthread &thread : m_workerThreads) {
                        thread.request_stop();
                        // TODO: Log
                    }
                    workAvailable.notify_all();
                    for (std::jthread &thread : m_workerThreads) {
                        if (thread.joinable())
                            thread.join();
                    }
                    // Update build graph for a failed node
                    {
                        std::unique_lock<std::mutex> lock(stepsMutex);
                        job.node->status = Object::BuildGraph::DependencyNode::Status::Failed;
                        m_buildGraph->Update(job.node);
                    }

                    return false;
                } else {
                    throw std::runtime_error("Caught BuildStep::State that shouldn't be possible in context.");
                }
            }

            {
                std::unique_lock<std::mutex> lock(stepsMutex);
                if (buildProgress)
                    buildProgress->ApplyChanges();
                ReloadJobs(jobs, progressLogger);
                if (!jobs.empty())
                    workAvailable.notify_all();

                if (m_buildGraph->IsCompleted() && jobs.empty() && finishedJobs.empty())  {
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
