#include <gtest/gtest.h>
#include <square_roots.hpp>

TEST(SquareRoots, NoRoots) {
    const auto res = square_roots::solve(1., 0., 1.);
    const auto expected = std::vector<double>{};
    EXPECT_EQ(expected, res);
}

TEST(SquareRoots, OneRoot) {
    const auto res = square_roots::solve(1., 2., 0.999'999'999'9);
    const auto expected = std::vector<double>{-1.};
    EXPECT_EQ(expected, res);
}

TEST(SquareRoots, TwoRoots) {
    const auto res = square_roots::solve(1., 0., -1.);
    const auto expected = std::vector<double>{-1., 1.};
    EXPECT_EQ(expected, res);
}

struct SolveParam {
    double a{};
    double b{};
    double c{};
    const char* what{};
    constexpr static const double nan{std::numeric_limits<double>::quiet_NaN()};
    constexpr static const double pinf{std::numeric_limits<double>::infinity()};
    constexpr static const double ninf{-std::numeric_limits<double>::infinity()};
};

class SquareRootsExceptionsTest : public ::testing::TestWithParam<SolveParam> {};

TEST_P(SquareRootsExceptionsTest, ThrowsRuntimeErrorWithExpectedMessage) {
    const auto p = GetParam();
    try {
        const auto res = square_roots::solve(p.a, p.b, p.c);
        FAIL() << "std::runtime_error expected";
    } catch (const std::runtime_error& ex) {
        EXPECT_STREQ(p.what, ex.what());
    } catch (...) {
        FAIL() << "std::runtime_error expected";
    }
}

INSTANTIATE_TEST_SUITE_P(
    ExceptionsTest,
    SquareRootsExceptionsTest,
    ::testing::Values(
        SolveParam{0., 1.0, 1.0, "quadratic coefficient is zero"},

        SolveParam{SolveParam::nan, 1.0, 1.0, "quadratic coefficient is NaN"},
        SolveParam{1.0, SolveParam::nan, 1.0, "linear coefficient is NaN"},
        SolveParam{1.0, 1.0, SolveParam::nan, "constant is NaN"},

        SolveParam{SolveParam::pinf, 1.0, 1.0, "quadratic coefficient is not finite"},
        SolveParam{1.0, SolveParam::pinf, 1.0, "linear coefficient is not finite"},
        SolveParam{1.0, 1.0, SolveParam::pinf, "constant is not finite"},

        SolveParam{SolveParam::ninf, 1.0, 1.0, "quadratic coefficient is not finite"},
        SolveParam{1.0, SolveParam::ninf, 1.0, "linear coefficient is not finite"},
        SolveParam{1.0, 1.0, SolveParam::ninf, "constant is not finite"},

        SolveParam{1.0, 1e200, 1e200, "discriminant is not finite"},
        SolveParam{1e155, 1e155, 1e155, "discriminant is NaN"}
    )
);
