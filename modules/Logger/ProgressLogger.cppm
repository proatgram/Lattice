export module Lattice.Logger.ProgressLogger;

import Lattice.Logger.ILogger;
import Lattice.Logger.TextLogger;

export namespace Lattice::Logger {
    /**
     * @brief Logger implementation that can also log build progress.
     *
     * The progress portion of this logger is transactional.
     * That is, the changes you make to it aren't used in the
     * `Generate()` function unless you apply them. Applying
     * the changes is done through a thread-safe manner.
     */
    class BuildProgress {
        public:
            /**
             * @brief Abstraction over a build step.
             */
            struct Step {
                std::string Id;
                std::string Description;
                inline auto operator<(const Step &other) {
                    return Id < other.Id;
                }
            };
            /**
             * @brief Abstraction over an object.
             */
            struct Object {
                std::string Id;
                std::vector<Step> Steps;
                std::size_t TotalSteps;
                std::size_t CompletedSteps;
                inline auto operator<(const Object &other) {
                    return Id < other.Id;
                }
            };
            /**
             * @brief Describes a drawing that is drawn to a persistent part of the terminal.
             */
            struct DrawDescription {
                std::string output;
                std::size_t lines;
                std::size_t previousLineCount;
            };

            /**
             * @brief Gets the total number of objects.
             *
             * @return The total number of objects.
             */
            auto GetTotalObjects() const -> std::size_t;
            /**
             * @brief Sets the total number of objects.
             *
             * @param[in] objectCount The new total number of objects.
             */
            auto SetTotalObjects(std::size_t objectCount) -> void;

            /**
             * @brief Gets the count of finished objects.
             *
             * @return The count of finished objects.
             */
            auto GetObjectsDone() const -> std::size_t;
            /**
             * @brief Sets the number of objects that are done.
             *
             * @param[in] objectCount The number of objects that are done.
             */
            auto SetObjectsDone(std::size_t objectCount) -> void;

            /**
             * @brief Adds an object to the progress bar.
             *
             * @param[in] object The object to add.
             */
            auto AddObject(Object object) -> bool;
            /**
             * @brief Removes an object from the progress bar.
             *
             * @param[in] objectId The ID of the object to remove.
             *
             * @return `true` if the object was removed, `false` otherwise.
             */
            auto RemoveObject(const std::string &objectId) -> bool;
            /**
             * @brief Checks if the progress bar has an object.
             *
             * @param[in] objectId The ID of the object to look for.
             *
             * @return `true` if the object exists, `false` otherwise.
             */
            auto ContainsObject(const std::string &objectId) const -> bool;
            /**
             * @brief Gets an object by its ID.
             *
             * @param[in] objectId The ID of the object to obtain.
             *
             * @return An optional reference wrapper to a const `Object` if the object exists, otherwise std::nullopt.
             */
            auto GetObject(const std::string &objectId) const -> std::optional<std::reference_wrapper<const Object>>;
            /**
             * @brief Gets an object by its ID.
             *
             * @param[in] objectId The ID of the object to obtain.
             *
             * @return An optional reference wrapper to an `Object` if the object exists, otherwise std::nullopt.
             */
            auto GetObject(const std::string &objectId) -> std::optional<std::reference_wrapper<Object>>;
            
            /**
             * @brief Removes a step from an object.
             *
             * @param[in] objectId The ID for the object the step is a part of.
             * @param[in] stepId The ID for the step to remove.
             *
             * @return `true` if the step was removed, `false` otherwise.
             */
            auto RemoveStep(const std::string &objectId, const std::string &stepId) -> bool;
            /**
             * @brief Adds a step to an object.
             *
             * @param[in] objectId The ID for the object the step is a part of.
             * @param[in] step The step to add.
             *
             * @return `true` if the step was added, `false` otherwise.
             */
            auto AddStep(const std::string &objectId, Step step) -> bool;

            /**
             * @brief Applies the current changes to the generate function.
             *
             * This function is thread safe and protected using locks and a mutex.
             */
            auto ApplyChanges() -> void;

            /**
             * @brief Generates the description for the progress bar.
             *
             * @param[in] requestedColumnWidth A column width to request.
             * The `requestedColumnWidth` parameter will try draw the header and progress
             * bar such that it takes up that much column space. If that requirement can't
             * be met, you will be left with a bar that has 1 progress character, along with
             * the rest of the header material.
             *
             * @return An optional `DrawDescription` containing the description of the progress bar, if one can be made.
             */
            auto Generate(std::size_t requestedColumnWidth = 0) -> std::optional<DrawDescription>;
        private:
            struct Transaction {
                std::size_t totalObjects;
                std::size_t currentObjectsDone;
                std::vector<Object> objects;
            };

            std::size_t m_currentLineCount{};
            Transaction m_currentTransaction;
            Transaction m_temporaryTransaction;
            std::mutex m_mutex;
    };

    class ProgressLogger final : public TextLogger {
        public:
            ProgressLogger(Constructable);
            virtual ~ProgressLogger();

            /**
             * @brief Gets the progress bar instance for this logger.
             *
             * @return A shared pointer to the `BuildProgress` instance associated with this logger.
             */
            auto GetProgress() -> std::shared_ptr<BuildProgress>;

        private:
            auto Update() -> void final;
            std::condition_variable m_finish;

            std::shared_ptr<BuildProgress> m_buildProgress;
    };
}
