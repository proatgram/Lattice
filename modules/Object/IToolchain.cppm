module;
export module Lattice.Object.IToolchain;

export import std;
export import Lattice.Object;
export import Lattice.IFactory;
export import Lattice.Object.Capabilities.LanguageIdentifiable;

export namespace Lattice::Object {
    /**
     * @brief Represents a toolchain.
     *
     * A Lattice toolchain represents something a bit different
     * from other systems like CMake. It provides means of access
     * to concrete implementation of different objects and tooling.
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
                return ProvidesImpl(typeid(T));
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
                if (std::optional<std::any> feature = GetImpl(typeid(T)); feature.has_value())
                    return {std::any_cast<T>(feature.value())};

                return {};
            }

        protected:
            using LanguageIdentifiable::SetSupportedLanguages;
            using LanguageIdentifiable::AddSupportedLanguage;

            virtual auto ProvidesImpl(const std::type_index &type) const -> bool = 0;
            virtual auto GetImpl(const std::type_index &type) const -> std::optional<std::any> = 0;

            friend class IToolchainFactory;
    };

    struct ToolchainDefault {
        std::string toolchainID{};
        std::set<std::string> fileExtentions{};
    };

    class IToolchainFactory final : public IObjectFactory<IToolchainFactory> {
        public:
            inline IToolchainFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;

            auto TryGetDefault(const std::string &objectData) const -> std::optional<std::shared_ptr<IToolchain>>;
    };

    template <typename Factory>
    class ToolchainFactory : public ISingletonFactory<Factory, IToolchain>, public virtual Capabilities::LanguageIdentifiable {protected: using Capabilities::LanguageIdentifiable::SetSupportedLanguages; using Capabilities::LanguageIdentifiable::AddSupportedLanguages;};
}  // export namespace Lattice::Object
