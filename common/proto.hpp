#pragma once
#include <cstdint>

enum class PidVariableType : uint8_t {
    Proportional = 0,
    Integral = 1,
    Derivative = 2,
    WindUp = 3
};

enum class WorkMode : uint8_t {
    Stopped = 0,
    CurrentSource = 1,
    TemperatureStab = 2,
    Debug = 3
};
