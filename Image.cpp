#include "Image.h"
#include "DupeHunter.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Defines constructor for Image class.
Image::Image(std::string filepathString) {
	filepath = filepathString;
	std::vector<unsigned char> temp;

	// Attempts to populate the new image's width and height.
	bool success = loadImage(temp, filepath, width, height);
	if (!success) {
		std::cerr << "!- " << __func__ <<
			" Error: Could not load image at path:\n\t" << filepathString
			<< std::endl;
		exit(EXIT_FAILURE);
	}
	imageData = temp;

	if (width < 0 || height < 0) {
		std::cerr << "!- " << __func__ << " Error: Negative image dimensions."
			"  Cannot load image." << std::endl;
		exit(EXIT_FAILURE);
	}
}


// Getter for Image class's attribute width.
const int Image::getWidth() const {
	return width;
}


// Getter for Image class's attribute height.
const int Image::getHeight() const {
	return height;
}


// Getter for Image class's attribute filepath.
const std::string Image::getFilepath() const {
	return filepath;
}


// Loads an image's data from a filename, overwrites values passed by 
// reference, and returns true if the image was successfully loaded.  Static
// class method.
// From https://cplusplus.com/forum/beginner/267364/
bool Image::loadImage(std::vector<unsigned char>& image, const std::string& filename,
	int& x, int& y) {
	// Loads as RGBA... even if file is only RGB.
	// Feel free to adjust this if you so please, by changing the 4 to a 0.
	int n;

	unsigned char* data = stbi_load(filename.c_str(), &x, &y, &n, 4);
	if (data != nullptr) {
		image = std::vector<unsigned char>(data, data + x * y * 4);
	}
	stbi_image_free(data);
	return (data != nullptr);
}


/*
* Name: fileIsImage
* Input: A path object that likely contains an image file or subfolder
* Output: A bool stating whether inputPath leads to an image file or not
* Description: Checks the extension of inputPath and returns true if it is
*	an image file, or returns false otherwise.  Static class method.
*/
bool Image::fileIsImage(pathObject inputPath) {
	// Declares all image extensions to be considered valid.  Image types
	// most common to my gallery are listed first for quicker access.
	const std::string validImageExtensions[] = {
		".png",
		".jpg",
		".jpeg",
		".bmp",
		".webp",
		".gif",
		".tif",
		".tiff",
		".heif"
	};
	size_t validExtensionCount =
		sizeof(validImageExtensions) / sizeof(std::string);

	// Checks inputPath's extension to see whether it matches any of the valid
	// image extensions.
	bool isImage = false;
	for (unsigned int i = 0; i < validExtensionCount; i++) {
		if (inputPath.extension().string() == validImageExtensions[i]) {
			isImage = true;
			break;
		}
	}

	return isImage;
}


// Image class method.  Returns the colors of the pixel at the passed
// coordinates.
Pixel Image::getColorsAtCoordinates(int x, int y) {
	// If a pixel outside of the image is requested, returns a black pixel.		
	if (x + 1 > width || x < 0 || y + 1 > height || y < 0) {
		Pixel black(0, 0, 0);
		std::cerr << "!- " << __func__ << " Error: Requested pixel out of "
			"image range.  Image range is (0, 0) to (" << width - 1 << ", " <<
			height - 1 << "), but passed coordinates are (" << x << ", "
			<< y << ").  Image filepath:\n\t" << filepath << std::endl;
		return black;
	}

	const size_t RGBA = 4;

	size_t index = RGBA * ((size_t)y * width + x);
//	std::cout << "RGBA pixel: "
//		<< static_cast<int>(imageData[index + 0]) << " "
//		<< static_cast<int>(imageData[index + 1]) << " "
//		<< static_cast<int>(imageData[index + 2]) << " "
//		<< static_cast<int>(imageData[index + 3]) << '\n';

	return Pixel(
		static_cast<int>(imageData[index + 0]),
		static_cast<int>(imageData[index + 1]),
		static_cast<int>(imageData[index + 2]));
}


