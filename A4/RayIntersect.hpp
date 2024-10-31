#pragma once

#include <optional>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

// evaluates the ray forula:
// eye + t(pixel - eye)
inline glm::dvec4 evaluateRay(const glm::dvec4 & eye, const glm::dvec4 & pixel, const double & t)
{
    return eye + t * (pixel - eye);
}

/* desc:
 *
 * return:
 *    - optional pair of:
 *      - t (time for parameterized line function)
 *      - normal vec
 */
std::optional<std::pair<double, glm::dvec4>>
findRaySphereIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                                const glm::dvec4 &centre = {0.0, 0.0, 0.0, 1.0},
                                const double &radius = 1.0);

bool doesRayIntersectSphere(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                            const glm::dvec4 &centre = {0.0, 0.0, 0.0, 1.0},
                            const double &radius = 1.0);

/* desc:
 *  Calculates the t value for a ray intersecting a box with one corner at
 * `corner` and the diagonally opposite corner at (corner.x + width, corner.y +
 * width, corner.z + width)
 *
 * return:
 *    - optional pair of:
 *      - t (time for parameterized line function)
 *      - normal vec
 */
std::optional<std::pair<double, glm::dvec4>>
findRayBoxIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                             const glm::dvec4 &corner = {0.0, 0.0, 0.0, 1.0},
                             const double &width = 1.0);

/* desc:
 *
 * paramaters:
 *  - vertices
 *    polygon vertices in CCW order, should have >= 3 vertices
 */
std::optional<std::pair<double, glm::dvec4>>
findRayPolygonIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                                 const std::vector<glm::dvec4> &vertices);

std::optional<double> findRayPlaneIntersect(const glm::dvec4 &eye,
                                            const glm::dvec4 &pixel,
                                            const glm::dvec4 &planeNormal,
                                            const glm::dvec4 &planePoint);

inline bool doesRayIntersectPlane(const glm::dvec4 &eye,
                                  const glm::dvec4 &pixel,
                                  const glm::dvec4 &planeNormal)
{
    return !(glm::dot(pixel - eye, planeNormal) == 0.0);
}
