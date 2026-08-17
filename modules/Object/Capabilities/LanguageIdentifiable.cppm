export module Lattice.Object.Capabilities.LanguageIdentifiable;

export import std;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Capability for an Object to have supported languages.
     *
     * Use this Capability to give an object the ability to have
     * languages that it can support.
     */
    class LanguageIdentifiable : public ICapability {
        public:
            virtual ~LanguageIdentifiable() = default;

            LanguageIdentifiable() = default;
            LanguageIdentifiable(const std::list<std::string> &supportedLanguages);

            /**
             * @brief Get the supported languages for the implementing Object.
             *
             * @return A list of supported languages.
             */ 
            auto GetSupportedLanguages() const -> std::list<std::string>;

        protected:
            /**
             * @brief Sets the list of supported languages for the implementing Object.
             *
             * @param supportedLanguages[in] List of supported languages.
             */
            auto SetSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void;

            /**
             * @brief Adds a list of supported languages for the implementing Object.
             *
             * @param supportedLanguages[in] List of supported languages.
             */
            auto AddSupportedLanguages(const std::list<std::string> &supportedLanguages) -> void;

            /**
             * @brief Adds a supported language for the implementing Object.
             *
             * @param supportedLanguages[in] List of supported languages.
             */
            auto AddSupportedLanguage(const std::string &supportedLanguage) -> void;

        private:
            std::list<std::string> m_supportedLanguages;
    };
}  // export namespace Lattice::Object::Capabilities
