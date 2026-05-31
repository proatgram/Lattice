module;

#include <yaml-cpp/yaml.h>

module Lattice.Project;

using namespace Lattice;

Project::Project(Object::Constructable, const std::string &identifier) :
    Object(Object::Constructable(), identifier)
{
    
}

auto Project::GetIdentifier() const -> std::string {
    return m_identifier;
}

auto Project::SetDescription(const std::string &description) -> std::shared_ptr<Project> {
    m_description = description;
    return As<Project>().value();
}

auto Project::GetDescription() const -> std::string {
    return m_description;
}

auto Project::SetVersion(const std::string &version) -> std::shared_ptr<Project> {
    m_version = version;
    return As<Project>().value();
}

auto Project::GetVersion() const -> std::string {
    return m_version;
}

auto Project::SetHomepageUrl(const std::string &homepageUrl) -> std::shared_ptr<Project> {
    m_homepageUrl = homepageUrl;
    return As<Project>().value();
}

auto Project::GetHomepageUrl() const -> std::optional<std::string> {
    return m_homepageUrl;
}

auto Project::GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> {
    std::bitset<Object::TOTAL_PROPERTIES> properties;
    properties.set(std::to_underlying(Object::Properties::Buildable));

    return properties;
}

auto Project::Build() -> void {

}

auto Project::GetObjects() const -> std::map<std::string, std::shared_ptr<Object>> {
    return m_objects;
}
auto Project::GetObject(const std::string &identifier) const -> std::optional<std::shared_ptr<Object>> {
    if (!m_objects.contains(identifier)) {
        return {};
    }

    return m_objects.at(identifier);
}

auto Project::AddObject(const std::string &identifier, const std::shared_ptr<Object> &object) -> void {
    m_objects.insert({identifier, object});
}

auto ProjectFactory::Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> {
    YAML::Node config;
    try {
        config = YAML::Load(objectData.value_or(""));
    } catch (const YAML::ParserException &err) {
        throw std::runtime_error(std::format("Failed to create project {}: Project configuration is malformed: {}", identifier, err.what()));
    }

    std::shared_ptr<Project> project = std::make_shared<Project>(Project::Constructable{}, identifier);

    if (config["version"]) {
        project->m_version = config["version"].as<std::string>();
    }
    
    if (config["description"]) {
        project->m_description = config["description"].as<std::string>();
    }
    
    if (config["homepage"]) {
        project->m_homepageUrl = config["homepage"].as<std::string>();
    }

    return project;
}
