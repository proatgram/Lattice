module Lattice.Object.Linkable;

using namespace Lattice;

auto Linkable::AddLinkableArtifact(const Artifact &artifact) -> void {
    m_linkableArtifacts.push_back(artifact);
}
