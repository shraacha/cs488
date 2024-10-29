#pragma once

#include <glm/glm.hpp>

#include "Image.hpp"

inline void setPixelColour(Image & image, const uint &x, const uint &y,
                           const double &r, const double &g, const double &b)
{
    image(x, y, 0) = r; // Red:
    image(x, y, 1) = g; // Green:
    image(x, y, 2) = b; // Blue:
}

inline void setPixelColour(Image & image, const uint &x, const uint &y,
                           const glm::dvec3 & colour)
{
    image(x, y, 0) = colour.x; // Red:
    image(x, y, 1) = colour.y; // Green:
    image(x, y, 2) = colour.z; // Blue:
}
