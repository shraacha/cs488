#include "Intersection.hpp"

// constructors
Intersection::Intersection(const double &t, const glm::dvec4 normal): m_t{t}, m_normal{normal} {}

// member functions
double Intersection::getT() { return m_t; }

glm::dvec4 Intersection::getNormal()
{
    return m_normal;
}
