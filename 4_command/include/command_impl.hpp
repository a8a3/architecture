#pragma once

#include <cmath>

#include <primitives.hpp>

#include "command_interface.hpp"
#include "command_exception_impl.hpp"
#include "fuel_consuming_obj_interface.hpp"

namespace command {

class Move : public ICommand {
public:
    explicit Move(game::IMovingObject* obj)
    : obj_{obj} {}

    void Execute() override {
        obj_->setLocation(obj_->getLocation().MoveTo(obj_->getVelocity()));
    }

private:
    game::IMovingObject* obj_{nullptr};
};

class Rotate : public ICommand {
public:
    explicit Rotate(game::IRotatingObject* obj) 
    : obj_{obj} {}

    void Execute() override {
        obj_->setAngle(obj_->getAngle() + obj_->getAngularVelocity());
    }

private:
    game::IRotatingObject* obj_;
};

// Реализовать команду для модификации вектора мгновенной скорости при повороте. 
// Необходимо учесть, что не каждый разворачивающийся объект движется.
class ChangeVelocity : public ICommand{
public:
    explicit ChangeVelocity(game::IRotatingObject* rObj, game::IMovingObject* mObj)
    : rotatingObj_{rObj}
    , movingObj_{mObj}
    {}

    void Execute() override {
        const auto currentVelocity{movingObj_->getVelocity()};
        if (currentVelocity.isZero())
            return;
        
        const auto cx = static_cast<double>(currentVelocity.x);
        const auto cy = static_cast<double>(currentVelocity.y);

        const auto currentAngle = rotatingObj_->getAngle();
        const auto c = std::cos(currentAngle.rad);
        const auto s = std::sin(currentAngle.rad);

        const auto nx = cx * c - cy * s;
        const auto ny = cx * s + cy * c;

        const auto newVelocity = game::Vector{static_cast<int>(std::lround(nx)),
                                              static_cast<int>(std::lround(ny))};

        movingObj_->setLocation(movingObj_->getLocation().MoveTo(newVelocity));
    }

private:
    game::IRotatingObject* rotatingObj_{nullptr};
    game::IMovingObject* movingObj_{nullptr};
};

class CheckFuel : public ICommand {
public:
    explicit CheckFuel(IFuelConsumingObject* obj)
    : obj_{obj} {}

    void Execute() override {
        if (!obj_->CheckFuel()) {
            throw CommandException{"Not enough fuel"};
        }
    }
private:
    IFuelConsumingObject* obj_;
};

class BurnFuel : public ICommand {
public:
    explicit BurnFuel(IFuelConsumingObject* obj)
    : obj_{obj} {}

    void Execute() override {
        obj_->BurnFuel();
    }

private:
    IFuelConsumingObject* obj_;
};

class FuelConsumingObjectAdapter : public IFuelConsumingObject {
    constexpr static auto key = "fuel";
public:
    explicit FuelConsumingObjectAdapter(game::IEntity* entity)
    : entity_{entity} {}

    bool CheckFuel() const override {
        return game::IntegerProperty::fromString(entity_->getProperty(key)) > 0;
    }

    void BurnFuel() {
        int fuelAmount = game::IntegerProperty::fromString(entity_->getProperty(key)).val;
        --fuelAmount;
        entity_->setProperty(key, game::IntegerProperty{fuelAmount}.toString());
    }
    
private:
    game::IEntity* entity_;
};



} // namespace command