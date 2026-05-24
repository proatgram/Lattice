export module Lattice.Tooling.Compiler;

export import Lattice.Tooling.Configuration;
export import Lattice.Plugin.IFactory;

export import std;

template <typename T>
concept ConstIterable = requires (T type) {
    { std::cbegin(type) };
    { std::cend(type) };
};

export namespace Lattice::Tooling {
    class CompilerConfiguration : Configuration {
        public:
            CompilerConfiguration(CompilerConfiguration::Constructable);

            static constexpr std::string ARGUMENTS_KEY = "arguments";
            static constexpr std::string OUTPUT_KEY = "output";
            static constexpr std::string SOURCES_KEY = "sources";

            auto GetArguments() const -> std::vector<std::string>;
            auto SetArguments(const std::initializer_list<std::string> &args) -> CompilerConfiguration&;
            template <typename T> requires ConstIterable<T>
            inline auto SetArguments(const T &args) -> CompilerConfiguration& {
                m_configStore[ARGUMENTS_KEY] = std::vector<std::string>(std::cbegin(args), std::cend(args));

                return *this;
            }
            auto AddArgument(const std::string &arg) -> CompilerConfiguration&;

            auto GetOutput() const -> std::optional<std::string>;
            auto SetOutput(const std::string &outputName) -> CompilerConfiguration&;

            auto GetSources() const -> std::list<std::filesystem::path>;
            auto SetSources(const std::initializer_list<std::filesystem::path> &sources) -> CompilerConfiguration&;
            template <typename T> requires ConstIterable<T>
            inline auto SetSources(const T &sources) -> CompilerConfiguration& {
                m_configStore[SOURCES_KEY] = std::list<std::filesystem::path>(std::cbegin(sources), std::cend(sources));

                return *this;
            }
            auto AddSource(const std::filesystem::path &source) -> CompilerConfiguration&;

            friend class ICompiler;
    };

    class ICompiler {
        public:
            virtual ~ICompiler() = default;
            virtual auto CreateConfiguration() const -> CompilerConfiguration;
            // TODO: Create Command class
            virtual auto CreateCommand(const CompilerConfiguration &configuration) const -> void;

        private:
    };

    template <typename Factory>
    using ICompilerFactory = Plugin::ISingletonFactory<Factory, ICompiler>;

}  // export namespace Lattice::Tooling
