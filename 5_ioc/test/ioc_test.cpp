#include <chrono>
#include <gtest/gtest.h>
#include <format>
#include <latch>
#include <thread>

#include <command_interface.hpp>
#include <command_impl.hpp>

#include <game_entity_interface.hpp>
#include <game_entity_impl.hpp>

#include <ioc.hpp>

using namespace ioc;
using namespace ioc::command;

TEST(IoC, BasicCmdResolving) {
    auto& ioc = Container::getInstance();

    int val{0};
    auto incGenFunc = std::function<ICommandUPtr()>([&val] () {
        return std::make_unique<IncCommand>(val);
    });

    ioc.Resolve<ICommand>(ObjType::Register, ObjType::Inc, incGenFunc)->Execute();
    ASSERT_EQ(0, val);

    ioc.Resolve<ICommand>(ObjType::Inc)->Execute();
    ASSERT_EQ(1, val);

    int val1{0};
    auto incNTimesGenFunc = std::function<command::ICommandUPtr(int)>([&val1] (int times) {
        std::vector<ICommandUPtr> cmds;
        for (int i = 0; i < times; ++i) {
            cmds.emplace_back(std::make_unique<IncCommand>(val1));
        }
        return std::make_unique<MacroCommand>(std::move(cmds));
    });

    ioc.Resolve<ICommand>(ObjType::Register, ObjType::IncNTimes, incNTimesGenFunc)->Execute();
    ioc.Resolve<ICommand>(ObjType::IncNTimes, 5)->Execute();
    EXPECT_EQ(5, val1);
}

TEST(IoC, BasicEntityResolving) {
    auto& container = Container::getInstance();

    auto spaceShipGenFunc = std::function<entity::IGameEntityUPtr()>([] () {
        return std::make_unique<entity::SpaceShip>();
    });

    container.Resolve<ICommand>(ObjType::Register, ObjType::SpaceShip, spaceShipGenFunc)->Execute();
    EXPECT_NO_THROW(container.Resolve<entity::IGameEntity>(ObjType::SpaceShip));
}

TEST(IoC, MultithreadedResolving) {
    auto threadFunc = [] (int incTimes) {
        std::cout << "I'm inside [" << std::this_thread::get_id() << "] thread\n";

        auto& ioc = Container::getInstance();
        int val1{0};
        auto incNTimesGenFunc = std::function<command::ICommandUPtr()>([&val1, incTimes] () {
            std::vector<ICommandUPtr> cmds;
            for (int i = 0; i < incTimes; ++i) {
                cmds.emplace_back(std::make_unique<IncCommand>(val1));
            }
            return std::make_unique<MacroCommand>(std::move(cmds));
        });

        ioc.Resolve<ICommand>(ObjType::Register, ObjType::IncNTimes, incNTimesGenFunc)->Execute();
        ioc.Resolve<ICommand>(ObjType::IncNTimes)->Execute();
        EXPECT_EQ(incTimes, val1);
    };

    std::jthread t1{threadFunc, 5 };
    std::jthread t2{threadFunc, 10};
}