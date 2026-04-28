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
    return shared_from_this();
}

auto Project::GetDescription() const -> std::string {
    return m_description;
}

auto Project::SetVersion(const std::string &version) -> std::shared_ptr<Project> {
    m_version = version;
    return shared_from_this();
}

auto Project::GetVersion() const -> std::string {
    return m_version;
}

auto Project::SetHomepageUrl(const std::string &homepageUrl) -> std::shared_ptr<Project> {
    m_homepageUrl = homepageUrl;
    return shared_from_this();
}

auto Project::GetHomepageUrl() const -> std::optional<std::string> {
    return m_homepageUrl;
}

auto Project::GetProperties() const -> std::bitset<8> {
    std::bitset<8> properties;
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

auto Project::Parse(const std::string &parsableString) -> void {
    YAML::Node parsable = YAML::Load(parsableString);

    if (parsable["version"]) {
        SetVersion(parsable["version"].as<std::string>());
    }
    
    if (parsable["description"]) {
        SetDescription(parsable["description"].as<std::string>());
    }
    
    if (parsable["homepage"]) {
        SetHomepageUrl(parsable["homepage"].as<std::string>());
    }
}

auto ProjectFactory::Create(const std::string &identifier) -> std::shared_ptr<Project> {
    return std::make_shared<Project>(Project::Constructable(), identifier);
}
