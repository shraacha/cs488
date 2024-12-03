// Termm--Fall 2024

#pragma once

#include <vector>
#include <optional>
#include <memory>

#include "Debug.hpp"
#include "Image.hpp"
#include "Intersection.hpp"
#include "Light.hpp"
#include "Ray.hpp"
#include "UVMap.hpp"

#include <glm/glm.hpp>

const double c_f0 = 0.04;

using material_type_t = unsigned int;

enum MaterialTypeFlags
{
    None = 0,
    Direct = 1 << 0,
    SpecularReflective = 1 << 1,
    TransmissiveReflective = 1 << 2,
    TransmissiveDiffuse = 1 << 3,
    TransmissiveRefractive = 1 << 4
};

enum struct MaterialAction
{
    Reflect,
    Transmit,
    Absorb
};

MaterialAction decideMaterialAction(double reflect, double transmit);

struct MaterialActionAndConstants
{
    MaterialAction action;
    glm::dvec3 kS;
    glm::dvec3 kD;
};

static glm::dvec3 c_defaultDVec3(0.0);
static std::pair<glm::dvec3, double> c_defaultDVec3DoublePair(glm::dvec3(0.0), 0);

class Material {
  public:
    virtual ~Material();

    virtual glm::dvec3 getRadiance(
        const Ray & ray, const Intersection & intersect,
        const glm::dvec3 & ambient, const std::vector<const Light *> & lights,
        const glm::dvec3 & reflectionDir = glm::dvec3(0.0),
        const glm::dvec3 & reflectionRadiance = glm::dvec3(0.0),
        const glm::dvec3 & refractionDir = glm::dvec3(0.0),
        const glm::dvec3 & refractionRadiance = glm::dvec3(0.0)) const = 0;

    // both vectors should be facing away from the surface
    virtual glm::dvec3 evaluateBRDF(const glm::dvec3 & vIncidence,
                                    const glm::dvec3 & vOut,
                                    const Intersection & intersect) const
    {
        // DLOG("base evaluateBRDF called");
        return glm::dvec3(0.0);
    }

    bool isDirect() const;
    bool isSpecularReflective() const;
    bool isTransmissiveReflective() const;
    bool isTransmissiveRefractive() const;
    bool isTransmissiveDiffuse() const;

    material_type_t getTypeFlags() const;

    void addAlbedoMap(Image * map);
    void addNormalMap(Image * map);

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

    virtual glm::dvec3 getAlbedo(const std::optional<UVLookup> & uvLookup = std::nullopt) const
    {
        DLOG("base getAlbedo called");
        return c_defaultDVec3;
    }

    virtual MaterialActionAndConstants
    russianRouletteAction(const Ray & ray, const Intersection & intersect) const
    {
        DLOG("base russianRouletteAction called");
        return {MaterialAction::Absorb, glm::dvec3(0.0), glm::dvec3(0.0)};
    }

    virtual std::pair<glm::dvec3, double>
    sampleReflectionDirection(const Ray & ray, const Intersection & intersect) const = 0;

    virtual std::pair<glm::dvec3, double>
    sampleRefractionDirection(const Ray & ray, const Intersection & intersect,
                              double ior1) const = 0;

    virtual std::pair<glm::dvec3, double>
    sampleDiffuseDirection(const Ray & ray, const Intersection & intersect) const

    {
        DLOG("base sampleDiffuseDirection called");
        return c_defaultDVec3DoublePair;
    }

  protected:
    Material();

    Material(unsigned int flags, double ior);

    material_type_t m_typeFlags;
    double m_ior;

    UVMap m_albedoMap;
    UVMap m_normalMap;
};
