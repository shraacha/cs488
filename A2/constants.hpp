#pragma once

#include <cmath>
#include <glm/glm.hpp>

#include <utility>
#include <vector>

#include "customTypes.hpp"

const float SQRT1_3 = 0.57735026919f;

// colours
const colour c_black = {0.0f, 0.0f, 0.0f};
const colour c_white = {1.0f, 1.0f, 1.0f};
const colour c_red = {1.0f, 0.0f, 0.0f};
const colour c_green = {0.0f, 1.0f, 0.0f};
const colour c_blue = {0.0f, 0.0f, 1.0f};
const colour c_cyan = {0.0f, 1.0f, 1.0f};
const colour c_magenta = {1.0f, 0.0f, 1.0f};
const colour c_yellow = {1.0f, 1.0f, 0.0f};

// points
const glm::vec3 c_initalCameraTsl = {4.0f, 4.0f, 4.0f};
const glm::vec3 c_initalCameraRot = {-M_PI/4, M_PI/4, 0.0f};

const glm::mat4 c_defaultModelScaleMatrix = glm::mat4();
const glm::mat4 c_defaultModelToWorldMatrix = glm::mat4();

// this is the camera-to-world (world assumed to be standard frame) change of frame matrix.
//  - the cam x axis is perp. to the world y axis, at [1/sqrt(2), 0, -1/sqrt(2)]
//  - the cam y axis is the cross prod. of the z & x axes, at [-1/sqrt(6), sqrt(2)/sqrt(3), -1/sqrt(6)]
//  - the cam z axis is at [1/sqrt(3), 1/sqrt(3), 1/sqrt(3)]
//
//  - the camera is placed at [4, 4, 4]
// column-wise
const float c_defaultCameraToWorldMatrix[16] =
{M_SQRT1_2f, 0, -M_SQRT1_2f, 0,
- M_SQRT1_2f * SQRT1_3 , M_SQRT2f * SQRT1_3 , - M_SQRT1_2f * SQRT1_3, 0,
SQRT1_3 , SQRT1_3, SQRT1_3, 0,
5, 5, 5, 1
};

const float c_simpleCameraToWorldMatrix[16] =
{1, 0, 0, 0,
 0, 1, 0, 0,
 0, 0, 1, 0,
0, 0, 6, 1
};

// lines

// The vertices of the cube are at:
// (-1.0f, -1.0f, -1.0f)
// (-1.0f, -1.0f, 1.0f)
// (-1.0f, 1.0f, -1.0f)
// (-1.0f, 1.0f, 1.0f)
// (1.0f, -1.0f, -1.0f)
// (1.0f, -1.0f, 1.0f)
// (1.0f, 1.0f, -1.0f)
// (1.0f, 1.0f, 1.0f)
const std::vector<line4> c_cubeLines = {
    // bottom face
    {{-1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f, 1.0f}},
    {{-1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f, 1.0f}},
    {{1.0f, -1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}},
    {{1.0f, -1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f, 1.0f}},
    // top face
    {{-1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, 1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, -1.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, 1.0f, -1.0f, 1.0f}},
    // middle layer
    {{-1.0f, 1.0f, -1.0f, 1.0f}, {-1.0f, -1.0f, -1.0f, 1.0f}},
    {{-1.0f, 1.0f, 1.0f, 1.0f}, {-1.0f, -1.0f, 1.0f, 1.0f}},
    {{1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, -1.0f, 1.0f, 1.0f}},
    {{1.0f, 1.0f, -1.0f, 1.0f}, {1.0f, -1.0f, -1.0f, 1.0f}}};

const line4 c_unitLineX = {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}};
const line4 c_unitLineY = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}};
const line4 c_unitLineZ = {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}};

const glm::vec4 c_unitX = {1.0f, 0.0f, 0.0f, 1.0f};
const glm::vec4 c_unitY = {0.0f, 1.0f, 0.0f, 1.0f};
const glm::vec4 c_unitZ = {0.0f, 0.0f, 1.0f, 1.0f};

const glm::vec4 c_standardBasisX = {1.0f, 0.0f, 0.0f, 0.0f};
const glm::vec4 c_standardBasisY = {0.0f, 1.0f, 0.0f, 0.0f};
const glm::vec4 c_standardBasisZ = {0.0f, 0.0f, 1.0f, 0.0f};

const float c_defaultNearDistance = 3.0f;
const float c_defaultFarDistance = 9.0f;

const float c_minNearFarDistance = 2.0f;
const float c_maxNearFarDistance= 30.0f;

const float c_defaultFOV = M_PI / 6.0;
const float c_minFOV = M_PI / 36.0f;  // 5 degrees
const float c_maxFOV = M_PI * (8.0f / 9.0f); // 160 degrees

const glm::vec3 c_defaultModelScale = {1.0f, 1.0f, 1.0f};

const float c_minScale = 0.0f;
const float c_maxScale = 40.0f;

const float c_defaultViewPortion = 0.9;

const InteractionMode c_defaultInteractionMode = InteractionMode::RotateModel;

const float c_fullWindowRotation = M_PI;
