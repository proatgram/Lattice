export module Lattice.Tooling.Compiler;

export import Lattice.Tooling.Configuration;
export import Lattice.Plugin.IFactory;
export import Lattice.System.Command;
export import Lattice.Object;

export import std;

template <typename T, typename V>
concept ConstIterable = requires (T type) {
    { std::cbegin(type) };
    { std::cend(type) };
    requires std::is_same_v<typename T::value_type, V>;
};

export namespace Lattice::Tooling {
    
    /**
     * @brief Configuration for a compiler implementation.
     *
     */
    class CompilerConfiguration : Configuration {
        public:
            CompilerConfiguration(CompilerConfiguration::Constructable);

            /* Configuration keys */
            static constexpr std::string ARGUMENTS_KEY = "arguments";
            static constexpr std::string OUTPUT_KEY = "output";
            static constexpr std::string SOURCES_KEY = "sources";

            /**
             * @brief Gets the compiler arguments.
             * 
             * @return Vector of compiler arguments.
             */
            auto GetArguments() const -> std::vector<std::string>;

            /**
             * @brief Sets the compiler argument list.
             *
             * This function will erase the current set of arguments
             * and replace them with these.
             *
             * @param[in] args An initializer list containing the arguments to set.
             *
             * @return Instance to self.
             */
            auto SetArguments(const std::initializer_list<std::string> &args) -> CompilerConfiguration&;

            /**
             * @brief Sets the compiler argument list.
             *
             * This function will erase the current set of arguments
             * and replace them with these.
             *
             * @param[in] args Any type supporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */
            template <typename T> requires ConstIterable<T, std::string>
            inline auto SetArguments(const T &args) -> CompilerConfiguration& {
                m_configStore[ARGUMENTS_KEY] = std::vector<std::string>(std::cbegin(args), std::cend(args));

                return *this;
            }

            /**
             * @brief Adds a compiler argument.
             *
             * @param[in] arg A compiler argument.
             *
             * @return Instance to self.
             */
            auto AddArgument(const std::string &arg) -> CompilerConfiguration&;

            /**
             * @brief Adds compiler arguments.
             *
             * @param[in] args An initializer list containing the arguments to add.
             *
             * @return Instance to self.
             */
            auto AddArguments(const std::initializer_list<std::string> &args) -> CompilerConfiguration&;

            /**
             * @brief Adds compiler arguments.
             *
             * @param[in] args Any type sypporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */
            template <typename T> requires ConstIterable<T, std::string>
            inline auto AddArguments(const T &args) -> CompilerConfiguration& {
                if (m_configStore.contains(ARGUMENTS_KEY) && m_configStore.at(ARGUMENTS_KEY).has_value() && m_configStore.at(ARGUMENTS_KEY).type() == typeid(std::vector<std::string>)) {
                    std::vector<std::string> &currentArgs = std::any_cast<std::vector<std::string>&>(m_configStore.at(ARGUMENTS_KEY));
                    currentArgs.insert(std::end(currentArgs), std::cbegin(args), std::cend(args));
                } else {
                    m_configStore[ARGUMENTS_KEY] = std::vector<std::string>(std::cbegin(args), std::cend(args));
                }

                return *this;
            }

            /**
             * @brief Gets the output name that the compiler will output to.
             *
             * @return An optional string, containing a value if the output is set.
             */
            auto GetOutput() const -> std::optional<std::string>;

            /**
             * @brief Sets the output name that the compiler will output to.
             *
             * @param[in] outputName The new output name.
             *
             * @return Instance to self.
             */
            auto SetOutput(const std::string &outputName) -> CompilerConfiguration&;

            /**
             * @brief Gets the current sources list that the compiler will compile.
             *
             * @return A list of paths to each source.
             */
            auto GetSources() const -> std::list<std::filesystem::path>;

