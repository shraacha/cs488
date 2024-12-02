#include "ImageKernelHelpers.hpp"

#include "glm/detail/func_vector_relational.hpp"

glm::dvec3 varianceThresholdKernelFunc(
    const std::array<std::optional<glm::dvec3>, 1> & elements)
{
    double threshold = 0.1;

    glm::dvec3 returnPixel(0.0);

    for (const auto & pixel : elements)
    {
        if (pixel)
        {
            if (glm::any(glm::greaterThanEqual(pixel.value(),
                                               glm::dvec3(threshold))))
            {
                returnPixel = glm::dvec3(1.0);
            }
        }
    }

    return returnPixel;
}
