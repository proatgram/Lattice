module Lattice.Artifact.CompiledObject;

using namespace Lattice;

auto CompiledObject::GetSource() const -> std::shared_ptr<const Object> {
    return m_source;
}

auto CompiledObject::GetSource() -> std::shared_ptr<Object> {
    return m_source;
}

auto CompiledObject::GetFilePath() const -> std::filesystem::path {
    return m_filePath;
}
