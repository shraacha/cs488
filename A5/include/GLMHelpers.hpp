#pragma once

#include <cmath>

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

glm::dmat3 getChangeOfBasis(glm::dvec3 up, glm::dvec3 coplanar);

// perturb source vector to target's frame
glm::dvec3 perturbVector(const glm::dvec3 & target,
                         const glm::dvec3 & coplanarTarget,
                         const glm::dvec3 & source);

// y up, -z forward
inline glm::dvec3 sphericalToCartesianYUp(const double &theta, const double &phi) {
    return glm::dvec3(sin(phi) * sin(theta), cos(theta), cos(phi) * sin(theta));
};
