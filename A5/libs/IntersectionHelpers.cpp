#include "IntersectionHelpers.hpp"

#include <optional>
#include <limits>

#include <glm/ext.hpp>
#include "glm/detail/func_geometric.hpp"

#include "Intersection.hpp"
#include "Ray.hpp"
#include "Debug.hpp"
#include "Clipping.hpp"
#include "UVMap.hpp"
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

static inline double getClosestRoot(const double & a, const double & b, const double & c)
{
        double roots[2];
        quadraticRoots(a, b, c, roots);

        if (roots[0] < roots[1])
        {
            return roots[0];
        } else {
            return roots[1];
        }
}

static inline std::optional<double> getClosestPositiveRoot(const double & a, const double & b, const double & c)
{
        double roots[2];
        quadraticRoots(a, b, c, roots);

        if (roots[0] < 0 && roots[1] < 0)
        {
            return std::nullopt;
        } else if (roots[0] < 0) {
            return roots[1];
        } else if (roots[1] < 0) {
            return roots[0];
        } else {
            return std::min(roots[0], roots[1]);
        }
}

// -------------------------------------------------
std::optional<Intersection> findRaySphereIntersection(const Ray &ray,
                                                      const glm::dvec4 &centre,
                                                      const double &radius) {

    glm::dvec4 pixelDelta = ray.getDirection();
    glm::dvec4 eyeToCentre = ray.getEyePoint() - centre;

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

        return std::make_optional<Intersection>(
            closestRoot, evaluate(ray, closestRoot),
            evaluate(ray, closestRoot) - centre,
            UVLookup(getUnitSphereUVMap(evaluate(ray, closestRoot)), 0));
    }
    else
    {
        return std::nullopt;
    }
}

// -------------------------------------------------
bool doesRayIntersectSphere(const Ray & ray,
                            const glm::dvec4 &centre, const double &radius)
{
    glm::dvec4 pixelDelta = ray.getDirection();
    glm::dvec4 eyeToCentre = ray.getEyePoint() - centre;

    return isDeterminantPositive(
        glm::dot(pixelDelta, pixelDelta), glm::dot(eyeToCentre, pixelDelta),
        glm::dot(eyeToCentre, eyeToCentre) - radius * radius);
}

// -------------------------------------------------
static inline std::vector<glm::dvec4>
getBoxFaceVertices(const glm::dvec4 &corner, const glm::dvec3 &width,
                   const size_t &face)
{
    switch (face) {
    case 0: // back face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z, 1.0)};
        break;
    case 1: // left face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z, 1.0)};
        break;
    case 2: // bottom face
        return {glm::dvec4(corner.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width.z, 1.0)};
        break;
    case 3: // front face
        return {glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z + width.z,
                           1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x, corner.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z + width.z, 1.0)};
        break;
    case 4: // right face
        return {glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z + width.z,
                           1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z + width.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y, corner.z, 1.0),
                glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z, 1.0)};
        break;
    default: // top face
        return {glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z + width.z,
                           1.0),
                glm::dvec4(corner.x + width.x, corner.y + width.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z, 1.0),
                glm::dvec4(corner.x, corner.y + width.y, corner.z + width.z, 1.0)};
        break;
    }
}

std::optional<Intersection>
findRayBoxIntersection(const Ray &ray, const glm::dvec4 &corner, const glm::dvec3 &width) {
    double t = std::numeric_limits<double>::max();
    glm::dvec4 normal;

    for (size_t i = 0; i < 6; ++i) {
        std::optional<Intersection> result{std::nullopt};

        result = findRayPolygonIntersection(
            ray, getBoxFaceVertices(corner, width, i));

        if (result.has_value() && result->getT() < t && result->getT() >= 0.0) {
            t = result->getT();
            normal = result->getNormal();
        }
    }

    if (t != std::numeric_limits<double>::max())
    {
        return Intersection(t, normal);
    } else {
        return std::nullopt;
    }
}

