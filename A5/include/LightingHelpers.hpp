#pragma once

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
inline double calculateChi(glm::dvec3 v1, glm::dvec3 v2)
{
    return glm::dot(v1, v2) > 0.0 ? 1.0 : 0.0;
}

inline double calculateChi(double v1dotv2)
{
    return v1dotv2 > 0.0 ? 1.0 : 0.0;
}

double evaluateDistributionGGX(glm::dvec3 n, glm::dvec3 h, double a);

inline double evaluateLambertBRDF(double c)
{
    return c * M_1_PI;
}

glm::dvec3 evaluateLambertBRDF(glm::dvec3 c)
{
    return M_1_PI * c;
}

double evaluateGeometryPartialGGX(glm::dvec3 w, glm::dvec3 h, glm::dvec3 n,
                                  double a);

double evaluateGeometryGGX(glm::dvec3 wi, glm::dvec3 wo, glm::dvec3 h, glm::dvec3 n,
                           double a);

double calculateFresnelShlick(double f0, glm::dvec3 v1, glm::dvec3 v2);
