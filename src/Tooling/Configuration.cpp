module Lattice.Tooling.Configuration;

using namespace Lattice::Tooling;

Configuration::Configuration(Configuration::Constructable) :
    m_configStore() {}

auto Configuration::GetConfigStore() const -> const std::map<std::string, std::any>& {
    return m_configStore;
}
