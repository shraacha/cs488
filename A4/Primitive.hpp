// Termm--Fall 2024

#pragma once

#include <glm/glm.hpp>

enum class PrimitiveType {
  NHSphere,
  NHBox,
  Sphere,
  Cube,
  Mesh
};

class Primitive {
public:
  virtual ~Primitive();

  PrimitiveType m_primitiveType;
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

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
};
