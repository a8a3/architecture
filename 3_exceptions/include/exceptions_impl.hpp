#pragma once

#include <format>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "command_impl.hpp"
#include "exceptions_interface.hpp"
#include "queue_interface.hpp"

namespace exceptions {

class ExceptionHandler {
public: 
    ExceptionHandler() = default;
    ~ExceptionHandler() = default;

    static ICommandUPtr Handle(const ICommandUPtr& cmd, const IException& ex) noexcept;

    template<typename TCmd, typename TExc>
    static void Register(ICommandUPtr handler) {
        handlersStore_[typeid(TCmd)][typeid(TExc)] = std::move(handler);
    }

private:
    using CommandKey = std::type_index;
    using ExceptionKey = std::type_index;
    using ExceptionTable = std::unordered_map<ExceptionKey, ICommandUPtr>;
    static inline std::unordered_map<CommandKey, ExceptionTable> handlersStore_;

    static ICommandUPtr GetDefaultCommand(std::string_view);
};

class TestException : public IException {
public:
    TestException();
};

}; // namespace exceptions