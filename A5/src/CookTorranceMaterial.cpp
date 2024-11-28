#include "CookTorranceMaterial.hpp"

#include "LightingHelpers.hpp"

CookTorranceMaterial::CookTorranceMaterial(const glm::vec3 & albedo,
                                           const glm::vec3 & ks,
                                           double roughness)
    : m_albedo{albedo}, m_ks(ks), m_roughness(roughness)
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

    glm::dvec3 surfaceNormal =
        glm::normalize(glm::dvec3(intersect.getNormal()));

    double f0 = 0.004;
    double scaleFactor = 2.5; // not sure exactly why I need this scale factor,
                              // but otherwise the objects are too dim

    for (const Light *light : lights)
    {
        glm::dvec3 normalizedLightVector =
            glm::normalize(glm::dvec3(light->position) - intersectionPoint);

        double lightDotNormal =
            glm::dot(normalizedLightVector, intersect.getNormalizedNormal());
        double attenuationFactor =
            light->calculateAttenuation(intersectionPoint);

        glm::dvec3 radiance = glm::dvec3(light->colour) * attenuationFactor;

        glm::dvec3 halfway = glm::normalize(outVector + normalizedLightVector);

        // cook-torrance brdf
        double NDF =
            evaluateDistributionGGX(surfaceNormal, halfway, getRoughness());
        double G = evaluateGeometryGGX(normalizedLightVector, outVector,
                                       surfaceNormal, halfway, getRoughness());
        double F = calculateFresnelSchlick(f0, halfway, outVector);

        // glm::dvec3 kS(F); // Technically the numerator calculation already
        //                   // accounts for fresnel, but otherwise the specular
        //                   // contributions are too bright
        // glm::dvec3 kD(1 - F);

        glm::dvec3 kS = getKS();
        glm::dvec3 kD = getKD();

        glm::dvec3 numerator(F * NDF * G);
        double denominator =
            4.0 *
            std::max(glm::dot(surfaceNormal, outVector) *
                         glm::dot(surfaceNormal, normalizedLightVector),
                     0.0) *
            +0.0001;
        glm::dvec3 specular = numerator / denominator;

        // add to outgoing radiance Lo
        double NdotL =
            std::max(glm::dot(surfaceNormal, normalizedLightVector), 0.0);
        lightOut += scaleFactor * (kD * getAlbedo() / M_PI + kS * specular) *
                    radiance * NdotL;
    }

    return lightOut;
}

glm::dvec3 CookTorranceMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance) const
{
    return getRadiance(ray, intersect, ambient, lights);
}

glm::dvec3 CookTorranceMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    return getRadiance(ray, intersect, ambient, lights);
}
