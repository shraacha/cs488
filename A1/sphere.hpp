#pragma once

#include <vector>
#include <utility>

// generates the vertices and the indices for a sphere centered at the origin
std::pair<std::vector<float>, std::vector<int> >
generateStackedSphereVerticesAndIndices(const float &radius,
                                        const unsigned int &numLayers);
