#pragma once

#include "SceneNode.hpp"
#include "KDTree.hpp"
#include "Photon.hpp"

SceneNode * createPhotonScene(const KDTree<Photon, double> & kdTree);
