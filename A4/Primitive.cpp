// Termm--Fall 2024

#include "Primitive.hpp"

// ------------------------------
Primitive::~Primitive() {}

// ------------------------------
Sphere::Sphere() {}

Sphere::~Sphere() {}

// ------------------------------
Cube::Cube() {}

Cube::~Cube() {}

// ------------------------------
NonhierSphere::NonhierSphere(const glm::vec3 &pos, double radius)
    : m_pos(pos), m_radius(radius)
{
}

NonhierSphere::~NonhierSphere() {}

glm::dvec4 NonhierSphere::getPosAsDvec4()
{
    return {m_pos.x, m_pos.y, m_pos.z, 1.0};
}

double NonhierSphere::getRadius() { return m_radius; }

// ------------------------------
NonhierBox::NonhierBox(const glm::vec3 &pos, double size)
    : m_pos(pos), m_size(size)
{
}

NonhierBox::~NonhierBox() {}
