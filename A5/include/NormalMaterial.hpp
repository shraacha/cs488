#pragma once

#include "Material.hpp"

class NormalMaterial : public Material
{
  public:
    NormalMaterial();
    virtual ~NormalMaterial();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::dvec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const override;
};
