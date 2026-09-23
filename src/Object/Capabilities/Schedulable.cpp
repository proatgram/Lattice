module Lattice.Object.Capabilities.Schedulable;

import Lattice.Object;

using namespace Lattice::Object::Capabilities;


Schedulable::StepDescription::StepDescription(const std::string &action, const std::string &name) :
    m_action(action),
    m_name(name) {}

auto Schedulable::StepDescription::GetAction() const -> std::string {
    return m_action;
}

auto Schedulable::StepDescription::GetName() const -> std::string {
    return m_name;
}

auto Schedulable::StepDescription::GetFullDescription() const -> std::string {
    return std::format("{} {}", m_action, m_name);
}

Schedulable::Step::Step(Constructable, const std::weak_ptr<Schedulable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<Step>> &dependencies) :
    m_state((dependencies.empty() ? State::Ready : State::Pending)),
    m_parentSchedulable(parent),
    m_function(stepFunction),
    m_id(stepId),
    m_unfinishedDependencyCount(dependencies.size()),
    m_description(stepDescription),
    m_dependents(),
    m_dependencies(dependencies) {}

auto Schedulable::Step::Create(const std::weak_ptr<Schedulable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<Step>> &dependencies) -> std::shared_ptr<Step> {
    std::shared_ptr<Step> step = std::make_shared<Step>(Constructable(), parent, stepFunction, stepId, stepDescription, dependencies);

    for (std::shared_ptr<Step> dependency : dependencies) {
        dependency->m_dependents.push_back(step);
    }

    return step;
}

auto Schedulable::Step::GetID() const -> const std::string& {
    return m_id;
}

auto Schedulable::Step::GetDescription() const -> const StepDescription& {
    return m_description;
}

auto Schedulable::Step::Run() -> State {
    if (m_function()) {
        return State::Finished;
    }

    return State::Failed;
}

auto Schedulable::Step::GetState() const -> Schedulable::Step::State {
    return m_state;
}

auto Schedulable::Step::GetDependents() const -> const std::vector<std::shared_ptr<Step>>& {
    return m_dependents;
}

auto Schedulable::Step::GetDependencies() const -> const std::vector<std::shared_ptr<Step>>& {
    return m_dependencies;
}

auto Schedulable::Step::GetUnfinishedDependencyCount() const -> std::size_t {
    return m_unfinishedDependencyCount;
}

auto Schedulable::Step::GetParent() const -> std::weak_ptr<Schedulable> {
    return m_parentSchedulable;
}

auto Schedulable::GetReadySteps(std::size_t max) const -> std::vector<std::shared_ptr<Step>> {
    auto ready = m_steps | std::views::filter([](const std::shared_ptr<Step> &step) -> bool {
        return step->GetState() == Step::State::Ready;
    }) | std::views::take(max);

    return std::ranges::to<std::vector<std::shared_ptr<Step>>>(ready);
}

auto Schedulable::GetTotalSteps() const -> std::size_t {
    return m_steps.size();
}

auto Schedulable::GetRemainingSteps() const -> std::size_t {
    return std::ranges::count_if(m_steps, [](const std::shared_ptr<Step> &step) -> bool { return step->GetState() != Step::State::Finished; });
}

auto Schedulable::UpdateStep(const std::shared_ptr<Step> &step, Step::State state) -> std::expected<void, Step::State> {
    Step::State expected;
    switch (state) {
        case Step::State::Finished:
            expected = Step::State::Running;
            if (step->m_state.compare_exchange_strong(expected, state)) {
                for (const std::shared_ptr<Step> &dependent : step->GetDependents()) {
                    if (dependent->m_unfinishedDependencyCount.fetch_sub(1) == 1)
                        if (auto err = UpdateStep(dependent, Step::State::Ready); !err) {
                            throw std::runtime_error("ERROR: Failed to change dependent to ready despite it's unfinished dependency count being 0.");
                        }
                }
                return {};
            }
            return std::unexpected{expected};
        case Step::State::Failed:
            expected = Step::State::Running;
            if (step->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case Step::State::Ready:
            expected = Step::State::Pending;
            if (step->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case Step::State::Running:
            expected = Step::State::Ready;
            if (step->m_state.compare_exchange_strong(expected, state))
                return {};
            return std::unexpected{expected};
        case Step::State::Pending:
        default:
            return {};
    }
}

auto Schedulable::IsComplete() const -> bool {
    return std::ranges::count_if(m_steps, [](const std::shared_ptr<Step> &step) -> bool {
        return step->GetState() == Step::State::Finished && step->m_unfinishedDependencyCount.load() == 0;
    }) == m_steps.size();
}

auto Schedulable::AddStep(const std::shared_ptr<Step> &step) -> void {
    m_steps.push_back(step);
}
