#pragma once
#include <cassert>

#include "command_impl.hpp"
#include "exceptions_impl.hpp"
#include "queue_interface.hpp"

namespace exceptions::cmd_loop {

void run(IQueue& queue) {
    while(!queue.IsEmpty()) {
        const ICommandUPtr& front = queue.Front();
        try {
            front->Execute();
        } catch (const IException& e) {
            ExceptionHandler::Handle(front, e)->Execute();
        }
        queue.Pop();
    }
}

} // namespace exceptions::cmd_loop