#include "CookTorranceMaterial.hpp"

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
