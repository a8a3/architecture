#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "command_interface.hpp"
#include "queue_interface.hpp"

namespace exceptions {

class ThrowException: public ICommand {
public:
    void Execute() const override;
    ICommandUPtr Clone() const override;
};

class PrintError: public ICommand {
public:
    explicit PrintError(std::string_view err);
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    std::string err_;
};

// 4. Implement command that writes exception info into the log file
class LogErrorCommand: public ICommand {
public:
    explicit LogErrorCommand(std::string_view err, std::string_view logPath = "errors.log");
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    std::string err_;
    std::string logPath_;
};

// 5. Implement command that enqueues LogError command into the main queue
class EnqueueCommand : public ICommand {
public:
    EnqueueCommand(IQueue&, ICommand&);
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    IQueue& q_;
    ICommand& enqueued_cmd_;
};

// 6. Implement command that repeats another command
class RepeatCommand : public ICommand {
public:
    RepeatCommand(ICommand&);
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    ICommand& repeatCmd_;
};

// 7. Implement command that repeats after first throw and logs after second throw
class RepeatAndLogCommand : public ICommand {
public:
    RepeatAndLogCommand(ICommand&);
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    ICommand& repeatCmd_;
};

// 9. Implement command that in case of exception repeats cmd twice 
// and logs error after
class RepeatTwiceCommand : public ICommand {
public:
    RepeatTwiceCommand(ICommand&);
    void Execute() const override;
    ICommandUPtr Clone() const override;

private:
    ICommand& repeatCmd_;
};

} // namespace exceptions