#pragma once

#include <glm/glm.hpp>

class Intersection {
    double m_t;
    glm::dvec4 m_position;
    glm::dvec4 m_normal;

    public:
        Intersection();
        Intersection(const double & t, const glm::dvec4 normal);
        Intersection(const double & t, const glm::dvec4 position, const glm::dvec4 normal);

        double getT() const;
        glm::dvec4 getNormal() const;
        glm::dvec3 getNormalizedNormal() const;
        glm::dvec4 getPosition() const;

        void setT(const double & t);
        void setNormal(const glm::dvec4 & normal);
        void setPosition(const glm::dvec4 & position);
};
