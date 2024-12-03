#include "Ray.hpp"

static inline double calculateThreshold (double length) {
    double epsilon = 0.005;
    return epsilon / length;
}

// constructors
Ray::Ray(const glm::dvec4 & eye, const glm::dvec4 & pixel)
    : m_eye{eye}, m_pixel{pixel}
{
    m_minThreshold = calculateThreshold(glm::length(pixel - eye));
}

Ray::Ray(const glm::dvec4 & eye, const glm::dvec3 & direction)
    : m_eye{eye}, m_pixel{eye + glm::dvec4(direction, 0.0)}
{
    m_minThreshold = calculateThreshold(glm::length(direction));
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

glm::dvec3 Ray::getNormalizedDirection() const
{
    return glm::normalize(glm::dvec3(getDirection()));
}
