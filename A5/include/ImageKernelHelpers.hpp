#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/exponential.hpp>

#include "Image.hpp"
#include "ImageKernel.hpp"

#include "debug.hpp"
#include "glm/gtx/string_cast.hpp"


template <unsigned int rings>
glm::dvec3 varianceKernelFunc(
    const std::array<std::optional<glm::dvec3>, elementsInKernel(rings)> & elements)
{
    // DLOG("elements size: %ld", elements.size());
    int numElements;
    glm::dvec3 mean;
    glm::dvec3 sum;

    // get mean
    for(const auto & pixel : elements) {
        if (pixel) {
            mean += pixel.value();
            ++numElements;
        }
    }

    mean = mean * ((double)1 / (double)numElements);

    for (const auto & pixel : elements)
    {
        if (pixel)
        {
            sum += glm::pow( pixel.value() - mean, glm::dvec3(2));
        }
    }

    return glm::sqrt(sum * ((double) 1 / (double)numElements));
}

glm::dvec3 varianceThresholdKernelFunc(
    const std::array<std::optional<glm::dvec3>, 1> & elements);
