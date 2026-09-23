export module Lattice.Logger.ILogger;

export import std;

export namespace Lattice::Logger {
    /**
     * @brief Represents an abstract logger.
     *
     * This class can be inherited to create a logger type that can be logged to.
     *
     * The logger includes a logging thread, and a mutex to guarentee thread safety.
     * The logging thread calls `Update()` which can be implemented in whatever way,
     * but is intended to sync the log to the log medium.
     */
    class ILogger {
        protected:
            struct Constructable{};

        public:
            /**
             * @brief Represents the log level.
             */
            enum class Level {
                Info,
                Warn,
                Error
            };

            ILogger();
            virtual ~ILogger();

            /**
             * @brief Creates a new logger instance if it is possible.
             *
             * @tparam T A type with a base of ILogger and can be constructed with Constructable, to be created.
             *
             * @return An optional shared pointer to the newly constructed `T` logger instance.
             */
            template <class T> requires std::is_base_of_v<ILogger, T> && std::is_constructible_v<T, Constructable>
            static inline auto Create() -> std::optional<std::shared_ptr<T>> {
                if (!CanCreate())
                    return {};

                return std::make_shared<T>(Constructable{});
            }

            /**
             * @brief Gets the default logger in the registry.
             *
             * This is only a helper function, the default logger
             * exists in the registry by default as "default" under
             * std::shared_ptr<ILogger>.
             *
             * @return A shared pointer to the default logger.
             */
            static auto GetDefault() -> std::shared_ptr<ILogger>;

            /**
             * @brief Logs the given message with the given level.
             *
             * @param[in] level The level to log the message as.
             * @param[in] message The message to log.
             */
            virtual auto Log(Level level, const std::string &message) -> void = 0;

            /**
             * @brief Logs the given message as `Level::Info`.
             *
             * @param[in] message The message to log.
             */
            auto Info(const std::string &message) -> void;
            /**
             * @brief Logs the given message as `Level::Warn`.
             *
             * @param[in] message The message to log.
             */
            auto Warn(const std::string &message) -> void;
            /**
             * @brief Logs the given message as `Level::Error`.
             *
             * @param[in] message The message to log.
             */
            auto Error(const std::string &message) -> void;
        protected:
            auto EnsureLoggingThreadStarted() -> void;
            std::mutex m_mutex;
            std::once_flag m_startFlag;

        private:
            static auto CanCreate() -> bool;
            virtual auto Update() -> void = 0;
            static inline bool s_canCreate{true};
            std::jthread m_loggingThread;
    };
}
