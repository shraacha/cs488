#include "Intersection.hpp"

// constructors
Intersection::Intersection(const double &t, const glm::dvec4 & normal): m_t{t}, m_normal{normal} {}

Intersection::Intersection(const double &t, const glm::dvec4 & position,
                           const glm::dvec4 & normal)
    : m_t{t}, m_position{position}, m_normal{normal} {}

// member functions
double Intersection::getT() const { return m_t; }

glm::dvec4 Intersection::getNormal() const
{
    return m_normal;
}

glm::dvec3 Intersection::getNormalizedNormal() const
{
    return glm::normalize(glm::dvec3(m_normal));
}

glm::dvec4 Intersection::getPosition() const
{
    return m_position;
}

void Intersection::setT(const double &t)
{
    m_t = t;
}

void Intersection::setNormal(const glm::dvec4 &normal)
{
    m_normal = normal;
}


void Intersection::setPosition(const glm::dvec4 &position)
{
    m_position = position;
}
