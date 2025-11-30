#pragma once

#include "command_interface.hpp"
#include <vector>

namespace command {

class MacroCommand : public ICommand {
public:
    using ICommandsArr = std::vector<ICommand*>;

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

} // namespace command