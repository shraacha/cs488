#include "Ray.hpp"
// constructors
Ray::Ray(const glm::dvec4 &eye, const glm::dvec4 &pixel) : m_eye{eye}, m_pixel{pixel}
{
    double epsilon = 0.05;
    m_minThreshold = epsilon / glm::length(pixel - eye);
}

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

double Ray::getMinThreshold() const
{
    return m_minThreshold;
}
