export module Lattice.Object;
export import Lattice.Plugin.IFactory;

import std;

export namespace Lattice {
    /**
     * @brief Represents any object defined in a lattice config.
     *
     * This class provides a base for any object (configuration section)
     * defined in a set of lattice configs.
     * (e.g. library object, module object, project object, binary object, etc.)
     *
     */
    class Object {
        public:
            enum class Properties {
                Buildable,
                Linkable,
                Includable,
                Exportable,
                Importable,
                Runnable,
                Parsable
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
            virtual auto GetProperties() const -> std::bitset<8> = 0;

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

    template <class Factory>
    using IObjectFactory = Plugin::IFactory<Factory, Object>;
}  // export namespace Lattice
