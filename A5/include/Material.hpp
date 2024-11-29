// Termm--Fall 2024

#pragma once

#include <vector>

#include "Ray.hpp"
#include "Intersection.hpp"
#include "Light.hpp"

#include <glm/glm.hpp>


enum MaterialTypeFlags
{
    None = 0,
    Reflective = 1 << 0,
    Refractive = 1 << 1,
    Direct = 1 << 2
};

enum struct MaterialAction
{
    Reflect,
    Transmit,
    Absorb
};

class Material {
  public:
    virtual ~Material();

    virtual glm::dvec3 getRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::vec3 & ambient,
                           const std::vector<const Light *> & lights) const = 0;

    virtual glm::dvec3 getReflectedRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::dvec3 & reflectionDir,
                           const glm::dvec3 & reflectionRadiance) const = 0;

    virtual glm::dvec3 getRefractedRadiance(const Ray & ray, const Intersection & intersect,
                           const glm::dvec3 & refractionDir,
                           const glm::dvec3 & refractionRadiance) const = 0;

    bool isDirect() const;
    bool isReflective() const;
    bool isRefractive() const;

    virtual MaterialAction russianRouletteAction() const = 0;

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const = 0;
    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const = 0;

  protected:
    Material();

    Material(unsigned int flags, double ior);

    unsigned int m_typeFlags;
    double m_ior;
};
