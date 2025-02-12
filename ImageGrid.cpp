#include <iomanip>


#include "DupeHunter.h"
#include "ImageGrid.h"


/*
* Name: ImageGrid (constructor)
* Input: an Image instance
* Output: none
* Description: Constructor for class ImageGrid using an Image instance
*	as input.
*/
ImageGrid::ImageGrid(Image inputImage) {
	// Populates attributes with the passed Image instance's attributes.
	filepath = inputImage.getFilepath();
	height = inputImage.getHeight();
	width = inputImage.getWidth();

	try {
		absoluteFilepath = std::filesystem::canonical(filepath).string();
	}
	catch (std::system_error) {
		std::cout << "!- " << __func__ << " Constructor Error: Could not "
			"find absolute form of filepath:\n\t" << filepath << std::endl;
	}
	populateSections(inputImage);
}


/*
* Name: ImageGrid (constructor)
* Input: a string filepath to an image file
* Output: none
* Description: Constructor for class ImageGrid using a filepath to an image
*	file.  Uses the filepath to generate an Image instance on the stack
*	instead of receiving a persistent Image instance like the other
*	constructor.
*/
ImageGrid::ImageGrid(std::string filepathString) {
	// Uses filepathString to initialize an Image instance, which is bigger
	// than the ImageGrid instance but is cleared from the stack when the
	// constructor returns.
	Image sourceImage(filepathString);

	// Populates attributes with the Image instance's attributes.
	filepath = sourceImage.getFilepath();
	height = sourceImage.getHeight();
	width = sourceImage.getWidth();

	try {
		absoluteFilepath = std::filesystem::canonical(filepath).string();
	}
	catch (std::system_error) {
		std::cout << "!- " << __func__ << " Constructor Error: Could not "
			"find absolute form of filepath:\n\t" << filepath << std::endl;
	}
	populateSections(sourceImage);
}


/*
* Name: populateSections
* Input: an Image instance
* Output: none, but populates class attribute ImageGrid
* Descripttion: Receives an Image, divides it into a number of sections
*	specified by static attribute numSectionsInRow, and for each section,
*	calculates the average color of all pixels in the
*	section.  Allocates memory for an array of Pixels to be stored in attribute
*	sections and stores the calculations as Pixels in that attribute.
*/
void ImageGrid::populateSections(Image inputImage) {
	// No matter how many pixels an image's width or height is, it will be
	// divided into an n by n grid, where n is attribute numSectionsInRow.
	// This block calculates how many pixels wide and tall any given section
	// will be.
	if (numSectionsInRow < 1) {
		std::cerr << "!- " << __func__ << " Error: attribute numSectionsInRow, "
			"value " << numSectionsInRow << ", is less than 1.  It's a const "
			"value, so a mistake was made before compiling, not at runtime.  "
			" Image filepath:\n\t" << filepath << std::endl;
		exit(EXIT_FAILURE);
	}

	const float sectionWidth = width / (float)numSectionsInRow,
		sectionHeight = height / (float)numSectionsInRow;

	// Builds two arrays containing the height and width boundaries that each
	// section will use, rounding to the nearest integer.
	int widthBorders[numSectionsInRow + 1],
		heightBorders[numSectionsInRow + 1];
	float currentX = 0.0, currentY = 0.0;
	for (unsigned int i = 0; i < numSectionsInRow + 1; i++) {
		widthBorders[i] = (int)round(currentX);
		heightBorders[i] = (int)round(currentY);
		currentX += sectionWidth;
		currentY += sectionHeight;
	}

	// Arrays widthBorders and heightBorders contain the pixel indices where
	// the image will be sliced into sections.  Their first value will be 0,
	// while their last values should be the image's width and height
	// respectively.
	if (widthBorders[numSectionsInRow] != width) {
		std::cerr << "!- " << __func__ << " Error: image width and ImageGrid's"
			" farthest-right bound do not match.\n\tWidth:       " <<
			width << "\n\tRight bound: " <<
			widthBorders[numSectionsInRow] << "\n\t" << filepath << std::endl;
		exit(EXIT_FAILURE);
	}
	if (heightBorders[numSectionsInRow] != height) {
		std::cerr << "!- " << __func__ << " Error: image height and ImageGrid's"
			" bottom bound do not match.\n\tHeight:       " <<
			height << "\n\tBottom bound: " <<
			heightBorders[numSectionsInRow] << "\n\t" << filepath << std::endl;
		exit(EXIT_FAILURE);
	}

	// Uses widthBorders and heightBorders to break the image into sections that
	// have extremely similar but not necessarily identical shapes and sizes.
	// For each section, finds the average color of all pixels in the section
	// and represents that average color as a Pixel instance.
	Pixel previous(0, 0, 0);
	for (unsigned int y = 0; y < numSectionsInRow; y++) {
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			// If any section has a width or height of zero, uses the last
			// calculated average color value for that improper section's
			// average.
			Pixel tempAverageColor(255, 255, 255);
			if (widthBorders[x] == widthBorders[x + 1] - 1 ||
				heightBorders[y] == heightBorders[y + 1] - 1) {
				tempAverageColor = previous;
			}
			// Otherwise, finds the average color of a slice from the beginning
			// of one section to the beginning of the next section minus one
			// pixel.
			else {
				tempAverageColor = inputImage.getAverageColorOfImageSlice(
					widthBorders[x], widthBorders[x + 1] - 1, heightBorders[y],
					heightBorders[y + 1] - 1);
			}
			sections[y * numSectionsInRow + x] = tempAverageColor;
			previous = tempAverageColor;
		}
	}
}


