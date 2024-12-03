#include "NormalMaterial.hpp"

#include "GLMHelpers.hpp"

NormalMaterial::NormalMaterial() {}

NormalMaterial::~NormalMaterial() {}

glm::dvec3 NormalMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::dvec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{

    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, -ray.getNormalizedDirection(), value.value());
    }

    return (surfaceNormal + glm::dvec3{1.0, 1.0, 1.0}) * 0.5;
}
