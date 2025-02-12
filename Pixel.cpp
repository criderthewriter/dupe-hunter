#include "DupeHunter.h"
#include "Pixel.h"


/*
* Name: Pixel (constructor)
* Input: three unsigned ints representing the red, green, and blue values of
*	a single pixel
* Output: none
* Description: Constructor for class Pixel.
* 
*/
Pixel::Pixel(unsigned int redInput, unsigned int greenInput,
	unsigned int blueInput) {
	// Error checking.
	const int maxColorValue = 255;
	if (redInput > maxColorValue || greenInput > maxColorValue ||
		blueInput > maxColorValue) {
		std::cerr << "!- " << __func__ << " Constructor Error: Attempted to "
			"generate a pixel with a color value greater than 255: (" <<
			redInput << ", " << greenInput << ", " << blueInput << ")" <<
			std::endl;
		red = 0, green = 0, blue = 0;
	}
	else {
		red = redInput;
		green = greenInput;
		blue = blueInput;
	}
}


/*
* Name: Pixel (default constructor)
* Input: none
* Output: none
* Description: Default constructor for class Pixel.
*
*/
Pixel::Pixel() {
	red = 0;
	green = 0;
	blue = 0;
}


/*
* Name: calculateColorDistance
* Input: Two Pixel instances which contain RGB color values for two pixels
*	to compare
* Output: a double representing the weighted difference between the two
*	pixels' colors
* Description: Compares the RGB values of two pixels and returns a double
*	that approximates how different the pixels are.  Static class method.
*	Written by alonisser and copied from:
*	https://stackoverflow.com/questions/9018016/how-to-compare-two-colors-for-similarity-difference
*/
double Pixel::calculateColorDistance(Pixel e1, Pixel e2) {
	long rmean = ((long)e1.red + (long)e2.red) / 2;
	long r = (long)e1.red - (long)e2.red;
	long g = (long)e1.green - (long)e2.green;
	long b = (long)e1.blue - (long)e2.blue;

	return sqrt((((512 + rmean) * r * r) >> 8)
		+ 4 * g * g + (((767 - rmean) * b * b) >> 8));
}


/*
* Name: convertRgbToHsl
* Input: none
* Output: an HslPixel instance containing the current Pixel's values converted
*	to HSL format
* Description: Converts the Pixel instance's RGB color values to HSL, rounding
*	to the nearest integer, and returns an HslPixel containing the calculated
*	HSL values.
*	Source: https://stackoverflow.com/questions/39118528/rgb-to-hsl-conversion
*/
HslPixel Pixel::convertRgbToHsl() const {
	double tempRed = red / 255.0,
		tempGreen = green / 255.0,
		tempBlue = blue / 255.0;

	double minimum = std::min(tempRed, std::min(tempGreen, tempBlue));
	double maximum = std::max(tempRed, std::max(tempGreen, tempBlue));

	double lightness = 50.0 * (minimum + maximum),
		saturation = 0, hue = 0;

	bool done = false;
	if (doublesAreEqual(minimum, maximum)) {
		saturation = 0;
		hue = 0;
		done = true;
	}
	else if (lightness < 50.0)
	{
		saturation = 100.0 * (maximum - minimum) / (maximum + minimum);
	}
	else {
		saturation = 100.0 * (maximum - minimum) / (2.0 - maximum - minimum);
	}

	if (!done) {
		if (doublesAreEqual(maximum, tempRed)) {
			hue = 60 * (tempGreen - tempBlue) / (maximum - minimum);
		}
		if (doublesAreEqual(maximum, tempGreen)) {
			hue = 60 * (tempBlue - tempRed) / (maximum - minimum) + 120;
		}
		if (doublesAreEqual(maximum, tempBlue)) {
			hue = 60 * (tempRed - tempGreen) / (maximum - minimum) + 240;
		}
		if (hue < 0) {
			hue += 360;
		}
	}

	// Rounds hue, saturation, and lightness to the nearest tenth and returns
	// resulting HslPixel.
	return HslPixel(
		(unsigned int)std::round(hue),
		(unsigned int)std::round(saturation),
		(unsigned int)std::round(lightness)
	);
}


/*
* Name: describeHslColor
* Input: an HslPixel, which is a collection of a single pixel's hue,
*	saturation, and lightness values
* Output: a string containing text which describes the inputted color
* Description: Returns a string that generalizes the passed HSL color values
*	into a named color and hue.  Static class method.
*/
std::string Pixel::describeHslColor(HslPixel inputHsl) {
	std::string hueString = "",
		saturationString = "",
		lightnessString = "",
		result = "";

	// Checks hue to choose a color.
	if (inputHsl.hue <= 15 || inputHsl.hue > 340) {
		hueString = "red";
	}
	else if (inputHsl.hue > 15 && inputHsl.hue <= 40) {
		hueString = "orange";
	}
	else if (inputHsl.hue > 40 && inputHsl.hue <= 65) {
		hueString = "yellow";
	}
	else if (inputHsl.hue > 65 && inputHsl.hue <= 165) {
		hueString = "green";
	}
	else if (inputHsl.hue > 165 && inputHsl.hue <= 185) {
		hueString = "cyan";
	}
	else if (inputHsl.hue > 185 && inputHsl.hue <= 260) {
		hueString = "blue";
	}
	else if (inputHsl.hue > 260 && inputHsl.hue <= 290) {
		hueString = "violet";
	}
	else if (inputHsl.hue > 290 && inputHsl.hue <= 340) {
		hueString = "magenta";
	}

	// Checks saturation to choose how dull or saturated the color is.
	bool colorIsDull = false;
	if (inputHsl.saturation <= 50) {
		saturationString = "dull";
		colorIsDull = true;
	}
	if (inputHsl.saturation <= 25) {
		saturationString = "v.dull";
		colorIsDull = true;
	}
	if (inputHsl.saturation <= 5) {
		saturationString = "";
		hueString = "gray";
	}

	// Checks lightness to choose how bright or dark the color is.
	if (inputHsl.lightness >= 50 && !colorIsDull) {
		lightnessString = "bright";
	}
	if (inputHsl.lightness >= 75) {
		lightnessString = "pale";
	}
	if (inputHsl.lightness <= 30) {
		lightnessString = "dark";
	}

	// Builds a resulting color description.
	if (!saturationString.empty()) result += saturationString + " ";
	if (!lightnessString.empty()) result += lightnessString + " ";
	result += hueString;

	// Handles extreme cases.
	if (inputHsl.lightness >= 97) {
		result = "white";
	}
	if (inputHsl.lightness <= 10) {
		result = "black";
	}

	return result;
}


/*
* Name: pixelsAreIdentical
* Input: a Pixel instance to check against the calling instance
* Output: a bool stating whether the two Pixels are identical
* Description: Checks the color values of the calling Pixel and the passed
*	Pixel, and returns true if their colors match, or returns false otherwise.
*/
const bool Pixel::pixelsAreIdentical(Pixel pixelToCompare) const {
	return red == pixelToCompare.getRed() &&
		green == pixelToCompare.getGreen() &&
		blue == pixelToCompare.getBlue();
}