/*
* Name: getFilepath
* Input: none
* Output: a string containing an image's filepath
* Description: Getter for class attribute filepath.  Returns a string
*	containing the filepath of the image used to initialize the ImageGrid
*	instance.
*/
const std::string ImageGrid::getFilepath() {
	return filepath;
}


/*
* Name: getNumSectionsInRow
* Input: none
* Output: an unsigned int representing how many vertical or horizontal sections
*	the ImageGrid constructor should break an image into
* Description: Getter for static class attribute numSectionsInRow.  Any image
*	used to initialize an ImageGrid instance is divided n by n sections where
*	n = numSectionsInRow.
*/
const unsigned int ImageGrid::getNumSectionsInRow() {
	return numSectionsInRow;
}


/*
* Name: checkSection
* Input: a pair of unsigned ints representing cartesian coordinates for what
*	section of an ImageGrid to check
* Output: a Pixel representing the average color in the requested section of
*	the ImageGrid, from the ImageGrid attribute sections.
* Description: Checks ImageGrid attribute sections, which is a 2D array of
*	average colors from various sections of the image, and returns the section
*	at the passed cartesian coordinates.
*/
const Pixel ImageGrid::checkSection(unsigned int x, unsigned int y) const {
	Pixel result(0, 0, 0);
	if (x >= numSectionsInRow || y >= numSectionsInRow) {
		std::cerr << "!- " << __func__ << " Error: requested section at "
			"coordinates (" << x << ", " << y << ") is outside ImageGrid "
			"bounds.  The ImageGrid only has " << numSectionsInRow <<
			" by " << numSectionsInRow << " sections.  Filepath: \n\t"
			<< filepath << std::endl;
	}
	else {
		result = sections[(numSectionsInRow - y - 1) * numSectionsInRow + x];
	}
	return result;
}


// Getter for ImageGrid class's attribute width.
const unsigned int ImageGrid::getWidth() const {
	return width;
}


// Getter for ImageGrid class's attribute height.
const unsigned int ImageGrid::getHeight() const {
	return height;
}


// Calculates and returns the source image's height-width ratio, used to
// compare images without having to immediately resort to color comparison.
const double ImageGrid::getHeightWidthRatio() const {
	return height / (double)width;
}

// Calculates the source image's total pixel count.
const unsigned int ImageGrid::getTotalPx() const {
	return width * height;
}


