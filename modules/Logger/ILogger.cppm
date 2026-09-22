export module Lattice.Logger.ILogger;

export import std;

export namespace Lattice::Logger {
    class ILogger {
        public:
            enum class Level {
                Info,
                Warn,
                Error
            };

            virtual ~ILogger();

            virtual auto Log(Level level, const std::string &message) -> void = 0;
            virtual auto Update() -> void = 0;

            auto Info(const std::string &message) -> void;
            auto Warn(const std::string &message) -> void;
            auto Error(const std::string &message) -> void;
        protected:
            auto StartLoggingThread() -> void;
            std::mutex m_mutex;

        private:
            std::jthread m_loggingThread;
    };
}
