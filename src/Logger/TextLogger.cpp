module Lattice.Logger.TextLogger;

import Lattice.Logger.EscapeSequences;

using namespace Lattice::Logger;
using namespace EscapeSequences::Colors;

TextLogger::TextLogger(ILogger::Constructable) {}

auto TextLogger::Log(Level level, const std::string &message) -> void {
    EnsureLoggingThreadStarted();
    std::unique_lock lock(m_mutex);
    m_messagesQueue.push({level, message});
}

auto TextLogger::Update() -> void {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (!m_messagesQueue.empty()) {
        Message msg = std::move(m_messagesQueue.front());
        m_messagesQueue.pop();
        switch (msg.level) {
            case ILogger::Level::Info:
                std::cout << Foreground::White(msg.text) << std::endl;
                break;
            case ILogger::Level::Warn:
                std::cout << Foreground::Yellow(msg.text) << std::endl;
                break;
            case ILogger::Level::Error:
                std::cerr << Foreground::Red(msg.text) << std::endl;
                break;
        }
    }
}
