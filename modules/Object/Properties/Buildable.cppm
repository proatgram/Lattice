export module Lattice.Object.Buildable;

import std;

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
             * @brief Builds the buildable object.
             *
             * @return Void.
             */
            virtual auto Build() -> void = 0;

            /**
             * @brief Gets the toolchain ID for this buildable object.
             *
             * @return Optional string containing nothing or the toolchainId.
             */
            auto GetToolchainId() const -> std::optional<std::string>;

        protected:
            /**
             * @brief Sets the toolchain ID
             *
             * @param[in] toolchainId The Toolchain ID
             *
             */
            auto SetToolchainId(const std::string &toolchainId) -> void;

        private:
            std::optional<std::string> m_toolchainId;
    };
}  // export namespace Lattice::Object