            /**
             * @brief Sets the compiler sources list.
             *
             * This function will erase the current set of sources
             * and replace them with these.
             *
             * @param[in] sources An initializer list containing the sources to set.
             *
             * @return Instance to self.
             */
            auto SetSources(const std::initializer_list<std::filesystem::path> &sources) -> CompilerConfiguration&;
            /**
             * @brief Sets the compiler sources list.
             *
             * This function will erase the current set of sources
             * and replace them with these.
             *
             * @param[in] sources Any type sypporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */
            template <typename T> requires ConstIterable<T, std::filesystem::path>
            inline auto SetSources(const T &sources) -> CompilerConfiguration& {
                m_configStore[SOURCES_KEY] = std::list<std::filesystem::path>(std::cbegin(sources), std::cend(sources));

                return *this;
            }

            /**
             * @brief Adds a compiler source.
             *
             * @param[in] source A path to a source file.
             *
             * @return Instance to self.
             */
            auto AddSource(const std::filesystem::path &source) -> CompilerConfiguration&;

            /**
             * @brief Adds compiler sources.
             *
             * @param[in] sources An initializer list containing the sources to add.
             *
             * @return Instance to self.
             */
            auto AddSources(const std::initializer_list<std::filesystem::path> &sources) -> CompilerConfiguration&;
            /**
             * @brief Adds to the compiler sources list.
             *
             * @param[in] sources Any type sypporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */

            template <typename T> requires ConstIterable<T, std::filesystem::path>
            inline auto AddSources(const T &sources) -> CompilerConfiguration& {
                if (m_configStore.contains(SOURCES_KEY) && m_configStore.at(SOURCES_KEY).has_value() && m_configStore.at(SOURCES_KEY).type() == typeid(std::list<std::filesystem::path>)) {
                    std::list<std::filesystem::path>& srcs = std::any_cast<std::list<std::filesystem::path>&>(m_configStore.at(SOURCES_KEY));
                    srcs.insert(std::end(srcs), std::cbegin(sources), std::cend(sources));
                } else {
                    m_configStore[SOURCES_KEY] = std::list<std::filesystem::path>(std::cbegin(sources), std::cend(sources));
                }
            }

            friend class ICompiler;
    };

    /**
     * @brief Abstract Compiler interface.
     *
     * This class can be extended to add different compilers to Lattice.
     *
     * Compilers are configured based on how they are called in the command line and
     * how they are configured in Lattice configs and in source. To add a new compiler,
     * inherit from this class, and implement the `CreateCommand(..)` method and the
     * `CreateConfiguration(...)` methods to create a method of obtaining your specific
     * compilers command calls and configurations.
     */
    class ICompiler {
        public:
            virtual ~ICompiler() = default;

            /**
             * @brief Returns a default configuration for the compiler.
             *
             * This method returns a configuration object used to configure a
             * compiler implementation. This is used to create a command instance.
             *
             * Implementing this function allows you to return a custom and tailored
             * configuration implementation specifically for a specific compiler.
             *
             * Additionally, passing in an object type allows the compiler to handle building the command and configuration for that object.
             *
             * @param[in] obj An optional object to use to configure the configuration. 
             *
             * @return A default `CompilerConfiguration` instance for this compiler.
             */
            virtual auto CreateConfiguration(const std::optional<std::shared_ptr<Object>> &obj = std::nullopt) const -> CompilerConfiguration;

            /**
             * @brief Returns a `Command` instance tailored for this compiler.
             *
             * This method returns a `Command` object which is used to run a compilation
             * given a specific configuration.
             *
             * Implementing this function allows you to return a custom and tailored
             * `Command` implementation specifically for a specific compiler, allowing you
             * to specify and manage how a compiler is called.
             *
             * @param[in] configuration The compiler configuration to use to create the command.
             *
             * @return A tailored `Command` instance for this compiler and the configuration given.
             */
            virtual auto CreateCommand(const CompilerConfiguration &configuration) const -> System::Command;
    };

    template <typename Factory>
    using ICompilerFactory = Plugin::ISingletonFactory<Factory, ICompiler>;

}  // export namespace Lattice::Tooling
