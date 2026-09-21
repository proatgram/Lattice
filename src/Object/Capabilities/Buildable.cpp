module Lattice.Object.Capabilities.Buildable;

import Lattice.Object;

using namespace Lattice::Object::Capabilities;


Buildable::StepDescription::StepDescription(const std::string &action, const std::string &name) :
    m_action(action),
    m_name(name) {}

auto Buildable::StepDescription::GetAction() const -> std::string {
    return m_action;
}

auto Buildable::StepDescription::GetName() const -> std::string {
    return m_name;
}

auto Buildable::StepDescription::GetFullDescription() const -> std::string {
    return std::format("{} {}", m_action, m_name);
}

Buildable::BuildStep::BuildStep(Constructable, const std::weak_ptr<Buildable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<BuildStep>> &dependencies) :
    m_state((dependencies.empty() ? State::Ready : State::Pending)),
    m_parentBuildable(parent),
    m_function(stepFunction),
    m_id(stepId),
    m_unfinishedDependencyCount(dependencies.size()),
    m_description(stepDescription),
    m_dependents(),
    m_dependencies(dependencies) {}

auto Buildable::BuildStep::Create(const std::weak_ptr<Buildable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<BuildStep>> &dependencies) -> std::shared_ptr<BuildStep> {
    std::shared_ptr<BuildStep> buildStep = std::make_shared<BuildStep>(Constructable(), parent, stepFunction, stepId, stepDescription, dependencies);

    for (std::shared_ptr<BuildStep> dependency : dependencies) {
        dependency->m_dependents.push_back(buildStep);
    }

    return buildStep;
}

auto Buildable::BuildStep::GetID() const -> const std::string& {
    return m_id;
}

auto Buildable::BuildStep::GetDescription() const -> const StepDescription& {
    return m_description;
}

auto Buildable::BuildStep::Run() -> State {
    if (m_function()) {
        return State::Finished;
    }

    return State::Failed;
}

auto Buildable::BuildStep::GetState() const -> Buildable::BuildStep::State {
    return m_state;
}

auto Buildable::BuildStep::GetDependents() const -> const std::vector<std::shared_ptr<BuildStep>>& {
    return m_dependents;
}

auto Buildable::BuildStep::GetDependencies() const -> const std::vector<std::shared_ptr<BuildStep>>& {
    return m_dependencies;
}

auto Buildable::BuildStep::GetUnfinishedDependencyCount() const -> std::size_t {
    return m_unfinishedDependencyCount;
}

auto Buildable::BuildStep::GetParent() const -> std::weak_ptr<Buildable> {
    return m_parentBuildable;
}

auto Buildable::GetReadySteps(std::size_t max) const -> std::vector<std::shared_ptr<BuildStep>> {
    auto ready = m_buildSteps | std::views::filter([](const std::shared_ptr<BuildStep> &buildStep) -> bool {
        return buildStep->GetState() == BuildStep::State::Ready;
    }) | std::views::take(max);

    return std::ranges::to<std::vector<std::shared_ptr<BuildStep>>>(ready);
}

auto Buildable::GetTotalSteps() const -> std::size_t {
    return m_buildSteps.size();
}

auto Buildable::GetRemainingSteps() const -> std::size_t {
    return std::ranges::count_if(m_buildSteps, [](const std::shared_ptr<BuildStep> &buildStep) -> bool { return buildStep->GetState() != BuildStep::State::Finished; });
}

auto Buildable::UpdateBuiltStep(const std::shared_ptr<BuildStep> &buildStep, BuildStep::State state) -> std::expected<void, BuildStep::State> {
    BuildStep::State expected;
    switch (state) {
        case BuildStep::State::Finished:
            expected = BuildStep::State::Running;
            if (buildStep->m_state.compare_exchange_strong(expected, state)) {
                for (const std::shared_ptr<BuildStep> &dependent : buildStep->GetDependents()) {
                    if (dependent->m_unfinishedDependencyCount.fetch_sub(1) == 1)
                        if (auto err = UpdateBuiltStep(dependent, BuildStep::State::Ready); !err) {
                            throw std::runtime_error("ERROR: Failed to change dependent to ready despite it's unfinished dependency count being 0.");
                        }
                }
                return {};
            }
            return std::unexpected{expected};
        case BuildStep::State::Failed:
            expected = BuildStep::State::Running;
            if (buildStep->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case BuildStep::State::Ready:
            expected = BuildStep::State::Pending;
            if (buildStep->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case BuildStep::State::Running:
            expected = BuildStep::State::Ready;
            if (buildStep->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case BuildStep::State::Pending:
        default:
            return {};
    }
}

auto Buildable::IsBuilt() const -> bool {
    return std::ranges::count_if(m_buildSteps, [](const std::shared_ptr<BuildStep> &buildStep) -> bool {
        return buildStep->GetState() == BuildStep::State::Finished && buildStep->m_unfinishedDependencyCount.load() == 0;
    }) == m_buildSteps.size();
}

auto Buildable::AddStep(const std::shared_ptr<BuildStep> &step) -> void {
    m_buildSteps.push_back(step);
}
