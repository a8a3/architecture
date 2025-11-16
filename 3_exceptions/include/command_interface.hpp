#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace exceptions {

using ICommandUPtr = std::unique_ptr<class ICommand>;

class ICommand {
public:
    virtual void Execute() const = 0; // may throw anything
    virtual ICommandUPtr Clone() const = 0;
    virtual ~ICommand() = default;
};

} // namespace exceptions