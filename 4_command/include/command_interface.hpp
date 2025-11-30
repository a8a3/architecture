#pragma once

namespace command {

class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void Execute() = 0;
};

using ICommandUPtr = std::unique_ptr<ICommand>;

} // namespace command