#include "Photon.hpp"

Photon::Photon() {}

Photon::Photon(const glm::dvec3 & power)
    : m_power(power), m_position(0.0), m_incidenceDir(0.0)
{
}

Photon::Photon(const glm::dvec3 & power, const glm::dvec3 & position)
    : m_power(power), m_position(position), m_incidenceDir(0.0)
{
}

glm::dvec3 Photon::getPower() const { return m_power; }

glm::dvec3 Photon::getPosition() const
{
    return m_position;
}

glm::dvec3 Photon::getIncidenceDir() const
{
    return m_incidenceDir;
}

void Photon::piecewiseAverageScale(glm::dvec3 scaler)
{
    double sum = glm::dot(scaler, glm::dvec3(1));

    glm::dvec3 averagedScale(scaler.x / sum, scaler.y / sum, scaler.z / sum);

    m_power = m_power * averagedScale;
}

void Photon::setPower(const glm::dvec3 & val)
{
    m_power = val;
}

void Photon::setPosition(const glm::dvec3 & val)
{
    m_position = val;
}

void Photon::setIncidenceDir(const glm::dvec3 & val)
{
    m_incidenceDir = val;
}

bool PhotonCompare1::operator()(const Photon & first, const Photon & second)
{
    return first.m_position.x < second.m_position.x;
}

bool PhotonCompare2::operator()(const Photon & first, const Photon & second)
{
    return first.m_position.y < second.m_position.y;
}

bool PhotonCompare3::operator()(const Photon & first, const Photon & second)
{
    return first.m_position.z < second.m_position.z;
}

double PhotonDistance1::operator()(const Photon & first, const Photon & second)
{
    return glm::length(first.m_position.x - second.m_position.x);
}

double PhotonDistance2::operator()(const Photon & first, const Photon & second)
{
    return glm::length(first.m_position.y - second.m_position.y);
}

double PhotonDistance3::operator()(const Photon & first, const Photon & second)
{
    return glm::length(first.m_position.z - second.m_position.z);
}
