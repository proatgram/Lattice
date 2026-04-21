export module Lattice;
export import std;

export import Lattice.Project;

export namespace Lattice {
    class Lattice {
            struct Constructable {};
        public:
            inline Lattice(Constructable) {};
            static auto GetInstance() -> Lattice&;

            Lattice(const Lattice&) = delete;

            auto LoadConfig(const std::filesystem::path configPath) -> void;

            auto AddProject(const std::string &identifier) -> std::optional<std::shared_ptr<Project>>;
            auto GetProject(const std::string &identifier) -> std::shared_ptr<Project>;

        private:
            std::map<std::string, std::shared_ptr<Project>> m_projects;
    };
}  // export namespace Lattice
