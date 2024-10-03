#pragma once

typedef std::pair<glm::vec4, glm::vec4> pointAndNormal;
typedef std::pair<glm::vec4, glm::vec4> line4;
typedef std::pair<glm::vec2, glm::vec2> line2;
typedef glm::vec3 colour;

enum InteractionMode {
    RotateView = 0,
    TranslateView = 1,
    Perspective = 2,
    RotateModel = 3,
    TranslateModel = 4,
    ScaleModel = 5,
    ViewportMode = 6
};