/*
* Name: printSectionsRgb
* Input: none
* Output: none, but outputs text
* Description: Prints a grid, with borders, stating the RGB color value of
*	every section of the ImageGrid.
*/
void ImageGrid::printSectionsRgb() const {
	for (unsigned int y = numSectionsInRow - 1; y != 0; y--) {
		// Prints an upper border for the current row.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "+--------";
		}
		std::cout << "+" << std::endl;
		// Prints each sections's Red value.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| r: " << std::setw(3) <<
				checkSection(x, y).getRed() << " ";
		}
		std::cout << "|" << std::endl;
		// Prints each section's Green value.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| g: " << std::setw(3) <<
				checkSection(x, y).getGreen() << " ";
		}
		std::cout << "|" << std::endl;
		// Prints each section's Blue value.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| b: " << std::setw(3) <<
				checkSection(x, y).getBlue() << " ";
		}
		std::cout << "|" << std::endl;
	}
	// Prints a lower border.
	for (unsigned int x = 0; x < numSectionsInRow; x++) {
		std::cout << "+--------";
	}
	std::cout << "+" << std::endl;
}


/*
* Name: printSectionDescriptions
* Input: none
* Output: none, but outputs text
* Description: Prints a grid, with borders, describing the color of
*	every section of the ImageGrid.
*/
void ImageGrid::printSectionDescriptions() const {
	for (unsigned int y = numSectionsInRow - 1; y != 0; y--) {
		// Prints an upper border for the current row.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "+---------";
		}
		std::cout << "+" << std::endl;

		// For each color in the row, breaks at each space into up to three
		// lines so it can be printed without becoming too wide.
		std::string colorDescriptions[numSectionsInRow][3],
			tempString = "";
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			colorDescriptions[x][0] = "";
			colorDescriptions[x][1] = "";
			colorDescriptions[x][2] = "";

			std::string colorString =
				Pixel::describeHslColor(checkSection(x, y).convertRgbToHsl());
			size_t index1 = colorString.find(" "),
				index2 = std::string::npos;
			if (index1 == std::string::npos) {
				colorDescriptions[x][0] = colorString;
			}
			if (index1 != std::string::npos) {
				colorDescriptions[x][0] = colorString.substr(0, index1);
				tempString = colorString.substr(index1 + 1);
				colorDescriptions[x][1] = tempString;
				index2 = tempString.find(" ");
			}
			if (index2 != std::string::npos) {
				colorDescriptions[x][1] = tempString.substr(0, index2);
				colorDescriptions[x][2] = tempString.substr(index2 + 1);
			}
		}

		// Prints the first line of each section's color.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| " << std::left << std::setw(7) <<
				colorDescriptions[x][0] << " ";
		}
		std::cout << "|" << std::endl;

		// Prints the second line of each section's color.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| " << std::left << std::setw(7) <<
				colorDescriptions[x][1] << " ";
		}
		std::cout << "|" << std::endl;

		// Prints the third line of each section's color.
		for (unsigned int x = 0; x < numSectionsInRow; x++) {
			std::cout << "| " << std::left << std::setw(7) <<
				colorDescriptions[x][2] << " ";
		}
		std::cout << "|" << std::endl;
	}
	// Prints a lower border.
	for (unsigned int x = 0; x < numSectionsInRow; x++) {
		std::cout << "+---------";
	}
	std::cout << "+" << std::endl;
}


