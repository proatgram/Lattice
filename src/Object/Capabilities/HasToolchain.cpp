module Lattice.Object.Capabilities.HasToolchain;

import Lattice.Registry;

using namespace Lattice::Object::Capabilities;

auto HasToolchain::GetToolchain() -> std::shared_ptr<IToolchain> {
    if (m_toolchain)
        return m_toolchain;

    return Registry<std::shared_ptr<IToolchain>>::GetInstance()->Query(m_toolchainId).value_or(nullptr);
}

auto HasToolchain::GetToolchain() const -> std::shared_ptr<const IToolchain> {
    if (m_toolchain)
        return m_toolchain;

    return Registry<std::shared_ptr<IToolchain>>::GetInstance()->Query(m_toolchainId).value_or(nullptr);
}

auto HasToolchain::GetToolchainId() const -> std::string {
    return m_toolchainId;
}

auto HasToolchain::SetToolchain(const std::shared_ptr<IToolchain> &toolchain) -> void {
    m_toolchain = toolchain;

    if (toolchain)
        m_toolchainId = toolchain->GetIdentifier();
}

auto HasToolchain::SetToolchainId(const std::string &toolchainId) -> void {
    m_toolchainId = toolchainId;

    if (Registry<std::shared_ptr<IToolchain>>::GetInstance()->Contains(toolchainId))
        m_toolchain = Registry<std::shared_ptr<IToolchain>>::GetInstance()->Query(toolchainId).value();
}
