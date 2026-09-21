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

auto BuildScheduler::ReloadJobs(std::queue<Job> &jobs) -> bool {
    bool modified{false};
    for (std::shared_ptr<Object::BuildGraph::DependencyNode> depNode : m_buildGraph->GetReady()) {
        if (std::shared_ptr<Object::Capabilities::Buildable> buildable = depNode->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); buildable) {
            for (std::shared_ptr<Object::Capabilities::Buildable::BuildStep> buildStep : buildable->GetReadySteps()) {
                jobs.push({buildStep, depNode});
                modified = true;
            }
        } else {
            // The scheduler doesn't know how to handle non-buildable objects in the build graph.
            // TODO: Figure out what to do with these.
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

        std::queue<BuildScheduler::Job> finishedJobs;
        std::queue<BuildScheduler::Job> jobs;

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

                    job = std::move(jobs.front());
                    jobs.pop();
                }

                // Run task
                job.step->Run();

                // Finish task
                {
                    std::unique_lock<std::mutex> lock(stepsMutex);
                    finishedJobs.push(job);
                    workFinished.notify_all();
                }
            }
        };

        for (std::size_t i = 0; i < m_maxJobs; i++) {
            m_workerThreads.emplace_back(workerFunction);
        }

        {
            std::lock_guard<std::mutex> lock(stepsMutex);

            if (!ReloadJobs(jobs)) {

            }

            workAvailable.notify_all();
        }

        while (true) {
            std::queue<Job> finishedToProcess;

            {
                std::unique_lock<std::mutex> lock(stepsMutex);
                workFinished.wait_for(lock, std::chrono::milliseconds(100), [&finishedJobs]() -> bool {
                    return !finishedJobs.empty();
                });

                if (finishedJobs.empty()) {
                    if (!jobs.empty())
                        workAvailable.notify_all();
                    continue;
                }

                finishedToProcess.swap(finishedJobs);
            }

            for (std::size_t i = 0; i < finishedToProcess.size(); i++) {
                auto job = std::move(finishedToProcess.front());
                finishedToProcess.pop();

                if (job.step->GetState() == Object::Capabilities::Buildable::BuildStep::State::Finished) {
                    auto buildable = job.node->object->GetResolvedObject()->GetCapability<Object::Capabilities::Buildable>().value_or(nullptr); 
                    if (!buildable) {
                        throw std::runtime_error("Non-buildable encountered in buildscheduler.");
                    }
                    // First update dependents
                    for (auto &dependent : job.step->GetDependents()) {
                        buildable->UpdateBuiltStep(job.step);
                    }
                    // Then check if that made the buildable completely build
                    if (buildable->IsBuilt()) {
                        job.node->status = Object::BuildGraph::DependencyNode::Status::Finished;
                        m_buildGraph->Update(job.node);
                    }
                } else if (job.step->GetState() == Object::Capabilities::Buildable::BuildStep::State::Failed) {
                    // Signal for all workers to stop
                    for (std::jthread &thread : m_workerThreads) {
                        thread.request_stop();
                        // TODO: Log
                    }
                    for (std::jthread &thread : m_workerThreads) {
                        if (thread.joinable())
                            thread.join();
                    }
                    // Update build graph for a failed node
                    job.node->status = Object::BuildGraph::DependencyNode::Status::Failed;
                    m_buildGraph->Update(job.node);

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
