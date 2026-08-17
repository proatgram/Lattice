export module Lattice;
export import std;

export import Lattice.Project;
export import Lattice.Object;

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

            auto RegisterObjectFactory(const std::string &identifier, const std::function<std::shared_ptr<Object::Object>(const std::string &)> factoryFunction) -> void;

            /**
             * @brief Loads lattice configurations from a file.
             *
             * @param configPath The path to the configuration file.
             */
            auto LoadConfig(const std::filesystem::path configPath) -> void;

            auto GetGlobalObjects() const -> std::map<std::string, std::shared_ptr<Object::Object>>;
            auto GetGlobalObject(const std::string &identifier) const -> std::optional<std::shared_ptr<Object::Object>>;

        private:
            std::map<std::string, std::shared_ptr<Object::Object>> m_globalObjects;
    };
}  // export namespace Lattice
