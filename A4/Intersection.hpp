#pragma once

#include <glm/glm.hpp>

class Intersection {
    double m_t;
    glm::dvec4 m_normal;

    public:
        Intersection();
        Intersection(const double & t, const glm::dvec4 normal);

        double getT();
        glm::dvec4 getNormal();
};
