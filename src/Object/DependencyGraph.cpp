module Lattice.Object.DependencyGraph;

import Lattice.Object.Capabilities.HasDependencies;
import Lattice.Object.Capabilities.Cacheable;
import Lattice.Object.Capabilities.Schedulable;
import Lattice.Logger.ILogger;
import Lattice.Logger.TextLogger;
import Lattice.Project;
import Lattice.Registry;

using namespace Lattice::Object;

// Goes throughand checks if this object and any other potential dependencies needs to be rebuilt.
auto DependencyGraph::RecursiveDependencyGraph(const std::shared_ptr<Resolver> &objectResolver, std::map<std::string, std::shared_ptr<DependencyGraph::DependencyNode>> &currentGraph, const std::optional<std::shared_ptr<DependencyNode>> &dependee) -> bool {
    std::shared_ptr<Logger::TextLogger> textLogger = std::dynamic_pointer_cast<Logger::TextLogger>(Logger::ILogger::GetDefault());
    std::shared_ptr<DependencyNode> dependencyNode;
    if (!objectResolver->IsResolved() && !objectResolver->TryResolve().has_value())
        throw std::runtime_error(std::format("Unable to generate dependency graph: {} is unable to be resolved.", objectResolver->GetResolverContext().identifier));
    if (!objectResolver->GetResolvedObject()->GetCapability<Capabilities::Schedulable>())
        throw std::runtime_error(std::format("Unable to generate dependency graph: {} is not Schedulable.", objectResolver->GetResolvedObject()->GetIdentifier()));


    if (auto it = currentGraph.find(objectResolver->GetResolvedObject()->GetIdentifier()); it != currentGraph.end())
        dependencyNode = it->second;
    else
        dependencyNode = std::make_shared<DependencyNode>(objectResolver, std::list<std::shared_ptr<DependencyNode>>{}, 0, DependencyNode::Status::Pending);

    if (auto hasDependencies = objectResolver->GetResolvedObject()->GetCapability<Capabilities::HasDependencies>().value_or(nullptr); hasDependencies) {
        if (!hasDependencies->GetAllDependencies().empty()) {
            for (const Capabilities::HasDependencies::Dependency &dependency : hasDependencies->GetAllDependencies()) {
                if (RecursiveDependencyGraph(dependency.resolver, currentGraph, dependencyNode))
                    dependencyNode->dependencyCount++;
            }
        }
    } else if (auto project = objectResolver->GetResolvedObject()->As<Project>().value_or(nullptr); project) {
        for (const auto &[id, object] : project->GetObjects()) {
            if (RecursiveDependencyGraph(Resolver::Create({.identifier = object->GetIdentifier(), .dependee = project}), currentGraph, dependencyNode))
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

DependencyGraph::DependencyGraph(Constructable, const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers) {
    std::map<std::string, std::shared_ptr<DependencyGraph::DependencyNode>> tree;

    if (objectResolvers) {
        for (const std::shared_ptr<Resolver> &objectResolver : objectResolvers.value()) {
            RecursiveDependencyGraph(objectResolver, tree);
        }
    } else {
        for (const std::shared_ptr<Project> &project : Registry::GetInstance()->All<std::shared_ptr<Project>>()) {
            RecursiveDependencyGraph(Resolver::Create({
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

auto DependencyGraph::Generate(const std::optional<std::list<std::shared_ptr<Resolver>>> &objectResolvers) -> std::shared_ptr<DependencyGraph> {
    return std::make_shared<DependencyGraph>(Constructable{}, objectResolvers);
}

auto DependencyGraph::GetReady() const -> std::list<std::shared_ptr<DependencyNode>> {
    std::list<std::shared_ptr<DependencyNode>> readyNodes;

    for (const std::shared_ptr<DependencyNode> &dependencyNode : m_dependencyNodesSorted) {
        if (dependencyNode->status != DependencyNode::Status::Ready)
            break;

        readyNodes.push_back(dependencyNode);
    }

    return readyNodes;
}

auto DependencyGraph::GetTotalObjects() const -> std::size_t {
    return m_dependencyNodesSorted.size();
}

auto DependencyGraph::Update(const std::shared_ptr<DependencyNode> &node) -> void {
    switch (node->status) {
        case DependencyNode::Status::Finished:
            for (const std::shared_ptr<DependencyNode> &dependent : node->dependents) {
                dependent->dependencyCount--;
                if (dependent->dependencyCount == 0)
                    dependent->status = DependencyNode::Status::Ready;
            }
        case DependencyNode::Status::Ready:
        case DependencyNode::Status::Running:
        case DependencyNode::Status::Failed:
        case DependencyNode::Status::Pending:
        default:
            std::ranges::partition(m_dependencyNodesSorted, [](const std::shared_ptr<DependencyNode> &node) {
                return node->status == DependencyNode::Status::Ready;
            });
            return;
    }
}

auto DependencyGraph::IsCompleted() const -> bool {
    return std::ranges::count_if(m_dependencyNodesSorted, [](const std::shared_ptr<DependencyNode> &node) -> bool {
        return node->status == DependencyNode::Status::Finished;
    }) == m_dependencyNodesSorted.size();
}
