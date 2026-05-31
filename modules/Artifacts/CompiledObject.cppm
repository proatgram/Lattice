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

            auto GetSource() const -> std::shared_ptr<const Object>;
            auto GetSource() -> std::shared_ptr<Object>;
    
        private:
            std::filesystem::path m_filePath;

            std::shared_ptr<Object> m_source;
    };
}  // export namespace Lattice
