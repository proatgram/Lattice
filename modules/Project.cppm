export module Lattice.Project;

export import Lattice.Object;
export import Lattice.Object.Buildable;
export import Lattice.Object.Parsable;
export import Lattice.Plugin.IFactory;
import std;

export namespace Lattice {
    /**
     * @brief Represents a software project in the lattice.
     *
     * This class inherits from Object, Buildable, and Parsable to provide
     * a project configuration that can be built and parsed from YAML.
     *
     */
    class Project final : public Object, public Buildable, public Parsable, public std::enable_shared_from_this<Project> {
        public:
            /**
             * @brief Creates a new Project with the given identifier.
             *
             * @param identifier The project identifier.
             */
            Project(Constructable, const std::string &identifier);

            /**
             * @brief Gets the project identifier.
             *
             * @return Shared pointer to self for chaining.
             */
            auto GetIdentifier() const -> std::string;

            /**
             * @brief Sets the project description.
             *
             * @param description The description string.
             * @return Shared pointer to self for chaining.
             */
            auto SetDescription(const std::string &description) -> std::shared_ptr<Project>;

            /**
             * @brief Gets the project description.
             *
             * @return The project description.
             */
            auto GetDescription() const -> std::string;

            /**
             * @brief Sets the project version.
             *
             * @param version The version string.
             * @return Shared pointer to self for chaining.
             */
            auto SetVersion(const std::string &version) -> std::shared_ptr<Project>;

            /**
             * @brief Gets the project version.
             *
             * @return The project version.
             */
            auto GetVersion() const -> std::string;

            /**
             * @brief Sets the project homepage URL.
             *
             * @param homepageUrl The homepage URL.
             * @return Shared pointer to self for chaining.
             */
            auto SetHomepageUrl(const std::string &homepageUrl) -> std::shared_ptr<Project>;

            /**
             * @brief Gets the project homepage URL.
             *
             * @return Optional homepage URL, or nullopt if not set.
             */
            auto GetHomepageUrl() const -> std::optional<std::string>;

            auto GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> final;

            auto Build() -> void final;

            /**
             * @brief Gets the objects owned by this project.
             *
             * @return A map with the objects.
             */
            auto GetObjects() const -> std::map<std::string, std::shared_ptr<Object>>;
            
            /**
             * @brief Gets an object owned by this project.
             *
             * @param An object identifier
             *
             * @return Shared pointer Object wrapped in an optional, or nullopt if not set.
             */
            auto GetObject(const std::string &identifier) const -> std::optional<std::shared_ptr<Object>>;

            /**
             * @brief Adds an object to this project.
             *
             * @param The object identifier
             * @param The object
             */
            auto AddObject(const std::string &identifier, const std::shared_ptr<Object> &object) -> void;

            /**
             * @brief Parses the project from a YAML string.
             *
             * @param parsableString The YAML string to parse.
             * @return Void.
             */
            auto Parse(const std::string &parsableString) -> void final;

        private:
            std::string m_description;
            std::string m_version;

            std::optional<std::string> m_homepageUrl;

            std::map<std::string, std::shared_ptr<Object>> m_objects;

            friend class ProjectFactory;
    };

    class ProjectFactory : public Plugin::IFactory<ProjectFactory, Project>  {
        public:
            inline ProjectFactory(Constructable) {};

            auto Create(const std::string &identifier) -> std::shared_ptr<Project> final;
    };

}  // export namespace Lattice
