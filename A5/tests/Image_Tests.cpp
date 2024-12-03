#include <gtest/gtest.h>

#include <glm/ext.hpp>

#include <iostream>

#include "Image.hpp"
#include "ImageSampleHelpers.hpp"

TEST(ImageTests, ImageSample)
{
    Image image("./Assets/textures/Ball1.png");



    EXPECT_EQ(image(0, 0), sample(image, glm::dvec2{0,0}));
    EXPECT_EQ(image(image.width(), image.height()), sample(image, glm::dvec2{1,1}));
}
