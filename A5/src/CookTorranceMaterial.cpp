#include "CookTorranceMaterial.hpp"

#include "LightingHelpers.hpp"
#include "Material.hpp"

CookTorranceMaterial::CookTorranceMaterial(const glm::vec3 & albedo,
                                           const glm::vec3 & ks,
                                           double roughness)
    // TODO make reflective
    : Material(MaterialTypeFlags::Direct, 1.0), m_albedo{albedo}, m_ks(ks),
      m_roughness(roughness)
{
}

CookTorranceMaterial::~CookTorranceMaterial() {}

glm::dvec3 CookTorranceMaterial::getKD() const { return glm::dvec3{1.0, 1.0, 1.0} - m_ks; }

glm::dvec3 CookTorranceMaterial::getKS() const { return m_ks; }

glm::dvec3 CookTorranceMaterial::getAlbedo() const { return m_albedo; }

double CookTorranceMaterial::getRoughness() const { return m_roughness; }

glm::dvec3 CookTorranceMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights) const
{
    // ambient
    glm::dvec3 lightOut{0, 0, 0};

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());
    glm::dvec3 outVector =
        glm::normalize(glm::dvec3(ray.getEyePoint()) - intersectionPoint);

    glm::dvec3 surfaceNormal = intersect.getNormalizedNormal();

    double f0 = 0.04;
    double scaleFactor = 2.0; // not sure exactly why I need this scale factor,
                              // but otherwise the objects are too dim

    for (const Light *light : lights)
    {
        glm::dvec3 normalizedLightVector =
            glm::normalize(glm::dvec3(light->position) - intersectionPoint);
        glm::dvec3 halfway = glm::normalize(outVector + normalizedLightVector);

        double lightDotNormal =
            glm::dot(normalizedLightVector, intersect.getNormalizedNormal());
        double attenuationFactor =
            light->calculateAttenuation(intersectionPoint);

        glm::dvec3 radiance = glm::dvec3(light->colour) * attenuationFactor;


        // cook-torrance brdf
        double ndf =
            evaluateDistributionGGX(surfaceNormal, halfway, getRoughness());
        double g = evaluateGeometrySmith(surfaceNormal, outVector,
                                         normalizedLightVector, getRoughness());
        double f = calculateFresnelSchlick(f0, halfway, outVector);

        glm::dvec3 kS (f);
        glm::dvec3 kD(1 - f);

        glm::dvec3 numerator(f * ndf * g);
        double denominator =
            4.0 * std::max(glm::dot(surfaceNormal, outVector), 0.0) *
                std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0) +
            0.001;
        glm::dvec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        double nDotL =
            std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0);
        lightOut += scaleFactor * (kD * getAlbedo() / M_PI + specular) *
                    radiance * nDotL;
    }

    return lightOut;
}

glm::dvec3 CookTorranceMaterial::getReflectedRadiance(
    const Ray & ray, const Intersection & intersect,
    const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance) const
{
    return glm::dvec3(0);
}

glm::dvec3 CookTorranceMaterial::getRefractedRadiance(
    const Ray & ray, const Intersection & intersect,
    const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    return glm::dvec3(0);
}

MaterialAction CookTorranceMaterial::russianRouletteAction() const
{
    // TODO
    return MaterialAction::Absorb;
}

std::pair<glm::dvec3, double> CookTorranceMaterial::sampleReflectionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal) const
{
    // TODO
    // sample based on roughness
    return std::make_pair(glm::dvec3(0.0), 0.0);
}

std::pair<glm::dvec3, double> CookTorranceMaterial::sampleRefractionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal) const
{
    return std::make_pair(glm::dvec3(0.0), 0.0);
}
