export module Lattice.Logger.TextLogger;

import Lattice.Logger.ILogger;

export namespace Lattice::Logger {
    /**
     * @brief Logger implementation that only logs text to stdout.
     */
    class TextLogger final : public ILogger {
        public:
            TextLogger(Constructable);

            auto Log(Level level, const std::string &message) -> void final;

        private:
            auto Update() -> void final;

            struct Message {
                Level level;
                std::string text;
            };

            std::queue<Message> m_messagesQueue;
    };
}
