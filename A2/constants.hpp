#pragma once

#include <glm/glm.hpp>

#include <utility>
#include <vector>

typedef std::pair<glm::vec4, glm::vec4> line4;
typedef std::pair<glm::vec2, glm::vec2> line2;
typedef glm::vec3 colour;

// colours
const colour c_white = {1.0f, 1.0f, 1.0f};
const colour c_red = {1.0f, 0.0f, 0.0f};
const colour c_green = {0.0f, 1.0f, 0.0f};
const colour c_blue = {0.0f, 0.0f, 1.0f};
const colour c_cyan = {0.0f, 1.0f, 1.0f};
const colour c_magenta = {1.0f, 0.0f, 1.0f};
const colour c_yellow = {1.0f, 1.0f, 0.0f};


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
