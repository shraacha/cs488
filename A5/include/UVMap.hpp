#pragma once

#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "Image.hpp"


struct UVMap {
    std::vector<std::shared_ptr<Image>> textures;
};

// sphere radius 1
glm::dvec2
uvMapSphere(const glm::dvec3 & coord);

glm::dvec2 uvMapSphere(const glm::dvec4 & coord);
