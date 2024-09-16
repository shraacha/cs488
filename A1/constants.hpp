#pragma once

#include <array>

const std::array<float, 24> c_unitCubeVertices = {
    0, 0, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    1, 1, 1,
};

// 0-indexed
const std::array<unsigned int, 36> c_unitCubeIndices = {
    // front
    3, 1, 5,
    5, 7, 3,
    // right
    7, 5, 4,
    4, 6, 7,
    // back
    6, 4, 0,
    0, 2, 6,
    // left
    2, 0, 1,
    1, 3, 2,
    // top
    2, 3, 7,
    7, 6, 2,
    // bottom
    1, 0, 4,
    4, 5, 1,
};
