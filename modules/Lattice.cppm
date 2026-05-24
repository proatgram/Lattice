export module Lattice;
export import std;

export import Lattice.Project;
export import Lattice.Tooling.Compiler;

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

            auto RegisterObjectFactory(const std::string &identifier, const std::function<std::shared_ptr<Object>(const std::string &)> factoryFunction) -> void;

            /**
             * @brief Loads lattice configurations from a file.
             *
             * @param configPath The path to the configuration file.
             */
            auto LoadConfig(const std::filesystem::path configPath) -> void;

            /**
             * @brief Adds a project with the given identifier.
             *
             * @param identifier The project identifier.
             * @return Optional shared pointer to the project, or nullopt if project already exists.
             */
            auto AddProject(const std::string &identifier) -> std::optional<std::shared_ptr<Project>>;

            /**
             * @brief Gets a project by its identifier.
             *
             * @param identifier The project identifier.
             * @return Optional shared pointer to the project, or nullopt if not found.
             */
            auto GetProject(const std::string &identifier) -> std::optional<std::shared_ptr<Project>>;

        private:
            std::map<std::string, std::shared_ptr<Project>> m_projects;
            std::map<std::string, std::function<std::shared_ptr<Object>(const std::string &)>> m_objectFactories;

    };
}  // export namespace Lattice
