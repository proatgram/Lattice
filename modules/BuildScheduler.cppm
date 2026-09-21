export module Lattice.BuildScheduler;

export import Lattice.Object.BuildGraph;

export import std;

import Lattice.Object.Capabilities.Buildable;

export namespace Lattice {
    class BuildScheduler {
        struct Constructable{};
        public:
            BuildScheduler(Constructable, const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs);
            static auto Create(const std::shared_ptr<Object::BuildGraph> &buildGraph, std::size_t maxJobs = 1) -> std::shared_ptr<BuildScheduler>;

            auto Start() -> std::optional<std::future<bool>>;

        private:
            struct Job {
                std::shared_ptr<Object::Capabilities::Buildable::BuildStep> step;
                std::shared_ptr<Object::BuildGraph::DependencyNode> node;
            };

            auto ReloadJobs(std::queue<Job> &jobs) -> bool;

            std::shared_ptr<Object::BuildGraph> m_buildGraph;
            std::vector<std::jthread> m_workerThreads;
            std::jthread m_schedulerThread;
            std::size_t m_maxJobs;
    };
}
