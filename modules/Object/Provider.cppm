export module Lattice.Provider;

import std;
export import Lattice.Object;
export import Lattice.Plugin.IFactory;

export namespace Lattice {
    /**
     * @brief Represents a dependency provider.
     *
     * This class represents an abstract dependency provider that can
     * be extended from to provide sources for dependencies used in projects.
     */
    class Provider {
        public:
            Provider(const std::string &providerName);
            virtual ~Provider() = default;

            /**
             * @brief Gets the providers name.
             *
             * @return The providers name.
             */
            auto GetName() const -> std::string;

            /**
             * @brief Query for a target through the provider.
             *
             * The structure of the object returned (if any at all) is completely
             * dependent on the provider implementation.
             *
             * @return An optional shared pointer to an object
             */
            virtual auto Query(const std::string &identifier) const -> std::optional<std::shared_ptr<Lattice::Object>> = 0;

        private:
            std::string m_providerName;
    };

    /**
     * @brief Abstract factory for a provider
     */
    template <typename Factory>
    using IProviderFactory = IObjectFactory<Factory>;
}  // export namespace Lattice
