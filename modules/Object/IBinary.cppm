export module Lattice.Object.IBinary;

export import std;

export import Lattice.Object;
export import Lattice.Object.Capabilities.LanguageIdentifiable;
export import Lattice.Object.Capabilities.ProjectIdentifiable;
export import Lattice.Object.Capabilities.HasToolchain;
export import Lattice.Object.Capabilities.HasDependencies;
export import Lattice.Object.Capabilities.HasProperties;
export import Lattice.IFactory;

export namespace Lattice::Object {
    class IBinary : public Lattice::Object::Object,
                    public Lattice::Object::Capabilities::ProjectIdentifiable,
                    public Lattice::Object::Capabilities::HasToolchain,
                    public Lattice::Object::Capabilities::HasDependencies,
                    public Lattice::Object::Capabilities::HasProperties
    {
        public:
            IBinary(Constructable, const std::string &identifier);
            virtual ~IBinary() = default;

        protected:
            friend class IBinaryFactory;
    };
    class IBinaryFactory final : public IObjectFactory<IBinaryFactory> {
        public:
            inline IBinaryFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;
    };

    template <typename Factory>
    class BinaryFactory : public ISingletonFactory<Factory, IBinary>, public virtual Capabilities::LanguageIdentifiable {protected: using Capabilities::LanguageIdentifiable::SetSupportedLanguages; using Capabilities::LanguageIdentifiable::AddSupportedLanguages;};
}  // export namespace Lattice::Object
