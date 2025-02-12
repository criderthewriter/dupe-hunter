#ifndef PIXEL_HEADER
#define PIXEL_HEADER


#include <iostream>


// Declares an object to hold an HSL (hue saturation lightness) representation
// of a Pixel instance's RGB values.
typedef struct {
	unsigned int hue, saturation, lightness;
} HslPixel;


// Defines the greatest difference between two Pixels' colors, which is the
// difference between a black Pixel and a white Pixel, as an upper bound for
// color difference analysis.
#define MAX_DISTANCE 764.8333152


// Defines a class to hold the RGB values of a single pixel and various methods
// for comparing and manipulating those values.
class Pixel
{
	public:
		// Constructors
		Pixel(unsigned int redInput, unsigned int greenInput,
			unsigned int blueInput);
		Pixel();

		// Getters
		const unsigned int getRed() const { return red; };
		const unsigned int getGreen() const { return green; };
		const unsigned int getBlue() const { return blue; };

		// Color comparison methods
		HslPixel convertRgbToHsl() const;
		static std::string describeHslColor(HslPixel inputHsl);
		static double calculateColorDistance(Pixel e1, Pixel e2);
		const bool pixelsAreIdentical(Pixel pixelToCompare) const;

	private:
		unsigned int red = 0;
		unsigned int green = 0;
		unsigned int blue = 0;

};


#endif