module Lattice.Provider;

using namespace Lattice;

Provider::Provider(const std::string &providerName) :
    m_providerName(providerName) {}

auto Provider::GetName() const -> std::string {
    return m_providerName;
}
