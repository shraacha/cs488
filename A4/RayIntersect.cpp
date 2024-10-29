#include "RayIntersect.hpp"

#include <optional>

#include "polyroots.hpp"

static inline bool isDeterminantPositive(const double & a, const double & b, const double & c)
{
    return (b * b - 4 * a * c) >= 0;
}

// -------------------------------------------------
std::optional<std::pair<double, glm::dvec4>>
findRaySphereIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                                const glm::dvec4 &centre, const double &radius)
{

    glm::dvec4 pixelDelta = pixel - eye;
    glm::dvec4 eyeToCentre = eye - centre;

    double a = glm::dot(pixelDelta, pixelDelta);
    double b = 2 * glm::dot(eyeToCentre, pixelDelta);
    double c = glm::dot(eyeToCentre, eyeToCentre) - radius * radius;

    if (isDeterminantPositive(a, b, c))
    {
        double roots[2];
        quadraticRoots(a, b, c, roots);

        double closestRoot;
        if (roots[0] < roots[1])
        {
            closestRoot = roots[0];
        } else {
            closestRoot = roots[1];
        }

        return std::make_optional<std::pair<double, glm::dvec4>>(closestRoot, glm::normalize(evaluateRay(eye, pixel, closestRoot) - centre));
    } else {
        return std::nullopt;
    }
}

// -------------------------------------------------
bool doesRayIntersectSphere(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                            const glm::dvec4 &centre, const double &radius)
{
    glm::dvec4 pixelDelta = pixel - eye;
    glm::dvec4 eyeToCentre = eye - centre;

    return isDeterminantPositive(
        glm::dot(pixelDelta, pixelDelta), glm::dot(eyeToCentre, pixelDelta),
        glm::dot(eyeToCentre, eyeToCentre) - radius * radius);
}

// -------------------------------------------------
glm::dvec4 evaluateRay(const glm::dvec4 & eye, const glm::dvec4 & pixel, const double & t)
{
    return eye + t * (pixel - eye);
}
