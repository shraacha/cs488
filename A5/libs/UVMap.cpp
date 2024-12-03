#include "UVMap.hpp"

#include <cmath>
#include <optional>

#include <glm/ext.hpp>

#include "ImageSampleHelpers.hpp"
#include "Debug.hpp"
#include "glm/gtc/quaternion.hpp"


// --------------------------------------------------------
UVLookup::UVLookup() {}

UVLookup::UVLookup(const glm::dvec2 & coordinates,
                   const unsigned int & texIndex)
    : m_coordinates(coordinates), m_texIndex(texIndex)
{
}

// --------------------------------------------------------
std::optional<glm::dvec3> UVMap::getValue(const UVLookup & uvLookup) const
{
    if (uvLookup.m_texIndex < m_textures.size()) {
        return sample(*m_textures[uvLookup.m_texIndex], uvLookup.m_coordinates);
    } else {
        return std::nullopt;
    }
}

std::optional<glm::dvec3> lookup(const UVMap & map, const std::optional<UVLookup> & lookup, bool swizzleYZ)
{
    if (lookup)
    {
        if (auto value = map.getValue(lookup.value()); value)
        {
            if (swizzleYZ)
            {
                return glm::dvec3(value.value().x, value.value().z, value.value().y);
            }
            else
            {
                return value.value();
            }
        }
    }

    return std::nullopt;
}


// --------------------------------------------------------
glm::dvec2 getUnitSphereUVMap(const glm::dvec3 & coord)
{
    double theta = acos(coord.y) * M_1_PI; // theta goes from 0 to pi
    double phi = (atan2(coord.x, coord.z) + M_PI) / (2 * M_PI); // phi goes from 0 to 2pi

    return {phi, theta};
}

glm::dvec2 getUnitSphereUVMap(const glm::dvec4 & coord)
{
    return getUnitSphereUVMap(glm::dvec3(coord));
}

glm::dvec2
getUnitCubeFaceUVMap(const glm::dvec3 & coord, const std::vector<glm::dvec4> & face, double width)
{
    double scale = 1 / width;
    // if operating on the top corner, we will end up with possibly negative vals
    glm::dvec3 distFromCorner = glm::abs(coord - glm::dvec3(face[0]));
    glm::dvec2 uv;

    if (distFromCorner.x == 0) {
        uv = glm::dvec2{distFromCorner.y, distFromCorner.z} * scale;
    } else if (distFromCorner.y == 0) {
        uv = glm::dvec2{distFromCorner.z, distFromCorner.x} * scale;
    } else {
        uv = glm::dvec2{distFromCorner.x, distFromCorner.y} * scale;
    }

    return glm::clamp(uv, glm::dvec2(0), glm::dvec2(1));
}
