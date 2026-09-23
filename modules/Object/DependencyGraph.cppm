export module Lattice.Object.DependencyGraph;

export import std;

export import Lattice.Object.Resolver;

export namespace Lattice::Object {
    /**
     * @brief Dependency Graph generator.
     *
     * This class allows you to generate a dependency graph for
     * the entire Lattice context, and then generate a sub graph
     * for a specific node in the full graph.
     */
    class DependencyGraph : std::enable_shared_from_this<DependencyGraph> {
        struct Constructable{};
        public:
            /**
             * @brief A node on the dependency graph.
             */
            struct DependencyNode {
                enum class Status {
                    Pending,
                    Ready,
                    Running,
                    Finished,
                    Failed
                };
                std::shared_ptr<Resolver> object;
                std::list<std::shared_ptr<DependencyNode>> dependents;

                std::size_t dependencyCount;
                Status status;
            };

            friend auto operator<(const DependencyNode &lhs, const DependencyNode &rhs) -> bool {
                return lhs.dependencyCount < rhs.dependencyCount;
            }

            /**
             * @brief Default constructor.
             *
             * Generates a dependency graph for the entire Lattice context, or
             * a specific group of objects specified in objectResolvers.
             *
             * This will check the cache to see if anything for the specific
             * target, or entire context needs to be re-scheduled.
             */
            DependencyGraph(Constructable, const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers = {});
            
            /**
             * @brief Generates the entire dependency graph.
             */
            static auto Generate(const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers = {}) -> std::shared_ptr<DependencyGraph>;

            /**
             * @brief Gets the currently ready to be worked on object nodes.
             *
             * @return A list of shared pointers to `DependencyNode`'s.
             */
            auto GetReady() const -> std::list<std::shared_ptr<DependencyNode>>;

            auto GetTotalObjects() const -> std::size_t;

            /**
             * @brief Checks if the dependency graph has finished traversing.
             *
             * @return true if the dependency graph is done, false otherwise.
             */
            auto IsCompleted() const -> bool;

            /**
             * @brief Updates the dependency graph for a node.
             *
             * This updates the dependency graph for the node
             * that has been processed and ran.
             * @param[in] node The node that has finished.
             */
            auto Update(const std::shared_ptr<DependencyNode> &node) -> void;

        private:
            auto RecursiveDependencyGraph(const std::shared_ptr<Resolver> &objectResolver, std::map<std::string, std::shared_ptr<DependencyNode>> &currentGraph, const std::optional<std::shared_ptr<DependencyNode>> &dependee = {}) -> bool;
            std::map<std::string, std::shared_ptr<DependencyNode>> m_dependencyNodesMap;
            std::list<std::shared_ptr<DependencyNode>> m_dependencyNodesSorted;
    };
}  // export namespace Lattice::Object
