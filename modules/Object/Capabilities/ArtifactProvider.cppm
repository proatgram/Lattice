export module Lattice.Object.Capabilities.ArtifactProvider;

export import std;

export import Lattice.Artifact;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    class ArtifactProvider : public ICapability {
        public:
            virtual ~ArtifactProvider() = default;

            template <typename Callable> requires std::is_invocable_r_v<bool, Callable, const Artifact&>
            inline auto GetArtifacts(const std::optional<Callable> predicate = {}) const -> std::list<Artifact> {
                if (!predicate)
                    return m_artifacts;

                std::list<Artifact> satisfied;

                for (const Artifact &artifact : m_artifacts) {
                    if (Callable(artifact)) {
                        satisfied.push_back(artifact);
                    }
                }

                return satisfied;
            }
        private:
            auto AddArtifact(const Artifact &artifact) -> void;

        protected:
            std::list<Artifact> m_artifacts;
    };
}  // export namespace Lattice::Object::Capabilities
