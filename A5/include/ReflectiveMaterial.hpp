#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class ReflectiveMaterial : public Material
{
  public:
    ReflectiveMaterial(const glm::vec3 & albedo, double roughness);
    virtual ~ReflectiveMaterial();

    virtual glm::dvec3
    getRadiance(const Ray & ray, const Intersection & intersect,
                const glm::vec3 & ambient,
                const std::vector<const Light *> & lights) const override;

    virtual glm::dvec3 getReflectedRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::dvec3 & reflectionDir,
                           const glm::dvec3 & reflectionRadiance) const override;

    virtual glm::dvec3 getRefractedRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::dvec3 & refractionDir,
                           const glm::dvec3 & refractionRadiance) const override;


    virtual MaterialAction russianRouletteAction() const override;

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const override;
    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const override;

    glm::dvec3 getAlbedo() const;
    double getRoughness() const;
  private:
    glm::dvec3 m_albedo;
    double m_roughness;
};
