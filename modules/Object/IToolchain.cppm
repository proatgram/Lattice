module;
export module Lattice.Object.IToolchain;

export import std;
export import Lattice.Object;
export import Lattice.Plugin.IFactory;
export import Lattice.Object.Capabilities.LanguageIdentifiable;

export namespace Lattice::Object {
    /**
     * @brief Represents a toolchain.
     *
     * Contains information on paths, versions, flags and
     * target ABI, Arch, and OS for the toolchain.
     *
     */
    class IToolchain : public Object, public Capabilities::LanguageIdentifiable {
        public:
            IToolchain(Constructable, const std::string &identifier);
            virtual ~IToolchain() = default;

            /**
             * @brief Checks if the Toolchain provides a feature.
             *  
             *  @tparam Type to check for.
             *
             * @return True if it provides a feature, else false.
             */
            template <typename T>
            inline auto Provides() const -> bool {
                return false;
            }

            /**
             * @brief Gets a feature from the toolchain.
             *
             * If the toolchain does not provide this feature,
             * it returns an empty optional.
             *
             * @return The provided feature, or an empty optional.
             */
            template <typename T>
            inline auto Get() const -> std::optional<T> {
                return std::nullopt;
            }

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

        private:

            std::string m_targetOS;
            std::string m_targetArchitecture;
            std::string m_targetABI;
            std::string m_targetVendor;

        protected:
            using LanguageIdentifiable::SetSupportedLanguages;
            using LanguageIdentifiable::AddSupportedLanguage;

            friend class IToolchainFactory;


    };

    class IToolchainFactory final : public IObjectFactory<IToolchainFactory> {
        public:
            inline IToolchainFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;

            auto CreateDefault() -> std::shared_ptr<IToolchain>;
    };

    template <typename Factory>
    class ToolchainFactory : public Plugin::ISingletonFactory<Factory, IToolchain>, public virtual Capabilities::LanguageIdentifiable {protected: using Capabilities::LanguageIdentifiable::SetSupportedLanguages; using Capabilities::LanguageIdentifiable::AddSupportedLanguages;};
}  // export namespace Lattice::Object
