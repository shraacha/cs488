#include "PhotonCollector.hpp"


void PhotonCollector::addPhotons(const std::vector<Photon> & photons)
{
    std::unique_lock<std::mutex> lock(m_addMutex);
    m_photons.insert(m_photons.end(), photons.begin(), photons.end());
}
