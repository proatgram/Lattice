module Lattice.Object.Capabilities.ProjectIdentifiable;

using namespace Lattice::Object::Capabilities;

auto ProjectIdentifiable::GetOwningProject() const -> std::shared_ptr<Project> {
    return m_owningProject;
}

auto ProjectIdentifiable::SetOwningProject(const std::shared_ptr<Project> &owningProject) -> void {
    m_owningProject = owningProject;
}
