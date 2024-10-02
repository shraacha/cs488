#pragma once

#include <glm/glm.hpp>

#include "constants.hpp"

struct Viewport {
    float x, y, L, H;
};

// returns viewport transformation matrix from V to W
glm::mat4 makeViewportTransformationMatrix(const Viewport & V, const Viewport & W);

std::vector<line4> getViewportExplicitEdges(const Viewport & V);

std::vector<pointAndNormal> getViewportImplicitEdges(const Viewport & V);

// assume 0 < portion <= 1
line4 makeDeviceViewportCornersFromPortion(const float & width, const float & height, const float & portion);

Viewport makeDeviceViewport(const float & width, const float & height);

Viewport makeNDCViewportFromCorners(const line4 & corners);

inline Viewport getNDCViewport()
{
    return {-1.0f, 1.0f, 2.0f, -2.0f};
}
