// Termm--Fall 2024

#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

#include "Ray.hpp"

// Represents a simple point light.
struct Light {
    Light();

    glm::vec3 colour;
    glm::vec3 position;
    double falloff[3];

    double calculateAttenuation(const glm::dvec3 & surfacePosition) const;

    std::pair<Ray, double> sampleRay() const;
};

std::ostream & operator<<(std::ostream & out, const Light & l);
