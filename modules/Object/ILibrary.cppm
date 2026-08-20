export module Lattice.Object.ILibrary;

export import std;

export import Lattice.Object;
export import Lattice.Object.Capabilities.LanguageIdentifiable;
export import Lattice.Object.Capabilities.ProjectIdentifiable;
export import Lattice.Object.Capabilities.HasToolchain;
export import Lattice.Object.Capabilities.HasDependencies;
export import Lattice.Object.Capabilities.Propagatable;
export import Lattice.Plugin.IFactory;

export namespace Lattice::Object {
    class ILibrary : public Object, public Capabilities::LanguageIdentifiable, public Capabilities::ProjectIdentifiable, public Capabilities::HasToolchain, public Capabilities::HasDependencies, public Capabilities::Propagatable {
        public:
            ILibrary(Constructable, const std::string &identifier);

        protected:

            friend class ILibraryFactory;
    };

    class ILibraryFactory final : public IObjectFactory<ILibraryFactory> {
        public:
            inline ILibraryFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;
    };

    template <typename Factory>
    class LibraryFactory : public Plugin::ISingletonFactory<Factory, ILibrary>, public virtual Capabilities::LanguageIdentifiable {protected: using Capabilities::LanguageIdentifiable::SetSupportedLanguages; using Capabilities::LanguageIdentifiable::AddSupportedLanguages;};
}  // export namespace Lattice::Object
