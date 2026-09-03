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

            auto StartBuild(const std::optional<std::list<std::string>> &objects = {}, const std::optional<std::size_t> &numberJobs = {}) -> void;

        private:
            std::map<std::string, std::shared_ptr<Object::Object>> m_globalObjects;
    };
}  // export namespace Lattice
