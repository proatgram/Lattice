module Lattice.Object.Capabilities.Buildable;

using namespace Lattice::Object::Capabilities;

auto Buildable::SetToolchainId(const std::string &toolchainId) -> void {
    m_toolchainId = toolchainId;
}

auto Buildable::GetToolchainId() const -> std::optional<std::string> {
    return m_toolchainId;
}
