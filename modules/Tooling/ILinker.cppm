export module Lattice.Tooling.ILinker;

export import Lattice.Tooling.Configuration;
export import Lattice.System.Command;
export import Lattice.Plugin.IFactory;
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
     * @brief Configration for a linker implementation.
     *
     */
    class LinkerConfiguration : Configuration {
        public:
            static constexpr std::string LINKABLE_FILES_KEY = "files";

            /**
             * @brief Gets a list of files that are linkable.
             *
             * @return A list of paths to linkable files.
             */
            auto GetLinkableFiles() const -> std::list<std::filesystem::path>;

            /**
             * @brief Sets the list of files that are linkable.
             *
             * @param[in] files An initializer list of file paths.
             *
             * @return Instance to self.
             */
            auto SetLinkableFiles(const std::initializer_list<std::filesystem::path> &files) -> LinkerConfiguration&;

            /**
             * @brief Sets the linkbale files list.
             *
             * This function will erase the current set of linkable files
             * and replace them with these.
             *
             * @param[in] args Any type supporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */
            template <typename T> requires ConstIterable<T, std::filesystem::path>
            inline auto SetLinkableFiles(const T &files) -> LinkerConfiguration& {
                if (m_configStore.contains(LINKABLE_FILES_KEY) && m_configStore.at(LINKABLE_FILES_KEY).has_value() && m_configStore.at(LINKABLE_FILES_KEY).type() == typeid(std::list<std::filesystem::path>)) {
                    std::list<std::filesystem::path> &files = std::any_cast<std::list<std::filesystem::path>&>(m_configStore.at(LINKABLE_FILES_KEY));
                    files.insert(std::end(files), std::cbegin(files), std::cend(files));
                } else {
                    m_configStore[LINKABLE_FILES_KEY] = std::list<std::filesystem::path>(std::cbegin(files), std::cend(files));
                }

                return *this;
            }

            /**
             * @brief Adds to the linkable files list.
             *
             * @param[in] args Any type sypporting const iteration by std::cbegin and std::cend.
             *
             * @return Instance to self.
             */
            template <typename T> requires ConstIterable<T, std::string>
            inline auto AddLinkableFiles(const T &files) -> LinkerConfiguration& {
                if (m_configStore.contains(LINKABLE_FILES_KEY) && m_configStore.at(LINKABLE_FILES_KEY).has_value() && m_configStore.at(LINKABLE_FILES_KEY).type() == typeid(std::vector<std::string>)) {
                    std::list<std::filesystem::path> &currentFiles = std::any_cast<std::list<std::filesystem::path>&>(m_configStore.at(LINKABLE_FILES_KEY));
                    currentFiles.insert(std::end(currentFiles), std::cbegin(files), std::cend(files));
                } else {
                    m_configStore[LINKABLE_FILES_KEY] = std::list<std::filesystem::path>(std::cbegin(files), std::cend(files));
                }

                return *this;
            }

            /**
             * @brief Adds to the linkable files list.
             *
             * @param[in] file A path to a linkable file.
             *
             * @return Instance to self.
             */
            auto AddLinkableFile(const std::filesystem::path &file) -> LinkerConfiguration&;
        private:

    };

    /**
     * @brief Abstract Linker interface.
     *
     * This class can be extended to add different linkers to Lattice.
     *
     * Linkers are configured based on how they are called in the command line and
     * how they are configured in Lattice configs and in source. To add a new linker,
     * inherit from this class, and implement the `CreateCommand(..)` method and the
     * `CreateConfiguration(...)` methods to create a method of obtaining your specific
     * linker command calls and configurations.
     */
    class ILinker {
        public:
            virtual ~ILinker() = default;

            /**
             * @brief Returns a default configuration for the linker.
             *
             * This method returns a configuration object used to configure a
             * linker implementation. This is used to create a command instance.
             *
             * Implementing this function allows you to return a custom and tailored
             * configuration implementation specifically for a specific linker.
             *
             * Additionally, passing in an object type allows the compiler to handle building the command and configuration for that object.
             *
             * @param[in] obj An optional object to use to configure the configuration. 
             *
             * @return A default `LinkerConfiguration` instance for this linker.
             */
            virtual auto CreateConfiguration(const std::optional<std::shared_ptr<::Lattice::Object>> &obj = std::nullopt) const -> LinkerConfiguration; 

            /**
             * @brief Returns a `Command` instance tailored for this linker.
             *
             * This method returns a `Command` object which is used to run a linker
             * given a specific configuration.
             *
             * Implementing this function allows you to return a custom and tailored
             * `Command` implementation specifically for a specific linker, allowing you
             * to specify and manage how a linker is called.
             *
             * @param[in] configuration The linker configuration to use to create the command.
             *
             * @return A tailored `Command` instance for this linker and the configuration given.
             */
            virtual auto CreateCommand(const LinkerConfiguration &config) -> System::Command;
    };

    template <typename Factory>
    using ILinkerFactory = Plugin::ISingletonFactory<Factory, ILinker>;

}  // export namespace Lattice::Tooling
