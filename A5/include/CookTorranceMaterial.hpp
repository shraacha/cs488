#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class CookTorranceMaterial : public Material {
public:
  CookTorranceMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess);
  virtual ~CookTorranceMaterial();

  glm::dvec3 getKD() const;
  glm::dvec3 getKS() const;
  glm::dvec3 getAlbedo() const;
  double getRoughness() const;

private:
  glm::dvec3 m_ks;
  glm::dvec3 m_albedo;
  double m_roughness;
};
