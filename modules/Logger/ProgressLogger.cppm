export module Lattice.Logger.ProgressLogger;

import Lattice.Logger.ILogger;

export namespace Lattice::Logger {
    class BuildProgress {
        public:
            struct Step {
                std::string Id;
                std::string Description;
                inline auto operator<(const Step &other) {
                    return Id < other.Id;
                }
            };
            struct Object {
                std::string Id;
                std::vector<Step> Steps;
                std::size_t TotalSteps;
                std::size_t CompletedSteps;
                inline auto operator<(const Object &other) {
                    return Id < other.Id;
                }
            };
            struct DrawDescription {
                std::string output;
                std::size_t lines;
                std::size_t previousLineCount;
            };

            auto GetTotalObjects() const -> std::size_t;
            auto SetTotalObjects(std::size_t objectCount) -> void;

            auto GetObjectsDone() const -> std::size_t;
            auto SetObjectsDone(std::size_t objectCount) -> void;

            auto AddObject(Object object) -> bool;
            auto RemoveObject(const std::string &objectId) -> bool;
            auto ContainsObject(const std::string &objectId) const -> bool;
            auto GetObject(const std::string &objectId) const -> std::optional<std::reference_wrapper<const Object>>;
            auto GetObject(const std::string &objectId) -> std::optional<std::reference_wrapper<Object>>;
            
            auto RemoveStep(const std::string &object, const std::string &stepId) -> bool;
            auto AddStep(const std::string &objectId, Step step) -> bool;

            auto ApplyChanges() -> void;

            auto Generate(std::size_t requestedColumnWidth = 0) -> DrawDescription;
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

    class ProgressLogger final : public ILogger {
        public:
            ProgressLogger();

            auto Log(Level level, const std::string &message) -> void final;
            auto Update() -> void final;

            auto GetProgress() const -> std::shared_ptr<BuildProgress>;

        private:
            struct Message {
                Level level;
                std::string text;
            };
            std::queue<Message> m_messagesQueue;
            std::shared_ptr<BuildProgress> m_buildProgress;
    };
}
