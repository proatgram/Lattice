export module Lattice.Object.ILibrary;

export import std;

export import Lattice.Object;
export import Lattice.Object.LanguageIdentifiable;
export import Lattice.Object.HasToolchain;
export import Lattice.Plugin.IFactory;

export namespace Lattice {
    class ILibrary : public Object, public LanguageIdentifiable, public HasToolchain {
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
    class LibraryFactory : public Plugin::ISingletonFactory<Factory, ILibrary>, public virtual LanguageIdentifiable {protected: using LanguageIdentifiable::SetSupportedLanguages; using LanguageIdentifiable::AddSupportedLanguages;};
}  // export namespace Lattice
