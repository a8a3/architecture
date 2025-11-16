#pragma once

#include <cstddef>
#include <memory>

#include "command_interface.hpp"

namespace exceptions {

class IQueue {
public:
    virtual void Push(ICommandUPtr) = 0;
    virtual void Pop() = 0;
    virtual const ICommandUPtr& Front() const noexcept = 0;
    virtual bool IsEmpty() const noexcept = 0;
    virtual std::size_t Size() const noexcept = 0;
    virtual ~IQueue() = default;
};

} // namespace exceptions