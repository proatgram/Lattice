module Lattice.Object.LanguageIdentifiable;

using namespace Lattice;

LanguageIdentifiable::LanguageIdentifiable(const std::list<std::string> &supportedLanguages) : m_supportedLanguages(supportedLanguages) {}

auto LanguageIdentifiable::GetSupportedLanguages() const -> std::list<std::string> {
    return m_supportedLanguages;
}

auto LanguageIdentifiable::SetSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void {
    m_supportedLanguages = std::list<std::string>(supportedLanguages);
}

auto LanguageIdentifiable::AddSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void {
    m_supportedLanguages.insert(m_supportedLanguages.end(), supportedLanguages.cbegin(), supportedLanguages.cend());
}

auto LanguageIdentifiable::AddSupportedLanguage(const std::string &supportedLanguage) -> void {
    m_supportedLanguages.push_back(supportedLanguage);
}
