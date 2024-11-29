#include "RefractiveMaterial.hpp"

#include "LightingHelpers.hpp"
#include "glm/detail/type_vec.hpp"

RefractiveMaterial::RefractiveMaterial(const glm::vec3 & albedo,
                                       double roughness, double ior)
    : Material(MaterialTypeFlags::Reflective | MaterialTypeFlags::Refractive,
               ior),
      m_albedo(albedo), m_roughness(roughness)
{
}

RefractiveMaterial::~RefractiveMaterial() {}

glm::dvec3 RefractiveMaterial::getAlbedo() const { return m_albedo; }

double RefractiveMaterial::getRoughness() const { return m_roughness; }

glm::dvec3 RefractiveMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    // ambient
    glm::dvec3 lightOut{0, 0, 0};

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());
    glm::dvec3 outVector =
        glm::normalize(glm::dvec3(ray.getEyePoint()) - intersectionPoint);

    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    double f0 = 0.04;
    double scaleFactor = 1.5; // not sure exactly why I need this scale factor,
                              // but otherwise the objects are too dim
    glm::dvec3 normalizedLightVector =
        glm::normalize(reflectionDir);
    glm::dvec3 halfway = glm::normalize(outVector + normalizedLightVector);

    double lightDotNormal =
        glm::dot(normalizedLightVector, intersect.getNormalizedNormal());

    glm::dvec3 radiance = reflectionRadiance;

    // cook-torrance brdf
    double ndf =
        evaluateDistributionGGX(surfaceNormal, halfway, getRoughness());
    double g = evaluateGeometrySmith(surfaceNormal, outVector,
                                     normalizedLightVector, getRoughness());
    double f = calculateFresnelSchlick(f0, halfway, outVector);

    glm::dvec3 kS(1.0);
    glm::dvec3 kD(1 - f);

    glm::dvec3 numerator(f * g);
    double denominator =
        4.0 * std::max(glm::dot(surfaceNormal, outVector), 0.0) *
            std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0) +
        0.001;
    glm::dvec3 specular = numerator / denominator;

    // add to outgoing radiance Lo
    double nDotL =
        std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0);
    lightOut +=
        scaleFactor * (kD * refractionRadiance + kS * specular * reflectionRadiance) * nDotL;

    return lightOut;
}

MaterialAction RefractiveMaterial::russianRouletteAction() const
{
    // TODO
    return MaterialAction::Absorb;
}

std::pair<glm::dvec3, double> RefractiveMaterial::sampleReflectionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal) const
{
    auto normalSample = sampleNormalGGX(-vin, surfaceNormal, getRoughness());
    return std::make_pair(getReflectedVector(vin, normalSample.first), normalSample.second);
}

std::pair<glm::dvec3, double> RefractiveMaterial::sampleRefractionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal, double ior1) const
{
    auto normalSample = sampleNormalGGX(-vin, surfaceNormal, getRoughness());
    auto refractedVector = getRefractedVector(vin, normalSample.first, m_ior, ior1);
    if (refractedVector)
    {
        return std::make_pair(refractedVector.value(), normalSample.second);
    }
    else
    {
        return std::make_pair(glm::dvec3(0.0), 0.0);
    }
}
