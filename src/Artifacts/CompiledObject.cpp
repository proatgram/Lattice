module Lattice.Artifact.CompiledObject;

using namespace Lattice;

auto CompiledObject::GetFilePath() const -> std::filesystem::path {
    return m_filePath;
}
