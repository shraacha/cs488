#pragma once

#include <vector>
#include <utility>

// numLayers should be >= 2, numSegments should be >= 1

// generates the vertices and the indices for a sphere centered at the origin
std::pair<std::vector<float>, std::vector<unsigned int> >
generateLayeredSphereVerticesAndIndices(const float &radius,
                                        const unsigned int &numLayers,
                                        const unsigned int &numSegments);

std::vector<float>
generateLayeredSphereVertices(const float &radius,
                              const unsigned int &numLayers,
                              const unsigned int &numSegments);

std::vector<unsigned int> generateLayeredSphereIndices(const float &radius,
                                              const unsigned int &numLayers,
                                              const unsigned int &numSegments);
