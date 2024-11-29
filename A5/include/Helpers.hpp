#pragma once

#include <cstdlib>

template<typename T>
inline auto clampValue(const T & value, const T & upper, const T & lower) -> T
{
    return (value <= upper) ? ((value >= lower) ? value : lower) : upper;
}

inline double getRand0To1 ()
{
    return (double)rand() / (double)RAND_MAX;
}
