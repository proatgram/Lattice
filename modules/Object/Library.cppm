export module Lattice.Object.Library;

export import std;

export import Lattice.Object;
export import Lattice.Object.Buildable;
export import Lattice.Object.Parsable;
export import Lattice.Plugin.IFactory;

export namespace Lattice {
    class Library final : public Object, public Buildable, public Parsable {
        public:
            Library(Constructable, const std::string &identifier);
            
            auto GetProperties() const -> std::bitset<TOTAL_PROPERTIES> final;

            auto Build() -> void final;

            auto Parse(const std::string &parsableString) -> void final;

        private:

            friend class LibraryFactory;
    };

    class LibraryFactory final : public IObjectFactory<LibraryFactory> {
        public:
            inline LibraryFactory(Constructable) {}

            auto Create(const std::string &identifier) -> std::shared_ptr<Object> final;
    };
}  // export namespace Lattice
