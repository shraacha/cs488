#include <cmath>
#include <algorithm>

#include "LightingHelpers.hpp"

double evaluateDistributionGGX(const glm::dvec3 & n, const glm::dvec3 & h,
                               double a)
{
    // double hdotn = std::max(glm::dot(h, n), 0.0001);
    // double hdotn2 = hdotn * hdotn;
    // double numer = a * a * calculateChi(hdotn);

    // double term = hdotn2 * a * a + (1.0 - hdotn2);
    // double denom = M_PI * term * term;

    // return numer / denom;

    double NoH = dot(n,h);
    double alpha2 = a * a;
    double NoH2 = NoH * NoH;
    double den = NoH2 * alpha2 + (1 - NoH2);
    return (calculateChi(NoH) * alpha2) / ( M_PI * den * den );
}

double evaluateGeometryPartialGGX(const glm::dvec3 & w, const glm::dvec3 & n,
                                  const glm::dvec3 & h, double a)
{
    // double wDotH = std::max(glm::dot(w, h), 0.0001);
    // double wDotH2 = wDotH * wDotH;
    // double denom = 1 + std::sqrt(1 + a * a * ((1 - wDotH2) / wDotH2));

    // return calculateChi(wDotH / glm::dot(w, n)) * (2 / denom);

    double VoH2 = dot(w, h);
    double chi = calculateChi(VoH2 / dot(w, n));
    VoH2 = VoH2 * VoH2;
    double tan2 = (1 - VoH2) / VoH2;
    return (chi * 2) / (1 + sqrt(1 + a * a* tan2));
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
