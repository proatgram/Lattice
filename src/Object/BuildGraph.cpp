module Lattice.Object.BuildGraph;

import Lattice.Object.Capabilities.HasDependencies;
import Lattice.Project;
import Lattice.Registry;

using namespace Lattice::Object;

// The default ctor generates a full build graph, and forward traverses the nodes, assigning dependents
// and dependency counts.
BuildGraph::BuildGraph(Constructable) : m_dependencyNodesMap{}, m_dependencyNodesSorted{} {
    std::function<void(const std::shared_ptr<DependencyNode>&)> recurseAdd = [this, &recurseAdd](const std::shared_ptr<DependencyNode> &currentDependencyNode) -> void {
        if (!currentDependencyNode->object->IsResolved() && !currentDependencyNode->object->TryResolve())
            throw std::runtime_error(std::format("Failed to compute dependency graph: Object {} has not been resolved.", currentDependencyNode->object->GetResolverContext().identifier));

        // If the object isn't in our storage, we add it.
        if (!m_dependencyNodesMap.contains(currentDependencyNode->object->GetResolvedObject()->GetIdentifier())) {
            m_dependencyNodesMap[currentDependencyNode->object->GetResolvedObject()->GetIdentifier()] = currentDependencyNode;
            m_dependencyNodesSorted.push_back(currentDependencyNode);
        }

        if (auto hasDependencies = currentDependencyNode->object->GetResolvedObject()->GetCapability<Capabilities::HasDependencies>(); hasDependencies) {
            // The dependency count tells us when a node is ready to be built.
            // If it's dependency count (the number of unbuilt dependencies)
            // reaches 0, it can be built since it's not waiting on anything.
            currentDependencyNode->dependencyCount = hasDependencies.value()->GetAllDependencies().size();

            // Forward traversing requires us to deal with dependencies, which is easy.
            for (const Capabilities::HasDependencies::Dependency &dependency : hasDependencies.value()->GetAllDependencies()) {
                std::shared_ptr<DependencyNode> dependencyNodeofDependencyCurrent;

                // If the dependency doesn't exist in our node containers, we add it.
                if (!m_dependencyNodesMap.contains(dependency.resolver->GetResolvedObject()->GetIdentifier())) {
                    dependencyNodeofDependencyCurrent = std::make_shared<DependencyNode>(
                        dependency.resolver,
                        std::list<std::shared_ptr<DependencyNode>>{},
                        0
                    );
                } else {
                    dependencyNodeofDependencyCurrent = m_dependencyNodesMap.at(dependency.resolver->GetResolvedObject()->GetIdentifier());
                }

                // We then add the current node we are working on as a dependent of this dependency.
                dependencyNodeofDependencyCurrent->dependents.push_back(currentDependencyNode);
                // And we call the same function for this dependency to grab everything.
                recurseAdd(dependencyNodeofDependencyCurrent);
           }
        } else if (auto project = currentDependencyNode->object->GetResolvedObject()->As<Project>().value_or(nullptr); project) {
            // Special case if the current node is a project, but does the same thing.

            currentDependencyNode->dependencyCount = project->GetObjects().size();
            for (const auto &[id, object] : project->GetObjects()) {
                std::shared_ptr<DependencyNode> dependencyNodeofDependencyCurrent;
                std::shared_ptr<Resolver> resolver = Resolver::Create({.identifier = id, .dependee = project});
                if (!m_dependencyNodesMap.contains(resolver->GetResolvedObject()->GetIdentifier())) {
                    dependencyNodeofDependencyCurrent = std::make_shared<DependencyNode>(
                        resolver,
                        std::list<std::shared_ptr<DependencyNode>>{},
                        0
                    );
                } else {
                    dependencyNodeofDependencyCurrent = m_dependencyNodesMap.at(resolver->GetResolvedObject()->GetIdentifier());
                }

                dependencyNodeofDependencyCurrent->dependents.push_back(currentDependencyNode);
                recurseAdd(dependencyNodeofDependencyCurrent);
            }
        }
    };

    for (const std::shared_ptr<Project> &project : Registry::GetInstance()->All<std::shared_ptr<Project>>()) {
        std::shared_ptr<DependencyNode> projectDependencyNode;
        if (!m_dependencyNodesMap.contains(project->GetIdentifier())) {
            projectDependencyNode = std::make_shared<DependencyNode>(
                Resolver::Create({
                    .identifier = project->GetIdentifier(),
                    .dependee = {}
                }),
                std::list<std::shared_ptr<DependencyNode>>{},
                0
            );

            recurseAdd(projectDependencyNode);
        }
    }

    m_dependencyNodesSorted.sort([](const std::shared_ptr<DependencyNode> &lhs, const std::shared_ptr<DependencyNode> &rhs) {
        return *lhs < *rhs;
    });
}

