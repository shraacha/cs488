#include "RayIntersect.hpp"
#include "debug.hpp"

#include <optional>
#include <limits>

#include "Clipping.hpp"
#include "polyroots.hpp"
// ---------- helpers ----------
enum class CoordinatePlane{
 xz, xy, yz
};

static inline CoordinatePlane getClosestCoordinatePlane(const glm::dvec4 & normal)
{
    glm::dvec4 absNormal = glm::abs(normal);
    if (absNormal.x >= absNormal.y && absNormal.x >= absNormal.z) {
        return CoordinatePlane::yz;
    } else if (absNormal.y >= absNormal.x && absNormal.y >= absNormal.z) {
        return CoordinatePlane::xz;
    } else {
        return CoordinatePlane::xy;
    }
}

static glm::dvec2 projectOntoPlane(const glm::dvec4 & point, const CoordinatePlane & plane)
{
    switch (plane) {
    case CoordinatePlane::xz:
        return {point.z, point.x};
        break;
    case CoordinatePlane::xy:
        return {point.x, point.y};
        break;
    default: // yz
        return {point.y, point.z};
    }
}

static double getNonCoordinatePlaneValue(const glm::dvec4 & vec, const CoordinatePlane & plane)
{
    switch (plane) {
    case CoordinatePlane::xz:
        return vec.y;
        break;
    case CoordinatePlane::xy:
        return vec.z;
        break;
    default: // yz
        return vec.x;
    }
}

static bool isNonCoordinatePlaneValueNonNegative(const glm::dvec4 & vec, const CoordinatePlane & plane)
{
    return getNonCoordinatePlaneValue(vec, plane) >= 0;
}

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
static inline std::vector<glm::dvec4>
getBoxFaceVertices(const glm::dvec4 &corner, const double &width,
                   const size_t &face)
{
    switch (face) {
    case 0: // back face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z, 1.0),
                glm::dvec4(corner.x + width, corner.y + width, corner.z, 1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z, 1.0)};
        break;
    case 1: // left face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width, 1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z + width, 1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z, 1.0)};
        break;
    case 2: // bottom face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z + width, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width, 1.0)};
        break;
    case 3: // front face
        return {glm::dvec4(corner.x + width, corner.y + width, corner.z + width,
                           1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z + width, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width, 1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z + width, 1.0)};
        break;
    case 4: // right face
        return {glm::dvec4(corner.x + width, corner.y + width, corner.z + width,
                           1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z + width, 1.0),
                glm::dvec4(corner.x + width, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width, corner.y + width, corner.z, 1.0)};
        break;
    default: // top face
        return {glm::dvec4(corner.x + width, corner.y + width, corner.z + width,
                           1.0),
                glm::dvec4(corner.x + width, corner.y + width, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width, corner.z + width, 1.0)};
        break;
    }
}

std::optional<std::pair<double, glm::dvec4>>
findRayBoxIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                             const glm::dvec4 &corner, const double &width)
{
    double t = std::numeric_limits<double>::max();
    glm::dvec4 normal;

    for (size_t i = 0; i < 6; ++i) {
        std::optional<std::pair<double, glm::dvec4>> result{std::nullopt};

        result = findRayPolygonIntersectAndNormal(
            eye, pixel, getBoxFaceVertices(corner, width, i));

        if (result.has_value() && result->first < t && result->first >= 0.0) {
            t = result->first;
            normal = result->second;
        }
    }

    if (t != std::numeric_limits<double>::max())
    {
        return std::make_pair(t, normal);
    } else {
        return std::nullopt;
    }
}

// -------------------------------------------------
std::optional<std::pair<double, glm::dvec4>> findRayPolygonIntersectAndNormal(const glm::dvec4 &eye, const glm::dvec4 &pixel,
                                                                              const std::vector<glm::dvec4> &vertices) {
    if (vertices.size() < 3) {
        return std::nullopt;
        DLOG("findRayPolygonIntersectAndNormal: less than 3 vertices supplied");
    }

    glm::dvec4 polyNormal(glm::cross(glm::dvec3(vertices[2] - vertices[1]), glm::dvec3(vertices[0] - vertices[1])), 0.0);
    std::optional<double> t = findRayPlaneIntersect(eye, pixel, polyNormal, vertices[0]);

    CoordinatePlane closestCoordinatePlane = getClosestCoordinatePlane(polyNormal);
    VertexOrder order = isNonCoordinatePlaneValueNonNegative(polyNormal, closestCoordinatePlane) ? VertexOrder::CCW : VertexOrder::CW;

    if (t.has_value()) {
        glm::dvec2 projectedPoint = projectOntoPlane(evaluateRay(eye, pixel, t.value()), closestCoordinatePlane);

        for (size_t i = 0; i < vertices.size(); ++i) {
            glm::dvec2 v1 = projectOntoPlane(vertices[i], closestCoordinatePlane);
            glm::dvec2 v2 = projectOntoPlane(vertices[(i + 1) % vertices.size()],
                                             closestCoordinatePlane);
            // mod by size otherwise we do not wrap back to the starting vertex when evaluating the final edge

            // for vector (a,b), the clockwise normal is given by (-b, a)
            if (!isInBoundsClip(projectedPoint, v1, {-v2.y + v1.y, v2.x - v1.x}, order)) {
                return std::nullopt;
            }
        }
        return std::make_pair(t.value(), polyNormal);
    } else {
        return std::nullopt;
    }
}

// -------------------------------------------------
std::optional<double> findRayPlaneIntersect(const glm::dvec4 &eye,
                                            const glm::dvec4 &pixel,
                                            const glm::dvec4 &planeNormal,
                                            const glm::dvec4 &planePoint)
{
    if (doesRayIntersectPlane(eye, pixel, planeNormal)) {
        return glm::dot(planeNormal, planePoint - eye) / glm::dot(planeNormal, pixel - eye);
    }
    else {
        return std::nullopt;
    }
}
