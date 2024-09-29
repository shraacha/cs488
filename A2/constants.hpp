#pragma once

#include <cmath>
#include <glm/glm.hpp>

#include <utility>
#include <vector>

typedef std::pair<glm::vec4, glm::vec4> line4;
typedef std::pair<glm::vec2, glm::vec2> line2;
typedef glm::vec3 colour;

const float SQRT1_3 = 0.57735026919f;

// colours
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

// this is the camera-to-world (world assumed to be standard frame) change of frame matrix.
//  - the cam x axis is perp. to the world y axis, at [1/sqrt(2), 0, -1/sqrt(2)]
//  - the cam y axis is the cross prod. of the z & x axes, at [-1/sqrt(6), sqrt(2)/sqrt(3), -1/sqrt(6)]
//  - the cam z axis is at [1/sqrt(3), 1/sqrt(3), 1/sqrt(3)]
//
//  - the camera is placed at [4, 4, 4]
// column-wise
const float c_cameraToWorldMatrix[16] =
{M_SQRT1_2f, 0, -M_SQRT1_2f, 0,
- M_SQRT1_2f * SQRT1_3 , M_SQRT2f * SQRT1_3 , - M_SQRT1_2f * SQRT1_3, 0,
SQRT1_3 , SQRT1_3, SQRT1_3, 0,
4, 4, 4, 1
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

const std::vector<line4> c_unitGnomonLines = {
    // bottom face
    {{0.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
    {{0.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}}};
