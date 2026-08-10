export module Lattice.Object.HasToolchain;

export import std;

export import Lattice.Object.IToolchain;

export namespace Lattice {
    /**
     * @brief An object that inherits this property is associated with
     * a toolchain.
     */
    class HasToolchain {
        public:
            auto GetToolchain() const -> std::shared_ptr<const IToolchain>;
            auto GetToolchain() -> std::shared_ptr<IToolchain>;

            auto GetToolchainId() const -> std::string;
        protected:

            auto SetToolchain(const std::shared_ptr<IToolchain> &toolchain) -> void;
            auto SetToolchainId(const std::string &toolchainId) -> void;

        private:
            std::shared_ptr<IToolchain> m_toolchain;
            std::string m_toolchainId;

    };
}  // export namespace Lattice
