// Termm--Fall 2024

#include <iostream>
#include <fstream>
#include <limits>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "Primitive.hpp"
#include "IntersectionHelpers.hpp"
#include "debug.hpp"

// helpers
static BoundingBox generateBoundingBox(const Mesh &mesh)
{
    glm::dvec3 minCorner(std::numeric_limits<double>::max());
    glm::dvec3 maxCorner(-std::numeric_limits<double>::max());

    for (const auto & vertex : mesh.getVertices()) {
      if(vertex.x < minCorner.x) minCorner.x = vertex.x;
      if(vertex.y < minCorner.y) minCorner.y = vertex.y;
      if(vertex.z < minCorner.z) minCorner.z = vertex.z;

      if(vertex.x > maxCorner.x) maxCorner.x = vertex.x;
      if(vertex.y > maxCorner.y) maxCorner.y = vertex.y;
      if(vertex.z > maxCorner.z) maxCorner.z = vertex.z;
    }

    if (minCorner.x == maxCorner.x) maxCorner.x += 0.001;
    if (minCorner.y == maxCorner.y) maxCorner.y += 0.001;
    if (minCorner.z == maxCorner.z) maxCorner.z += 0.001;

    return BoundingBox(minCorner, maxCorner - minCorner);
}

size_t readFaceVertEatRest(std::ifstream & ifs) {
    static unsigned int count = 0;
    size_t faceVert;
    size_t dump;

    // read vert info
    ifs >> faceVert;

    if (ifs.peek() == '/') {
        ifs.get();
    } else {
        // no tex or normal info
        return faceVert;
    }

    if (ifs.peek() == '/') {
        ifs.get();
        ifs >> dump;
    } else {
        ifs >> dump;

        if (ifs.peek() == '/') {
            ifs.get();
            ifs >> dump;
        }
    }

    if (count++ < 10) {
        DLOG("faceVert %lu", faceVert);
    }

    return faceVert;
}


// ctors
Mesh::Mesh(const std::string &fname) : m_vertices(), m_faces() {
    std::string code;
    double vx, vy, vz;
    size_t s1, s2, s3;

    std::ifstream ifs(fname.c_str());
    while (ifs >> code) {
        if (code == "v") {
            ifs >> vx >> vy >> vz;
            m_vertices.push_back(glm::vec3(vx, vy, vz));
        } else if (code == "f") {
            s1 = readFaceVertEatRest(ifs);
            s2 = readFaceVertEatRest(ifs);
            s3 = readFaceVertEatRest(ifs);

            m_faces.push_back(Triangle(s1 - 1, s2 - 1, s3 - 1));
        }
    }

    m_boundingBox = generateBoundingBox(*this);
}

std::ostream &operator<<(std::ostream &out, const Mesh &mesh) {
    out << "mesh {";
    /*

    for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
          const MeshVertex& v = mesh.m_verts[idx];
          out << glm::to_string( v.m_position );
          if( mesh.m_have_norm ) {
            out << " / " << glm::to_string( v.m_normal );
          }
          if( mesh.m_have_uv ) {
            out << " / " << glm::to_string( v.m_uv );
          }
    }

  */
    out << "}";
    return out;
}

//  ~~~ other ~~~
std::optional<Intersection> Mesh::intersectDirectlyWithMesh(const Ray & ray) const
{
    double t = std::numeric_limits<double>::max();
    glm::dvec4 normal;
    const std::vector<glm::vec3> & meshVerts = m_vertices;
    std::optional<Intersection> result{std::nullopt};

    for (const Triangle & face : m_faces) {
        result = findRayPolygonIntersection(
            ray, {glm::dvec4(meshVerts[face.v1], 1.0),
                  glm::dvec4(meshVerts[face.v2], 1.0),
                  glm::dvec4(meshVerts[face.v3], 1.0)});

        if (result.has_value() && result->getT() < t &&
            result->getT() >= ray.getMinThreshold()) {
            t = result->getT();
            normal = result->getNormal();
        }
    }

    if (t != std::numeric_limits<double>::max()) {
        return Intersection(t, normal);
    } else {
        return std::nullopt;
    }
}

std::optional<Intersection> Mesh::intersect(const Ray & ray) const {
#ifdef RENDER_BOUNDING_VOLUMES
    std::optional<Intersection> result = m_boundingBox.intersect(ray);
#else
    std::optional<Intersection> result{std::nullopt};
    auto boundingBoxIntersection = m_boundingBox.intersect(ray);
    if (boundingBoxIntersection.has_value() &&
        boundingBoxIntersection->getT() >= 0) {
        result = intersectDirectlyWithMesh(ray);
    } else {
        result = std::nullopt;
    }
#endif

    if (result.has_value()) {
        result->setPosition(evaluate(ray, result->getT()));
    }

    return result;
}

// ~~~ getters and accessors ~~~
const std::vector<glm::vec3> &Mesh::getVertices() const { return m_vertices; }

const std::vector<Triangle> &Mesh::getFaces() const { return m_faces; }

BoundingBox Mesh::getBoundingBox() const { return m_boundingBox; }
