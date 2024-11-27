#include <cmath>
#include <algorithm>

#include "LightingHelpers.hpp"

double evaluateDistributionGGX(glm::dvec3 n, glm::dvec3 h, double a)
{
    double hdotn = std::max(glm::dot(h, n), 0.0001);
    double hdotn2 = hdotn * hdotn;
    double numer = a * a * calculateChi(hdotn);

    double term = a * a + (1.0 - hdotn2) / hdotn2;
    double denom = M_PI * hdotn2 * term * term;

    return numer / denom;
}

double evaluateGeometryPartialGGX(glm::dvec3 w, glm::dvec3 h, glm::dvec3 n,
                                  double a)
{
    double wDotH = std::max(glm::dot(w, h), 0.0001);
    double wDotH2 = wDotH * wDotH;
    double denom = 1 + std::sqrt(1 + a * a * ((1 - wDotH2) / wDotH2));

    return calculateChi(wDotH / glm::dot(w, n)) * (2 / denom);
}

double evaluateGeometryGGX(glm::dvec3 wi, glm::dvec3 wo, glm::dvec3 h, glm::dvec3 n,
                           double a)
{
    return evaluateGeometryPartialGGX(wi, h, n, a) *
           evaluateGeometryPartialGGX(wo, h, n, a);
}

double calculateFresnelShlick(double f0, glm::dvec3 v1, glm::dvec3 v2)
{
    return f0 + (1 - f0) * pow(1 - glm::dot(v1, v2), 5);
}
