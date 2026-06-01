module Lattice.Tooling.ILinker;

using namespace Lattice::Tooling;

auto LinkerConfiguration::GetLinkableFiles() const -> std::list<std::filesystem::path> {
    if (m_configStore.contains(LINKABLE_FILES_KEY) && m_configStore.at(LINKABLE_FILES_KEY).has_value() && m_configStore.at(LINKABLE_FILES_KEY).type() == typeid(std::list<std::filesystem::path>)) {
        return std::any_cast<std::list<std::filesystem::path>>(m_configStore.at(LINKABLE_FILES_KEY));
    }
    
    return {};
}

auto LinkerConfiguration::SetLinkableFiles(const std::initializer_list<std::filesystem::path> &files) -> LinkerConfiguration& {
    m_configStore[LINKABLE_FILES_KEY] = std::list<std::filesystem::path>(files);

    return *this;
}

auto LinkerConfiguration::AddLinkableFile(const std::filesystem::path &file) -> LinkerConfiguration& {

    return *this;
}
