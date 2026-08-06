export module Lattice.Object.LanguageIdentifiable;

export import std;

export namespace Lattice {
    class LanguageIdentifiable {
        public:
            LanguageIdentifiable() = default;
            LanguageIdentifiable(const std::list<std::string> &supportedLanguages);

            auto GetSupportedLanguages() const -> std::list<std::string>;

        protected:
            auto SetSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void;

            auto AddSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void;

            auto AddSupportedLanguage(const std::string &supportedLanguage) -> void;

        private:

            std::list<std::string> m_supportedLanguages;
    };
}
