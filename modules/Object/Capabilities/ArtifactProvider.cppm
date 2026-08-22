export module Lattice.Object.Capabilities.ArtifactProvider;

export import std;

import Lattice.Object.Properties.IProperty;
import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    class ArtifactProvider : public ICapability {
        public:
            virtual ~ArtifactProvider() = default;

            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto GetArtifacts() const -> std::optional<std::shared_ptr<Properties::IProperty>> {
                try {
                    return m_artifacts.at(typeid(T));
                } catch (const std::out_of_range &err) {
                    return {};
                }
            }
        private:
            template <typename T> requires std::is_base_of_v<Properties::IProperty, T>
            inline auto AddArtifact(const std::shared_ptr<T> &artifact) -> void {
                m_artifacts[typeid(T)] = artifact;
            }

        protected:
            std::map<std::type_index, std::shared_ptr<Properties::IProperty>> m_artifacts;
    };
}  // export namespace Lattice::Object::Capabilities
