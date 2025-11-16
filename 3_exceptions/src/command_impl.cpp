#include "command_impl.hpp"
#include "exceptions_impl.hpp"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>

namespace exceptions {

// ThrowException impl
void ThrowException::Execute() const {
    throw TestException{};
}

ICommandUPtr ThrowException::Clone() const {
    return std::make_unique<ThrowException>(*this);
}

// PrintError impl
PrintError::PrintError(std::string_view err)
: err_{err} {}

void PrintError::Execute() const {
    std::cerr << err_ << '\n';
}

ICommandUPtr PrintError::Clone() const {
    return std::make_unique<PrintError>(*this);
}

// LogErrorCommand impl
LogErrorCommand::LogErrorCommand(std::string_view err, std::string_view logPath)
: err_(err)
, logPath_(logPath) {}

void LogErrorCommand::Execute() const {
    std::ofstream ofs(logPath_, std::ios::app);
    if (!ofs) {
        std::cerr << std::format(
            "LogError: failed to open log file: {} - {}\n", logPath_, err_);
        return;
    }

    using namespace std::chrono;
    auto now = system_clock::now();
    auto ts = floor<seconds>(now);

    ofs << std::format("{:%F %T} {}\n", ts, err_);
    ofs.flush();
}

ICommandUPtr LogErrorCommand::Clone() const {
    return std::make_unique<LogErrorCommand>(*this);
}

// EnqueueCommand impl
EnqueueCommand::EnqueueCommand(IQueue& queue, ICommand& cmd)
: q_{queue}
, enqueued_cmd_{cmd}
{}

void EnqueueCommand::Execute() const {
    q_.Push(enqueued_cmd_.Clone());
}

ICommandUPtr EnqueueCommand::Clone() const {
    return std::make_unique<EnqueueCommand>(*this);
}

// RepeateCommand impl
RepeatCommand::RepeatCommand(ICommand& repeatCmd)
: repeatCmd_{repeatCmd} {}


void RepeatCommand::Execute() const {
    repeatCmd_.Execute();
}

ICommandUPtr RepeatCommand::Clone() const {
    return std::make_unique<RepeatCommand>(*this);
}

// RepeatTwiceCommand impl
RepeatTwiceCommand::RepeatTwiceCommand(ICommand& repeatCmd)
: repeatCmd_{repeatCmd} {}

void RepeatTwiceCommand::Execute() const {
    repeatCmd_.Execute();
}

ICommandUPtr RepeatTwiceCommand::Clone() const {
    return std::make_unique<RepeatTwiceCommand>(*this);
}

} // namespace exceptions