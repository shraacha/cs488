#pragma once

#include <glm/glm.hpp>

enum class VertexOrder { CCW, CW };

//  Description
//  - returns (Q - P) * n, where P is a point on the line,
//  n is normal to the line, and Q is the point being tested.
template <typename T>
inline double implicitLineEquation(const T & Q, const T & P, const T & n) {
    return glm::dot(Q - P, n);
}

//  Description
//  - clips in CCW order
inline bool isInBoundsClip(const glm::dvec2 & point,
                           const glm::dvec2 & linePoint,
                           const glm::dvec2 & lineNormal,
                           const VertexOrder & vertexOrder) {
    bool ret;

    switch (vertexOrder) {
    case (VertexOrder::CCW):
        ret = implicitLineEquation(point, linePoint, lineNormal) >= 0;
        break;
    case (VertexOrder::CW):
        ret = implicitLineEquation(point, linePoint, lineNormal) <= 0;
    }

    return ret;
}
