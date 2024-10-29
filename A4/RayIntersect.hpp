#pragma once

#include <optional>
#include <utility>

#include <glm/glm.hpp>

/* desc:
 *
 *  return:
 *    - optional pair of:
 *      - t (time for parameterized line function)
 *      - normal vec
 */
std::optional<std::pair<double, glm::dvec4>>
findRaySphereIntersectAndNormal(const glm::dvec4 & eye, const glm::dvec4 & pixel,
                                const double & radius = 1.0,
                                const glm::dvec4 & centre = {0.0, 0.0, 0.0, 1.0});

bool doesRayIntersectSphere(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                            const double &radius = 1.0,
                            const glm::dvec4 &centre = {0.0, 0.0, 0.0, 1.0});

// evaluates the ray forula:
// eye + t(pixel - eye)
inline glm::dvec4 evaluateRay(const glm::dvec4 & eye, const glm::dvec4 & pixel, const double & t);
