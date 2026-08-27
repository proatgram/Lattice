export module Lattice;
export import std;

export import Lattice.Project;
export import Lattice.Object;
export import Lattice.Object.BuildGraph;

export namespace Lattice {
    /**
     * @brief The Lattice singleton manages projects and their configurations.
     *
     * This class provides a singleton instance that loads and manages
     * lattice projects from configuration files.
     *
     */
    class Lattice {
            struct Constructable {};
        public:
            /**
             * @brief Creates the Lattice singleton instance.
             */
            Lattice(Constructable);
            static auto GetInstance() -> std::shared_ptr<Lattice>;

            /**
             * @brief Loads lattice configurations from a file.
             *
             * @param configPath The path to the configuration file.
             */
            auto LoadConfig(const std::filesystem::path configPath) -> void;

            /**
             * @brief Gets the build graph for the entire configuration.
             *
             * @return A shared pointer to the build graph.
             */
            auto GetBuildGraph() const -> std::shared_ptr<Object::BuildGraph>;

        private:
            std::map<std::string, std::shared_ptr<Object::Object>> m_globalObjects;
            std::shared_ptr<Object::BuildGraph> m_globalBuildGraph;
    };
}  // export namespace Lattice
