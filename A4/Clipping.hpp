#pragma once

#include <glm/glm.hpp>

//  Description
//  - returns (Q - P) * n, where P is a point on the line,
//  n is normal to the line, and Q is the point being tested.
template<typename T>
inline double implicitLineEquation(const T & Q, const T & P, const T & n) {
    return glm::dot(Q - P, n);
}

inline bool isInBoundsClip(const glm::dvec2 &point, const glm::dvec2 &linePoint,
                    const glm::dvec2 &lineNormal)
{
    return implicitLineEquation(point, linePoint, lineNormal) >= 0;
}
