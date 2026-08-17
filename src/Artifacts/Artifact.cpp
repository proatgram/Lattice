module Lattice.Artifact;

using namespace Lattice;

auto Artifact::GetSourceObject() const -> std::shared_ptr<const Object::Object> {
    return m_sourceObject;
}

auto Artifact::GetSourceObject() -> std::shared_ptr<Object::Object> {
    return m_sourceObject;
}
