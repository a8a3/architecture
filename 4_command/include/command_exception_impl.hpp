#pragma once

#include <string_view>
#include <exceptions_interface.hpp>

namespace command {

class CommandException : public exceptions::IException {
public:
    explicit CommandException(std::string_view what) 
    : IException{what} {}
};

} // namespace command