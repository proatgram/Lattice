module Lattice.Object.Capabilities.HasDependencies;

import Lattice.Object.Capabilities.ProjectIdentifiable;

using namespace Lattice::Object::Capabilities;

auto HasDependencies::GetPublicDependencies() const -> std::list<std::shared_ptr<Object>> {
    return std::ranges::to<std::list<std::shared_ptr<Object>>>(
        m_dependencies | std::views::filter([](const Dependency &dep) -> bool {
            return dep.visibility == Visibility::Public;
        })
        | std::views::transform([](const Dependency &dep) -> auto {
            if (!dep.resolver->IsResolved())
                throw std::runtime_error(std::format("Dependency {} is not resolved. This is a bug and should not happen. (where: {})", dep.resolver->GetResolverContext().identifier, __PRETTY_FUNCTION__));

            return dep.resolver->GetResolvedObject();
        })
    );
}

auto HasDependencies::GetPrivateDependencies() const -> std::list<std::shared_ptr<Object>> {
    return std::ranges::to<std::list<std::shared_ptr<Object>>>(
        m_dependencies | std::views::filter([](const Dependency &dep) -> bool {
            return dep.visibility == Visibility::Private;
        })
        | std::views::transform([](const Dependency &dep) -> auto {
            if (!dep.resolver->IsResolved())
                throw std::runtime_error(std::format("Dependency {} is not resolved. This is a bug and should not happen. (where: {})", dep.resolver->GetResolverContext().identifier, __PRETTY_FUNCTION__));

            return dep.resolver->GetResolvedObject();
        })
    );
}

auto HasDependencies::GetAllDependencies() const -> std::list<Dependency> {
    return m_dependencies;
}

auto HasDependencies::AddDependency(const Dependency &dependency) -> void {
    m_dependencies.push_back(dependency);
}
