#pragma once
#include <glm/glm.hpp>

inline glm::dvec3 maxWithZero(const glm::dvec3 & vec)
{
    glm::dvec3 returnVec(vec);

    if (vec.x < 0) {
        returnVec.x = 0;
    }

    if (vec.y < 0) {
        returnVec.y = 0;
    }

    if (vec.z < 0) {
        returnVec.z = 0;
    }

    return returnVec;
}
