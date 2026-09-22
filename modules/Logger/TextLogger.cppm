export module Lattice.Logger.TextLogger;

import Lattice.Logger.ILogger;

export namespace Lattice::Logger {
    class TextLogger final : public ILogger {
        public:
            TextLogger();

            auto Log(Level level, const std::string &message) -> void final;

            auto Update() -> void final;

        private:
            struct Message {
                Level level;
                std::string text;
            };

            std::queue<Message> m_messagesQueue;
    };
}
