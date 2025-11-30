#pragma once

namespace command {

class IFuelConsumingObject {
public:
    virtual bool CheckFuel() const = 0;
    virtual void BurnFuel() = 0;
    virtual ~IFuelConsumingObject() = default;
};

} // namespace command