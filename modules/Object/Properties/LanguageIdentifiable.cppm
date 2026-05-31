export module Lattice.Object.LanguageIdentifiable;

export import std;

export namespace Lattice {
    class LanguageIdentifiable {
        public:
            virtual constexpr auto GetTargetLanguage() const -> std::string_view = 0;
    };
}
