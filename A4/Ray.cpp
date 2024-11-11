#include "Ray.hpp"
// constructors
Ray::Ray(const glm::dvec4 &eye, const glm::dvec4 &pixel) : m_eye{eye}, m_pixel{pixel} {}

// member functions
glm::dvec4 Ray::getEyePoint() const
{
    return m_eye;
}

glm::dvec4 Ray::getPixelPoint() const
{
    return m_pixel;
}

glm::dvec4 Ray::getDirection() const
{
    return m_pixel - m_eye;
}
