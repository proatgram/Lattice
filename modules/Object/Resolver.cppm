export module Lattice.Object.Resolver;

export import std;

export import Lattice.Object;

export namespace Lattice::Object {
    /**
     * @brief Handles dependency resolution.
     * 
     * Through the use of instance tracking, Resolver can access every created
     * resolvable entity and attempt to resolve it, making the resolved object
     * accessible directly to the consumer of the dependency.
     */
    class Resolver {
        struct Constructable {};
        public:
            /**
             * @brief Context for resolution of a dependency.
             *
             * Each resolvable dependency will need to pass in
             * a resolver context for resolution to work.
             */
            struct Context {
                std::string identifier;
                std::optional<std::shared_ptr<Object>> dependee;
            };

            /**
             * @brief Creates a new resolver instance given a context.
             *
             * @param[in] resolverContext The context for the resolution.
             *
             * @return A shared_ptr to the created Resolver instance.
             */
            static auto Create(const Context &resolverContext) -> std::shared_ptr<Resolver>;

            Resolver(Constructable, const Context &resolverContext);

            /**
             * @brief Attempts to resolve all available declared dependencies.
             *
             * This function will run through all of the Resolver instances and
             * try to resolve each of them from ID to Object. It aggrigates a list
             * of dependencies that failed to resolve, and returns it if there are
             * any failures. If there are none, then the default-constructed return
             * type is returned to the caller.
             *
             * @return An expected value of void on success. A list containing the {identifier, reason} for
             * resolution failure on fail.
             */
            static auto TryResolveAll() -> std::expected<void, std::list<std::pair<std::string, std::string>>>;

            /**
             * @brief Attempts to resolve this instance.
             *
             * @return An expected value of void on success. A string containing the reason for resolution
             * failure on fail.
             */
            auto TryResolve() -> std::expected<void, std::string>;

            /**
             * @brief Returns if this instance has been resolved.
             *
             * @return true if this has already been resolved, false if not.
             */
            auto IsResolved() const -> bool;

            /**
             * @brief Obtains the resolved object.
             *
             * If this instance has not been resolved, it will return
             * a nullptr instance of the shared_ptr.
             *
             * @return A shared_ptr containing the resolved object.
             */
            auto GetResolvedObject() const -> std::shared_ptr<Object>;

            /**
             * @brief Gets the Resolver Context.
             *
             * @return The Resolver Context.
             */
            auto GetResolverContext() const -> Context;

        private:
            static std::list<std::shared_ptr<Resolver>> s_resolvers;

            Context m_resolverContext;

            std::optional<std::shared_ptr<Object>> m_resolvedObject;
    };
}  // export namespace Lattice::Object
