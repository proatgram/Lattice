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
}  // export namespace Lattice::Object::Capabilities
