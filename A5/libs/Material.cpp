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

bool Material::isDirect() const
{
    return m_typeFlags & MaterialTypeFlags::Direct;
}

bool Material::isSpecularReflective() const
{
    return m_typeFlags & MaterialTypeFlags::SpecularReflective;
}

bool Material::isTransmissiveReflective() const
{
    return m_typeFlags & MaterialTypeFlags::TransmissiveReflective;
}

bool Material::isTransmissiveRefractive() const
{
    return m_typeFlags & MaterialTypeFlags::TransmissiveRefractive;
}

bool Material::isTransmissiveDiffuse() const
{
    return m_typeFlags & MaterialTypeFlags::TransmissiveDiffuse;
}

double Material::getIOR() const
{
    return m_ior;
}

void Material::setAlbedoMap(std::shared_ptr<Image> imagePtr)
{
    m_albedoMap = imagePtr;
}
