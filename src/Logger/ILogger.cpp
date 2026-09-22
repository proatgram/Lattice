module Lattice.Logger.ILogger;

using namespace Lattice::Logger;

ILogger::~ILogger() {
    if (m_loggingThread.joinable()) {
        m_loggingThread.request_stop();
        m_loggingThread.join();
    }
}

auto ILogger::StartLoggingThread() -> void {
    m_loggingThread = std::jthread([this](const std::stop_token &stop) -> void {
        while (!stop.stop_requested()) {
            Update();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    });
}

auto ILogger::Info(const std::string &message) -> void {
    Log(Level::Info, message);
}

auto ILogger::Warn(const std::string &message) -> void {
    Log(Level::Warn, message);
}

auto ILogger::Error(const std::string &message) -> void {
    Log(Level::Error, message);
}
