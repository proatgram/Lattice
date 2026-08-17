export module Lattice.System.Process;

export import std;
export import Lattice.System.Command;

export namespace Lattice::System {
    /**
     * @brief Represents a process on the system.
     *
     * Represents a process on the system that can:
     *  - Be ran
     *  - Be waited for
     *  - Provide all 3 output streams (out, in, err)
     *  - Provide run info (exec time, exit code, Command)
     *  - Be cancelled.
     */
    class Process {
        struct Constructable {};
        public:
            Process(Constructable);
            /**
             * @brief Spawns a process from a Command.
             *
             * @param[in] command The command to execute.
             *
             * @return An expected shared_ptr to the Process on sucess. An error through a string on failure.
             */
            static auto Spawn(const Command &command) -> std::expected<std::shared_ptr<Process>, std::string>;

            /**
             * @brief Gets the process exit code.
             *
             * @return The exit code if the process is finished. Otherwise std::nullopt.
             */
            auto GetExitCode() const -> std::optional<int>;
            /**
             * @brief Obtains the process start time.
             *
             * @return A C++ time_point representing the starting time for the process.
             */
            auto GetStartTime() const -> std::chrono::steady_clock::time_point;
            /**
             * @brief Obtains the process end time if it's finished.
             *
             * @return A C++ time_point representing the ending time for the process if the
             * process is finished. Otherwise std::nullopt.
             */
            auto GetEndTime() const -> std::optional<std::chrono::steady_clock::time_point>;
            /**
             * @brief Obtains the process' execution time if it's finished.
             *
             * @return A C++ duration representing the execution time for the process if the
             * process is finished. Otherwise std::nullopt.
             */
            auto GetExecutionTime() const -> std::optional<std::chrono::steady_clock::duration>;
            /**
             * @brief Gets the Command used to execute this process.
             *
             * @return The Command used to execute this process.
             */
            auto GetCommand() const -> Command;

            /**
             * @brief Waits for the process to finish.
             *
             * By specifying the optional `waitTime` argument, you can provide a
             * timeout to the wait function.
             *
             * @param[in] waitTime An optional C++ duration to Wait for until a timeout happens.
             *
             * @return true if the process finished during the Wait. false otherwise.
             */
            auto Wait(const std::optional<std::chrono::system_clock::duration> waitTime = std::nullopt) const -> bool;
            /**
             * @brief Checks if the Process is finished.
             *
             * @return true if the Process is finished. false otherwise.
             */
            auto IsFinished() const -> bool;
            /**
             * @brief Requests for the Process to cancel.
             * 
             * Requests the polling and execution threads to stop.
             *
             * @return true if the stop request was sent. false otherwise.
             */
            auto RequestCancel() -> bool;

            /**
             * @brief Gets the process stdout
             *
             * @return istream capable of talking to the process' stdout.
             */
            auto GetProcessStdOut() -> std::istream&;
            /**
             * @brief Gets the process stdin
             *
             * @return ostream capable of talking to the process' stdin.
             */
            auto GetProcessStdIn() -> std::ostream&;
            /**
             * @brief Gets the process stderr
             *
             * @return istream capable of talking to the process' stderr.
             */
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
