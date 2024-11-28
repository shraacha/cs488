// Termm--Fall 2024

#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << glm::to_string(l.colour) 
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  }
  out << "]";
  return out;
}

double Light::calculateAttenuation( const glm::dvec3 & surfacePosition) const
{
    glm::dvec3 lightVector = glm::dvec3(position) - surfacePosition;

    double rSquared = glm::dot(lightVector, lightVector);

    return glm::dot(
        glm::dvec3(falloff[0], falloff[1], falloff[2]),
        glm::dvec3(1.0, sqrt(rSquared), rSquared));
}
