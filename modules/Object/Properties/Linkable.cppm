export module Lattice.Object.Linkable;

export import Lattice.Artifact;
export import std;

export namespace Lattice {
    class Linkable {
        public:
            /**
             * @brief Gets all of the artifacts that have the ability to be linked from this object.
             *
             * @return A list of Artifacts that can be linked to.
             */
            virtual auto GetLinkableArtifacts() const -> std::list<Artifact> = 0;

        protected:

            /**
             * @brief Adds a linkable artifact to the linkable artifact list.
             *
             * @param[in] artifact An artifact that can be linked.
             */
            auto AddLinkableArtifact(const Artifact &artifact) -> void;

        private:
            std::list<Artifact> m_linkableArtifacts;
    };
}  // export namespace Lattice
