#pragma once
#include <cmath>
#include <stdexcept>
#include <vector>

namespace square_roots {

using result_t = std::vector<double>;

inline result_t solve(double a, double b, double c) {
    constexpr double epsilon{1e-9};
    auto throw_if_nan = [] (double x, const char* what) {
        if (std::isnan(x)) throw std::runtime_error(what);
    };

    throw_if_nan(a, "quadratic coefficient is NaN");
    throw_if_nan(b, "linear coefficient is NaN");
    throw_if_nan(c, "constant is NaN");

    auto throw_if_not_finite = [] (double x, const char* what) {
        if (!std::isfinite(x)) throw std::runtime_error(what);
    };

    throw_if_not_finite(a, "quadratic coefficient is not finite");
    throw_if_not_finite(b, "linear coefficient is not finite");
    throw_if_not_finite(c, "constant is not finite");

    if (std::abs(a) < epsilon) {
        throw std::runtime_error("quadratic coefficient is zero");
    }

    const double discriminant = b * b - 4.0 * a * c;

    if (std::isnan(discriminant)) {
        throw std::runtime_error("discriminant is NaN");
    }

    if (!std::isfinite(discriminant)) {
        throw std::runtime_error("discriminant is not finite");
    }

    if (std::abs(discriminant) < epsilon) { // one real root
        return result_t{-b / (2.0 * a)};
    }

    if (discriminant < 0.0) { // no real roots
        return result_t{};
    }

    const double sqrt_d{std::sqrt(discriminant)};
    double r1 = (-b + sqrt_d) / (2.0 * a);
    double r2 = (-b - sqrt_d) / (2.0 * a);

    if (r1 > r2) std::swap(r1, r2);
    return result_t{r1, r2};
}

} // namespace square_roots
