module Lattice.Object.Capabilities.Buildable;

using namespace Lattice::Object::Capabilities;

auto Buildable::IsBuilt() const -> bool {
    return m_built;
}

auto Buildable::SetBuilt(bool built) -> void {
    m_built = built;
}
