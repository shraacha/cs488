#include "Buffer.hpp"

#include <cstring>

Buffer::Buffer(unsigned int width, unsigned int height)
    : m_width(width), m_height(height)
{
    size_t numElements = m_width * m_height;
    m_data = new glm::dvec4[numElements];
    memset(m_data, 0, numElements * sizeof(glm::dvec4));
}
