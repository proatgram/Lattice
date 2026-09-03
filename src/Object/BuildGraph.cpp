module Lattice.Object.BuildGraph;

import Lattice.Object.Capabilities.HasDependencies;
import Lattice.Project;
import Lattice.Registry;

using namespace Lattice::Object;

// Goes throughand checks if this object and any other potential dependencies needs to be rebuilt.
auto BuildGraph::RecursiveBuildGraph(const std::shared_ptr<Resolver> &objectResolver, std::map<std::string, std::shared_ptr<BuildGraph::DependencyNode>> &currentGraph, const std::optional<std::shared_ptr<DependencyNode>> &dependee) -> bool {
    std::shared_ptr<DependencyNode> dependencyNode;
    if (!objectResolver->IsResolved() && !objectResolver->TryResolve().has_value())
        throw std::runtime_error(std::format("Unable to generate a build graph: {} is unable to be resolved.", objectResolver->GetResolverContext().identifier));

    if (auto it = currentGraph.find(objectResolver->GetResolvedObject()->GetIdentifier()); it != currentGraph.end())
        dependencyNode = it->second;
    else
        dependencyNode = std::make_shared<DependencyNode>(objectResolver, std::list<std::shared_ptr<DependencyNode>>{}, 0);

    if (auto hasDependencies = objectResolver->GetResolvedObject()->GetCapability<Capabilities::HasDependencies>().value_or(nullptr); hasDependencies) {
        for (const Capabilities::HasDependencies::Dependency &dependency : hasDependencies->GetAllDependencies()) {
            if (RecursiveBuildGraph(dependency.resolver, currentGraph, dependencyNode))
                dependencyNode->dependencyCount++;
        }
    } else if (auto project = objectResolver->GetResolvedObject()->As<Project>().value_or(nullptr); project) {
        for (const auto &[id, object] : project->GetObjects()) {
            if (RecursiveBuildGraph(Resolver::Create({.identifier = object->GetIdentifier(), .dependee = project}), currentGraph, dependencyNode))
                dependencyNode->dependencyCount++;
        }
    }

    if (dependencyNode->dependencyCount > 0 || true /* TODO: When Cache system implemented: check cache to see if this needs to be rebuilt */) {
        if (dependee)
            dependencyNode->dependents.push_back(dependee.value());

        currentGraph.insert({objectResolver->GetResolvedObject()->GetIdentifier(), dependencyNode});

        return true;
    }

    return false;
}

BuildGraph::BuildGraph(Constructable, const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers) {
    std::map<std::string, std::shared_ptr<BuildGraph::DependencyNode>> tree;

    if (objectResolvers) {
        for (const std::shared_ptr<Resolver> &objectResolver : objectResolvers.value()) {
            RecursiveBuildGraph(objectResolver, tree);
        }
    } else {
        for (const std::shared_ptr<Project> &project : Registry::GetInstance()->All<std::shared_ptr<Project>>()) {
            RecursiveBuildGraph(Resolver::Create({
                .identifier = project->GetIdentifier(),
                .dependee = {}
            }), tree);
        }
    }

    m_dependencyNodesMap = tree;
    m_dependencyNodesSorted = std::ranges::to<std::list<std::shared_ptr<DependencyNode>>>(m_dependencyNodesMap | std::views::values);
    m_dependencyNodesSorted.sort([](const std::shared_ptr<DependencyNode> &lhs, const std::shared_ptr<DependencyNode> &rhs) {
        return *lhs < *rhs;
    });
}

auto BuildGraph::Generate(const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers) -> std::shared_ptr<BuildGraph> {
    return std::make_shared<BuildGraph>(Constructable{}, objectResolvers);
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
    for (const std::shared_ptr<DependencyNode> &dependent : node->dependents) {
        dependent->dependencyCount--;
    }
    
    m_dependencyNodesMap.erase(node->object->GetResolvedObject()->GetIdentifier());
    m_dependencyNodesSorted.remove_if([&node](const std::shared_ptr<DependencyNode> &other) -> bool {
        return node->object->GetResolvedObject()->GetIdentifier() == other->object->GetResolvedObject()->GetIdentifier();
    });

    m_dependencyNodesSorted.sort([](const std::shared_ptr<DependencyNode> &lhs, const std::shared_ptr<DependencyNode> &rhs) {
        return *lhs < *rhs;
    });
}

auto BuildGraph::IsCompleted() const -> bool {
    return m_dependencyNodesSorted.size() == 0;
}
