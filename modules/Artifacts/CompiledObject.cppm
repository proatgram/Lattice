export module Lattice.Artifact.CompiledObject;

export import Lattice.Artifact;
export import Lattice.Object;
export import std;

export namespace Lattice {
    class CompiledObject : public Artifact {
        public:
            enum class ObjectType {
                Object,
                Library
            };

            auto GetFilePath() const -> std::filesystem::path;

        private:
            std::filesystem::path m_filePath;
    };
}  // export namespace Lattice
