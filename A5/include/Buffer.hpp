#pragma once

#include <glm/glm.hpp>

struct Buffer
{
    Buffer(unsigned int width, unsigned int height);

    unsigned int m_width;
    unsigned int m_height;
    glm::dvec4 *m_data; // colour and depth
};
