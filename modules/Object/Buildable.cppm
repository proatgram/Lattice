export module Lattice.Object.Buildable;

export namespace Lattice {
    /**
     * @brief Provides a base class for objects that can be built.
     *
     * This class defines the Build() interface for objects
     * that have a build process.
     *
     */
    class Buildable {
        public:
            /**
             * @brief Default constructor for Buildable.
             */
            Buildable() = default;

            /**
             * @brief Builds the buildable object.
             *
             * @return Void.
             */
            virtual auto Build() -> void = 0;

        private:
    };
}  // export namespace Lattice::Object
