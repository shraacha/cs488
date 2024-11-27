#include <cmath>
#include <algorithm>

#include "LightingHelpers.hpp"

double evaluateDistributionGGX(const glm::dvec3 & n, const glm::dvec3 & h,
                               double a)
{
    double hdotn = std::max(glm::dot(h, n), 0.0001);
    double hdotn2 = hdotn * hdotn;
    double numer = a * a * calculateChi(hdotn);

    double term = hdotn2 * a * a + (1.0 - hdotn2);
    double denom = M_PI * term * term;

    return numer / denom;
}

double evaluateGeometryPartialGGX(const glm::dvec3 & w, const glm::dvec3 & n,
                                  const glm::dvec3 & h, double a)
{
    double wDotH = std::max(glm::dot(w, h), 0.0001);
    double wDotH2 = wDotH * wDotH;
    double denom = 1 + std::sqrt(1 + a * a * ((1 - wDotH2) / wDotH2));

    return calculateChi(wDotH / glm::dot(w, n)) * (2 / denom);
}

double evaluateGeometryGGX(const glm::dvec3 & wi, const glm::dvec3 & wo,
                           const glm::dvec3 & n, const glm::dvec3 & h, double a)
{
    return evaluateGeometryPartialGGX(wi, n, h, a) *
           evaluateGeometryPartialGGX(wo, n, h, a);
}

double calculateFresnelSchlick(double f0, const glm::dvec3 & v1,
                               const glm::dvec3 & v2)
{
    return f0 + (1 - f0) * pow(1 - std::max(glm::dot(v1, v2), 0.0), 5);
}
