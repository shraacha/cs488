#include "GLMHelpers.hpp"

glm::dmat3 getChangeOfBasis(glm::dvec3 up, glm::dvec3 coplanar)
{
    if (acos(glm::dot(up, coplanar))) {
        coplanar = -coplanar;
    }

    glm::dvec3 z = glm::normalize(glm::cross(up, coplanar));
    glm::dvec3 x = glm::normalize(glm::cross(up, z));

    // column-wise
    glm::dmat3 changeOfBasis{x.x, x.y, x.z,
                             up.x, up.y, up.z,
                             z.x, z.y, z.z};

    return changeOfBasis;
}

glm::dvec3 perturbVector(const glm::dvec3 & target,
                         const glm::dvec3 & coplanarTarget,
                         const glm::dvec3 & source)
{
    glm::dmat3 sourceToTarget = getChangeOfBasis(target, coplanarTarget);
    return glm::normalize(sourceToTarget * source);
}
