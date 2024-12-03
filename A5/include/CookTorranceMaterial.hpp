#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "UVMap.hpp"

class CookTorranceMaterial : public Material
{
  public:
    CookTorranceMaterial(const glm::vec3 & albedo, const glm::vec3 & ks,
                         double roughness);
    virtual ~CookTorranceMaterial();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::dvec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const override;

    virtual MaterialActionAndConstants
    russianRouletteAction(const Ray & ray, const Intersection & intersect) const override;

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const Ray & ray, const Intersection & intersect) const override;

    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const Ray & ray, const Intersection & intersect,
                              double ior1) const override;

    virtual std::pair<glm::dvec3, double>
    sampleDiffuseDirection(const Ray & ray, const Intersection & intersect) const override;

    virtual glm::dvec3 getKS() const override;
    virtual glm::dvec3 getKD() const override;
    virtual glm::dvec3 getAlbedo(const std::optional<UVLookup> & uvlookup = std::nullopt) const override;

    double getRoughness() const;
  private:
    glm::dvec3 m_albedo;
    glm::dvec3 m_ks;
    double m_roughness;
};
