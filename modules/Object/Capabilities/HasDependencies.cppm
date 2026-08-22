export module Lattice.Object.Capabilities.HasDependencies;

export import std;

export import Lattice.Object;
export import Lattice.Object.Resolver;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Capability giving an Object dependency capabilities.
     *
     * Inheriting Objects will gain the ability to have dependencies
     * discoverable by the Lattice engine.
     */
    class HasDependencies : public ICapability {
        public:
            virtual ~HasDependencies() = default;

            /**
             * @brief Obtains the public dependencies for the implementing Object.
             *
             * @return A list of shared_ptr's to each dependency.
             */
            auto GetPublicDependencies() const -> std::list<std::shared_ptr<Object>>;

        protected:

            struct Dependency {
                enum class Visibility { Public, Private };

                Visibility visibility;

                std::shared_ptr<Resolver> resolver;
            };

            /**
             * @brief Obtains the private dependencies for the implementing Object.
             *
             * @return A list of shared_ptr's to each dependency.
             */
            auto GetPrivateDependencies() const -> std::list<std::shared_ptr<Object>>;

            /**
             * @brief Obtains all of the dependencies for the implementing Object.
             *
             * @return A list of Dependency instances.
             */
            auto GetAllDependencies() const -> std::list<Dependency>;

            /**
             * @brief Adds a dependency.
             */
            auto AddDependency(const Dependency &dependency) -> void;

        private:
            std::list<Dependency> m_dependencies;
    };
}  // export namespace Lattice::Object::Capabilities
