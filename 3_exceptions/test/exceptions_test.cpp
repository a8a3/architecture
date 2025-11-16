#include <gtest/gtest.h>
#include <filesystem>

#include <cmd_loop.hpp>
#include <command_impl.hpp>
#include <exceptions_impl.hpp>
#include <queue_impl.hpp>

using namespace exceptions;
namespace fs = std::filesystem;

namespace test {

template<bool DoThrow>
class CountingCommand: public ICommand {
public:
    explicit CountingCommand(int& counter) : counter_{counter} {}
    void Execute() const override { 
        ++counter_; 
        if constexpr (DoThrow) {
            throw TestException{};
        }
    }
    ICommandUPtr Clone() const {
        return std::make_unique<CountingCommand>(*this);
    }
private:
    int& counter_;
};
using CountingNoThrow = CountingCommand<false>;
using CountingWithThrow = CountingCommand<true>;

}  // namespace test

TEST(UnregisteredHandler, CmdLoopContinuesAfterException) {
    QueueImpl q;
    int counter{0};
    q.Push(std::make_unique<ThrowException>()); // will throw
    q.Push(std::make_unique<test::CountingNoThrow>(counter)); // this one should be executed

    ASSERT_EQ(2, q.Size());
    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_TRUE(q.IsEmpty());
    EXPECT_EQ(counter, 1);
}

TEST(RegisteredHandler, CmdLoopContinuesAfterException) {
    auto throwCmd = std::make_unique<ThrowException>();
    auto exception = std::make_unique<TestException>();

    int counter{0};
    ExceptionHandler::Register<ThrowException, TestException>(
        std::make_unique<test::CountingNoThrow>(counter)
    );

    QueueImpl q;
    q.Push(std::move(throwCmd)); // will throw
    ASSERT_EQ(1, q.Size());

    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_TRUE(q.IsEmpty());
    EXPECT_EQ(counter, 1);
}

TEST(LogErrorTest, CreatesLogFile) {
    const fs::path logPath = fs::temp_directory_path() / "exceptions_log_test.log";

    if (fs::exists(logPath)) fs::remove(logPath);
    EXPECT_FALSE(fs::exists(logPath));

    LogErrorCommand cmd("Test", logPath.string());
    cmd.Execute();

    EXPECT_TRUE(fs::exists(logPath));
    if (fs::exists(logPath)) fs::remove(logPath);
}

TEST(RegisteredHandler, HandlerEnqueuesLogErrorCmd) {
    auto throwCmd = std::make_unique<ThrowException>();
    auto exception = std::make_unique<TestException>();

    const fs::path logPath = fs::temp_directory_path() / "exceptions_log_test.log";
    if (fs::exists(logPath)) fs::remove(logPath);
    EXPECT_FALSE(fs::exists(logPath));

    LogErrorCommand cmd{"Test", logPath.string()};

    QueueImpl q;
    ExceptionHandler::Register<ThrowException, TestException>(
        std::make_unique<EnqueueCommand>(q, cmd)
    );

    q.Push(std::move(throwCmd)); // will throw
    ASSERT_EQ(1, q.Size());

    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_TRUE(q.IsEmpty());

    EXPECT_TRUE(fs::exists(logPath));
    if (fs::exists(logPath)) fs::remove(logPath);
}

TEST(RepeatCmdTest, EnqueCmdThatRepeatThrowCommand) {
    int counter{1};
    auto throwCmd = std::make_unique<test::CountingWithThrow>(counter);
    auto exception = std::make_unique<TestException>();
    auto repeatCmd = std::make_unique<RepeatCommand>(*throwCmd);
    QueueImpl q;
    ExceptionHandler::Register<test::CountingWithThrow, TestException>(
        std::make_unique<EnqueueCommand>(q, *repeatCmd)
    );

    q.Push(std::move(repeatCmd));
    ASSERT_EQ(1, q.Size());

    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_EQ(2, counter);
}
 
// 8. Implement scenario: 
// repeat cmd after throw once and log error in case if repeat throws either
TEST(RepeatCmdTest, EnqueCmdThatRepeatsOnceAndLogCommand) {
    int counter{0};
    auto throwCmd = std::make_unique<test::CountingWithThrow>(counter);
    auto exception = std::make_unique<TestException>();
    auto repeatCmd = std::make_unique<RepeatCommand>(*throwCmd);

    QueueImpl q;
    q.Push(throwCmd->Clone());

    ExceptionHandler::Register<test::CountingWithThrow, TestException>(
        std::make_unique<EnqueueCommand>(q, *repeatCmd)
    );

    const fs::path logPath = fs::temp_directory_path() / "exceptions_log_test.log";
    if (fs::exists(logPath)) fs::remove(logPath);
    EXPECT_FALSE(fs::exists(logPath));

    LogErrorCommand logCmd{"Log after 1st repeat throws", logPath.string()};

    ExceptionHandler::Register<RepeatCommand, TestException>(
        std::make_unique<EnqueueCommand>(q, logCmd)
    );

    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_TRUE(q.IsEmpty());
    EXPECT_EQ(2, counter); // cmd was executed two times

    EXPECT_TRUE(fs::exists(logPath));
    if (fs::exists(logPath)) fs::remove(logPath);
}

TEST(RepeatCmdTest, EnqueCmdThatRepeatsTwiceAndLogCommand) {
    int counter{0};
    auto throwCmd = std::make_unique<test::CountingWithThrow>(counter);
    auto exception = std::make_unique<TestException>();
    auto repeatCmd = std::make_unique<RepeatCommand>(*throwCmd);
    auto repeatTwiceCmd = std::make_unique<RepeatTwiceCommand>(*throwCmd);

    QueueImpl q;
    q.Push(throwCmd->Clone());

    // 1st throw -> RepeatTwiceCommand
    ExceptionHandler::Register<test::CountingWithThrow, TestException>(
        std::make_unique<EnqueueCommand>(q, *repeatTwiceCmd)
    );

    // 2nd throw -> RepeatCommand
    ExceptionHandler::Register<RepeatTwiceCommand, TestException>(
        std::make_unique<EnqueueCommand>(q, *repeatCmd)
    );

    const fs::path logPath = fs::temp_directory_path() / "exceptions_log_test.log";
    if (fs::exists(logPath)) fs::remove(logPath);
    EXPECT_FALSE(fs::exists(logPath));

    LogErrorCommand logCmd{"Log after 2nd repeat throws", logPath.string()};

    // 3rd throw -> LogErrorCommand
    ExceptionHandler::Register<RepeatCommand, TestException>(
        std::make_unique<EnqueueCommand>(q, logCmd)
    );

    EXPECT_NO_THROW(cmd_loop::run(q));
    EXPECT_TRUE(q.IsEmpty());
    EXPECT_EQ(3, counter); // cmd was executed three times

    EXPECT_TRUE(fs::exists(logPath));
    if (fs::exists(logPath)) fs::remove(logPath);
}