module;
export module Lattice.Object.Toolchain;

export import std;
export import Lattice.Object;
export import Lattice.Plugin.IFactory;
export import Lattice.Tooling.Compiler;

export namespace Lattice {
    /**
     * @brief Represents a toolchain.
     *
     * Contains information on paths, versions, flags and
     * target ABI, Arch, and OS for the toolchain.
     *
     */
    class Toolchain : public Object {
        public:
            Toolchain(Constructable, const std::string &identifier);

            auto GetProperties() const -> std::bitset<Object::TOTAL_PROPERTIES> final;

            /**
             * @brief Gets the compiler name.
             *
             * @return The compiler name.
             */
            auto GetCompilerName() const -> std::string;

            /**
             * @brief Gets the full path to the compiler.
             *
             * @return The full path to the compiler.
             */
            auto GetCompilerPath() const -> std::filesystem::path;

            /**
             * @brief Gets the compiler version.
             *
             * @return A version string for the compiler.
             */
            auto GetCompilerVersion() const -> std::string;

            /**
             * @brief Gets the default system include paths.
             *
             * @return A `std::list<>` of the default system include paths.
             */
            auto GetSystemIncludePaths() const -> std::list<std::filesystem::path>;

            /**
             * @brief Gets the default library paths.
             *
             * @return A `std::list<>` of the default library paths.
             */
            auto GetLibraryPaths() const -> std::list<std::filesystem::path>;

            /**
             * @brief Gets the default compiler flags.
             * 
             * The compile flags can be ordered.
             *
             * @return A `std::vector<>` of the default compiler flags.
             */
            auto GetCompilerFlags() const -> std::vector<std::string>;

            /**
             * @brief Gets the default linker flags.
             *
             * The link flags can be ordered.
             *
             * @return A 'std::vector<>' of the default linker flags.
             */
            auto GetLinkerFlags() const -> std::vector<std::string>;

            /**
             * @brief Gets the default sysroot path.
             *
             * Not all toolchains will utilize this, but for the ones that
             * do, this has the effect of changing the root path for
             * the toolchain.
             *
             * When set, each path returned from this `Toolchain` will be relative
             * to the sysroot path.
             *
             * @return An optional path to the sysroot.
             */
            auto GetSysrootPath() const -> std::optional<std::filesystem::path>;

            /**
             * @brief Gets the supported languages for this toolchain
             *
             * @return A `std::list<>` of supported languages.
             */
            auto GetLanguages() const -> std::list<std::string>;

            /**
             * @brief Gets the target OS that this toolchain builds for.
             *
             * @return The `Toolchain`'s target OS.
             */
            auto GetTargetOS() const -> std::string;

            /**
             * @brief Gets the target Architecture that this toolchain builds for.
             *
             * @return The `Toolchain`'s target Architecture.
             */
            auto GetTargetArchitecture() const -> std::string;

            /**
             * @brief Gets the target ABI that this toolchain builds for.
             *
             * @return The 'Toolchain''s target ABI.
             */
            auto GetTargetABI() const -> std::string;

            /**
             * @brief Gets the target Vendor for this toolchain.
             *
             * @return The `Toolchain`'s Vendor
             */
            auto GetTargetVendor() const -> std::string;

            /**
             * @brief Gets the Target information in Triple format.
             *
             * Triple format is build from the Architecture, OS, Environment/ABI, and Vendor.
             * It follows the format of: `arch-vendor-os-env`.
             *
             * @return The Target Triple for the `Toolchain`.
             */
            auto GetTargetTriple() const -> std::string;

            /**
             * @brief Gets the compiler instance or creates one for this toolchain.
             *
             * @return an instance to the compiler configured for this toolchain.
             *
             */
            auto GetCompiler() const -> std::optional<std::shared_ptr<Tooling::ICompiler>>;

        private:
            std::string m_compilerName;
            std::filesystem::path m_compilerPath;
            std::string m_compilerVersion;
            std::list<std::filesystem::path> m_defaultSystemIncludes;
            std::list<std::filesystem::path> m_defaultLibraryPaths;
            std::vector<std::string> m_defaultCompilerFlags;
            std::vector<std::string> m_defaultLinkerFlags;
            std::optional<std::filesystem::path> m_optionalSysrootPath;

            std::list<std::string> m_languages;

            std::string m_targetOS;
            std::string m_targetArchitecture;
            std::string m_targetABI;
            std::string m_targetVendor;

            friend class ToolchainFactory;
    };

    class ToolchainFactory : public IObjectFactory<ToolchainFactory> {
        public:
            inline ToolchainFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;

            auto CreateDefault() -> std::shared_ptr<Toolchain>;
    };
}  // export namespace Lattice
