#pragma once

#include <cmath>
#include <numbers>

template<typename T>
T wrap_2pi(T angle) {
    constexpr T TWO_PI = std::numbers::pi_v<T> * 2.0;
    angle = std::fmod(angle, TWO_PI);
    if (angle < 0) angle += TWO_PI;
    return angle;
}

template<typename T>
T wrap_pi(T angle) {
    constexpr T PI = std::numbers::pi_v<T>;
    angle = std::fmod(angle + PI, 2 * PI) - PI;
    return angle < 0 ? angle + PI : angle - PI;
}