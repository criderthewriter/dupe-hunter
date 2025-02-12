#ifndef IMAGE_HEADER
#define IMAGE_HEADER


#include <iostream>
#include <vector>


#include "DupeHunter.h"
#include "Pixel.h"


// Defines a class to represent an image with its pixels and their color values
// loaded.
class Image {
	public:
		// Constructors
		Image(std::string filepathString);

		// Getters
		const int getWidth() const;
		const int getHeight() const;
		const std::string getFilepath() const;

		// External methods
		static bool fileIsImage(pathObject inputPath);
		Pixel getColorsAtCoordinates(int x, int y);
		Pixel getAverageColorOfImageSlice(
			unsigned int leftEdge, unsigned int rightEdge,
			unsigned int topEdge, unsigned int bottomEdge);
		static double calculateAverageDifference(Image image1, Image image2);
		static double getDifferenceAsPercentage(double input);

	private:
		// Internal methods
		static bool loadImage(std::vector<unsigned char>& image,
			const std::string& filename, int& x, int& y);

		// Attributes
		std::string filepath;
		std::vector<unsigned char> imageData;
		int width;
		int height;
};


#endif