#pragma once

#include <cmath>
#include <memory>
#include <vector>

#include <primitives.hpp>

#include "command_interface.hpp"

namespace ioc::command {

class IncCommand : public ICommand {
public:
    explicit IncCommand(int& val)
    : val_{val} {}

    void Execute() override {
        ++val_;
    }

private:
    int& val_;
};
using IncCommandUPtr = std::unique_ptr<IncCommand>;

class MacroCommand : public ICommand {
public:
    using ICommandsArr = std::vector<ICommandUPtr>;

    explicit MacroCommand(ICommandsArr&& commands)
    : commands_(std::move(commands)) 
    {}

    void Execute() override {
        for (auto& cmd: commands_) {
            cmd->Execute();
        }
    }

private:
    ICommandsArr commands_;
};
using MacroCommandUPtr = std::unique_ptr<MacroCommand>;


} // namespace ioc::command