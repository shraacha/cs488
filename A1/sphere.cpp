#include <cmath>
#include <iostream>

#include "sphere.hpp"

std::pair<std::vector<float>, std::vector<unsigned int> >
generateLayeredSphereVerticesAndIndices(const float &radius,
                                        const unsigned int &numLayers,
                                        const unsigned int &numSegments){
  return std::make_pair(
      generateLayeredSphereVertices(radius, numLayers, numSegments),
      generateLayeredSphereIndices(radius, numLayers, numSegments));
}

/*
 * diagramatically: with x to the right, y up, and z towards the screen
 * x = [r * cos(lambda)] * cos(theta)
 * y = [r * sin(lambda)]
 * z = [r * cos(lambda)] * sin(theta)
 *
 * - lambda is the vertical angle from y = 0
 *   Subdivide lambda like so:
 *   lambda = 3PI/4 * PI * (currentLayerNum / numLayers)
 * - theta is the horizontal angle from x = 0
 *   Subdivide theta like so:
 *   theta = 2PI * (currentSegmentNum / numSegments)
*/
std::vector<float>
generateLayeredSphereVertices(const float &radius,
                              const unsigned int &numLayers,
                              const unsigned int &numSegments) {
    std::vector<float> vertices;
    float layerStep = M_PI / numLayers;
    float segmentStep = -2.0 * M_PI / numSegments;

    // layers start at the top and go downwards
    //
    // Note, we do '<= num<layers/segments>' since we want to include the final layer!
    // Otherwise, we miss the bottom points, or life gets a little harder when connecting vertices for triangle strips
    for(unsigned int currentLayerNum = 0; currentLayerNum <= numLayers; currentLayerNum++ ) {
        float lambda = M_PI_2 - currentLayerNum * layerStep;
        for (unsigned int currentSegmentNum = 0; currentSegmentNum <= numSegments; currentSegmentNum++) {
            float theta = currentSegmentNum * segmentStep;

            float x = (radius * cosf(lambda) * cosf(theta)); // x
            float y = (radius * sinf(lambda));                  // y
            float z = (radius * cosf(lambda) * sinf(theta)); // z

            // std::cout<< "(" << x << ", " << y << ", " << z << ")" << std::endl; // TESTING

            vertices.emplace_back(radius * cosf(lambda) * cosf(theta)); // x
            vertices.emplace_back(radius * sinf(lambda));                  // y
            vertices.emplace_back(radius * cosf(lambda) * sinf(theta)); // z
        }
    }

    return vertices;
}

/*
 * The top layer triangles look like this:
 *
 *     v11
 *     / \
 *    /   \
 *   /     \
 * v21----v22
 *
 * The middle layer triangles look like this:
 * - each segment is composed of pairs of columns of vertices
 *
 * v11---v12
 * |  \   |
 * |   \  |
 * |    \ |
 * v21---v22
 *
 * The bottom layer triangles look like this:
 *
 *  v11----v12
 *   \     /
 *    \   /
 *     \ /
 *     v21
 *
 * We begin at the top layer and go down to the second last layer
 *
 * NOTE this could be optimized but I don't care enough rn.
 */
std::vector<unsigned int> generateLayeredSphereIndices(const float &radius,
                                              const unsigned int &numLayers,
                                              const unsigned int &numSegments) {
    std::vector<unsigned int> indices;
    unsigned int vertsInLayer = numSegments + 1;

    unsigned int v11, v12, v21, v22;
    for(unsigned int currentLayerNum = 0; currentLayerNum < numLayers; currentLayerNum++ ) {
        if (currentLayerNum == 0) {
            for (unsigned int currentSegmentNum = 0; currentSegmentNum < numSegments; currentSegmentNum++) {
                v11 = currentLayerNum * vertsInLayer + currentSegmentNum;
                v21 = v11 + vertsInLayer;
                v22 = v21 + 1;

                indices.push_back(v11);
                indices.push_back(v21);
                indices.push_back(v22);
            }
        } else if (currentLayerNum == numLayers - 1) {
            for (unsigned int currentSegmentNum = 0; currentSegmentNum < numSegments; currentSegmentNum++) {
                v11 = currentLayerNum * vertsInLayer + currentSegmentNum;
                v12 = v11 + 1;
                v21 = v11 + vertsInLayer;

                indices.push_back(v11);
                indices.push_back(v21);
                indices.push_back(v12);
            }
        } else {
            for (unsigned int currentSegmentNum = 0; currentSegmentNum < numSegments; currentSegmentNum++) {
                v11 = currentLayerNum * vertsInLayer + currentSegmentNum;
                v12 = v11 + 1;
                v21 = v11 + vertsInLayer;
                v22 = v21 + 1;

                indices.push_back(v11);
                indices.push_back(v21);
                indices.push_back(v22);

                indices.push_back(v22);
                indices.push_back(v12);
                indices.push_back(v11);
            }
        }
    }
    return indices;
}
