export module Lattice.Logger.EscapeSequences;

export import std;

export namespace Lattice::Logger::EscapeSequences {
    class IEscapeSequence {
        public:
            ~IEscapeSequence() = default;

            friend inline auto operator<<(std::ostream  &os, const IEscapeSequence &escapeSequence) -> std::ostream& {
                return escapeSequence.ApplyEscapeSequence(os);
            }

        protected:
            virtual auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& = 0;
    };

    template <auto Code> requires std::is_convertible_v<decltype(Code), unsigned short int>
    class TerminalStyle final : public IEscapeSequence {
        public:
            inline constexpr auto operator()(const std::string &data) const -> std::string {
                return std::format("\033[{}m{}\033[0m", Code, data);
            }

        private:
            inline constexpr auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final {
                os << std::format("\033[{}m", Code);
                return os;
            }
    };

    namespace Cursor {
        class MoveUp final : public IEscapeSequence {
            public:
                MoveUp(std::size_t lines = 1);

            private:
                std::size_t m_lines;

                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class MoveDown final : public IEscapeSequence {
            public:
                MoveDown(std::size_t lines = 1);

            private:
                std::size_t m_lines;

                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class MoveLeft final : public IEscapeSequence {
            public:
                MoveLeft(std::size_t lines = 1);

            private:
                std::size_t m_lines;

                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class MoveRight final : public IEscapeSequence {
            public:
                MoveRight(std::size_t lines = 1);

            private:
                std::size_t m_lines;

                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class MoveToColumn final : public IEscapeSequence {
            public:
                MoveToColumn(std::size_t column);

            private:
                std::size_t m_column;

                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class SaveCursorPosition final : public IEscapeSequence {
            private:
                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
        class RestoreCursorPosition final : public IEscapeSequence {
            private:
                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;
        };
    }

    namespace Clearing {
        class Clear : public IEscapeSequence {
            public:
                enum class Where {
                    CursorToEnd,
                    StartToCursor,
                    EntireLine,
                    CursorToEndScreen,
                    CursorToBeginningScreen,
                    EntireScreen,
                    EntireScreenAndBuffer
                };

                Clear(Where where = Where::EntireLine);

            private:
                auto ApplyEscapeSequence(std::ostream &os) const -> std::ostream& final;

                Where m_where;
        };
    }

    namespace Colors {
        namespace Foreground {
            inline constexpr auto Black = TerminalStyle<30>();
            inline constexpr auto Red = TerminalStyle<31>();
            inline constexpr auto Green = TerminalStyle<32>();
            inline constexpr auto Yellow = TerminalStyle<33>();
            inline constexpr auto Blue = TerminalStyle<34>();
            inline constexpr auto Magenta = TerminalStyle<35>();
            inline constexpr auto Cyan = TerminalStyle<36>();
            inline constexpr auto White = TerminalStyle<37>();
        }
        namespace Background {
            inline constexpr auto Black = TerminalStyle<40>();
            inline constexpr auto Red = TerminalStyle<41>();
            inline constexpr auto Green = TerminalStyle<42>();
            inline constexpr auto Yellow = TerminalStyle<43>();
            inline constexpr auto Blue = TerminalStyle<44>();
            inline constexpr auto Magenta = TerminalStyle<45>();
            inline constexpr auto Cyan = TerminalStyle<46>();
            inline constexpr auto White = TerminalStyle<47>();
        }
    }
    namespace Style {
        inline constexpr auto Reset = TerminalStyle<0>();
        inline constexpr auto Bold = TerminalStyle<1>();
        inline constexpr auto Underline = TerminalStyle<4>();
        inline constexpr auto Blink = TerminalStyle<5>();
        inline constexpr auto Inverse = TerminalStyle<7>();
    }
}
