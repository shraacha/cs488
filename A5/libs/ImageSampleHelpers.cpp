#include "ImageSampleHelpers.hpp"

// uv coordinates should be between 0 and 1
glm::dvec3 sample(const Image & image, const glm::dvec2 & uvCoordinate)
{
    double width = image.width();
    double height = image.height();

    int x = uvCoordinate.x * width;
    int y = uvCoordinate.y * height;

    return image(x, y);
}
