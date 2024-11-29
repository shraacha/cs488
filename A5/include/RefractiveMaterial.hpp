#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class RefractiveMaterial : public Material
{
  public:
    RefractiveMaterial(const glm::vec3 & albedo, double roughness, double ior);
    virtual ~RefractiveMaterial();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::vec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const override;

    virtual MaterialAction russianRouletteAction() const override;

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const override;

    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal,
                              double ior1) const override;

    glm::dvec3 getAlbedo() const;
    double getRoughness() const;
  private:
    glm::dvec3 m_albedo;
    double m_roughness;
};
