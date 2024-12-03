#include "ImageKernelHelpers.hpp"

glm::dvec3 varianceThresholdKernelFunc(
    const std::array<std::optional<glm::dvec3>, 1> & elements)
{
    double threshold = 0.1;

    glm::dvec3 returnPixel(0.0);

    for (const auto & pixel : elements)
    {
        if (pixel)
        {
            if (glm::dot(pixel.value(), glm::dvec3(1)) > threshold)
            {
                returnPixel = glm::dvec3(1.0);
            }
        }
    }

    return returnPixel;
}
