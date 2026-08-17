export module Lattice.Object.Capabilities.Linkable;

export import std;

export import Lattice.Object;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Provides a base class for objects that can be linked.
     *
     * This class defines the Link(const std::shared_ptr<Object>&) interface for objects
     * that have a link process.
     *
     */
    class Linkable : public ICapability {
        public:
            virtual ~Linkable() = default;

            /**
             * @brief Links the linkable object to another.
             *
             * @return Void.
             */
            virtual auto Link(const std::shared_ptr<Object> &object) -> void = 0;
    };
}  // export namespace Lattice::Object::Capabilities
