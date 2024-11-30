// Termm--Fall 2024

#pragma once

#include <vector>

#include "Ray.hpp"
#include "Intersection.hpp"
#include "Light.hpp"
#include "debug.hpp"

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

static glm::dvec3 c_defaultDVec3(0.0);
static std::pair<glm::dvec3, double> c_defaultDVec3DoublePair(glm::dvec3(0.0), 0);

class Material {
  public:
    virtual ~Material();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::vec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const = 0;

    bool isDirect() const;
    bool isReflective() const;
    bool isRefractive() const;

    double getIOR() const;
    virtual glm::dvec3 getKS() const
    {
        DLOG("base getKS called");
        return c_defaultDVec3;
    }
    virtual glm::dvec3 getKD() const
    {
        DLOG("base getKD called");
        return c_defaultDVec3;
    }
    virtual glm::dvec3 getAlbedo() const
    {
        DLOG("base getAlbedo called");
        return c_defaultDVec3;
    }

    virtual MaterialAction
    russianRouletteAction(const glm::dvec3 vin,
                          const glm::dvec3 surfaceNormal) const
    {
        DLOG("base russianRouletteAction called");
        return MaterialAction::Absorb;
    }

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal) const = 0;

    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const glm::dvec3 vin,
                              const glm::dvec3 surfaceNormal,
                              double ior1) const = 0;

    virtual std::pair<glm::dvec3, double>
    sampleDiffuseDirection(const glm::dvec3 vin,
                           const glm::dvec3 surfaceNormal) const

    {
        DLOG("base sampleDiffuseDirection called");
        return c_defaultDVec3DoublePair;
    }

  protected:
    Material();

    Material(unsigned int flags, double ior);

    unsigned int m_typeFlags;
    double m_ior;
};
