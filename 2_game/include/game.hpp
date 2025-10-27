#pragma once

#include <charconv>
#include <format>
#include <string_view>
#include <unordered_map>

namespace game {

struct Angle {
    double rad;

    std::string toString() const {
        return std::format("{:.6f}", rad);
    }

    static Angle fromString(std::string_view str) {
        double angleVal;
        auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), angleVal);
        if (ec != std::errc{}) {
            throw std::invalid_argument(std::format("Unable to parse angle from: '{}'", str));
        }
        return Angle{.rad = angleVal};
    }
}; 

Angle operator+(const Angle& lhs, const Angle& rhs) {
    return Angle{.rad = lhs.rad + rhs.rad};
}

struct Vector {
    int x;
    int y;

    std::string toString() const {
        return std::format("{},{}", x, y);
    }

    static Vector fromString(std::string_view str) {
        const auto comma_pos = str.find(',');
        if (std::string_view::npos == comma_pos) {
            throw std::invalid_argument(std::format("Unable to parse Vector object from: '{}'", str));
        }

        int x{-1}, y{-1};
        
        auto x_str = str.substr(0, comma_pos);
        auto [_, x_ec] = std::from_chars(x_str.data(), x_str.data() + x_str.size(), x);
        if (x_ec != std::errc{}) {
            throw std::invalid_argument(std::format("Unable to parse x val from: '{}'", x_str));
        }

        auto y_str = str.substr(comma_pos + 1);
        auto [y_ptr, y_ec] = std::from_chars(y_str.data(), y_str.data() + y_str.size(), y);
        if (y_ec != std::errc{}) {
            throw std::invalid_argument(std::format("Unable to parse y val from: '{}'", y_str));
        }
        return Vector{.x = x, .y = y};
    }
};

class Point {
public: 
    Point(int x, int y)
    : x_{x}
    , y_{y} {}

    Point& MoveTo(const Vector& v) {
        x_ += v.x;
        y_ += v.y;
        return *this;
    }

    bool operator==(const Point& other) const {
        return other.x_ == x_ && other.y_ == y_;
    }

    std::string toString() const {
        return std::format("{},{}", x_, y_);
    }

    static Point fromString(std::string_view str) {
        const auto comma_pos = str.find(',');
        if (std::string_view::npos == comma_pos) {
            throw std::invalid_argument(std::format("Unable to parse Point object from string: '{}'", str));
        }

        int x{-1}, y{-1};
        
        auto x_str = str.substr(0, comma_pos);
        auto [_, x_ec] = std::from_chars(x_str.data(), x_str.data() + x_str.size(), x);
        if (x_ec != std::errc{}) {
            throw std::invalid_argument(std::format("Unable to parse x coordinate from: '{}'", x_str));
        }

        auto y_str = str.substr(comma_pos + 1);
        auto [y_ptr, y_ec] = std::from_chars(y_str.data(), y_str.data() + y_str.size(), y);
        if (y_ec != std::errc{}) {
            throw std::invalid_argument(std::format("Unable to parse y coordinate from: '{}'", y_str));
        }
        return Point{x, y};
    }
private:
    int x_{0};
    int y_{0};
}; 

class IMovingObject {
public:
    virtual Point getLocation() const = 0;
    virtual void setLocation(const Point&) = 0;
    virtual Vector getVelocity() const = 0;

    virtual ~IMovingObject() = default;
};

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

class IRotatingObject {
public:
    virtual Angle getAngle() const = 0;
    virtual void setAngle(const Angle&) = 0;
    virtual Angle getAngularVelocity() const = 0;

    virtual ~IRotatingObject() = default;
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

class IEntity {
public:
    virtual std::string getProperty(std::string_view key) const = 0;
    virtual void setProperty(std::string_view key, std::string_view val) = 0;
    virtual ~IEntity() = default;
};

class MovingObjectAdapter : public IMovingObject {
public:
    explicit MovingObjectAdapter(IEntity* entity)
    : entity_{entity} {}

    Point getLocation() const override {
        return Point::fromString(entity_->getProperty("location"));
    }

    void setLocation(const Point& newLocation) {
        entity_->setProperty("location", newLocation.toString());
    }

    Vector getVelocity() const {
        return Vector::fromString(entity_->getProperty("velocity"));
    }

private:
    IEntity* entity_;
};

class RotatingObjectAdapter : public IRotatingObject {
public:
    explicit RotatingObjectAdapter(IEntity* entity)
    : entity_{entity} {}

    Angle getAngle() const override {
        return Angle::fromString(entity_->getProperty("angle"));
    }

    void setAngle(const Angle& newAngle) {
        entity_->setProperty("angle", newAngle.toString());
    }

    Angle getAngularVelocity() const {
        return Angle::fromString(entity_->getProperty("angular_velocity"));
    }

private:
    IEntity* entity_;
};

class SpaceShip : public IEntity {
public:
    std::string getProperty(std::string_view key) const {
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