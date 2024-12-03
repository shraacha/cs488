#include "Camera.hpp"

Camera::Camera(){}

Camera::Camera(const glm::dvec3 & eye, const glm::dvec3 & view,
               const glm::dvec3 & up, double fovy)
    : m_eye(eye), m_view(view), m_up(up), m_fovy(fovy)
{
}
