export module Lattice.Object.Parsable;
export import std;

export namespace Lattice {
    class Parsable {
        public:
            virtual auto Parse(const std::string &parsableString) -> void = 0;
        protected:
    };
}  // export namespace Lattice
