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
std::optional<std::pair<double, glm:;vec3>> findRaySphereIntersectAndNormal();
