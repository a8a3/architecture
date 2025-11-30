#pragma once

#include <charconv>
#include <format>
#include <string_view>
#include <unordered_map>

#include "primitives.hpp"

namespace game {

class Move {
public:
    explicit Move(IMovingObject* obj)
    : obj_{obj} {}

    void Execute() {
        obj_->setLocation(obj_->getLocation().MoveTo(obj_->getVelocity()));
    }
private:
    IMovingObject* obj_{nullptr};
};

class Rotate {
public:
    explicit Rotate(IRotatingObject* obj) 
    : obj_{obj} {}

    void Execute() {
        obj_->setAngle(obj_->getAngle() + obj_->getAngularVelocity());
    }

private:
    IRotatingObject* obj_;
};

class SpaceShip : public IEntity {
public:
    std::string getProperty(std::string_view key) const override {
        const auto iter = properties_.find(key);
        if (std::end(properties_) == iter) {
            throw std::logic_error(std::format("Unable to find '{}' property in SpaceShip object", key));
        }
        return iter->second;
    }

    void setProperty(std::string_view key, std::string_view val) {
        auto iter = properties_.find(key);
        if (std::end(properties_) == iter) {
            throw std::logic_error(std::format("Unable to set '{}' property in SpaceShip object", key));
        }
        iter->second = val;
    }
   
private:
    std::unordered_map<std::string_view, std::string> properties_{
         {"location", Point{0, 0}.toString()}
        ,{"velocity", Vector{0, 0}.toString()}
        ,{"angle", Angle{.rad = 0.}.toString()}
        ,{"angular_velocity", Angle{.rad = 0.}.toString()}
    };
};

}  // namespace game