#include <cassert>
#include <format>

#include "command_impl.hpp"
#include "exceptions_impl.hpp"

namespace exceptions {

ICommandUPtr ExceptionHandler::Handle(const ICommandUPtr& cmd, const IException& ex) noexcept {
    try {
        auto exceptionTblIter = handlersStore_.find(typeid(*cmd));
        if (std::end(handlersStore_) == exceptionTblIter) return GetDefaultCommand(ex.What());

        auto handlerIter = exceptionTblIter->second.find(typeid(ex));
        if (std::end(exceptionTblIter->second) == handlerIter) return GetDefaultCommand(ex.What());

        return handlerIter->second->Clone();
    }
    catch(...) {
        return GetDefaultCommand("Unknown exception caught");
    }
    assert(false);
    return nullptr;
}

ICommandUPtr ExceptionHandler::GetDefaultCommand(std::string_view what) {
    return std::make_unique<PrintError>(std::format(
        "No registered handler for '{}'", what
    ));
}

TestException::TestException()
: IException("Test exception") {}

}; // namespace exceptions