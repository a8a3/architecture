#include <gtest/gtest.h>
#include <game.hpp>

TEST(GameTest, BasicMovement) {
    game::SpaceShip ship;
    ship.setProperty("location", game::Point{12, 5}.toString());
    ship.setProperty("velocity", game::Vector{-7, 3}.toString());
    
    game::MovingObjectAdapter moa{&ship};
    game::Move moveCommand{&moa};

    EXPECT_NO_THROW(moveCommand.Execute());

    const auto newLocation{moa.getLocation()};
    EXPECT_EQ(newLocation, game::Point(5, 8));
}

TEST(GameTest, BasicRotation) {
    game::SpaceShip ship;
    ship.setProperty("angle", game::Angle{.rad = 1.}.toString());
    ship.setProperty("angular_velocity", game::Angle{.rad = 0.5}.toString());
    
    game::RotatingObjectAdapter roa{&ship};
    game::Rotate rotateCommand{&roa};

    EXPECT_NO_THROW(rotateCommand.Execute());

    const auto newAngle{roa.getAngle()};
    EXPECT_NEAR(newAngle.rad, 1.5, 1e-5);
}

TEST(GameTest, UnableToGetSetBadProperties) {
    game::SpaceShip ship;
    const auto bad_property = "bad_location";

    try {
        ship.getProperty(bad_property);
        FAIL() << "logic_error expected";
    } catch (const std::logic_error& ex) {
        EXPECT_STREQ(ex.what(), std::format("Unable to find '{}' property in SpaceShip object", bad_property).c_str());
    } catch (...) {
        FAIL() << "logic_error expected";
    }

    try {
        ship.setProperty(bad_property, "0");
        FAIL() << "logic_error expected";
    } catch (const std::logic_error& ex) {
        EXPECT_STREQ(ex.what(), std::format("Unable to set '{}' property in SpaceShip object", bad_property).c_str());
    } catch (...) {
        FAIL() << "logic_error expected";
    }
}
