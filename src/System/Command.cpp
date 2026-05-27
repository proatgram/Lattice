module;

module Lattice.System.Command;

using namespace Lattice::System;

auto Command::GetExecutable() const -> std::string {
    return m_executable;
}

auto Command::SetExecutable(const std::string &executable) -> Command& {
    m_executable = executable;
    return *this;
}

auto Command::GetArguments() const -> const std::vector<std::string>& {
    return m_arguments;
}

auto Command::SetArguments(const std::initializer_list<std::string> &arguments) -> Command& {
    m_arguments = arguments;
    return *this;
}

auto Command::AddArgument(const std::string &argument) -> Command& {
    m_arguments.push_back(argument);

    return *this;
}

auto Command::GetCommandLine() const -> std::string {
    std::string cmdLine = m_executable;

    for (const std::string &arg : m_arguments) {
        cmdLine += " " + arg;
    }

    return cmdLine;
}

auto Command::GetWorkingDirectory() const -> std::optional<std::filesystem::path> {
    return m_workingDirectory;
}

auto Command::SetWorkingDirectory(const std::filesystem::path &workingDirectory) -> Command& {
    m_workingDirectory = workingDirectory;

    return *this;
}

auto Command::GetEnvironments() const -> const std::unordered_map<std::string, std::string>& {
    return m_environmentVariables;
}

auto Command::SetEnvironments(const std::initializer_list<std::pair<const std::string, std::string>> &environmentVariables) -> Command& {
    m_environmentVariables = environmentVariables;

    return *this;
}

auto Command::AddEnvironments(const std::string &key, const std::string &value) -> Command& {
    m_environmentVariables[key] = value;

    return *this;
}
