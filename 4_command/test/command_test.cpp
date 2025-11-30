#include <gtest/gtest.h>
#include <numbers>

#include <command_impl.hpp>
#include <game.hpp>
#include <macro_impl.hpp>
#include <primitives.hpp>

class SpaceShip : public game::IEntity {
public:
    std::string getProperty(std::string_view key) const override {
        const auto iter = properties_.find(key);
        if (std::end(properties_) == iter) {
            throw std::logic_error(std::format("Unable to find '{}' property in SpaceShip object", key));
        }
        return iter->second;
    }

    void setProperty(std::string_view key, std::string_view val) override {
        auto iter = properties_.find(key);
        if (std::end(properties_) == iter) {
            throw std::logic_error(std::format("Unable to set '{}' property in SpaceShip object", key));
        }
        iter->second = val;
    }
   
private:
    std::unordered_map<std::string_view, std::string> properties_{
         {"location", game::Point{0, 0}.toString()}
        ,{"velocity", game::Vector{0, 0}.toString()}
        ,{"angle", game::Angle{.rad = 0.}.toString()}
        ,{"angular_velocity", game::Angle{.rad = 0.}.toString()}
        ,{"fuel", game::IntegerProperty{.val = 0}.toString()}
    };
};

TEST(BurnFuelCommandTest, Basics) {
    int initialFuelAmount{10};
    SpaceShip ship;
    ship.setProperty("fuel", game::IntegerProperty{initialFuelAmount}.toString());

    command::FuelConsumingObjectAdapter fcoa{&ship};
    command::BurnFuel burnFuelCmd{&fcoa};

    EXPECT_NO_THROW(burnFuelCmd.Execute());

    const auto newFuelAmount{game::IntegerProperty::fromString(ship.getProperty("fuel"))};
    EXPECT_EQ(initialFuelAmount - 1, newFuelAmount.val);
}

TEST(CheckFuelCommandTest, Basics) {
    int initialFuelAmount{1};
    SpaceShip ship;
    ship.setProperty("fuel", game::IntegerProperty{initialFuelAmount}.toString());

    command::FuelConsumingObjectAdapter adapter{&ship};

    command::CheckFuel checkFuelCmd{&adapter};
    EXPECT_NO_THROW(checkFuelCmd.Execute());

    command::BurnFuel burnFuelCmd{&adapter};
    EXPECT_NO_THROW(burnFuelCmd.Execute());

    EXPECT_THROW(checkFuelCmd.Execute(), command::CommandException);

    const auto newFuelAmount{game::IntegerProperty::fromString(ship.getProperty("fuel"))};
    EXPECT_EQ(initialFuelAmount - 1, newFuelAmount.val);
}

TEST(MacroCommandTest, Basics) {
    int counter{0};

    class CounterIncCommand : public command::ICommand {
        public:
            explicit CounterIncCommand(int& counter) : counter_{counter} {}
            void Execute() override { ++counter_; }
        private:
            int& counter_;
    };
    CounterIncCommand cmd1{counter};
    CounterIncCommand cmd2{counter};
    CounterIncCommand cmd3{counter};

    command::MacroCommand macroCmd (
        command::MacroCommand::ICommandsArr{&cmd1, &cmd2, &cmd3}
    );
    EXPECT_NO_THROW(macroCmd.Execute());
    EXPECT_EQ(3, counter);
}

TEST(MacroCommandTest, BurnFuel) {
    int initialFuelAmount{10};
    SpaceShip ship;
    ship.setProperty("fuel", game::IntegerProperty{initialFuelAmount}.toString());
    
    game::MovingObjectAdapter moa{&ship};
    command::Move moveCmd{&moa};

    command::FuelConsumingObjectAdapter fcoa{&ship};
    command::CheckFuel checkFuelCmd{&fcoa};
    command::BurnFuel burnFuelCmd{&fcoa};

    command::MacroCommand macroCmd (
        command::MacroCommand::ICommandsArr{
            &checkFuelCmd
           ,&moveCmd
           ,&burnFuelCmd
        }
    );
    EXPECT_NO_THROW(macroCmd.Execute());

    const auto newFuelAmount{game::IntegerProperty::fromString(ship.getProperty("fuel"))};
    EXPECT_EQ(initialFuelAmount - 1, newFuelAmount.val);
}

TEST(ChangeVelocityCommandTest, RotateBy90Degrees) {
    const auto pi = std::numbers::pi_v<double>;
    SpaceShip ship;
    // thi ship's location is {0, 0}
    // the ship's velocity is {1, 0}
    ship.setProperty("velocity", game::Vector{.x = 1, .y = 0}.toString());
    // the ship's angle is 90
    ship.setProperty("angle", game::Angle{.rad = pi/2.0 }.toString());

    game::MovingObjectAdapter moa{&ship};
    game::RotatingObjectAdapter roa{&ship};

    command::ChangeVelocity cmd{&roa, &moa};
    cmd.Execute();

    // the new ship's location must be {0, 1}
    const auto newLocation = game::Vector::fromString(ship.getProperty("location"));
    EXPECT_EQ(newLocation.x, 0);
    EXPECT_EQ(newLocation.y, 1);
}

TEST(MacroCommandTest, MoveRotateAndChangeVelocity){
    const auto pi = std::numbers::pi_v<double>;
    SpaceShip ship;
    // thi ship's location is {0, 0}
    // the ship's angle is 0

    // the sip's fuel is 2 (for 2 commands)
    ship.setProperty("fuel", game::IntegerProperty{.val = 2}.toString());
    // the ship's velocity is {1, 0}
    ship.setProperty("velocity", game::Vector{.x = 1, .y = 0}.toString());
    // the ship's angular velocity is 90 degrees
    ship.setProperty("angular_velocity", game::Angle{.rad = pi / 2.}.toString());

    game::MovingObjectAdapter moa{&ship};
    game::RotatingObjectAdapter roa{&ship};
    command::FuelConsumingObjectAdapter fcoa{&ship};

    command::Rotate rotateCmd{&roa};
    command::ChangeVelocity changeVCmd{&roa, &moa};
    command::CheckFuel checkFuelCmd{&fcoa};
    command::BurnFuel burnFuelCmd{&fcoa};
    
    command::MacroCommand macroCmd (
        command::MacroCommand::ICommandsArr {
            &checkFuelCmd,
            &burnFuelCmd,
            &rotateCmd,
            &checkFuelCmd,
            &burnFuelCmd,
            &changeVCmd
       }
    );

    macroCmd.Execute();

    // the new ship's location must be {0, 1}
    const auto newLocation = game::Vector::fromString(ship.getProperty("location"));
    EXPECT_EQ(newLocation.x, 0);
    EXPECT_EQ(newLocation.y, 1);
}