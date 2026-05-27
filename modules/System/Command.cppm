export module Lattice.System.Command;

export import std;

template <typename T>
concept ConstIterable = requires (T type) {
    { std::cbegin(type) };
    { std::cend(type) };
};


export namespace Lattice::System {
    class Command {
        public:
            Command() = default;

            auto GetExecutable() const -> std::string;
            auto SetExecutable(const std::string &executable) -> Command&;

            auto GetArguments() const -> const std::vector<std::string>&;
            auto SetArguments(const std::initializer_list<std::string> &arguments) -> Command&;
            template <typename T> requires ConstIterable<T>
            inline auto SetArguments(const T &arguments) -> Command& {
                m_arguments.insert(std::end(m_arguments), std::cbegin(arguments), std::cend(arguments));

                return *this;
            }
            auto AddArgument(const std::string &argument) -> Command&;

            auto GetCommandLine() const -> std::string;

            auto GetWorkingDirectory() const -> std::optional<std::filesystem::path>;
            auto SetWorkingDirectory(const std::filesystem::path &workingDirectory) -> Command&;

            auto GetEnvironments() const -> const std::unordered_map<std::string, std::string>&;
            auto SetEnvironments(const std::initializer_list<std::pair<const std::string, std::string>> &environmentvariables) -> Command&;
            auto AddEnvironments(const std::string &key, const std::string &value) -> Command&;
        private:
            std::string m_executable;
            std::vector<std::string> m_arguments;
            std::optional<std::filesystem::path> m_workingDirectory;
            std::unordered_map<std::string, std::string> m_environmentVariables;
    };
}  // export namespace Lattice::System
