#ifndef DUPE_HUNTER_HEADER
#define DUPE_HUNTER_HEADER

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>


// Creates an alias for a Windows filepath.
typedef std::filesystem::path pathObject;

// Declares an enum class used for prompt pages.
enum class PromptPage {
	menu, nameCheck, imageCheck, exit
};


// Declares program navigation functions, defined in miscFunctions.cpp.
int promptGeneral();

// Declares basic file access functions, defined in miscFunctions.cpp.
std::string getAbsoluteFilepath(bool useTestImages = true);
void trimWhitespace(std::string& inputString);
std::vector<std::string> getFilenamesFromPath(
	pathObject inputPath, bool errorMsg = false);
size_t collectImagesInFolder(pathObject rootFolder,
	std::vector<pathObject> &imageFiles);
int saveTextToOutputFile(std::string stringToSave,
	std::string outputFilepath = "output.txt");

// Declares functions for manipulating file paths, defined in
// miscFunctions.cpp.
bool comparePathObjectFilenames(pathObject inputPathA, pathObject inputPathB);
unsigned int printNameDuplicates(std::vector<pathObject> inputPathObjects);
const int findAndOutputNameDuplicates(std::string imageDirectory = "");

// Declares various utility functions, defined in miscFunctions.cpp.
bool doublesAreEqual(double inputA, double inputB, double epsilon = 0.001);
std::string makeLowercase(std::string inputString);

// Declares functions used in testing, defined in miscFunctions.cpp.
bool assertTest(bool testBool, int lineNum, bool expectMessage = false);
void startClock(std::chrono::steady_clock::time_point& begin);
double stopClock(std::chrono::steady_clock::time_point& begin, bool print);

// Declares name checking tests, defined in tests.cpp.
void testGetFilenamesFromPath();
void testCollectImagesInFolder();
void testPrintNameDuplicates();
void testConvertRgbToHsl();
void testFindAndOutputNameDuplicates();

// Declares color tests, defined in tests.cpp.
void testGetColors();
void testWriteStringToFile();	// FIXME Redundant, example code only
void testGetAverageColorOfImageSlice();
void testDoublesAreEqual();
void testDescribeHslColor();

// Declares ImageGrid image comparison tests, defined in tests.cpp.
void testImageGridConstructors();
void testCompareImageDimensions();
void testCompareImageColors();
void testCompareImageSections();
void testCollectAndCompare();
void testFindAndOutputImageDuplicates();
void testGetDetailsAsString();
void testImageGridSpeed();
void testImageGridSpeedMulti();

// Declares file access tests, defined in tests.cpp.
void testSaveTextToOutputFile();


#endif

