module Lattice.Object;

using namespace Lattice;

Object::Object(Constructable, const std::string &identifier) : m_identifier(identifier) {}

auto Object::GetIdentifier() const -> std::string {
    return m_identifier;
}
