module;

#include <yaml-cpp/yaml.h>

module Lattice;

auto Lattice::Lattice::GetInstance() -> Lattice& {
    static Lattice instance{Constructable()};
    return instance;
}

auto Lattice::Lattice::LoadConfig(const std::filesystem::path configPath) -> void {
    YAML::Node currentConfig = YAML::LoadFile(configPath.string());
    if (currentConfig["project"]) {
        YAML::Node projects = currentConfig["project"];
        if (!projects.IsMap()) {
            throw std::runtime_error(std::format("Error parsing configurations: \"project:\" in file {} is illformed.", configPath.string()));
        }

        for (YAML::const_iterator it = projects.begin(); it != projects.end(); ++it) {
            std::optional<std::shared_ptr<Project>> project = AddProject(it->first.as<std::string>());
            if (!project.has_value()) {
                throw std::runtime_error(std::format("Error parsing configurations: Project \"{}\" was defined more than once.", it->first.as<std::string>()));
            }

            project.value()->Parse(YAML::Dump(it->second));
        }
    }


}

auto Lattice::Lattice::AddProject(const std::string &identifier) -> std::optional<std::shared_ptr<Project>> {
    auto [it, inserted] = m_projects.insert({identifier, Project::Create(identifier)});
    if (inserted) {
        return it->second;
    }

    return {};
}

auto Lattice::Lattice::GetProject(const std::string &identifier) -> std::shared_ptr<Project> {
    return m_projects.at(identifier);
}

