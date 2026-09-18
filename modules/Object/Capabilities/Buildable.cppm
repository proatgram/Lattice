export module Lattice.Object.Capabilities.Buildable;

export import std;

import Lattice.Object.Capabilities.ICapability;

export namespace Lattice::Object::Capabilities {
    /**
     * @brief Provides a base class for objects that can be built.
     *
     * This class defines methods and processes to create build steps
     * for objects that can be built, a way to query which steps can
     * be built, and a way to run said steps.
     *
     */
    class Buildable : public ICapability {
        public:
            /**
             * @brief Represents a Build Steps description.
             *
             * Essentially, the action done onto name.
             */
            class StepDescription {
                public:
                    /**
                     * @brief Constructs a new `StepDescription` instance.
                     *
                     * @param[in] action The action verb for the description.
                     * @param[in] name The name or reciever for the action.
                     */
                    StepDescription(const std::string &action, const std::string &name);

                    /**
                     * @brief Gets the action verb string.
                     *
                     * @return std::string containing the action verb.
                     */
                    auto GetAction() const -> std::string;
                    /**
                     * @brief Gets the action name or reciever for the action.
                     *
                     * @return std::string containing the name or receiver.
                     */
                    auto GetName() const -> std::string;

                    /**
                     * @brief Gets a formatted string containing the action and reciever.
                     *
                     * @return std::string containing a formatted string.
                     */
                    auto GetFullDescription() const -> std::string;

                private:
                    std::string m_action;
                    std::string m_name;
            };

            /**
             * @brief A class representing a step in the build process for a `Buildable` object.
             *
             * A `Buildable` object can have any number of `BuildStep`'s. Each build step defined can
             * depend on one or more other `BuildStep`'s in the `Buildable`.
             *
             * Additionally, a `BuildStep` has a `State` associated with it describing it's current
             * progress or result.
             */
            class BuildStep {
                struct Constructable{};
                public:
                    /**
                     * @brief Enum containing the state of the step.
                     * 
                     * Below is a description of the values contained in the `State`:
                     *  - Pending: The step has dependencies that are or need to be ran before it can run.
                     *  - Ready: The step has no unfinished dependencies and can be ran.
                     *  - Running: The step is currently running.
                     *  - Finished: The step finished running successfully.
                     *  - Failed: The step failed to run successfully.
                     */
                    enum class State {
                        Pending,
                        Ready,
                        Running,
                        Finished,
                        Failed
                    };

                    BuildStep(Constructable, const std::weak_ptr<Buildable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<BuildStep>> &dependencies = {});

                    /**
                     * @brief Creates a new `BuildStep`.
                     *
                     * @param[in] parent The parent `Buildable` that owns the step.
                     * @param[in] stepFunction The function that will get run when this step gets ran.
                     * @param[in] stepId An identifier to refer to this step by.
                     * @param[in] stepDescription A `StepDescription` describing the step.
                     * @param[in] dependencies A set of dependencies that this step depends on before it can run.
                     *
                     * @return A newly created `BuildStep` encased in a `std::shared_ptr`.
                     */
                    static auto Create(const std::weak_ptr<Buildable> &parent, const std::function<bool(void)> &stepFunction, const std::string &stepId, const StepDescription &stepDescription, const std::vector<std::shared_ptr<BuildStep>> &dependencies = {}) -> std::shared_ptr<BuildStep>;

                    /**
                     * @brief Gets the step ID.
                     *
                     * @return The step ID.
                     */
                    auto GetID() const -> const std::string&;
                    /**
                     * @brief Gets the step decription.
                     *
                     * @return The step description.
                     */
                    auto GetDescription() const -> const StepDescription&;

                    /**
                     * @brief Runs the step.
                     *
                     * If the step hasn't started running yet, then it will start running
                     * and set it's state to `State::Running`. If it is already running, or
                     * has already ran, it will return the `State` it finished with.
                     *
                     * Once the step finishes, it will update its state according to if it
                     * failed or succeeded, and return said `State` back.
                     *
                     * @return The ending state of the step.
                     */
                    auto Run() -> State;
                    /**
                     * @brief Gets the current state of the step.
                     *
                     * @return The current `State` of the step.
                     */
                    auto GetState() const -> State;

                    /**
                     * @brief Gets the steps that depend on this step.
                     *
                     * @return A vector to the dependents of this step.
                     */
                    auto GetDependents() const -> const std::vector<std::shared_ptr<BuildStep>>&;
                    /**
                     * @brief Gets the steps that this step depends on.
                     *
                     * @return The dependencies of this step.
                     */
                    auto GetDependencies() const -> const std::vector<std::shared_ptr<BuildStep>>&;
                    /**
                     * @brief Gets the current count of dependencies that haven't finished.
                     *
                     * This count represents the number of dependencies that this object depends on
                     * and that haven't been successfully ran yet. Until this count reaches zero,
                     * this step will not be ready.
                     *
                     * @return The count of unfinished dependency steps.
                     */
                    auto GetUnfinishedDependencyCount() const -> std::size_t;
                    /**
                     * @brief Gets the parent `Buildable` that owns this step.
                     *
                     * @return The parent `Buildable` that owns this step.
                     */
                    auto GetParent() const -> std::weak_ptr<Buildable>;

                private:
                    std::atomic<State> m_state;

                    std::weak_ptr<Buildable> m_parentBuildable;
                    std::function<bool(void)> m_function;
                    std::string m_id;

                    std::atomic<std::size_t> m_unfinishedDependencyCount;

                    StepDescription m_description;
                    std::vector<std::shared_ptr<BuildStep>> m_dependents;
                    std::vector<std::shared_ptr<BuildStep>> m_dependencies;
            };

            virtual ~Buildable() = default;

            /**
             * @brief Gets the `BuildStep`'s that are currently ready to be ran.
             *
             * This function will get up to `max` `BuildStep`'s that are ready to be ran.
             *
             * @return Up to `max` build steps that are ready.
             */
            auto GetReadySteps(std::size_t max = 1) const -> std::vector<std::shared_ptr<BuildStep>>;
            /**
             * @brief Gets the total number of steps this build has.
             *
             * @return The total build steps.
             */
            auto GetTotalSteps() const -> std::size_t;
            /**
             * @brief Gets the remaining steps that haven't ran.
             *
             * @return The remaining steps that haven't ran yet.
             */
            auto GetRemainingSteps() const -> std::size_t;

            /**
             * @brief Checks if the `Buildable` object has been fully built.
             *
             * @return `true` if it's been fully built, otherwise `false`.
             */
            auto IsBuilt() const -> bool;

            /**
             * @brief Configures the `Buildable` object.
             *
             * This function should be overridden in order to configure
             * all neccessary things that this object needs in order to
             * be ready to be built. E.g., setting up and adding build steps.
             */
            virtual auto Configure() -> void = 0;

        protected:
            /**
             * @brief Adds a `BuildStep` to the `Buildable`.
             *
             * @param[in] step The `BuildStep` to add.
             */
            auto AddStep(const std::shared_ptr<BuildStep> &step) -> void;

        private:
            std::vector<std::shared_ptr<BuildStep>> m_buildSteps;
    };
}  // export namespace Lattice::Object::Capabilities
