#include "NormalMaterial.hpp"

NormalMaterial::NormalMaterial() {}

NormalMaterial::~NormalMaterial() {}

glm::dvec3
NormalMaterial::getRadiance(const Ray & ray, const Intersection & intersect,
                            const glm::vec3 & ambient,
                            const std::vector<const Light *> & lights) const
{
    return (intersect.getNormalizedNormal() + glm::dvec3{1.0, 1.0, 1.0}) * 0.5;
}

glm::dvec3
NormalMaterial::getReflectedRadiance(const Ray & ray,
                                    const Intersection & intersect,
                                    const glm::dvec3 & reflectionDir,
                                    const glm::dvec3 & reflectionRadiance) const
{
    return glm::dvec3(0);
}

glm::dvec3
NormalMaterial::getRefractedRadiance(const Ray & ray,
                                    const Intersection & intersect,
                                    const glm::dvec3 & refractionDir,
                                    const glm::dvec3 & refractionRadiance) const
{
    return glm::dvec3(0);
}