/*
* Name: compareImageDimensions
* Input: an ImageGrid instance, which will be compared to the executing
*	ImageGrid instance, and a double (default 0.05) containing a percentage of
*	how different two images' dimensional ratios can be before they are
*	considered to be different images
* Output: an unsigned int which is a code from the below key
* Description: Compares the dimensions of the executing Image to the input
*	image and returns a summary of how similar the dimensions are.  The
*	returned unsigned int matches one of the items from the below key:
*
*	6) The images' dimensions are identical.
*	5) The images' dimensional ratios are identical but their dimensions are
*		not.
*	4) The images have one identical dimension and their dimensional ratios
*		are <(threshold)% different.
*	3) The images' dimensional ratios are <(threshold)% different but they
*		have no identical dimensions.
*	2) The images have one identical dimension but their dimensional ratios
*		are >=(threshold)% different.
*	1) The images have no dimensions in common and their dimensional ratios
*		are >=(threshold)% different.
*/
const unsigned int ImageGrid::compareImageDimensions(ImageGrid * inputGrid,
	double threshold) const {
	unsigned int result = 99;

	// Compares image dimensions directly and checks for cases where the
	// Images' heights and widths are identical.
	bool heightIdentical = (height == inputGrid->getHeight());
	bool widthIdentical = (width == inputGrid->getWidth());
	if (heightIdentical && widthIdentical) {
		result = 6;
	}

	// Compares dimensional ratios, which is an Image's height / width raio.
	double dimensionalRatioComparison = 0.0;
	if (result == 99) {
		dimensionalRatioComparison =
			(getHeightWidthRatio()) / (inputGrid->getHeightWidthRatio());
	}

	// Checks for cases where the Images' heights and widths are not identical
	// but their dimensional ratios are 1, suggesting one image is a resized
	// version of the other with no distortion--rare but possible.
	if (result == 99) {
		if (doublesAreEqual(dimensionalRatioComparison, 1.0, 0.0001)) {
			result = 5;
		}
	}
	// Checks for all other expected cases.
	if (result == 99) {
		bool ratiosWithinThreshold =
			(dimensionalRatioComparison <= (1 + threshold) &&
				dimensionalRatioComparison >= (1 - threshold));

		if ((heightIdentical && !widthIdentical) ||
			(!heightIdentical && widthIdentical)) {
			if (ratiosWithinThreshold) result = 4;
			else result = 2;
		}
		if (!heightIdentical && !widthIdentical) {
			if (ratiosWithinThreshold) result = 3;
			else result = 1;
		}
	}
	return result;
}


/*
* Name: compareImageSections
* Input: an ImageGrid instance to compare the calling ImageGrid instance to
* Output: an unsigned int which is a code from the below key
* Description: For each section in the calling ImageGrid's 2D vector attribute
*	sections, compares the section's color to the color of the input
*	imageGrid's matching section.  Returns an unsigned int describing how
*	similar the ImageGrids are overall.  The main value checked is "distance"
*	between any two checked sections, ranging from 0 for identical colors to
*	764.8333152 for opposite colors like pure black and white.
*
*	6) All sections have a distance of <5; the ImageGrids have identical
*		colors and so come from the same image.
*	5) All sections have a distance of <15 but some have >=5; the ImageGrids
*		have nearly identical colors and most likely come from the same image.
*	4) All sections have a distance of <50 but some have >=15; the ImageGrids
*		have generally identical colors and possibly come from the same image.
*	3) <10 sections have a distance of <15 and <20% have >=50; the ImageGrids
*		have slightly identical colors and possibly come from the same image.
*	2) >=10 sections have a distance of <15 but <20% have >=50; the ImageGrids
*		have barely identical colors and likely describe different images.
*	1) >=20% of sections have a distance of >=50; the ImageGrids almost
*		certainly describe different images.
*/
const unsigned int ImageGrid::compareImageSections(ImageGrid * inputGrid) const {
	unsigned int modestDistanceCount = 0,
		majorDistanceCount = 0;
	bool minorDistanceFound = false;
	const unsigned int twentyPercentOfSections = numSectionsInRow * 2;

	for (unsigned int i = 0; i < numSectionsInRow; i++) {
		for (unsigned int j = 0; j < numSectionsInRow; j++) {
			// Repeatedly Calculates color distance between two matching
			// sections and checks whether each distance meets various
			// thresholds.
//			double distance = Pixel::calculateColorDistance(
//				sections.at(i).at(j), inputGrid.getSections().at(i).at(j));
			double distance = Pixel::calculateColorDistance(
				checkSection(i, j), inputGrid->checkSection(i, j));

			if (distance >= 5.0) minorDistanceFound = true;
			if (distance >= 15.5) modestDistanceCount++;
			if (distance >= 50.0) majorDistanceCount++;

			// Ends loop early if more than 20% of the compared images'
			// sections present major differences.
			if (majorDistanceCount >= twentyPercentOfSections) break;
		}
		if (majorDistanceCount >= twentyPercentOfSections) break;
	}

	// Returns an unsigned int set by the results of counting how of the
	// ImageGrids' sections have notably different colors.
	unsigned int result = 0;
	if (!minorDistanceFound) result = 6;
	if (result == 0 && minorDistanceFound && modestDistanceCount == 0 &&
		majorDistanceCount == 0) result = 5;
	if (result == 0 && modestDistanceCount > 0 && majorDistanceCount == 0)
		result = 4;
	if (result == 0 && modestDistanceCount < twentyPercentOfSections &&
		majorDistanceCount < twentyPercentOfSections)
		result = 3;
	if (result == 0 && modestDistanceCount >= twentyPercentOfSections &&
		majorDistanceCount < twentyPercentOfSections)
		result = 2;
	if (result == 0 && majorDistanceCount >= twentyPercentOfSections)
		result = 1;

	if (result == 0) {
		std::cerr << "!- " << __func__ << " Error: Could not compare "
			"ImageGrids for the following images:\n\t" << filepath <<
			"\n\t" << inputGrid->getFilepath() << std::endl;
	}
	return result;
}


