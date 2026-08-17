export module Lattice.Object.Capabilities.HasToolchain;

export import std;

export import Lattice.Object.IToolchain;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief An Object that inherits this Capabiloty is associated with
     * a toolchain.
     */
    class HasToolchain : public ICapability {
        public:
            virtual ~HasToolchain() = default;

            /**
             * @brief Gets the toolchain associated with this Object.
             *
             * @return A shared_ptr to the IToolchain.
             */
            auto GetToolchain() const -> std::shared_ptr<const IToolchain>;
            /**
             * @brief Gets the toolchain associated with this Object.
             *
             * @return A shared_ptr to the IToolchain.
             */
            auto GetToolchain() -> std::shared_ptr<IToolchain>;

            /**
             * @brief Gets the Toolchain ID that is associated with this Object.
             *
             * @return The Toolchain ID.
             */
            auto GetToolchainId() const -> std::string;
        protected:
            /**
             * @brief Sets the IToolchain for the associated Object.
             *
             * Setting this will updated the Toolchain ID of this Capability.
             *
             * @param[in] toolchain The IToolchain to set for this Object.
             */
            auto SetToolchain(const std::shared_ptr<IToolchain> &toolchain) -> void;
            /**
             * @brief Sets the Toolchain ID for the associated Object.
             *
             * Setting this will attempt to update the IToolchain instance
             * associated with the Object. If the Toolchain ID is invalid
             * the IToolchain instance will be `nullptr`.
             *
             * @param[in] toolchainId The Toolchain ID to set for this Object.
             */
            auto SetToolchainId(const std::string &toolchainId) -> void;

        private:
            std::shared_ptr<IToolchain> m_toolchain;
            std::string m_toolchainId;

    };
}  // export namespace Lattice::Object::Capabilities
