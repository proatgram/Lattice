module Lattice.BuildScheduler;

using namespace Lattice;

BuildScheduler::BuildScheduler(Constructable, const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs) :
    m_buildGraph(buildGraph),
    m_maxJobs(maxJobs)
{

}

auto BuildScheduler::Create(const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs) -> std::shared_ptr<BuildScheduler> {
    std::shared_ptr<BuildScheduler> buildScheduler = std::make_shared<BuildScheduler>(Constructable{}, buildGraph, maxJobs);

    return buildScheduler;
}

auto BuildScheduler::ReloadJobs(std::deque<Job> &jobs) -> bool {
    bool modified{false};
    std::list<std::shared_ptr<Object::BuildGraph::DependencyNode>> ready = m_buildGraph->GetReady();
    for (std::shared_ptr<Object::BuildGraph::DependencyNode> depNode : ready) {
        if (std::shared_ptr<Object::Capabilities::Buildable> buildable = depNode->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); buildable) {
            if (buildable->IsBuilt()) {
                depNode->status = Object::BuildGraph::DependencyNode::Status::Finished;
                m_buildGraph->Update(depNode);
                continue;
            }
            for (std::shared_ptr<Object::Capabilities::Buildable::BuildStep> buildStep : buildable->GetReadySteps()) {
                if (!std::ranges::any_of(jobs, [&buildStep](const Job &job) -> bool {
                    return buildStep.get() == job.step.get();
                })) {
                    jobs.push_back({buildStep, depNode, buildStep->GetState()});
                    modified = true;
                }
            }
        } else {
            // The scheduler doesn't know how to handle non-buildable objects in the build graph.
            // TODO: Figure out what to do with these.
            throw std::runtime_error("Object not buildable.");
        }
    }

    return modified;
}

auto BuildScheduler::Start() -> std::optional<std::future<bool>> {
    if (m_schedulerThread.joinable())
        return {};

    std::packaged_task<bool(void)> scheduler([this]() -> bool {
        std::mutex stepsMutex;
        std::condition_variable workAvailable;
        std::condition_variable workFinished;

        std::deque<BuildScheduler::Job> finishedJobs;
        std::deque<BuildScheduler::Job> jobs;

        auto workerFunction = [&stepsMutex, &workAvailable, &workFinished, &finishedJobs, &jobs](const std::stop_token &stopToken) -> void {
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
                        // TODO: Logging

                        std::println("WARN: Worker thread encountered already running job for {}.", jobs.front().node->object->GetResolvedObject()->GetIdentifier());
                        continue;
                    } else if (!err) {
                        throw std::runtime_error("ERROR: Failed to transition job to running.");
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

            if (!ReloadJobs(jobs)) {
                for (std::jthread &thread : m_workerThreads) {
                    thread.request_stop();
                    // TODO: Log
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
                    return !finishedJobs.empty() || m_buildGraph->IsCompleted();
                });

                if (m_buildGraph->IsCompleted())  {
                    // Signal for all workers to stop
                    for (std::jthread &thread : m_workerThreads) {
                        thread.request_stop();
                        // TODO: Log
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

                if (finishedJobs.empty()) {
                    ReloadJobs(jobs);
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
                    if (!buildable) {
                        throw std::runtime_error("Non-buildable encountered in buildscheduler.");
                    }
                    {
                        std::unique_lock<std::mutex> lock(stepsMutex);

                        // First update dependents
                        if (auto err = buildable->UpdateBuiltStep(job.step, job.result); !err) {
                            throw std::runtime_error("ERROR: Failed to process finished job and transition job to finished.");
                        }

                        // Then check if that made the buildable completely build
                        if (buildable->IsBuilt()) {
                            job.node->status = Object::BuildGraph::DependencyNode::Status::Finished;
                            m_buildGraph->Update(job.node);
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
                ReloadJobs(jobs);
                if (!jobs.empty())
                    workAvailable.notify_all();
            }
        }

        return true;
    });

    std::future<bool> future = scheduler.get_future();

    m_schedulerThread = std::jthread(std::move(scheduler));

    return std::optional<std::future<bool>>{std::move(future)};
}
