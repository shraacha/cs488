// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>
#include <optional>

#include "Intersection.hpp"
#include "Ray.hpp"

class Primitive {
  public:
    // ctors
    virtual ~Primitive();

    // other
    virtual std::optional<Intersection> intersect(const Ray & ray) const = 0;
};

class Sphere : public Primitive {
  public:
    // ctors
    Sphere();
    virtual ~Sphere();

    // other
    std::optional<Intersection> intersect(const Ray & ray) const override;
};

class Cube : public Primitive {
  public:
    // ctors
    Cube();
    virtual ~Cube();

    // other
    std::optional<Intersection> intersect(const Ray & ray) const override;
};

class NonhierSphere : public Primitive {
  public:
    // ctors
    NonhierSphere(const glm::vec3 & pos, double radius);
    virtual ~NonhierSphere();

    // other
    std::optional<Intersection> intersect(const Ray & ray) const override;

    // getters and accessors
    glm::dvec4 getPosAsDvec4() const;
    double getRadius() const;

  private:
    glm::vec3 m_pos;
    double m_radius;
};

class NonhierBox : public Primitive {
  public:
    // ctors
    NonhierBox(const glm::vec3 & pos, double size);
    virtual ~NonhierBox();

    // other
    std::optional<Intersection> intersect(const Ray & ray) const override;

    // getters and accessors
    glm::dvec4 getPosAsDvec4() const;
    double getSize() const;

  private:
    glm::vec3 m_pos;
    double m_size;
};

class BoundingBox : public Primitive {
  public:
    // ctors
    BoundingBox();
    BoundingBox(const glm::dvec3 & pos, const glm::dvec3 & size);
    virtual ~BoundingBox();

    // other
    std::optional<Intersection> intersect(const Ray & ray) const override;

    // getters and accessors
    glm::dvec4 getPosAsDvec4() const;
    glm::dvec3 getPos() const;
    glm::dvec3 getSize() const;

  private:
    glm::vec3 m_pos;
    glm::vec3 m_size;
};
