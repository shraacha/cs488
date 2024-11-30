#pragma once

#include <cmath>
#include <vector>

#include <glm/glm.hpp>
#include "cs488-framework/MathUtils.hpp"

#include "Helpers.hpp"

// paramaeters:
// - fovy
//   fov in DEGREES
inline double getScreenDepth(const double & height, const double & fovy)
{
  return (height / 2) / std::tan(degreesToRadians(fovy) / 2);
}


// paramaeters:
// - flip
//   whether to flip the position, this is needed when supplying an device coordinate index
inline double getScreenPosition(const double &sideLength,
                                       const uint &index, const bool &flip = false)
{
    if (flip)
    {
        return - (double)index + sideLength / 2 - 0.5;
    } else {
        return (double)index - sideLength / 2 + 0.5;
    }
}

inline std::vector<glm::dvec4>
generateSubScreenPositions(const glm::vec4 &screenPosition, unsigned int wSubDivs = 1,
                           unsigned int hSubDivs = 1, bool jitter = true) {
    std::vector<glm::dvec4> positions;

    glm::vec4 bottomLeft(screenPosition.x - 0.5, screenPosition.y - 0.5, screenPosition.z, 1.0);

    double wStep = 1.0 / (double)(wSubDivs);
    double hStep = 1.0 / (double)(hSubDivs);

    double wHalfStep = wStep / 2.0;
    double hHalfStep = hStep / 2.0;

    glm::vec4 startLocation(bottomLeft.x + wHalfStep, bottomLeft.y + hHalfStep, bottomLeft.z, bottomLeft.w);
    double row = startLocation.y;
    double startCol = startLocation.x;

    for (unsigned int i = 0; i < hSubDivs; ++i) {
        double col = startCol;
        for (unsigned int j = 0; j < wSubDivs; ++j) {
            double wJitter = jitter ? ((static_cast<double>(std::rand()) / RAND_MAX) * wStep) - wHalfStep : 0;
            double hJitter = jitter ? ((static_cast<double>(std::rand()) / RAND_MAX) * hStep) - hHalfStep : 0;

            positions.emplace_back(col + hJitter, row + wJitter, startLocation.z,  startLocation.w);

            row += wStep;
        }
        col += hStep;
    }

    return positions;
}
