module;

#ifdef __linux__

#include <sys/ioctl.h>
#include <unistd.h>

#endif

module Lattice.Logger.ProgressLogger;

import Lattice.Logger.EscapeSequences;

using namespace Lattice::Logger;
using namespace Lattice::Logger::EscapeSequences::Colors;
using namespace Lattice::Logger::EscapeSequences::Cursor;
using namespace Lattice::Logger::EscapeSequences::Clearing;

auto SchedulableProgress::GetTotalObjects() const -> std::size_t {
    std::unique_lock<std::mutex> lock(m_changesMutex);
    return m_temporaryTransaction.totalObjects;
}
auto SchedulableProgress::SetTotalObjects(std::size_t objectCount) -> void {
    std::unique_lock<std::mutex> lock(m_changesMutex);
    m_temporaryTransaction.totalObjects = objectCount;
}
auto SchedulableProgress::GetObjectsDone() const -> std::size_t {
    std::unique_lock<std::mutex> lock(m_changesMutex);
    return m_temporaryTransaction.currentObjectsDone;
}
auto SchedulableProgress::SetObjectsDone(std::size_t objectCount) -> void {
    std::unique_lock<std::mutex> lock(m_changesMutex);
    m_temporaryTransaction.currentObjectsDone = objectCount;
}

auto SchedulableProgress::AddObject(Object object) -> bool {
    if (ContainsObject(object.Id))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    m_temporaryTransaction.objects.push_back(object);
    return true;
}
auto SchedulableProgress::RemoveObject(const std::string &objectId) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    std::erase_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });

    return true;
}
auto SchedulableProgress::ContainsObject(const std::string &objectId) const -> bool {
    std::unique_lock<std::mutex> lock(m_changesMutex);
    return std::ranges::any_of(m_temporaryTransaction.objects, [&objectId](const Object &other) -> bool {
        return other.Id == objectId;
   });
}
auto SchedulableProgress::GetObjectInternal(const std::string &objectId) -> Object& {
    return *std::ranges::find_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });
}
auto SchedulableProgress::GetObjectInternal(const std::string &objectId) const -> const Object& {
    return *std::ranges::find_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });
}
auto SchedulableProgress::GetObject(const std::string &objectId) const -> std::optional<Object> {
    if (!ContainsObject(objectId))
        return {};

    std::unique_lock<std::mutex> lock(m_changesMutex);
    return GetObjectInternal(objectId);
}

auto SchedulableProgress::RemoveStep(const std::string &objectId, const std::string &stepId) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    Object& object = GetObjectInternal(objectId);
    if (!std::ranges::any_of(object.Steps, [&stepId](const Step &step) -> bool {
        return step.Id == stepId;
    }))
        return false;

    std::erase_if(object.Steps, [&stepId](const Step &step) -> bool {
        return step.Id == stepId;
    });

    return true;
}
auto SchedulableProgress::AddStep(const std::string &objectId, Step step) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    Object& object = GetObjectInternal(objectId);
    if (std::ranges::any_of(object.Steps, [&step](const Step &curStep) -> bool {
        return step.Id == curStep.Id;
    }))
        return false;

    object.Steps.push_back(step);
    return true;
}
auto SchedulableProgress::SetObjectTotalSteps(const std::string &objectId, std::size_t objectTotalSteps) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    GetObjectInternal(objectId).TotalSteps = objectTotalSteps;

    return true;
}
auto SchedulableProgress::IncrementCompletedSteps(const std::string &objectId) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::unique_lock<std::mutex> lock(m_changesMutex);
    GetObjectInternal(objectId).CompletedSteps++;

    return true;
}
auto SchedulableProgress::ApplyChanges() -> void {
    std::unique_lock<std::mutex> lock(m_transactionMutex);
    m_currentTransaction = m_temporaryTransaction;
}

