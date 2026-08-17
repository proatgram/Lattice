module;
#include <valarray>
export module Lattice.System.Command;

export import std;

template <typename T>
concept ConstIterable = requires (T type) {
    { std::cbegin(type) };
    { std::cend(type) };
};


export namespace Lattice::System {
    /**
     * @brief A Command that can be ran on the host system.
     */
    class Command {
        public:
            Command() = default;

            /**
             * @brief Gets the executable for the Command.
             *
             * @return The executable string.
             */
            auto GetExecutable() const -> std::string;
            /**
             * @brief Sets the executable for the Command.
             *
             * @param[in] executable The executable for the Command.
             *
             * @return The Command instance for chaining.
             */
            auto SetExecutable(const std::string &executable) -> Command&;

            /**
             * @brief Gets the Command's Argument list.
             *
             * @return A vector containing all of the arguments in order for the Command.
             */
            auto GetArguments() const -> const std::vector<std::string>&;
            /**
             * @brief Sets the Command's Argument list.
             *
             * @param[in] arguments The Arguments for the Command.
             *
             * @return The Command instance for chaining.
             */
            auto SetArguments(const std::vector<std::string> &arguments) -> Command&;
            template <typename T> requires ConstIterable<T>
            inline auto SetArguments(const T &arguments) -> Command& {
                m_arguments.insert(std::end(m_arguments), std::cbegin(arguments), std::cend(arguments));

                return *this;
            }
            /**
             * @brief Adds an Argument to the Command's Argument list.
             *
             * @param[in] argument The argument.
             *
             * @return The Command instance for chaining.
             */
            auto AddArgument(const std::string &argument) -> Command&;

            /**
             * @brief Gets the command-line for the Command.
             *
             * This is the full, parsed, output that will be ran on the system.
             *
             * @return The full command-line.
             */
            auto GetCommandLine() const -> std::string;

            /**
             * @brief Get an optional working directory for the Command.
             *
             * @return An optional path to the working directory.
             */
            auto GetWorkingDirectory() const -> std::optional<std::filesystem::path>;
            /**
             * @brief Sets the optional working directory for the Command.
             *
             * @param[in] workingDirectory The new working directory for the Command.
             *
             * @return The Command instance for chaining.
             */
            auto SetWorkingDirectory(const std::filesystem::path &workingDirectory) -> Command&;

            /**
             * @brief Gets the environment variables for the Command.
             *
             * @return An unordered map of {env, value} for the environment variables.
             */
            auto GetEnvironments() const -> const std::unordered_map<std::string, std::string>&;
            /**
             * @brief Sets the environment variables for the Command.
             *
             * @param[in] environmentVariables An unordered map of {env, value} for the Command.
             *
             * @return The Command instance for chaining.
             */
            auto SetEnvironments(const std::unordered_map<std::string, std::string> &environmentVariables) -> Command&;

            /**
             * @brief Adds an environment variable to the Command.
             *
             * @param[in] key The environment key
             * @param[in] value The value of the environment variable.
             *
             * @return The Command instance for chaining.
             */
            auto AddEnvironments(const std::string &key, const std::string &value) -> Command&;

        private:
            std::string m_executable;
            std::vector<std::string> m_arguments;
            std::optional<std::filesystem::path> m_workingDirectory;
            std::unordered_map<std::string, std::string> m_environmentVariables;
    };
}  // export namespace Lattice::System
