export module Lattice.Object.Capabilities.ProjectIdentifiable;

export import std;

export import Lattice.Project;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @breaf Capability tying an Object to a project.
     *
     * This capability can be used to associate an object with a project.
     */
    class ProjectIdentifiable : public ICapability {
        public:
            virtual ~ProjectIdentifiable() = default;

            /**
             * @brief Obtains the project that owns the implementing Object.
             *
             * @return A shared_ptr to a Project.
             */
            auto GetOwningProject() const -> std::shared_ptr<Project>;
        protected:
            /**
             * @brief Sets the project that owns the implementing Object.
             */
            auto SetOwningProject(const std::shared_ptr<Project> &owningProject) -> void;

        private:
            std::shared_ptr<Project> m_owningProject;
    };
}  // export namespace Lattice::Object::Capabilities
