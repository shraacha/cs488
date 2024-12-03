#include "ImageHelpers.hpp"

std::vector<glm::uvec2> getWhitePixelCoordinates (const Image & image)
{
    std::vector<glm::uvec2> pixels;
    for (unsigned int y = 0; y < image.height(); ++y)
    {
        for (unsigned int x = 0; x < image.width(); ++x)
        {
            if (image(x,y) == glm::dvec3(1.0)) {
                pixels.emplace_back(x, y);
            }
        }
    }

    return pixels;
}

std::vector<glm::uvec2> getAllPixelCoordinates(const Image & image)
{
    std::vector<glm::uvec2> pixels;
    for (unsigned int y = 0; y < image.height(); ++y)
    {
        for (unsigned int x = 0; x < image.width(); ++x)
        {
            pixels.emplace_back(x, y);
        }
    }

    return pixels;
}