std::optional<Intersection>
findRayBoxIntersection(const Ray &ray, const glm::dvec4 &corner, const double &width) {
    double t = std::numeric_limits<double>::max();
    glm::dvec4 normal;
    glm::dvec4 position;
    glm::dvec2 uvCoord;
    unsigned int uvIndex;

    for (size_t i = 0; i < 6; ++i) {
        std::optional<Intersection> result{std::nullopt};

        std::vector<glm::dvec4> boxFaceVertices =
            getBoxFaceVertices(corner, glm::dvec3(width), i);

        result = findRayPolygonIntersection(ray, boxFaceVertices);

        if (result.has_value() && result->getT() < t && result->getT() >= 0.0) {
            t = result->getT();
            normal = result->getNormal();
            position = evaluate(ray, t);

            // DLOG("corner: %s", glm::to_string(corner).c_str())
            // DLOG("position: %s", glm::to_string(position).c_str())

            uvCoord = getUnitCubeFaceUVMap(glm::dvec3(position), boxFaceVertices, width);
            uvIndex = i;
        }
    }

    if (t != std::numeric_limits<double>::max())
    {
        return Intersection(t, position, glm::normalize(normal), UVLookup(uvCoord, uvIndex));
    } else {
        return std::nullopt;
    }
}

// -------------------------------------------------

// clylinder radius 1, from y = -1 to y = 1
std::optional<Intersection>
findRayCylinderIntersection(const Ray &ray) {
    std::optional<double> closestT = std::nullopt;
    glm::dvec3 normal;

    // intersect with shell
    glm::dvec3 pixelDelta_xz = glm::dvec3(ray.getDirection()) * glm::dvec3(1.0, 0.0, 1.0);
    glm::dvec3 eyePoint_xz = glm::dvec3(ray.getEyePoint()) * glm::dvec3(1.0, 0.0, 1.0);

    double a = glm::dot(pixelDelta_xz, pixelDelta_xz);
    double b = 2 * glm::dot(eyePoint_xz, pixelDelta_xz);
    double c = glm::dot(eyePoint_xz, eyePoint_xz) - 1;

    if (isDeterminantPositive(a, b, c)) {
        auto temp = getClosestPositiveRoot(a, b, c);
        if (temp && evaluate(ray, temp.value()).y < 1 && evaluate(ray, temp.value()).y > -1) {
            closestT = temp;
            normal = glm::dvec3(evaluate(ray, closestT.value())) * glm::dvec3(1.0, 0.0, 1.0);
        }
    }

    // intersect with top and bottom
    glm::dvec3 top (0,1,0);
    glm::dvec3 bottom (0,-1,0);

    auto topIntersect = findRayPlaneIntersect(ray, top, top);

    if (topIntersect)
    {
        if(glm::length(glm::dvec3(evaluate(ray, topIntersect.value())) - top) < 1) {
            if(!closestT || topIntersect.value() < closestT.value())
            {
                closestT = topIntersect;
                normal = top;
            }
        }
    }

    auto bottomIntersect = findRayPlaneIntersect(ray, bottom, bottom);

    if (bottomIntersect)
    {
        if (glm::length(glm::dvec3(evaluate(ray, bottomIntersect.value())) -
                        bottom) < 1)
        {
            if (!closestT || bottomIntersect.value() < closestT.value())
            {
                closestT = bottomIntersect;
                normal = bottom;
            }
        }
    }

    if (!closestT)
    {
        return std::nullopt;
    }
    else
    {
        // take the closest (positive)
        return std::make_optional<Intersection>(closestT.value(),
                                                glm::dvec4(normal, 0.0));
    }
}

// -------------------------------------------------

