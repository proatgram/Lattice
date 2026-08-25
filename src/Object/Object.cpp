module;

#include <yaml-cpp/yaml.h>

module Lattice.Object;

using namespace Lattice::Object;

Object::Object(Constructable, const std::string &identifier) : m_identifier(identifier) {}

auto Object::GetIdentifier() const -> std::string {
    return m_identifier;
}

auto Lattice::Object::WalkYAMLNode(const std::string &yaml, const std::function<void(const std::string&)> &onScalar) -> void {
    YAML::Node node = YAML::Load(yaml);

    if (!node)
        return;


    switch (node.Type()) {
        case YAML::NodeType::Map:
            for (const auto& kv : node) {
                WalkYAMLNode(YAML::Dump(kv.first), onScalar);
                WalkYAMLNode(YAML::Dump(kv.second), onScalar);
            }
            break;
        case YAML::NodeType::Sequence:
            for (const auto& item : node) {
                WalkYAMLNode(YAML::Dump(item), onScalar);
            }
            break;
        case YAML::NodeType::Scalar:
            onScalar(YAML::Dump(node));
            break;
        case YAML::NodeType::Null:
        case YAML::NodeType::Undefined:
        default:
            break;
    }
}
