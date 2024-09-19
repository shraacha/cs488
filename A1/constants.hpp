#pragma once

#include <array>

const int c_minWallHeight = 0;
const int c_maxWallHeight = 10;
const int c_defaultWallHeight = 1;
const int c_wallHeightStepSize = 1;
const double c_distanceToAngleScale = 0.0025;

const std::array<float, 3> c_defaultFloorColour = {16/255.0, 11/255.0, 0/255.0};
const std::array<float, 3> c_defaultWallColour = {255/255.0, 224/255.0, 181/255.0};
const std::array<float, 3> c_defaultAvatarColour =  {202/255.0, 46/255.0, 85/255.0};
const std::array<float, 3> c_blackColour = {0,  0, 0};

const float c_worldScaleStep = 1;
const float c_camMinDist = 15;
const float c_camMaxDist = 150;

const std::array<float, 24> c_unitCubeVertices = {
    0, 0, 0,
    0, 0, 1,
    0, 1, 0,
    0, 1, 1,
    1, 0, 0,
    1, 0, 1,
    1, 1, 0,
    1, 1, 1
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
    4, 5, 1
};

const std::array<float, 24> c_unitSquareVertices = {
    0, 0, 1,
    1, 0, 0,
    0, 0, 0,
    0, 0, 1,
    1, 0, 1,
    1, 0, 0
};