/*
* Name: calculateAverageDifference
* Input: two Image instances with matching height and matching width
* Output: a double from 0 to MAX_DISTANCE indicating how similar the input
*	Images are
* Description: Calculates the average difference in the colors of each Image's
*	pixels to determine how similar the images are.  Static class method.
*	FIXME Not implemented for current project; only present as a model.
*/
double Image::calculateAverageDifference(Image image1, Image image2) {
	double sumDiff = 0;
	if (image1.getHeight() != image2.getHeight() ||
		image1.getWidth() != image2.getWidth()) {
		std::cerr << "Unmatched image size." << std::endl;
		return 0;
	}

	const int height = image1.getHeight();
	const int width = image1.getWidth();

	// For every pixel in both images, finds a mathematical representation
	// of the color difference between the two current pixels.
	for (int x = 0; x < width; ++x) {
		for (int y = 0; y < height; ++y) {
			Pixel pixel1 = image1.getColorsAtCoordinates(x, y);
			Pixel pixel2 = image2.getColorsAtCoordinates(x, y);
			sumDiff += Pixel::calculateColorDistance(pixel1, pixel2);
		}
	}

	// Returns the image's average difference between pixels.
	int numPixels = width * height;
	return sumDiff / numPixels;
}


// Converts a value returned by calculateAverageDifference into a percent
// difference.  Static class method.
double Image::getDifferenceAsPercentage(double input) {
	return input / MAX_DISTANCE * 100.0;
}


/*
* Name: getAverageColorOfImageSlice
* Input: four ints representing cartesian coordinates on a slice of the image:
*	a left edge, right edge, top edge, and bottom edge of the slice
* Output: a Pixel whose color represents the average color of all Pixels in the
*	image slice
* Description: Takes a slice of the Image instance's imageData bounded by the
*	input ints, finds the average color of all pixels in the slice, and returns
*	a Pixel containing that average.
*/
Pixel Image::getAverageColorOfImageSlice(
	unsigned int leftEdge, unsigned int rightEdge,
	unsigned int topEdge, unsigned int bottomEdge) {

	// If the slice has an area of zero or a passed edge goes out of the
	// image's bounds, prints an error and returns a black pixel.
	bool error = false;
	if (leftEdge > rightEdge) {
		std::cerr << "!- " << __func__ << " Error: invalid image slice width."
			"\n\tLeft edge: " << leftEdge << "\n\tRight edge:" << rightEdge <<
			"\n\t" << filepath << std::endl;
		error = true;
	}
	if (topEdge > bottomEdge) {
		std::cerr << "!- " << __func__ << " Error: invalid image slice height."
			"\n\tTop edge:   " << topEdge << "\n\tBottom edge:" << bottomEdge <<
			"\n\t" << filepath << std::endl;
		error = true;
	}
	if (leftEdge > (unsigned int)(width - 1) ||
		rightEdge > (unsigned int)(width - 1)) {
		std::cerr << "!- " << __func__ << " Error: image's farthest-right "
			"position is " << width - 1 << ", but passed slice's horizontal "
			"range is " << leftEdge << " to " << rightEdge <<
			".  Image filepath:\n\t" << filepath << std::endl;
		error = true;
	}
	if (topEdge > (unsigned int)(height - 1) ||
		bottomEdge > (unsigned int)(height - 1)) {
		std::cerr << "!- " << __func__ << " Error: image's lowest position is "
			<< height - 1 << ", but passed slice's vertical range is " <<
			topEdge << " to " << bottomEdge << ".  Image filepath:\n\t" <<
			filepath << std::endl;
		error = true;
	}

	// Finds the average of all pixels in the slice.
	unsigned int totalRed = 0, totalGreen = 0, totalBlue = 0;
	const unsigned int totalPixels = (rightEdge - leftEdge + 1) *
		(bottomEdge - topEdge + 1);
	if (!error) {
		for (unsigned int x = leftEdge; x <= rightEdge; x++) {
			for (unsigned int y = topEdge; y <= bottomEdge; y++) {
				Pixel tempPixel = getColorsAtCoordinates(x, y);
				totalRed += tempPixel.getRed();
				totalGreen += tempPixel.getGreen();
				totalBlue += tempPixel.getBlue();
			}
		}
	}

	Pixel result = Pixel(0, 0, 0);
	if (!error) {
		result = Pixel((unsigned int)std::round((float)totalRed / totalPixels),
			(unsigned int)std::round((float)totalGreen / totalPixels),
			(unsigned int)std::round((float)totalBlue / totalPixels));
	}
	return result;
}

