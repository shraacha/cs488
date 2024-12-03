#pragma once

#include <glm/glm.hpp>

#include "Image.hpp"

glm::dvec3 sample(const Image & image, const glm::dvec2 & coordinate);
