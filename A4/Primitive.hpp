// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

class Primitive {
public:
  virtual ~Primitive();
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius);
  virtual ~NonhierSphere();

  glm::dvec4 getPosAsDvec4();
  double getRadius();

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual ~NonhierBox();

  glm::dvec4 getPosAsDvec4();
  double getSize();

private:
  glm::vec3 m_pos;
  double m_size;
};

class BoundingBox : public Primitive {
public:
  BoundingBox();
  BoundingBox(const glm::dvec3& pos, const glm::dvec3& size);
  virtual ~BoundingBox();

  glm::dvec4 getPosAsDvec4() const;
  glm::dvec3 getPos() const;
  glm::dvec3 getSize() const;

private:
  glm::vec3 m_pos;
  glm::vec3 m_size;
};
