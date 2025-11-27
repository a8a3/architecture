#pragma once

#include <cassert>
#include "command_interface.hpp"
#include "queue_interface.hpp"

#include <queue>

namespace exceptions {

class QueueImpl : public IQueue {
public:
    void Push(ICommandUPtr cmd) override { impl_.push(std::move(cmd)); }
    void Pop()                  override { impl_.pop(); }

    const ICommandUPtr& Front() const noexcept override { 
        assert(!impl_.empty());
        return impl_.front(); 
    }
    bool IsEmpty()     const noexcept override { return impl_.empty(); }
    std::size_t Size() const noexcept override { return impl_.size(); }

private:
    std::queue<ICommandUPtr> impl_;
};

}; // namespace exceptions