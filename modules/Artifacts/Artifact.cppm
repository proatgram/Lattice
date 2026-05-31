export module Lattice.Artifact;

import std;

export namespace Lattice {
    class Artifact {
        public:
            template <typename T> requires std::is_base_of_v<Artifact, T>
            inline auto As() -> T& {
                return dynamic_cast<T&>(*this);
            }

            template <typename T> requires std::is_base_of_v<Artifact, T>
            inline auto As() const -> const T& {
                return dynamic_cast<const T&>(*this);
            }
    };
}  // export namespace Lattice
