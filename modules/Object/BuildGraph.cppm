export module Lattice.Object.BuildGraph;

export import std;

export import Lattice.Object.Resolver;

export namespace Lattice::Object {
    /**
     * @brief Build Graph generator.
     *
     * This class allows you to generate a build graph for
     * the entire Lattice context, and then generate a sub graph
     * for a specific node in the full graph.
     */
    class BuildGraph : std::enable_shared_from_this<BuildGraph> {
        struct Constructable{};
        public:
            /**
             * @brief A node on the build graph.
             */
            struct DependencyNode {
                std::shared_ptr<Resolver> object;
                std::list<std::shared_ptr<DependencyNode>> dependents;

                std::size_t dependencyCount;
            };

            friend auto operator<(const DependencyNode &lhs, const DependencyNode &rhs) -> bool {
                return lhs.dependencyCount < rhs.dependencyCount;
            }

            /**
             * @brief Default constructor.
             *
             * Generates a build graph for the entire Lattice context, or
             * a specific group of objects specified in objectResolvers.
             *
             * This will check the cache to see if anything for the specific
             * target, or entire context needs to be rebuild.
             */
            BuildGraph(Constructable, const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers = {});
            
            /**
             * @brief Generates the entire build graph.
             */
            static auto Generate(const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers = {}) -> std::shared_ptr<BuildGraph>;

            /**
             * @brief Gets the currently ready to be built object nodes.
             *
             * @return A list of shared pointers to `DependencyNode`'s.
             */
            auto GetReady() const -> std::list<std::shared_ptr<DependencyNode>>;

            /**
             * @brief Checks if the build graph has finished traversing.
             *
             * @return true if the build graph is done, false otherwise.
             */
            auto IsCompleted() const -> bool;

            /**
             * @brief Updates the build graph for a node.
             *
             * This updates the build graph for the node
             * that has been processed and built.
             *
             * The node will be removed from the internal list,
             * and every node that depended on it will have their
             * internal dependency count reduced by 1.
             *
             * @param[in] node The node that has finished.
             */
            auto UpdateBuilt(const std::shared_ptr<DependencyNode> &node) -> void;

        private:
            auto RecursiveBuildGraph(const std::shared_ptr<Resolver> &objectResolver, std::map<std::string, std::shared_ptr<DependencyNode>> &currentGraph, const std::optional<std::shared_ptr<DependencyNode>> &dependee = {}) -> bool;
            std::map<std::string, std::shared_ptr<DependencyNode>> m_dependencyNodesMap;
            std::list<std::shared_ptr<DependencyNode>> m_dependencyNodesSorted;
    };
}  // export namespace Lattice::Object
