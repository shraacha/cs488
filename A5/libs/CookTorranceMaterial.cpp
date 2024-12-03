#include "CookTorranceMaterial.hpp"

#include <cmath>

#include <glm/detail/func_exponential.hpp>

#include "LightingHelpers.hpp"
#include "Material.hpp"
#include "GLMHelpers.hpp"

#include "Debug.hpp"
#include "UVMap.hpp"
#include "glm/detail/type_vec.hpp"

CookTorranceMaterial::CookTorranceMaterial(const glm::vec3 & albedo,
                                           const glm::vec3 & ks,
                                           double roughness)
    // TODO make reflective
    : Material(MaterialTypeFlags::Direct ||
                   MaterialTypeFlags::TransmissiveDiffuse ||
                   MaterialTypeFlags::SpecularReflective,
               1.0),
      m_albedo{albedo}, m_ks(ks), m_roughness(roughness)
{
}

CookTorranceMaterial::~CookTorranceMaterial() {}

glm::dvec3 CookTorranceMaterial::getKD() const { return glm::dvec3{1.0, 1.0, 1.0} - m_ks; }

glm::dvec3 CookTorranceMaterial::getKS() const { return m_ks; }

glm::dvec3 CookTorranceMaterial::getAlbedo(const std::optional<UVLookup> & uvLookup) const
{
    if (auto value = lookup(m_albedoMap, uvLookup); value)
    {
        return value.value();
    }
    else
    {
        return m_albedo;
    }
}

double CookTorranceMaterial::getRoughness() const { return m_roughness; }

glm::dvec3 CookTorranceMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::dvec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    glm::dvec3 lightOut{0, 0, 0};

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());
    glm::dvec3 outVector =
        glm::normalize(glm::dvec3(ray.getEyePoint()) - intersectionPoint);

    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, outVector, value.value());
    }

    double scaleFactor = 1.0; // not sure exactly why I need this scale factor,
                              // but otherwise the objects are too dim

    for (const Light *light : lights)
    {
        glm::dvec3 normalizedLightVector =
            glm::normalize(glm::dvec3(light->position) - intersectionPoint);
        glm::dvec3 halfway = glm::normalize(outVector + normalizedLightVector);

        double attenuationFactor =
            light->calculateAttenuation(intersectionPoint);

        glm::dvec3 radiance = glm::dvec3(light->colour) * attenuationFactor;

        // cook-torrance brdf
        double ndf =
            evaluateDistributionGGX(surfaceNormal, halfway, getRoughness());
        double g = evaluateGeometrySmith(surfaceNormal, outVector,
                                         normalizedLightVector, getRoughness());
        double f = calculateFresnelSchlick(c_f0, halfway, outVector);

        glm::dvec3 kS(1);
        glm::dvec3 kD(1-f);

        // nuked fresnel since it was really attenuating things
        glm::dvec3 numerator(ndf * g);
        double denominator =
            4.0 * std::max(glm::dot(surfaceNormal, outVector), 0.0) *
                std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0) +
            0.001;
        glm::dvec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        double nDotL =
            std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0);
        lightOut +=
            radiance *
            (kD * glm::pow(getAlbedo(intersect.getUV()), glm::dvec3(2.2)) *
                 M_1_PI +
             specular) *
            nDotL;
    }

    return lightOut;
}

glm::dvec3 CookTorranceMaterial::evaluateBRDF(const glm::dvec3 & vIncidence,
                                    const glm::dvec3 & vOut,
                                    const Intersection & intersect) const
{
    glm::dvec3 lightOut{0, 0, 0};

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());
    glm::dvec3 inVector = glm::normalize(vIncidence);
    glm::dvec3 outVector = glm::normalize(vOut);

    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, outVector, value.value());
    }

    double scaleFactor = 1.0;

    glm::dvec3 halfway = glm::normalize(outVector + inVector);

    double vinDotN =
        glm::dot(vIncidence, surfaceNormal);

    // cook-torrance brdf
    double ndf =
        evaluateDistributionGGX(surfaceNormal, halfway, getRoughness());
    double g = evaluateGeometrySmith(surfaceNormal, outVector,
                                     vIncidence, getRoughness());
    double f = calculateFresnelSchlick(c_f0, halfway, outVector);

    glm::dvec3 kS(1);
    glm::dvec3 kD(1 - f);

    // nuked fresnel since it was really attenuating things
    glm::dvec3 numerator(g);
    double denominator =
        4.0 * std::max(glm::dot(surfaceNormal, outVector), 0.0) *
            std::max(glm::dot(surfaceNormal, vIncidence), 0.0) +
        0.001;
    glm::dvec3 specular = numerator / denominator;

    // add to outgoing radiance Lo
    double nDotL = std::max(glm::dot(surfaceNormal, vIncidence), 0.0);
    lightOut +=
        (kD * glm::pow(getAlbedo(intersect.getUV()), glm::dvec3(2.2)) * M_1_PI +
         specular);

    return lightOut;
}

MaterialActionAndConstants CookTorranceMaterial::russianRouletteAction(
    const Ray & ray, const Intersection & intersect) const
{
    glm::dvec3 vin = ray.getNormalizedDirection();
    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, -vin, value.value());
    }

    auto normalSample = sampleNormalGGX(-vin, surfaceNormal, getRoughness());

    glm::dvec3 halfway = glm::normalize(-vin + getReflectedVector(vin, normalSample.first));
    double f = calculateFresnelSchlick(c_f0, halfway, -vin);

    // DLOG("f: %f", f); // TESTING

    return {decideMaterialAction(f * 0.9, (1 - f) * 0.9), glm::dvec3(f * 0.9),
            glm::dvec3((1 - f) * 0.9)};
}

std::pair<glm::dvec3, double> CookTorranceMaterial::sampleReflectionDirection(
    const Ray & ray, const Intersection & intersect) const
{
    glm::dvec3 vin = ray.getNormalizedDirection();
    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, -vin, value.value());
    }

    auto normalSample = sampleNormalGGX(-vin, surfaceNormal, getRoughness());
    return std::make_pair(getReflectedVector(vin, normalSample.first), 1.0);
}

std::pair<glm::dvec3, double> CookTorranceMaterial::sampleRefractionDirection(
    const Ray & ray, const Intersection & intersect, double ior1) const
{
    return std::make_pair(glm::dvec3(0.0), 0.0);
}

std::pair<glm::dvec3, double>
CookTorranceMaterial::sampleDiffuseDirection(const Ray & ray, const Intersection & intersect) const
{
    glm::dvec3 vin = ray.getNormalizedDirection();
    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    if (auto value = lookup(m_normalMap, intersect.getUV(), true); value)
    {
        surfaceNormal = perturbVector(surfaceNormal, -vin, value.value());
    }

    return sampleCosineWeightedHemisphere(-vin, surfaceNormal);
}
