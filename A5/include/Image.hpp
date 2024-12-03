// Termm--Fall 2024

#pragma once

#include "lodepng/lodepng.h"
#include <glm/glm.hpp>

#include <string>
#include <array>

const uint c_colorComponents = 3; // Red, blue, green

typedef unsigned int uint;

template <typename T, size_t size>
struct Pixel {
	std::array<T, size> data;

	T operator[](size_t i) const
	{
		return data[i];
	}

	T& operator[](size_t i)
	{
		return data[i];
	}
};

/**
 * An image, consisting of a rectangle of floating-point elements.
 * Each pixel element consists of 3 components: Red, Blue, and Green.
 *
 * This class makes it easy to save the image as a PNG file.
 * Note that colours in the range [0.0, 1.0] are mapped to the integer
 * range [0, 255] when writing PNG files.
 */
class Image {
public:
	// Construct an empty image.
	Image();

	// Construct a black image at the given width/height.
	Image(uint width, uint height);

	// Construct an image from a png
	Image(const std::string & filename, LodePNGColorType colorType = LCT_RGB);

	// Copy an image.
	Image(const Image & other);

	// Move an image.
	Image(const Image && other);

	~Image();

	// Copy the data from one image to another.
	Image & operator=(const Image & other);

	// Returns the width of the image.
	uint width() const;

	// Returns the height of the image.
	uint height() const;

    // Retrieve a particular component from the image.
	double operator()(uint x, uint y, uint i) const;

	// Retrieve a particular component from the image.
	double & operator()(uint x, uint y, uint i);

    // Retrieve a particular component from the image.
	glm::dvec3 operator()(uint x, uint y) const;

    // Retrieve a particular component from the image.
	void set(uint x, uint y, const glm::dvec3 & pixel);

	// Save this image into the PNG file with name 'filename'.
	// Warning: If 'filename' already exists, it will be overwritten.
	bool savePng(const std::string & filename) const;

	const double * data() const;
	double * data();

private:
	uint m_width;
	uint m_height;
	double * m_data;

	static const uint m_colorComponents = c_colorComponents;
};
