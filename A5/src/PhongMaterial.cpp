// Termm--Fall 2024

#include "PhongMaterial.hpp"

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
