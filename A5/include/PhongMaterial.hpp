// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material
{
  public:
    PhongMaterial(const glm::vec3 & kd, const glm::vec3 & ks, double shininess);
    virtual ~PhongMaterial();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::dvec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const override;

    glm::dvec3 getKD() const;
    glm::dvec3 getKS() const;
    double getShininess() const;

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const Ray & ray, const Intersection & intersect) const override;

    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const Ray & ray, const Intersection & intersect,
                              double ior1) const override;
  private:
    glm::vec3 m_kd;
    glm::vec3 m_ks;

    double m_shininess;
};
