#pragma once

template<typename T>
inline auto clampValue(const T & value, const T & upper, const T & lower) -> T
{
    return (value <= upper) ? ((value >= lower) ? value : lower) : upper;
}
