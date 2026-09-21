module Lattice.Object.BuildGraph;

import Lattice.Object.Capabilities.HasDependencies;
import Lattice.Object.Capabilities.Cacheable;
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
        dependencyNode = std::make_shared<DependencyNode>(objectResolver, std::list<std::shared_ptr<DependencyNode>>{}, 0, DependencyNode::Status::Ready);

    if (auto hasDependencies = objectResolver->GetResolvedObject()->GetCapability<Capabilities::HasDependencies>().value_or(nullptr); hasDependencies) {
        if (!hasDependencies->GetAllDependencies().empty()) {
            for (const Capabilities::HasDependencies::Dependency &dependency : hasDependencies->GetAllDependencies()) {
                if (RecursiveBuildGraph(dependency.resolver, currentGraph, dependencyNode))
                    dependencyNode->dependencyCount++;
            }
        }
    } else if (auto project = objectResolver->GetResolvedObject()->As<Project>().value_or(nullptr); project) {
        for (const auto &[id, object] : project->GetObjects()) {
            if (RecursiveBuildGraph(Resolver::Create({.identifier = object->GetIdentifier(), .dependee = project}), currentGraph, dependencyNode))
                dependencyNode->dependencyCount++;
        }
    }

    bool cacheDirty{true};
    if (auto cacheable = objectResolver->GetResolvedObject()->GetCapability<Capabilities::Cacheable>().value_or(nullptr); cacheable) {
        if (auto objectCache = cacheable->GetCache().value_or(nullptr); objectCache) {
            cacheDirty = objectCache->IsDirty();
        }
    }

    if (dependencyNode->dependencyCount > 0 || cacheDirty) {
        if (dependee)
            dependencyNode->dependents.push_back(dependee.value());
        if (dependencyNode->dependencyCount == 0)
            dependencyNode->status = DependencyNode::Status::Ready;

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
    std::ranges::partition(m_dependencyNodesSorted, [](const std::shared_ptr<DependencyNode> &node) {
        return node->status == DependencyNode::Status::Ready;
    });
}

auto BuildGraph::Generate(const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers) -> std::shared_ptr<BuildGraph> {
    return std::make_shared<BuildGraph>(Constructable{}, objectResolvers);
}

auto BuildGraph::GetReady() const -> std::list<std::shared_ptr<DependencyNode>> {
    std::list<std::shared_ptr<DependencyNode>> readyNodes;

    for (const std::shared_ptr<DependencyNode> &dependencyNode : m_dependencyNodesSorted) {
        if (dependencyNode->status != DependencyNode::Status::Ready)
            break;

        readyNodes.push_back(dependencyNode);
    }

    return readyNodes;
}

auto BuildGraph::Update(const std::shared_ptr<DependencyNode> &node) -> void {
    switch (node->status) {
        case DependencyNode::Status::Finished:
            for (const std::shared_ptr<DependencyNode> &dependent : node->dependents) {
                dependent->dependencyCount--;
                if (dependent->dependencyCount == 0)
                    dependent->status = DependencyNode::Status::Ready;
            }
        case DependencyNode::Status::Ready:
        case DependencyNode::Status::Building:
        case DependencyNode::Status::Failed:
        default:
            std::ranges::partition(m_dependencyNodesSorted, [](const std::shared_ptr<DependencyNode> &node) {
                return node->status == DependencyNode::Status::Ready;
            });
        case DependencyNode::Status::Pending:
            // Unhandled
            return;
    }
}

auto BuildGraph::IsCompleted() const -> bool {
    return std::ranges::count_if(m_dependencyNodesSorted, [](const std::shared_ptr<DependencyNode> &node) -> bool {
        return node->status == DependencyNode::Status::Finished;
    }) == m_dependencyNodesSorted.size();
}
