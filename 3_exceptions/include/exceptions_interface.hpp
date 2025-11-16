#pragma once

#include <format>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace exceptions {

class IException {
protected:
    explicit IException(std::string_view what)
    : what_{what} {}

public:
    virtual ~IException() = default;

    virtual std::string_view What() const noexcept {
        return what_;
    }

private:
    const std::string what_;
};

}; // namespace exceptions