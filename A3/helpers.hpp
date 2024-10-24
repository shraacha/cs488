#pragma once

inline float clampValue(const float & value, const float & upper, const float & lower)
{
    return (value <= upper) ? ((value >= lower) ? value : lower) : upper;
}

inline void toggleBoolInPlace(bool & val)
{
    val = val ? false : true;
}
