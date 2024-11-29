// Termm--Fall 2024

#include "Material.hpp"

#include "debug.hpp"

Material::Material()
{}

Material::Material(unsigned int flags, double ior) : m_typeFlags(flags), m_ior(ior) {}

Material::~Material()
{}

bool Material::isDirect() const {
    return m_typeFlags & MaterialTypeFlags::Direct;
}

bool Material::isReflective() const {
    return m_typeFlags & MaterialTypeFlags::Reflective;
}

bool Material::isRefractive() const {
    return m_typeFlags & MaterialTypeFlags::Refractive;
}
