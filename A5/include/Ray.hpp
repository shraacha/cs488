#pragma once

#include <glm/glm.hpp>

#include "Intersection.hpp"

class Ray {
    glm::dvec4 m_eye;
    glm::dvec4 m_pixel;
    double m_minThreshold;

    public:
      Ray();
      Ray(const glm::dvec4 &eye, const glm::dvec4 &pixel);

      glm::dvec4 getEyePoint() const;
      glm::dvec4 getPixelPoint() const;
      glm::dvec4 getDirection() const;
      double getMinThreshold() const;
};

// evaluates the ray formula:
// eye + t(pixel - eye)
inline glm::dvec4 evaluate(const Ray & ray, const double & t)
{
    return ray.getEyePoint() + t * ray.getDirection();
}
