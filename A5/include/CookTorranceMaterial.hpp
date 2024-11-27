#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class CookTorranceMaterial : public Material {
public:
  CookTorranceMaterial(const glm::vec3& albedo, const glm::vec3& ks, double roughness);
  virtual ~CookTorranceMaterial();

  glm::dvec3 getKD() const;
  glm::dvec3 getKS() const;
  glm::dvec3 getAlbedo() const;
  double getRoughness() const;

private:
  glm::dvec3 m_albedo;
  glm::dvec3 m_ks;
  double m_roughness;
};
