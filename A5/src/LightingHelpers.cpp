#include <cmath>
#include <algorithm>
#include <optional>

#include "LightingHelpers.hpp"
#include "Helpers.hpp"

double evaluateDistributionGGX(const glm::dvec3 & n, const glm::dvec3 & h,
                               double a)
{
    double a2 = (a * a) * (a * a);
    double nDotH = std::max(glm::dot(n, h), 0.0);
    double nDotH2 = nDotH * nDotH;

    double num = a2;
    double denom = (nDotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;

    return num / denom;

    // double hdotn = std::max(glm::dot(h, n), 0.0001);
    // double hdotn2 = hdotn * hdotn;
    // double numer = a * a * calculateChi(hdotn);

    // double term = hdotn2 * a * a + (1.0 - hdotn2);
    // double denom = M_PI * term * term;

    // return numer / denom;
}

double evaluateGeometryPartialGGX(const glm::dvec3 & w, const glm::dvec3 & n,
                                  const glm::dvec3 & h, double a)
{
    double wDotH = std::max(glm::dot(w, h), 0.0001);
    double wDotH2 = wDotH * wDotH;
    double tan2 = (1 - wDotH2) / wDotH2;
    double denom = 1 + std::sqrt(1 + a * a * tan2);

    return (calculateChi(wDotH / glm::dot(w, n)) * 2) / denom;
}

double evaluateGeometryGGX(const glm::dvec3 & wi, const glm::dvec3 & wo,
                           const glm::dvec3 & n, const glm::dvec3 & h, double a)
{
    return evaluateGeometryPartialGGX(wi, n, h, a) *
           evaluateGeometryPartialGGX(wo, n, h, a);
}

// from https://learnopengl.com/PBR/Lighting
float evaluateGeometrySchlickGGX(double nDotV, double a)
{
    double r = (a + 1.0);
    double k = (r * r) / 8.0;

    double num = nDotV;
    double denom = nDotV * (1.0 - k) + k;

    return num / denom;
}

float evaluateGeometrySmith(const glm::dvec3 & n, const glm::dvec3 & v, const glm::dvec3 & l, double a)
{
    float nDotV = std::max(dot(n, v), 0.0);
    float nDotL = std::max(dot(n, l), 0.0);
    float ggx1 = evaluateGeometrySchlickGGX(nDotV, a);
    float ggx2 = evaluateGeometrySchlickGGX(nDotL, a);

    return ggx1 * ggx2;
}

double calculateFresnelSchlick(double f0, const glm::dvec3 & v1,
                               const glm::dvec3 & v2)
{
    return f0 + (1 - f0) * pow(1 - std::max(glm::dot(v1, v2), 0.0), 5);
}

std::optional<glm::dvec3> getRefractedVector(const glm::dvec3 vin, glm::dvec3 n, double ior2,
                              double ior1)
{
    double cos1 = clampValue(-1.0, 1.0, glm::dot(vin, n));
    if (cos1 < 0)
    {
        // If we have a negative val, that means the ray is intersecting from
        // outside the surface. We should work with positive cos, so we flip.
        cos1 = -cos1;
    }
    else
    {
        // If we have a positive val, intersecting from inside the surface. In
        // this case we should swap the normal dir for the calculation.
        std::swap(ior1, ior2);
        n = -n;
    }
    double eta = ior1 / ior2;
    double c = 1 - eta * eta * (1 - cos1 * cos1);
    // if c < 0, then we have total internal reflection
    return c < 0.0 ? std::nullopt : std::make_optional(eta * vin + (eta * cos1 - sqrt(c)) * n);
}
