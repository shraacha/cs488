#include <array>
#include <functional>
#include <optional>

#include "Image.hpp"

inline constexpr unsigned int elementsInKernel(unsigned int rings)
{
    return (rings * 2 + 1) * (rings * 2 + 1);
}

/* 0 rings => only applies to the current pixel
 * 1 ring  => applies to a 3x3 area around the pixel
 * etc
 */
template <unsigned int rings, typename T>
struct ImageKernel
{
    using param_t = const std::array<std::optional<T> , elementsInKernel(rings)> &;

    ImageKernel(const std::function<T(param_t)> & func) : m_function(func) {}

    T operator()(param_t elements) const
    {
        return m_function(elements);
    };

    const std::function<T(param_t)> m_function;
};

template <unsigned int rings, typename T>
Image apply(const Image& source, ImageKernel<rings, T> kernel)
{
    Image newImage(source.width(), source.height());
    // loop over each pixel
    for (unsigned int y = 0; y < source.height(); ++y)
    {
        for (unsigned int x = 0; x < source.width(); ++x)
        {
            // loop over the kernel
            std::array<std::optional<T>, elementsInKernel(rings)> elements;
            unsigned int elementNum = 0;

            for (unsigned int i = y - rings; i < y + rings + 1; ++i)
            {
                for (unsigned int j = x - rings; j < x + rings + 1; ++j)
                {
                    if (i >= 0 && i < source.height() && j >= 0 && j < source.width()) {
                        elements[elementNum] = source(j, i);
                    } else
                    {
                        elements[elementNum] = std::nullopt;
                    }
                    ++elementNum;
                }
            }

            newImage.set(x, y, kernel(elements));
        }
    }

    return newImage;
}
