export module Lattice.Logger.TextLogger;

import Lattice.Logger.ILogger;

export namespace Lattice::Logger {
    /**
     * @brief Logger implementation that only logs text to stdout.
     */
    class TextLogger : public ILogger {
        public:
            TextLogger(Constructable);

            auto Log(Level level, const std::string &message) -> void override;

        protected:
            auto Update() -> void override;

            struct Message {
                Level level;
                std::string text;
            };

            std::queue<Message> m_messagesQueue;
    };
}
