#pragma once

#include "Material.hpp"

class NormalMaterial : public Material
{
  public:
    NormalMaterial();
    virtual ~NormalMaterial();

    virtual glm::dvec3
    getRadiance(const Ray & ray, const Intersection & intersect,
                const glm::vec3 & ambient,
                const std::vector<const Light *> & lights) const override;

    virtual glm::dvec3
    getRadiance(const Ray & ray, const Intersection & intersect,
                const glm::vec3 & ambient,
                const std::vector<const Light *> & lights,
                const glm::dvec3 & reflectionDir,
                const glm::dvec3 & reflectionRadiance) const override;

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::vec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir, const glm::dvec3 & reflectionRadiance,
        const glm::dvec3 & refractionDir,
        const glm::dvec3 & refractionRadiance) const override;
};
