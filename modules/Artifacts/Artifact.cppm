export module Lattice.Artifact;

import std;

export import Lattice.Object;

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

            auto GetSourceObject() const -> std::shared_ptr<const Object::Object>;
            auto GetSourceObject() -> std::shared_ptr<Object::Object>;

        private:
            std::shared_ptr<Object::Object> m_sourceObject;
    };
}  // export namespace Lattice
