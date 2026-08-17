module Lattice.Object.Capabilities.ArtifactProvider;

using namespace Lattice::Object::Capabilities;

auto ArtifactProvider::AddArtifact(const Artifact &artifact) -> void {
    m_artifacts.push_back(artifact);
}
