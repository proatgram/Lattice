export module Lattice.Object.Capabilities.Buildable;

export import std;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Provides a base class for objects that can be built.
     *
     * This class defines the Build() interface for objects
     * that have a build process.
     *
     */
    class Buildable : public ICapability {
        public:
            virtual ~Buildable() = default;

            /**
             * @brief Builds the buildable object.
             *
             * @return Void.
             */
            virtual auto Build() -> void = 0;
    };
}  // export namespace Lattice::Object::Capabilities
