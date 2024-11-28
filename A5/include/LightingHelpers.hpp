#pragma once

#include <optional>

#include <glm/glm.hpp>

/*
 * Chi as specified in the GGX microfacet distribution model.
 *
 * Bruce Walter, Stephen R. Marschner, Hongsong Li, and Kenneth E. Torrance.
 * 2007. Microfacet models for refraction through rough surfaces. In Proceedings
 * of the 18th Eurographics conference on Rendering Techniques (EGSR'07).
 * Eurographics Association, Goslar, DEU, 195–206.
 *
 * https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.pdf
 */
inline double calculateChi(const glm::dvec3 & v1, const glm::dvec3 & v2)
{
    return glm::dot(v1, v2) > 0.0 ? 1.0 : 0.0;
}

inline double calculateChi(double v1dotv2) { return v1dotv2 > 0.0 ? 1.0 : 0.0; }

double evaluateDistributionGGX(const glm::dvec3 & n, const glm::dvec3 & h,
                               double a);

inline double evaluateLambertBRDF(double c) { return c * M_1_PI; }

inline glm::dvec3 evaluateLambertBRDF(const glm::dvec3 & c)
{
    return M_1_PI * c;
}

double evaluateGeometryPartialGGX(const glm::dvec3 & w, const glm::dvec3 & n,
                                  const glm::dvec3 & h, double a);

double evaluateGeometryGGX(const glm::dvec3 & wi, const glm::dvec3 & wo,
                           const glm::dvec3 & n, const glm::dvec3 & h,
                           double a);

double calculateFresnelSchlick(double f0, const glm::dvec3 & v1,
                               const glm::dvec3 & v2);

glm::dvec3 calculateFresnelSchlick(const glm::dvec3 & f0, const glm::dvec3 & v1,
                                   const glm::dvec3 & v2);

inline glm::dvec3 getReflectedVector(const glm::dvec3 vin, const glm::dvec3 n)
{
    return vin - 2 * glm::dot(n, vin) * n;
}

std::optional<glm::dvec3> getRefractedVector(const glm::dvec3 vin, glm::dvec3 n,
                              double ior2, double ior1 = 1);
