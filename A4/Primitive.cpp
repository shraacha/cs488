// Termm--Fall 2024

#include "Primitive.hpp"

Primitive::~Primitive()
{
}

Sphere::Sphere()
{
  m_primitiveType = PrimitiveType::Sphere;
}

Sphere::~Sphere()
{
}

Cube::Cube()
{
  m_primitiveType = PrimitiveType::Cube;
}

Cube::~Cube()
{
}

NonhierSphere::NonhierSphere(const glm::vec3 &pos, double radius)
    : m_pos(pos), m_radius(radius)
{
  m_primitiveType = PrimitiveType::NHSphere;
}

NonhierSphere::~NonhierSphere()
{
}

NonhierBox::NonhierBox(const glm::vec3 &pos, double size) : m_pos(pos), m_size(size)
{
  m_primitiveType = PrimitiveType::NHBox;
}

NonhierBox::~NonhierBox()
{
}
