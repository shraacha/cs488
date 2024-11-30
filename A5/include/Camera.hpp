#pragma once

#include <glm/glm.hpp>

struct Camera
{
    Camera();
    Camera(const glm::dvec3 & eye, const glm::dvec3 & view,
           const glm::dvec3 & up, double fovy);

    glm::dvec3 m_eye;
    glm::dvec3 m_view;
    glm::dvec3 m_up;
    double m_fovy;
};
