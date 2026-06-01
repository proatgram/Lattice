module Lattice.Tooling.Compiler;

import std;

using namespace Lattice::Tooling;

CompilerConfiguration::CompilerConfiguration(Configuration::Constructable) :
    Configuration(Configuration::Constructable()) {}

auto CompilerConfiguration::GetArguments() const -> std::vector<std::string> {
    if (m_configStore.contains(ARGUMENTS_KEY) && m_configStore.at(ARGUMENTS_KEY).has_value() && m_configStore.at(ARGUMENTS_KEY).type() == typeid(std::vector<std::string>()))
        return std::any_cast<std::vector<std::string>>(m_configStore.at(ARGUMENTS_KEY));

    return {};
}

auto CompilerConfiguration::SetArguments(const std::initializer_list<std::string> &args) -> CompilerConfiguration& {
    m_configStore[ARGUMENTS_KEY] = std::vector<std::string>(args);

    return *this;
}

auto CompilerConfiguration::AddArgument(const std::string &arg) -> CompilerConfiguration& {
    if (m_configStore.contains(ARGUMENTS_KEY) && m_configStore.at(ARGUMENTS_KEY).has_value() && m_configStore.at(ARGUMENTS_KEY).type() == typeid(std::vector<std::string>)) {
        std::any_cast<std::vector<std::string>&>(m_configStore.at(ARGUMENTS_KEY)).push_back(arg);
    } else {
        m_configStore[ARGUMENTS_KEY] = std::list<std::string>({arg});
    }

    return *this;
}

auto CompilerConfiguration::GetOutput() const -> std::optional<std::string> {
    if (m_configStore.contains(OUTPUT_KEY) && m_configStore.at(OUTPUT_KEY).has_value() && m_configStore.at(OUTPUT_KEY).type() == typeid(std::string))
        return std::any_cast<std::string>(m_configStore.at(OUTPUT_KEY));

    return {};
}

auto CompilerConfiguration::SetOutput(const std::string &outputName) -> CompilerConfiguration& {
    m_configStore[OUTPUT_KEY] = outputName;

    return *this;
}

auto CompilerConfiguration::GetSources() const -> std::list<std::filesystem::path> {
    if (m_configStore.contains(SOURCES_KEY) && m_configStore.at(SOURCES_KEY).has_value() && m_configStore.at(SOURCES_KEY).type() == typeid(std::list<std::filesystem::path>))
        return std::any_cast<std::list<std::filesystem::path>>(m_configStore.at(SOURCES_KEY));
    return {};
}

auto CompilerConfiguration::SetSources(const std::initializer_list<std::filesystem::path> &sources) -> CompilerConfiguration& {
    m_configStore[SOURCES_KEY] = std::list<std::filesystem::path>(sources);

    return *this;
}

auto CompilerConfiguration::AddSource(const std::filesystem::path &source) -> CompilerConfiguration& {
    if (m_configStore.contains(SOURCES_KEY) && m_configStore.at(SOURCES_KEY).has_value() && m_configStore.at(SOURCES_KEY).type() == typeid(std::list<std::filesystem::path>)) {
        std::any_cast<std::list<std::filesystem::path>&>(m_configStore.at(SOURCES_KEY)).push_back(source);
    } else {
        m_configStore[SOURCES_KEY] = std::list<std::filesystem::path>({source});
    }

    return *this;
}
