#pragma once

#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <vector>

#include "Image.hpp"


struct UVLookup {
    UVLookup();
    UVLookup(const glm::dvec2 & coordinates, const unsigned int & texIndex);

    glm::dvec2 m_coordinates = {0.0, 0.0};
    unsigned int m_texIndex = 0;
};

/**
 *  desc:
 *  - This structure stores a vector of images representing textures.
 *    Users of the map can make use of the multiple images to
 *    map different sides of an object (for example the six sides of
 *    a cube).
 */
struct UVMap {
    std::vector<Image *> m_textures;

    std::optional<glm::dvec3> getValue(const UVLookup & uvLookup) const;
};

// might need to swizzle y and z for normals
std::optional<glm::dvec3> lookup(const UVMap & map,
                                 const std::optional<UVLookup> & lookup,
                                 bool swizzleYZ = false);


// -----------------------------------------
// sphere radius 1
glm::dvec2
getUnitSphereUVMap(const glm::dvec3 & coord);

glm::dvec2 getUnitSphereUVMap(const glm::dvec4 & coord);

glm::dvec2
getUnitCubeFaceUVMap(const glm::dvec3 & coord, const std::vector<glm::dvec4> & face, double width = 1);

// cylinder radius 1, spanning from y = -1 to y = 1
glm::dvec2
getCylinderUVMap(const glm::dvec2 & coord);

glm::dvec2
getUnitCircleUVMap(const glm::dvec2 & coord);
