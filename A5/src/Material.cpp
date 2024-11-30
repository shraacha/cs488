// Termm--Fall 2024

#include "Material.hpp"

#include "Helpers.hpp"

Material::Material()
{}

Material::Material(unsigned int flags, double ior) : m_typeFlags(flags), m_ior(ior) {}

Material::~Material()
{}


MaterialAction decideMaterialAction(double reflect, double transmit)
{
    double rand = getRand0To1();

    if (rand <= reflect) {
        return MaterialAction::Reflect;
    } else if (rand <= transmit)
    {
        return MaterialAction::Transmit;
    } else {
        return MaterialAction::Absorb;
    }
}

bool Material::isReflective() const {
    return m_typeFlags & MaterialTypeFlags::Reflective;
}

bool Material::isRefractive() const {
    return m_typeFlags & MaterialTypeFlags::Refractive;
}

bool Material::isDirect() const {
    return m_typeFlags & MaterialTypeFlags::Direct;
}


double Material::getIOR() const
{
    return m_ior;
}
