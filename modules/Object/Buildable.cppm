export module Lattice.Object.Buildable;

export namespace Lattice {
    class Buildable {
        public:
            Buildable() = default;

            virtual auto Build() -> void = 0;

        private:
    };
}  // export namespace Lattice::Object
