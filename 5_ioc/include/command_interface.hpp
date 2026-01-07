#pragma once

#include <memory>

namespace ioc::command {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
};

using ICommandUPtr = std::unique_ptr<ICommand>;
using ICommandSPtr = std::shared_ptr<ICommand>;

} // namespace ioc::command