/*
* Name: findAndOutputImageDuplicates
* Input: a string (default blank) containing the absolute or relative path of
*	an image directory to check
* Output: an int containing whether the function was successful: 0 on success
*	or -1 on failure
* Description: Static function.  Searches the program's working directory or
*	the passed directory (if provided) for images, compares each image by
*	making an ImageGrid instance from it, outputs a list of all images that
*	are duplicates or likely duplicates, and saves the output list to a text
*	file.  Returns 0 on success or -1 otherwise.
*/
const int ImageGrid::findAndOutputImageDuplicates(
	std::string imageDirectory) {
	// Checks imageDirectory and, if it contains a valid directory, sets it
	// as the root directory to check for images.  If not populated, chooses
	// the program's working directory as the root folder to check for images.
	std::string rootFolder = getAbsoluteFilepath(false),
		tempPath = "";
	bool invalid = false;
	if (!imageDirectory.empty()) {
		try {
			pathObject imageDirectoryPath(imageDirectory);
			tempPath = imageDirectoryPath.string();
			if (std::filesystem::is_directory(imageDirectoryPath)) {
				rootFolder = imageDirectory;
			}
			else {
				invalid = true;
			}
		}
		catch (std::system_error) {
			invalid = true;
		}
	}
	if (invalid) {
		std::cerr << "!- " << __func__ << " Error: String passed as "
			"imageDirectory is not a valid directory:\n\t" <<
			imageDirectory << std::endl;
		return -1;
	}

	// Collects all images in rootFolder.
	std::vector<pathObject> allImages;
	size_t imagesFoundCount =
		collectImagesInFolder(rootFolder, allImages);
	if (imagesFoundCount == 0) {
		std::cout << "?- " << __func__ << " Warning: No images found in root "
			"directory:\n\t" << rootFolder << std::endl;
		return -1;
	}

	// Creates an ImageGrid instance for all images.
	std::cout << "Root image directory: \n\t" << rootFolder <<
		"\nThe root directory contains " << allImages.size() << " images."
		<< std::endl;
	std::vector<ImageGrid> allImageGrids;
	for (unsigned int i = 0; i < allImages.size(); i++) {
		allImageGrids.push_back(ImageGrid(allImages.at(i).string()));
		std::cout << "Making an ImageGrid for image " << i + 1 << " of " <<
			allImages.size() << ".\r";
	}

	std::cout << "Detected " << allImageGrids.size() <<
		" images to compare.  Comparing...\n" << std::endl;

	// Compares all collected ImageGrids and builds output text reporting which
	// ImageGrids are identical or nearly identical.
	std::string outputText = "";
	unsigned int count = 0;
	for (unsigned int i = 0; i < allImageGrids.size(); i++) {
		for (unsigned int j = i + 1; j < allImageGrids.size(); j++) {
			std::cout << "Comparing image " << i + 1 << " to image " <<
				j + 1 << ".\r";

			unsigned int colorComp =
				allImageGrids.at(i).compareImageSections(&allImageGrids.at(j));
			unsigned int dimensionComp =
				allImageGrids.at(i).compareImageDimensions(&allImageGrids.at(j));
			unsigned int imageOnePx = allImageGrids.at(i).getTotalPx(),
				imageTwoPx = allImageGrids.at(j).getTotalPx();

			// For high degrees of similarity (high values of colorComp and
			// dimensionComp), prints and outputs details about the similar
			// images.
			if (colorComp == 6 && dimensionComp == 6) {
				count++;
				outputText += "#" + std::to_string(count) +
					": These images are seemingly identical "
					"(color comparison " + std::to_string(colorComp) +
					", dimension comparison " + std::to_string(dimensionComp) +
					"):\n" +
					allImageGrids.at(i).getDetailsAsString(imageTwoPx) + "\n" +
					allImageGrids.at(j).getDetailsAsString(imageOnePx) + "\n\n";
			}
			if ((colorComp == 6 && dimensionComp == 5)) {
				count++;
				outputText += "#" + std::to_string(count) +
					": One of these images is likely a resize of the other "
					"(color comparison " + std::to_string(colorComp) +
					", dimension comparison " + std::to_string(dimensionComp) +
					"):\n" +
					allImageGrids.at(i).getDetailsAsString(imageTwoPx) + "\n" +
					allImageGrids.at(j).getDetailsAsString(imageOnePx) + "\n\n";
			}
			if (colorComp == 5 && dimensionComp >= 5) {
				count++;
				outputText += "#" + std::to_string(count) +
					": These images are basically identical "
					"(color comparison " + std::to_string(colorComp) +
					", dimension comparison " + std::to_string(dimensionComp) +
					"):\n" +
					allImageGrids.at(i).getDetailsAsString(imageTwoPx) + "\n" +
					allImageGrids.at(j).getDetailsAsString(imageOnePx) + "\n\n";
			}
			if (colorComp >= 5 &&
				(dimensionComp == 3 || dimensionComp == 4)) {
				count++;
				outputText += "#" + std::to_string(count) +
					": These images are nearly identical "
					"(color comparison " + std::to_string(colorComp) +
					", dimension comparison " + std::to_string(dimensionComp) +
					"):\n" +
					allImageGrids.at(i).getDetailsAsString(imageTwoPx) + "\n" +
					allImageGrids.at(j).getDetailsAsString(imageOnePx) + "\n\n";
			}
		}
	}
	std::cout << outputText << "\n" <<
		"Image comparison complete.  Found " << count << " suspected "
		"duplicate relationships between various images.  Saving comparison "
		"results to output file." << std::endl;
	return saveTextToOutputFile(outputText, "imageOutput.txt");
}


/*
* Name: getDetailsAsString
* Input: none
* Output: a string of text
* Description: Returns the current ImageGrid instance's filepath, height,
*	and width as a single string in the following format:
*	\t[filepath]
*	\t[width] x [height]px (larger/smaller/same dimensions)
*/
const std::string ImageGrid::getDetailsAsString(
	unsigned int comparedImagePx) {
	std::string tail = "\t(same dimensions)";
	unsigned int thisImagePx = getTotalPx();
	if (thisImagePx > comparedImagePx) {
		tail = "\t(larger)";
	}
	else if (thisImagePx < comparedImagePx) {
		tail = "\t(smaller)";
	}

	return "\t" + absoluteFilepath + "\n\t\t" + std::to_string(width) +
		" x " + std::to_string(height) + " px" + tail;
}