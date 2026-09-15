module Lattice.Logger.EscapeSequences;

using namespace Lattice::Logger::EscapeSequences;

Cursor::MoveUp::MoveUp(std::size_t lines) : m_lines(lines) {}
auto Cursor::MoveUp::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << std::format("\033[{}A", m_lines);
    return os;
}
Cursor::MoveDown::MoveDown(std::size_t lines) : m_lines(lines) {}
auto Cursor::MoveDown::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << std::format("\033[{}B", m_lines);
    return os;
}
Cursor::MoveLeft::MoveLeft(std::size_t lines) : m_lines(lines) {}
auto Cursor::MoveLeft::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << std::format("\033[{}D", m_lines);
    return os;
}
Cursor::MoveRight::MoveRight(std::size_t lines) : m_lines(lines) {}
auto Cursor::MoveRight::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << std::format("\033[{}C", m_lines);
    return os;
}
Cursor::MoveToColumn::MoveToColumn(std::size_t column) : m_column(column) {}
auto Cursor::MoveToColumn::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << std::format("\033[{}G", m_column);
    return os;
}

auto Cursor::SaveCursorPosition::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << "\033[s";
    return os;
}
auto Cursor::RestoreCursorPosition::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    os << "\033[u";
    return os;
}

Clearing::Clear::Clear(Clearing::Clear::Where where) : m_where(where) {}

auto Clearing::Clear::ApplyEscapeSequence(std::ostream &os) const -> std::ostream& {
    switch (m_where) {
        case Where::CursorToEnd:
            os << "\033[0K";
            break;
        case Where::StartToCursor:
            os << "\033[1K";
            break;
        case Where::EntireLine:
            os << "\033[2K";
            break;
        case Where::CursorToEndScreen:
            os << "\033[0J";
            break;
        case Where::CursorToBeginningScreen:
            os << "\033[1J";
            break;
        case Where::EntireScreen:
            os << "\033[2J";
            break;
        case Where::EntireScreenAndBuffer:
            os << "\033[3J";
            break;
        default:
            break;
    }

    return os;
}
