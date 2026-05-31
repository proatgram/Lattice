export module Lattice.Object.ILibrary;

export import std;

export import Lattice.Object;
export import Lattice.Object.Buildable;
export import Lattice.Object.LanguageIdentifiable;
export import Lattice.Plugin.IFactory;

export namespace Lattice {
    class ILibrary : public Object, public Buildable, public LanguageIdentifiable {
        public:
            ILibrary(Constructable, const std::string &identifier);

            /**
             * @brief Default build implementation.
             *
             * If your library implementation is something simple, then you
             * can probably use the default implementation.
             */
            auto Build() -> void override;
            
            /**
             * @brief Gets the default ILibrary properties.
             *
             * If your library implementation is something simple, then you
             * can probably use the default implementation.
             */
            auto GetProperties() const -> std::bitset<TOTAL_PROPERTIES> override;

        protected:

            friend class ILibraryFactory;
    };

    class ILibraryFactory : public IObjectFactory<ILibraryFactory> {
        public:
            inline ILibraryFactory(Constructable) {}

            auto Create(const std::string &identifier, const std::optional<std::string> &objectData) -> std::shared_ptr<Object> final;
    };

    template <typename Factory>
    class LibraryFactory : public Plugin::ISingletonFactory<Factory, ILibrary>, public LanguageIdentifiable {};
}  // export namespace Lattice
