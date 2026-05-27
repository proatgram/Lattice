export module Lattice.System.Process;

export import std;
export import Lattice.System.Command;

export namespace Lattice::System {
    class Process {
        struct Constructable {};
        public:
            Process(Constructable);
            static auto Spawn(const Command &command) -> std::expected<std::shared_ptr<Process>, std::string>;

            auto GetExitCode() const -> std::optional<int>;
            auto GetStartTime() const -> std::chrono::steady_clock::time_point;
            auto GetEndTime() const -> std::optional<std::chrono::steady_clock::time_point>;
            auto GetExecutionTime() const -> std::optional<std::chrono::steady_clock::duration>;
            auto GetCommand() const -> Command;

            auto Wait(const std::optional<std::chrono::system_clock::duration> waitTime = std::nullopt) const -> bool;
            auto IsFinished() const -> bool;
            auto RequestCancel() -> bool;

            auto GetProcessStdOut() -> std::istream&;
            auto GetProcessStdIn() -> std::ostream&;
            auto GetProcessStdErr() -> std::istream&;

        private:
            std::jthread m_pollingThread;
            mutable std::mutex m_pollingMutex;
            mutable std::condition_variable m_pollingConditionVariable;
            std::any m_systemHandle;
            std::unique_ptr<std::istream> m_processStdOut;
            std::unique_ptr<std::ostream> m_processStdIn;
            std::unique_ptr<std::istream> m_processStdErr;

            std::atomic_bool m_isFinished;
            std::atomic<std::chrono::steady_clock::duration> m_executionTime;
            std::atomic<std::chrono::steady_clock::time_point> m_startTime;
            std::atomic<std::chrono::steady_clock::time_point> m_endTime;
            std::atomic<int> m_exitCode;

            Command m_command;
    };

}  // export namespace Lattice::System
