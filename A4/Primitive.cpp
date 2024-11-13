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
{
    // flip the corner pos so it is always "bottom back left"
    if(size < 0)
    {
        this->m_pos = pos + glm::vec3(size);
        this->m_size = -1.0 * size;
    } else {
        this->m_pos = pos;
        this->m_size = size;
    }
}

NonhierBox::~NonhierBox() {}

glm::dvec4 NonhierBox::getPosAsDvec4()
{
    return {m_pos.x, m_pos.y, m_pos.z, 1.0};
}

double NonhierBox::getSize() {return m_size; }

// ------------------------------
BoundingBox::BoundingBox() {}

BoundingBox::BoundingBox(const glm::dvec3& pos, const glm::dvec3& size)
{
    m_pos = pos;
    m_size = size;

    // flip the corner pos so it is always "bottom back left"
    if(size.x < 0)
    {
        m_pos.x = pos.x + size.x;
        m_size.x = -1.0 * size.x;
    }

    if(size.y < 0)
    {
        m_pos.y = pos.y + size.y;
        m_size.y = -1.0 * size.y;
    }

    if(size.z < 0)
    {
        m_pos.z = pos.z + size.z;
        m_size.z = -1.0 * size.z;
    }
}

BoundingBox::~BoundingBox() {}

glm::dvec4 BoundingBox::getPosAsDvec4() const
{
    return {m_pos.x, m_pos.y, m_pos.z, 1.0};
}

glm::dvec3 BoundingBox::getPos() const
{
    return m_pos;
}

glm::dvec3 BoundingBox::getSize() const
{
    return m_size;
}
