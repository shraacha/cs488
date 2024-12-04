#include "Photon.hpp"

#include <vector>
#include <mutex>

struct PhotonCollector {
    void addPhotons(const std::vector<Photon> & photons);

    std::vector<Photon> m_photons;
    std::mutex m_addMutex;
};
