#pragma once

#include <glm/glm.hpp>

#include <vector>

class Photon {
    public:
        Photon();
        Photon(const glm::dvec3 & power);
        Photon(const glm::dvec3 & power, const glm::dvec3 & position);

        glm::dvec3 getPower() const;
        glm::dvec3 getPosition() const;
        glm::dvec3 getIncidenceDir() const;

        void piecewiseAverageScale(glm::dvec3 scaler);

        void setPower(const glm::dvec3 & val);
        void setPosition(const glm::dvec3 & val);
        void setIncidenceDir(const glm::dvec3 & val);

        friend struct PhotonCompare1;
        friend struct PhotonCompare2;
        friend struct PhotonCompare3;

        friend struct PhotonDistance1;
        friend struct PhotonDistance2;
        friend struct PhotonDistance3;

    private:
        glm::dvec3 m_power;
        glm::dvec3 m_position;
        glm::dvec3 m_incidenceDir; // vector should be *normalized* and *pointed into the surface*
};

struct PhotonCompare1
{
    bool operator()(const Photon & first, const Photon & second);
};

struct PhotonCompare2
{
    bool operator()(const Photon & first, const Photon & second);
};

struct PhotonCompare3
{
    bool operator()(const Photon & first, const Photon & second);
};

struct PhotonDistance1
{
    double operator()(const Photon & first, const Photon & second);
};

struct PhotonDistance2
{
    double operator()(const Photon & first, const Photon & second);
};

struct PhotonDistance3
{
    double operator()(const Photon & first, const Photon & second);
};


double getDistance(const Photon & p1, const Photon & p2);

Photon findFarthest(const Photon & source, const std::vector<Photon> & photons);