// cone radius 1, height 1
std::optional<Intersection>
findRayConeIntersection(const Ray &ray) {
    std::optional<double> closestT = std::nullopt;
    glm::dvec3 normal;

    // intersect with shell
    glm::dvec3 pixelDelta = glm::dvec3(ray.getDirection());
    glm::dvec3 eyePoint = glm::dvec3(ray.getEyePoint());
    glm::dvec3 flipY = glm::dvec3(1.0, -1.0, 1.0);

    double a = glm::dot(pixelDelta, pixelDelta * flipY);
    double b = 2 * glm::dot(eyePoint, pixelDelta * flipY);
    double c = glm::dot(eyePoint, eyePoint * flipY);

    if (isDeterminantPositive(a, b, c)) {
        auto temp = getClosestPositiveRoot(a, b, c);
        if (temp && evaluate(ray, temp.value()).y < 1 &&
            evaluate(ray, temp.value()).y > 0)
        {
            closestT = temp;
            normal = glm::dvec3(evaluate(ray, closestT.value())) *
                         glm::dvec3(1.0, 0.0, 1.0) +
                     glm::dvec3(0.0, -1.0, 0.0);
        }
    }

    // intersect with top and bottom
    glm::dvec3 top (0,1,0);
    glm::dvec3 bottom (0,-1,0);

    auto topIntersect = findRayPlaneIntersect(ray, top, top);

    if (topIntersect)
    {
        if(glm::length(glm::dvec3(evaluate(ray, topIntersect.value())) - top) < 1) {
            if(!closestT || topIntersect.value() < closestT.value())
            {
                closestT = topIntersect;
                normal = top;
            }
        }
    }

    if (!closestT)
    {
        return std::nullopt;
    }
    else
    {
        // take the closest (positive)
        return std::make_optional<Intersection>(closestT.value(),
                                                glm::dvec4(normal, 0.0));
    }
}

// -------------------------------------------------
std::optional<Intersection>
findRayPolygonIntersection(const Ray &ray, const std::vector<glm::dvec4> &vertices) {
    if (vertices.size() < 3) {
        return std::nullopt;
        DLOG("findRayPolygonIntersection: less than 3 vertices supplied");
    }

    glm::dvec4 polyNormal(glm::cross(glm::dvec3(vertices[2] - vertices[1]), glm::dvec3(vertices[0] - vertices[1])), 0.0);
    std::optional<double> t = findRayPlaneIntersect(ray, polyNormal, vertices[0]);

    CoordinatePlane closestCoordinatePlane = getClosestCoordinatePlane(polyNormal);
    VertexOrder order = isNonCoordinatePlaneValueNonNegative(polyNormal, closestCoordinatePlane) ? VertexOrder::CCW : VertexOrder::CW;

    if (t.has_value()) {
        glm::dvec2 projectedPoint = projectOntoPlane(evaluate(ray, t.value()), closestCoordinatePlane);

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
        return Intersection(t.value(), polyNormal);
    } else {
        return std::nullopt;
    }
}

// -------------------------------------------------
std::optional<double> findRayPlaneIntersect(const Ray & ray,
                                            const glm::dvec4 &planeNormal,
                                            const glm::dvec4 &planePoint)
{
    if (doesRayIntersectPlane(ray, planeNormal)) {
        return glm::dot(planeNormal, planePoint - ray.getEyePoint()) / glm::dot(planeNormal, ray.getDirection());
    }
    else {
        return std::nullopt;
    }
}


std::optional<double> findRayPlaneIntersect(const Ray & ray,
                                            const glm::dvec3 &planeNormal,
                                            const glm::dvec3 &planePoint)

{
    return findRayPlaneIntersect(ray, glm::dvec4(planeNormal, 0.0), glm::dvec4(planePoint, 1.0));
}

// -------------------------------------------------
Intersection operator*(const glm::dmat4 & mat, const Intersection & intersection)
{
    Intersection newIntersection(intersection);

    newIntersection.setPosition(mat * newIntersection.getPosition());
    newIntersection.setNormal(newIntersection.getNormal() * glm::inverse(mat)); // inverse transpose

    return newIntersection;
}
