module Lattice.Logger.ILogger;

import Lattice.Registry;

using namespace Lattice::Logger;

ILogger::ILogger() {
    s_canCreate = false;
}

ILogger::~ILogger() {
    if (m_loggingThread.joinable()) {
        m_loggingThread.request_stop();
        m_loggingThread.join();
    }

    s_canCreate = true;
}

auto ILogger::GetDefault() -> std::shared_ptr<ILogger> {
    return Registry::GetInstance()->Query<std::shared_ptr<ILogger>>("default").value_or(nullptr);
}

auto ILogger::EnsureLoggingThreadStarted() -> void {
    std::call_once(m_startFlag, [this] {
        m_loggingThread = std::jthread([this](const std::stop_token &stop) {
            while (!stop.stop_requested()) {
                Update();
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        });
    });
}

auto ILogger::CanCreate() -> bool {
    return s_canCreate;
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
