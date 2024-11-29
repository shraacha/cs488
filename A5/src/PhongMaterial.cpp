// Termm--Fall 2024

#include "PhongMaterial.hpp"

#include "Helpers.hpp"
#include "GLMHelpers.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
{}

PhongMaterial::~PhongMaterial()
{}

glm::dvec3 PhongMaterial::getKD() const
{
	return m_kd;
}

glm::dvec3 PhongMaterial::getKS() const
{
	return m_ks;
}

double PhongMaterial::getShininess() const
{
	return m_shininess;
}

glm::dvec3 PhongMaterial::getRadiance(
    const Ray & ray, const Intersection & intersect, const glm::vec3 & ambient,
    const std::vector<const Light *> & lights, const glm::dvec3 & reflectionDir,
    const glm::dvec3 & reflectionRadiance, const glm::dvec3 & refractionDir,
    const glm::dvec3 & refractionRadiance) const
{
    // ambient
    glm::dvec3 lightOut = getKD() * glm::dvec3(ambient);

    glm::dvec3 intersectionPoint = glm::dvec3(intersect.getPosition());

    for (const Light *light : lights)
    {
        glm::dvec3 normalizedLightVector =
            glm::normalize(glm::dvec3(light->position) - intersectionPoint);

        double lightDotNormal =
            glm::dot(normalizedLightVector, intersect.getNormalizedNormal());
        double attenuationFactor = light->calculateAttenuation(intersectionPoint);

        glm::dvec3 reflectedVector =
            -normalizedLightVector +
            2 * lightDotNormal * intersect.getNormalizedNormal();

        // difuse
        lightOut +=
            maxWithZero(getKD() * lightDotNormal * (1 / attenuationFactor) *
                        glm::dvec3(light->colour));

        // specular
        lightOut += maxWithZero(
            pow(glm::dot(reflectedVector,
                         glm::normalize(glm::dvec3(ray.getEyePoint()) -
                                        intersectionPoint)),
                getShininess()) *
            (1 / attenuationFactor) * getKS() * glm::dvec3(light->colour));
    }

    return lightOut;
}

MaterialAction PhongMaterial::russianRouletteAction() const
{
    // TODO
    return MaterialAction::Absorb;
}

std::pair<glm::dvec3, double> PhongMaterial::sampleReflectionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal) const
{
    // TODO
    // sample based on roughness
    return std::make_pair(glm::dvec3(0.0), 0.0);
}

std::pair<glm::dvec3, double> PhongMaterial::sampleRefractionDirection(
    const glm::dvec3 vin, const glm::dvec3 surfaceNormal, double ior1) const
{
    return std::make_pair(glm::dvec3(0.0), 0.0);
}
