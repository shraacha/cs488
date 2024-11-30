#pragma once

#include <cstdlib>

template<typename T>
inline auto clampValue(const T & value, const T & upper, const T & lower) -> T
{
    return (value <= upper) ? ((value >= lower) ? value : lower) : upper;
}

int intRand(const int & min, const int & max);

double getRand0To1();

double getRandNeg1To1();

int intRand(const int & min, const int & max);
