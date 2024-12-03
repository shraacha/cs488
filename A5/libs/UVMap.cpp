#include "UVMapHelpers.hpp"

#include <cmath>

glm::dvec2 uvMapSphere(const glm::dvec3 & coord)
{
    double theta = acos(coord.y) * M_1_PI; // theta goes from 0 to pi
    double phi = (atan2(coord.x, coord.z) + M_PI) / (2 * M_PI); // phi goes from 0 to 2pi

    return {phi, theta};
}

glm::dvec2 uvMapSphere(const glm::dvec4 & coord)
{
    return uvMapSphere(glm::dvec3(coord));
}
