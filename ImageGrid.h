#ifndef IMAGE_GRID_1D_Header
#define IMAGE_GRID_1D_Header


#include "Image.h"
#include "Pixel.h"


// Declares a class to contain a grid of colors that will take an image, split
// it into sections, and identify the average color of all pixels in each
// section.
class ImageGrid {
	public:
		// Constructors
		ImageGrid(Image inputImage);
		ImageGrid(std::string filepathString);

		// Getters
		const std::string getFilepath();
		static const unsigned int getNumSectionsInRow();
		const Pixel checkSection(unsigned int x, unsigned int y) const;
		const unsigned int getWidth() const;
		const unsigned int getHeight() const;
		const double getHeightWidthRatio() const;
		const unsigned int getTotalPx() const;

		// External methods
		void printSectionsRgb() const;
		void printSectionDescriptions() const;
		const unsigned int compareImageDimensions(ImageGrid * inputGrid,
			double threshold = 0.05) const;
		const unsigned int compareImageSections(ImageGrid * inputGrid) const;
		static const int findAndOutputImageDuplicates(
			std::string imageDirectory = "");
		const std::string getDetailsAsString(unsigned int comparedImagePx);


	private:
		// Internal methods
		void populateSections(Image inputImage);

		// Attributes
		static const unsigned int numSectionsInRow = 10;
		Pixel sections[numSectionsInRow * numSectionsInRow];
		std::string filepath;
		std::string absoluteFilepath;
		unsigned int width;
		unsigned int height;
};



#endif


/* 
Size of an ImageGrid: 80 + vector contents
Size of a Pixel: 12
Size of a vector of Pixels: 32 + vector contents
Size of a vector of vectors of Pixels: 32 + vector contents (a lot)
*/