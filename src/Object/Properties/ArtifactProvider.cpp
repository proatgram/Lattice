module Lattice.Object.ArtifactProvider;

using namespace Lattice;

auto ArtifactProvider::AddArtifact(const Artifact &artifact) -> void {
    m_artifacts.push_back(artifact);
}
