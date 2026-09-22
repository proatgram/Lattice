module Lattice.Logger.ProgressLogger;

import Lattice.Logger.EscapeSequences;

using namespace Lattice::Logger;
using namespace Lattice::Logger::EscapeSequences::Colors;
using namespace Lattice::Logger::EscapeSequences::Cursor;
using namespace Lattice::Logger::EscapeSequences::Clearing;

auto BuildProgress::GetTotalObjects() const -> std::size_t {
    return m_temporaryTransaction.totalObjects;
}
auto BuildProgress::SetTotalObjects(std::size_t objectCount) -> void {
    m_temporaryTransaction.totalObjects = objectCount;
}
auto BuildProgress::GetObjectsDone() const -> std::size_t {
    return m_temporaryTransaction.currentObjectsDone;
}
auto BuildProgress::SetObjectsDone(std::size_t objectCount) -> void {
    m_temporaryTransaction.currentObjectsDone = objectCount;
}

auto BuildProgress::AddObject(Object object) -> bool {
    if (ContainsObject(object.Id))
        return false;

    m_temporaryTransaction.objects.push_back(object);
    return true;
}
auto BuildProgress::RemoveObject(const std::string &objectId) -> bool {
    if (!ContainsObject(objectId))
        return false;

    std::erase_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });

    return true;
}
auto BuildProgress::ContainsObject(const std::string &objectId) const -> bool {
    return std::ranges::any_of(m_temporaryTransaction.objects, [&objectId](const Object &other) -> bool {
        return other.Id == objectId;
   });
}
auto BuildProgress::GetObject(const std::string &objectId) const -> std::optional<std::reference_wrapper<const Object>> {
    if (!ContainsObject(objectId))
        return {};

    return *std::ranges::find_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });
}
auto BuildProgress::GetObject(const std::string &objectId) -> std::optional<std::reference_wrapper<Object>> {
    if (!ContainsObject(objectId))
        return {};

    return *std::ranges::find_if(m_temporaryTransaction.objects, [&objectId](const Object &object) -> bool {
        return object.Id == objectId;
    });
}

auto BuildProgress::RemoveStep(const std::string &objectId, const std::string &stepId) -> bool {
    if (!ContainsObject(objectId))
        return false;

    Object& object = GetObject(objectId)->get();
    if (!std::ranges::any_of(object.Steps, [&stepId](const Step &step) -> bool {
        return step.Id == stepId;
    }))
        return false;

    std::erase_if(object.Steps, [&stepId](const Step &step) -> bool {
        return step.Id == stepId;
    });

    return true;
}
auto BuildProgress::AddStep(const std::string &objectId, Step step) -> bool {
    if (!ContainsObject(objectId))
        return false;

    Object &object = GetObject(objectId)->get();
    if (std::ranges::any_of(object.Steps, [&step](const Step &curStep) -> bool {
        return step.Id == curStep.Id;
    }))
        return false;

    object.Steps.push_back(step);
    return true;
}
auto BuildProgress::ApplyChanges() -> void {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_currentTransaction = m_temporaryTransaction;
}

auto BuildProgress::Generate(std::size_t requestedColumnWidth) -> BuildProgress::DrawDescription {
    std::unique_lock<std::mutex> lock(m_mutex);

    std::stringstream outputStream;

    // Total lines should be the header + 1 + numObjects + totalSteps + newline at end
    // Do two things at once, count lines and work on headers
    std::size_t lines = 3 + m_currentTransaction.objects.size();
    outputStream << "\nBuilding: ";
    for (std::size_t i = 0; i < m_currentTransaction.objects.size(); i++) {
        lines += m_currentTransaction.objects.at(i).Steps.size();
        outputStream << m_currentTransaction.objects.at(i).Id;
        if (i + 1 < m_currentTransaction.objects.size())
            outputStream << ", ";
    }

    // Build progress bar
    double progress = 0.0;
    if (m_currentTransaction.totalObjects > 0) {
        progress = static_cast<double>(m_currentTransaction.currentObjectsDone)
                 / static_cast<double>(m_currentTransaction.totalObjects);
        progress = std::clamp(progress, 0.0, 1.0);
    }

    std::size_t filled = static_cast<std::size_t>(std::lround(50.0 * progress));
    unsigned short int percent = static_cast<unsigned short int>(std::lround(progress * 100.0));

    outputStream << " [" << std::setw(50) << std::left << std::string(filled, '#') << std::right << ' '
                 << std::setw(3) << percent
                 << "%] " << m_currentTransaction.currentObjectsDone << '/'
                 << m_currentTransaction.totalObjects << " objects\n\n";
    
    // Write detailed progress
    for (const Object &object : m_currentTransaction.objects) {
        outputStream << std::format("{} ({}/{})\n", object.Id, object.CompletedSteps, object.TotalSteps);
        for (const Step &step : object.Steps) {
            outputStream << "  " << step.Description << '\n';
        }
    }

    DrawDescription description {
        .output = outputStream.str(),
        .lines = lines,
        .previousLineCount = m_currentLineCount
    };

    m_currentLineCount = lines;

    return description;
}

ProgressLogger::ProgressLogger() :
    m_messagesQueue(),
    m_buildProgress(std::make_shared<BuildProgress>()) {StartLoggingThread();}

auto ProgressLogger::Log(Level level, const std::string &message) -> void {
    std::unique_lock lock(m_mutex);
    m_messagesQueue.push({level, message});
}

auto ProgressLogger::Update() -> void {
    std::unique_lock<std::mutex> lock(m_mutex);
    BuildProgress::DrawDescription progressDescription = m_buildProgress->Generate();
    std::cout << MoveToColumn(0) << MoveUp(progressDescription.previousLineCount) << Clear(Clear::Where::CursorToEndScreen);

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

    std::cout << progressDescription.output;
    std::cout.flush();
}

auto ProgressLogger::GetProgress() const -> std::shared_ptr<BuildProgress> {
    return m_buildProgress;
}
