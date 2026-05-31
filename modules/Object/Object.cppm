export module Lattice.Object;
export import Lattice.Plugin.IFactory;

export import std;

export namespace Lattice {
    /**
     * @brief Represents any object defined in a lattice config.
     *
     * This class provides a base for any object (configuration section)
     * defined in a set of lattice configs.
     * (e.g. library object, module object, project object, binary object, etc.)
     *
     */
    class Object : public std::enable_shared_from_this<Object> {
        public:
            static constexpr short TOTAL_PROPERTIES = 8;
            enum class Properties {
                Buildable,
                Linkable,
                Includable,
                Exportable,
                Importable,
                Runnable,
                Parsable,
                LanguageIdentifiable
            };

            /**
             * @brief Gets the identifier for the object
             *
             * @return The object identifier
             */
            auto GetIdentifier() const -> std::string;

            /**
             * @brief Gets the properties for the object
             *
             * The properties are stored as a bitset with the
             * bits being of `Properties`.
             *
             * @return Bitset for properties.
             */
            virtual auto GetProperties() const -> std::bitset<TOTAL_PROPERTIES> = 0;

            template <typename T> requires std::is_base_of_v<Object, T>
            inline auto As() const -> std::optional<std::shared_ptr<T>> {
                return std::dynamic_pointer_cast<T>(shared_from_this());
            }

            template <typename T> requires std::is_base_of_v<Object, T>
            inline auto As() -> std::optional<std::shared_ptr<T>> {
                return std::dynamic_pointer_cast<T>(shared_from_this());
            }

        protected:
            struct Constructable{};
            /**
             * @brief Creates an object
             *
             * @param identifier The object's identifier.
             */
            Object(Constructable, const std::string &identifier);


            std::string m_identifier;
    };

    template <typename Factory>
    using IObjectFactory = Plugin::ISingletonFactory<Factory, Object>;
}  // export namespace Lattice