auto SchedulableProgress::Generate(std::size_t requestedColumnWidth) -> std::optional<SchedulableProgress::DrawDescription> {
    std::unique_lock<std::mutex> lock(m_transactionMutex);

    if (m_currentTransaction.totalObjects == 0)
        return {};

    std::stringstream outputStream;
    std::stringstream descriptionStream;

    // Total lines should be the header + 1 + numObjects + totalSteps + newline at end
    // Do two things at once, count lines and work on headers
    outputStream << '\n';
    std::string header = "Building: ";
    for (std::size_t i = 0; i < m_currentTransaction.objects.size(); i++) {
        header += m_currentTransaction.objects.at(i).Id;
        if (i + 1 < m_currentTransaction.objects.size())
            header += ", ";

        descriptionStream << std::format("{} ({}/{})\n", m_currentTransaction.objects.at(i).Id, m_currentTransaction.objects.at(i).CompletedSteps, m_currentTransaction.objects.at(i).TotalSteps);
        for (const Step &step : m_currentTransaction.objects.at(i).Steps) {
            descriptionStream << "  " << step.Description << '\n';
        }
    }

    // Build progress bar
    double progress = 0.0;
    if (m_currentTransaction.totalObjects > 0) {
        progress = static_cast<double>(m_currentTransaction.currentObjectsDone)
                 / static_cast<double>(m_currentTransaction.totalObjects);
        progress = std::clamp(progress, 0.0, 1.0);
    }
    unsigned short int percent = static_cast<unsigned short int>(std::lround(progress * 100.0));

    std::stringstream tmp;
    tmp << " [{} " << std::setw(3) << percent << "%] " << m_currentTransaction.currentObjectsDone << '/'
        << m_currentTransaction.totalObjects << " objects";

    header += tmp.str();

    std::size_t progressBarSize{75};
    if (requestedColumnWidth != 0) {
        std::size_t fixedHeaderSize = header.size() - 2;
        if (fixedHeaderSize >= requestedColumnWidth) {
            progressBarSize = 1;
        } else {
            progressBarSize = requestedColumnWidth - fixedHeaderSize;
        }
    }

    std::size_t filled = static_cast<std::size_t>(std::lround(progress * progressBarSize));

    std::string filledString = std::string(filled, '#'); 
    filledString += std::string(std::lround(progressBarSize) - filled, ' ');

    outputStream << std::vformat(header, std::make_format_args(filledString)) << "\n\n" << descriptionStream.str();

    // Calculate line wrapping
    std::size_t lines{0};
    std::size_t columnCount{0};
#ifdef __linux__
    winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    columnCount = size.ws_col;
#endif
    std::string outputLine;
    while (std::getline(outputStream, outputLine)) {
        lines += outputLine.empty() ? 1 : 1 + (outputLine.size() - 1) / columnCount;
    }

    DrawDescription description {
        .output = outputStream.str(),
        .lines = lines,
        .previousLineCount = m_currentLineCount
    };

    m_currentLineCount = lines;

    return description;
}

ProgressLogger::ProgressLogger(Constructable) :
    TextLogger(TextLogger::Constructable{}),
    m_schedulableProgress(std::make_shared<SchedulableProgress>()) {}

ProgressLogger::~ProgressLogger() {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_finish.wait(lock);
}

auto ProgressLogger::Update() -> void {
    std::unique_lock<std::mutex> lock(m_mutex);
    std::size_t columnCount{0};
#ifdef __linux__
    winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    columnCount = std::min<std::size_t>(size.ws_col, 75);
#endif

    std::optional<SchedulableProgress::DrawDescription> progressDescription = m_schedulableProgress->Generate(columnCount);
    if (progressDescription)
        std::cout << MoveToColumn(0) << MoveUp(progressDescription->previousLineCount) << Clear(Clear::Where::CursorToEndScreen);

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
    if (progressDescription)
        std::cout << progressDescription->output;
    std::cout.flush();
    m_finish.notify_one();
}

auto ProgressLogger::GetProgress() -> std::shared_ptr<SchedulableProgress> {
    EnsureLoggingThreadStarted();
    return m_schedulableProgress;
}
