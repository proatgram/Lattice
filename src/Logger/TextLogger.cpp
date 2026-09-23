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
                std::println("{}", Foreground::White(msg.text));
                break;
            case ILogger::Level::Warn:
                std::println("{}", Foreground::Yellow(msg.text));
                break;
            case ILogger::Level::Error:
                std::println("{}", Foreground::Red(msg.text));
                break;
        }
    }
}
