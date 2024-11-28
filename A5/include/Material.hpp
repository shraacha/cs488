// Termm--Fall 2024

#pragma once

#include <vector>

#include "Ray.hpp"
#include "Intersection.hpp"
#include "Light.hpp"

#include <glm/glm.hpp>


enum MaterialTypeFlags
{
    Reflective = 1 << 0,
    Refractive = 1 << 1
};

class Material {
  public:
    virtual ~Material();

    virtual glm::dvec3 getRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::vec3 & ambient,
                           const std::vector<const Light *> & lights) const = 0;

    virtual glm::dvec3 getRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::vec3 & ambient,
                           const std::vector<const Light *> & lights,
                           const glm::dvec3 & reflectionDir,
                           const glm::dvec3 & reflectionRadiance) const = 0;

    virtual glm::dvec3 getRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::vec3 & ambient,
                           const std::vector<const Light *> & lights,
                           const glm::dvec3 & reflectionDir,
                           const glm::dvec3 & reflectionRadiance,
                           const glm::dvec3 & refractionDir,
                           const glm::dvec3 & refractionRadiance) const = 0;

    bool isReflective() const;
    bool isRefractive() const;

  protected:
    Material();

    Material(int flags, double ior);

    int m_typeFlags;
    double m_ior;
};
