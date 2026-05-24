export module Lattice.Object.Parsable;
export import std;

export namespace Lattice {
    /**
     * @brief Provides a base class for objects that can be parsed from strings.
     *
     * This class defines the Parse() interface for objects
     * that can be constructed from YAML or similar string formats.
     *
     */
    class Parsable {
        public:
            /**
             * @brief Parses the parsable object from a string.
             *
             * @param parsableString The string to parse.
             * @return Void.
             */
            virtual auto Parse(const std::string &parsableString) -> void = 0;
        protected:
    };
}  // export namespace Lattice