// The ctor responsible for generating a subgraph of the original does a reverse traversal through the nodes
// to see which nodes need updating based on the specified object.
BuildGraph::BuildGraph(Constructable, const BuildGraph &other, const std::shared_ptr<const Object> &object) {
    if (!other.m_dependencyNodesMap.contains(object->GetIdentifier()))
        throw std::runtime_error(std::format("Failed to generate sub-graph for the build: {} is not in the build graph.", object->GetIdentifier()));

    std::function<void(const std::shared_ptr<DependencyNode>&, int)> reverseRecurse = [this, &reverseRecurse, &object, &other](const std::shared_ptr<DependencyNode> &dependencyNode, int dependencyCount) -> void {

        dependencyNode->dependencyCount = dependencyCount;

        std::list<std::shared_ptr<DependencyNode>> newDependents;
        for (std::shared_ptr<DependencyNode> &dependent : dependencyNode->dependents) {
            if (!m_dependencyNodesMap.contains(dependent->object->GetResolvedObject()->GetIdentifier())) {
                // Create new DependencyNode object so the shared_ptr doesn't modify the original build graph.
                m_dependencyNodesMap[dependent->object->GetResolvedObject()->GetIdentifier()] = std::make_shared<DependencyNode>(*dependent);
                m_dependencyNodesSorted.push_back(m_dependencyNodesMap[dependent->object->GetResolvedObject()->GetIdentifier()]);
            }

            // Update dependent to be the new shared_ptr for our new build graph.
            dependent = m_dependencyNodesMap[dependent->object->GetResolvedObject()->GetIdentifier()];
            newDependents.push_back(dependent);

            reverseRecurse(dependent, dependencyCount + 1);
        }

        dependencyNode->dependents = newDependents;
    };

    std::shared_ptr<DependencyNode> newDependencyNode = std::make_shared<DependencyNode>(*other.m_dependencyNodesMap.at(object->GetIdentifier()));
    m_dependencyNodesMap[object->GetIdentifier()] = newDependencyNode;
    m_dependencyNodesSorted.push_back(newDependencyNode);

    reverseRecurse(newDependencyNode, 0);

    m_dependencyNodesSorted.sort([](const std::shared_ptr<DependencyNode> &lhs, const std::shared_ptr<DependencyNode> &rhs) {
        return *lhs < *rhs;
    });
}

auto BuildGraph::Generate() -> std::shared_ptr<BuildGraph> {
    return std::make_shared<BuildGraph>(Constructable{});
}

auto BuildGraph::SubGraph(const std::shared_ptr<Resolver> &objectResolver) -> std::shared_ptr<BuildGraph> {
    if (!objectResolver || !objectResolver->IsResolved())
        throw std::runtime_error(std::format("Failed to generate sub-graph for the build: {} is not resolved.", objectResolver->GetResolverContext().identifier));

    return std::make_shared<BuildGraph>(Constructable(), *this, objectResolver->GetResolvedObject());
}

auto BuildGraph::GetReady() const -> std::list<std::shared_ptr<DependencyNode>> {
    std::list<std::shared_ptr<DependencyNode>> readyNodes;

    for (const std::shared_ptr<DependencyNode> &dependencyNode : m_dependencyNodesSorted) {
        if (dependencyNode->dependencyCount != 0)
            break;

        readyNodes.push_back(dependencyNode);
    }

    return readyNodes;
}

auto BuildGraph::UpdateBuilt(const std::shared_ptr<DependencyNode> &node) -> void {
    std::function<void(const std::shared_ptr<DependencyNode> &, std::unordered_set<std::string>&)> recurse = [&recurse](const std::shared_ptr<DependencyNode> &node, std::unordered_set<std::string> &alreadyChanged ) -> void {
        for (const std::shared_ptr<DependencyNode> &dependent : node->dependents) {
            if (!alreadyChanged.insert(dependent->object->GetResolvedObject()->GetIdentifier()).second)
                continue;

            dependent->dependencyCount--;
            recurse(dependent, alreadyChanged);
        }
    };

    std::unordered_set<std::string> changed{node->object->GetResolvedObject()->GetIdentifier()};
    recurse(node, changed);

    m_dependencyNodesSorted.remove_if([&node](const std::shared_ptr<DependencyNode> &other) -> bool {
        return node->object->GetResolvedObject()->GetIdentifier() == other->object->GetResolvedObject()->GetIdentifier();
    });

    m_dependencyNodesSorted.sort([](const std::shared_ptr<DependencyNode> &lhs, const std::shared_ptr<DependencyNode> &rhs) {
        return *lhs < *rhs;
    });
}
