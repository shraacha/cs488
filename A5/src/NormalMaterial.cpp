#include "NormalMaterial.hpp"

NormalMaterial::NormalMaterial() {}

NormalMaterial::~NormalMaterial() {}

glm::dvec3 NormalMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    return (intersect.getNormalizedNormal() + glm::dvec3{1.0, 1.0, 1.0}) * 0.5;
}
