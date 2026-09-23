export module Lattice.Scheduler;

export import Lattice.Object.DependencyGraph;

export import std;

import Lattice.Object.Capabilities.Schedulable;
import Lattice.Logger.ProgressLogger;

export namespace Lattice {
    class Scheduler {
        struct Constructable{};
        public:
            Scheduler(Constructable, const std::shared_ptr<Object::DependencyGraph> &dependencyGraph, std::size_t maxJobs);
            static auto Create(const std::shared_ptr<Object::DependencyGraph> &dependencyGraph, std::size_t maxJobs = 1) -> std::shared_ptr<Scheduler>;

            auto Start() -> std::optional<std::future<bool>>;

        private:
            struct Job {
                std::shared_ptr<Object::Capabilities::Schedulable::Step> step;
                std::shared_ptr<Object::DependencyGraph::DependencyNode> node;
                Object::Capabilities::Schedulable::Step::State result;
            };

            auto ReloadJobs(std::deque<Job> &jobs, const std::shared_ptr<Lattice::Logger::ProgressLogger> &progressLogger) -> bool;

            std::shared_ptr<Object::DependencyGraph> m_dependencyGraph;
            std::vector<std::jthread> m_workerThreads;
            std::jthread m_schedulerThread;
            std::size_t m_maxJobs;
    };
}
