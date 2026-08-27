module Lattice.Object.Resolver;

import Lattice.Registry;

import Lattice.Object.Capabilities.ProjectIdentifiable;
import Lattice.Object.Capabilities.HasDependencies;

using namespace Lattice::Object;

std::map<std::string, std::shared_ptr<Resolver>> Resolver::s_resolvers;

Resolver::Resolver(Constructable, const Context &resolverContext) : m_resolverContext(resolverContext) {}

auto Resolver::Create(const Context &resolverContext) -> std::shared_ptr<Resolver> {
    if (s_resolvers.contains(resolverContext.identifier)) 
        return s_resolvers.at(resolverContext.identifier);

    std::shared_ptr<Resolver> newResolver = std::make_shared<Resolver>(Constructable(), resolverContext);

    s_resolvers[resolverContext.identifier] = newResolver;

    return newResolver;
}

auto Resolver::TryResolveAll() -> std::expected<void, std::list<std::pair<std::string, std::string>>> {
    std::list<std::pair<std::string, std::string>> unresolvable;

    for (const std::shared_ptr<Resolver> resolver : s_resolvers | std::views::values) {
        if (auto err = resolver->TryResolve(); !err)
            unresolvable.push_back({resolver->m_resolverContext.identifier, err.error()});
    }

    if (unresolvable.size() != 0)
        return std::unexpected(unresolvable);

    return {};
}

auto Resolver::TryResolve() -> std::expected<std::shared_ptr<Object>, std::string> {
    if (m_resolvedObject)
        return m_resolvedObject.value();

    std::string fullIdentifier = m_resolverContext.identifier;
    std::shared_ptr<Object> resolvedObject;

    // Check if the object has '@' pointing to a project
    if (fullIdentifier.contains('@') && fullIdentifier.find_first_of('@') != fullIdentifier.size() - 1) {
        std::string projectId = fullIdentifier.substr(fullIdentifier.find_first_of('@') + 1);
        if (auto project = Registry::GetInstance()->Query<std::shared_ptr<Project>>(projectId).value_or(nullptr); project) {
            std::string rawObjectId = fullIdentifier.substr(0, fullIdentifier.find_first_of('@'));

            if (resolvedObject = project->GetObject(rawObjectId).value_or(nullptr); resolvedObject) {
                m_resolvedObject = resolvedObject;
                return m_resolvedObject.value();
            }
            return std::unexpected{"No object with that ID exists in the requested project search scope."};
        }
        return std::unexpected{"No project with that ID is defined."};
    }

    // If not, check if we have dependee context that we can retrieve a project from.
    if (m_resolverContext.dependee.has_value()) {
        if (auto projectIdentifiable = m_resolverContext.dependee.value()->GetCapability<Capabilities::ProjectIdentifiable>().value_or(nullptr); projectIdentifiable) {
            if (resolvedObject = projectIdentifiable->GetOwningProject()->GetObject(fullIdentifier).value_or(nullptr); resolvedObject) {
                m_resolvedObject = resolvedObject;
                return resolvedObject;
            }
        } else if (auto project = m_resolverContext.dependee.value()->As<Project>().value_or(nullptr); project) {
                if (resolvedObject = project->GetObject(fullIdentifier).value_or(nullptr); resolvedObject) {
                    m_resolvedObject = resolvedObject;
                    return resolvedObject;
                }
        }

    }

    // Check if the object is a project
    if (auto project = Registry::GetInstance()->Query<std::shared_ptr<Project>>(m_resolverContext.identifier).value_or(nullptr); project) {
        m_resolvedObject = project;
        resolvedObject = project;
        return resolvedObject;
    }

    /*
    // TODO: If all else fails, we check to see if it is a global object.
    // (fails from circular dependency)
    if (resolvedObject = Lattice::GetInstance()->GetGlobalObject(fullIdentifier).value_or(nullptr); resolvedObject) {
        m_resolvedObject = resolvedObject;
        return {};
    }
    */

    if (!m_resolvedObject)
        return std::unexpected{"Unable to resolve object from ID."};

    return {};
}

auto Resolver::IsResolved() const -> bool {
    return m_resolvedObject.has_value();
}

auto Resolver::GetResolvedObject() const -> std::shared_ptr<Object> {
    return m_resolvedObject.value_or(nullptr);
}

auto Resolver::GetResolverContext() const -> Context {
    return m_resolverContext;
}
