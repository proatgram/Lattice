export module Lattice.Object;

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
            };

            auto GetIdentifier() const -> std::string;

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
}  // export namespace Lattice
