#pragma once

#include <optional>
#include <utility>
#include <vector>

#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Ray.hpp"
#include "Intersection.hpp"

/* desc:
 *
 * return:
 *    - optional pair of:
 *      - t (time for parameterized line function)
 *      - normal vec
 */
std::optional<Intersection>
findRaySphereIntersection(const Ray &ray,
                                const glm::dvec4 &centre = {0.0, 0.0, 0.0, 1.0},
                                const double &radius = 1.0);

bool doesRayIntersectSphere(const Ray & ray,
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
std::optional<Intersection>
findRayBoxIntersection(const Ray & ray,
                             const glm::dvec4 &corner = {0.0, 0.0, 0.0, 1.0},
                             const double &width = 1.0);

std::optional<Intersection>
findRayMeshIntersection(const Ray & ray, const Mesh & mesh);

/* desc:
 *
 * paramaters:
 *  - vertices
 *    polygon vertices in CCW order, should have >= 3 vertices
 */
std::optional<Intersection>
findRayPolygonIntersection(const Ray & ray,
                                 const std::vector<glm::dvec4> &vertices);

std::optional<double> findRayPlaneIntersect(const Ray & ray,
                                            const glm::dvec4 &planeNormal,
                                            const glm::dvec4 &planePoint);

inline bool doesRayIntersectPlane(const Ray & ray,
                                  const glm::dvec4 &planeNormal)
{
    return !(glm::dot(ray.getDirection(), planeNormal) == 0.0);
}
