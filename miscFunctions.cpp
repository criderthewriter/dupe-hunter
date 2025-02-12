#include <fstream>
#include <tchar.h>
#define NOMINMAX
#include <Windows.h>


#include "DupeHunter.h"
#include "Image.h"
#include "ImageGrid.h"


/*
* Name: getAbsoluteFilepath
* Input: a bool, default true, stating whether the path of subfolder
*	"Test Images" should be returned instead of the root directory
* Output: a string containing an absolute directory filepath
* Description: Returns a string containing the program's current working
*	directory, or to working directory subfolder Test Images.
*/
std::string getAbsoluteFilepath(bool useTestImages) {
	std::string result = std::filesystem::current_path().string();
	for (unsigned int i = 0; i < result.size(); i++) {
		if (result.at(i) == '\\') {
			result.at(i) = '/';
		}
	}

	// If useTestImages is true (which it is by default), appends a string so
	// that the function returns the path of subfolder "Test Images" instead of
	// the root directory.
	if (useTestImages) result += std::string("/Test Images");

	// Checks to ensure validity of filename.
	if (std::filesystem::exists(result)) {
		return result;
	}
	else {
		std::cerr << "!- " << __func__ << " Error: \"" << result <<
			"\": Could find directory." << std::endl;
		exit(EXIT_FAILURE);
	}
}


/*
* Name: trimWhitespace
* Input: a string that may contain leading or trailing whitespace
* Output: none
* Description: Iteratively removes all leading and trailing whitespace from
*	the string, and updates the string to its trimmed version.
*/
void trimWhitespace(std::string& inputString) {
	while (true) {
		// Ends loop if currentString is empty.
		if (inputString.empty()) {
			break;
		}
		// Trims a space leading space.
		if (isspace(inputString.at(0))) {
			inputString = inputString.substr(1);
			continue;
		}
		// Trims a trailing space.
		if (isspace(inputString.at(inputString.size() - 1))) {
			inputString = inputString.substr(0, inputString.size() - 1);
			continue;
		}
		// If no more leading or trailing spaces are found, the loop ends.
		break;
	}
}


/*
* Name: getFilenamesFromPath
* Input: a filesystem::path object for a directory, and a bool, default false,
*	stating whether to print a warning if an unreadable file is found
* Output: a vector of strings wherein each string is a filename from the
*	directory.
* Description: Iterates through a given directory and collects the full
*	filepath of all files within, then returns a vector of strings containing
*	those paths.  If any file has an unreadable filename, skips the file and
*	prints a message if bool errorMsg is true.
*/
std::vector<std::string> getFilenamesFromPath(pathObject inputPath,
	bool errorMsg)
{
	// Windows, C++, or the Visual Studio 2019 compiler struggles to read or
	// write to whose filenames are longer than 195 characters.  This block
	// warns the user to rename a file with a too-long filename.
	if (inputPath.string().size() > 195) {
		std::cerr << "?- " << __func__ << " Warning: \"" << inputPath.string() <<
			"\": Possible overly long filename.  Consider renaming to a "
			"filename with 195 characters or fewer." << std::endl;
	}

	std::vector<std::string> filenames;
	// http://en.cppreference.com/w/cpp/experimental/fs/directory_iterator
	// https://en.cppreference.com/w/cpp/filesystem
	const std::filesystem::directory_iterator end{};

	// If the output vector should contain only names of files but not 
	// subdirectories, include the "if (is regular file)" statement.  If the
	// output vector should contain both filenames and subdirectory names, 
	// comment out the "if" conditional to leave the unconditional statement
	// "filenames.push_back(iter->path().string());"
	// http://en.cppreference.com/w/cpp/experimental/fs/is_regular_file 

	int count = -1;
	for (std::filesystem::directory_iterator iter{ inputPath }; iter != end;
		++iter) {
		count++;
		//		if (filesystem::is_regular_file(*iter))	{
		std::string tempStr = "";
		try {
			tempStr = iter->path().string();
		}
		catch (std::system_error) {
			if (errorMsg) {
				std::cerr << "!- " << __func__ << " Error: " <<
					inputPath.string() << ": Unreadable path at index " <<
					count << ".  Could not get filename." << std::endl;
			}
			continue;
		}

		// Cleans up current filename by changing backslashes to forward
		// slashes.
		for (unsigned int i = 0; i < tempStr.size(); ++i) {
			if (tempStr.at(i) == '\\') {
				tempStr.at(i) = '/';
			}
		}
		filenames.push_back(tempStr);
		//		}
	}

	return filenames;
};


/*
* Name: assertTest
* Input: a bool logical test, an int containing __LINE__, and a bool stating
*	whether to print a message.
* Output: none
* Description: Prints an error statement if the passed logical test fails, and
*	prints the line number of the test that failed.  If expectMessage (default
*	false) is true, prints a message stating that the user should expect to see
*	a warning or error message from the test whether it passes or fails.
*/
bool assertTest(bool testBool, int lineNum, bool expectMessage) {
	if (expectMessage) {
		std::cout << "  -- Error or Warning expected above." << std::endl;
	}
	if (!testBool) {
		std::cout << "Test failed: line " << lineNum << std::endl;
	}
	return testBool;
}


/*
* Name: collectImagesInFolder
* Input: a path object containing a root folder to check, and a vector of path
*	objects, passed by reference, to populate with found images.
* Output: a size_t stating how many images were found; also populates
*	input imageFiles, which is passed by reference
* Description: Recursively searches all folders and subfolders in rootFolder 
*	and populates vector imageFiles with path objects representing each image
*	file found.
*/
size_t collectImagesInFolder(pathObject rootFolder, std::vector<pathObject>
	&imageFiles) {
	std::vector<pathObject> subfolders;
	size_t initialImageCount = 0;

	// Prints a warning if the passed rootFolder is not a directory.
	if (!std::filesystem::is_directory(rootFolder)) {
		std::cerr << "?-  " << __func__ <<
			" Warning: passed path is not a directory: \n" <<
			rootFolder.string() << std::endl;
	}

	if (std::filesystem::is_directory(rootFolder)) {
		std::vector<std::string> filenames =
			getFilenamesFromPath(rootFolder.string());
		// Checks each file in rootFolder.  If a file is an image, appends it
		// to imageFiles.  If it is a directory, appends it to subFolders.
		for (unsigned int i = 0; i < filenames.size(); i++) {
			bool isImage = false, isDirectory = false;
			if (Image::fileIsImage(filenames.at(i))) {
				isImage = true;
			}
			if (std::filesystem::is_directory(filenames.at(i))) {
				isDirectory = true;
			}
			// Catches an undesirable case where a file is identified as both
			// an image and a directory, indicating that this function has
			// screwed up somehow.
			if (isImage && isDirectory) {
				std::cerr << "!- " << __func__ << " Error: checked path "
					"has been identified as both an image and a directory: \n"
					<< filenames.at(i) << std::endl;
				break;
			}

			if (isImage) imageFiles.push_back(filenames.at(i));
			if (isDirectory) subfolders.push_back(filenames.at(i));
		}
		// Recursively collects images in all subfolders found in rootPath.
		for (unsigned int i = 0; i < subfolders.size(); i++) {
			collectImagesInFolder(subfolders.at(i), imageFiles);
		}
	}

	if (imageFiles.size() - initialImageCount < 0) {
		std::cerr << "!- " << __func__ << " Error: vector imageFiles somehow "
			"had elements removed instead of added.  Root folder: " <<
			rootFolder.string() << std::endl;
	}

	// Returns the number of images specifically found in rootFolder and its
	// subfolders, not the total number of images found so far.
	return imageFiles.size() - initialImageCount;
}


/*
* Name: comparePathObjectFilenames
* Input: two pathObjects, each containing the path of an image file
* Output: a bool stating whether inputPathA's filename should be alphabetically
*	sorted before that of inputPathB's filename
* Description: Returns true if inputPathA's filename should appear before that
*	of inputPathB, or returns false otherwise.  Used for sorting a vector of
*	pathObjects.
*/
bool comparePathObjectFilenames(pathObject inputPathA, pathObject inputPathB) {
	return inputPathA.filename() < inputPathB.filename();
}


/*
* Name: printNameDuplicates
* Input: a vector of pathObjects, sorted by each pathObject's filename
* Output: an unsigned int stating how many duplicates were found
* Description: Checks each filename in inputPathObjects against all others and
*	prints a message for each duplicate filename found, then returns the number
*	of duplicates found.
*/
unsigned int printNameDuplicates(std::vector<pathObject> inputPathObjects) {
	unsigned int count = 0;
	const std::string duplicateMessage =
		"The following images have identical filenames: \n";

	// For each pathObject in inputPathObjects, checks it against the last one
	// and prints a message if their filenames match.
	pathObject previous;
	for (unsigned int i = 0; i < inputPathObjects.size(); i++) {
		if (i > 0) {
			if (inputPathObjects.at(i).filename() == previous.filename()) {
				std::cout << duplicateMessage << "\t" << previous.string() <<
					"\n\t" << inputPathObjects.at(i).string() << "\n" <<
					std::endl;
				count++;
			}
		}
		previous = inputPathObjects.at(i);
	}

	return count;
}


/*
* Name: findAndOutputNameDuplicates
* Input: a string (default blank) containing the path of an image directory to
*	check
* Output: an int containing whether the function was successful: 0 on success
*	or -1 on failure
* Description: Searches the program's working directory or
*	the passed directory (if provided) for images, compares the names of each
*	image, outputs a list of all images that have identical names, and saves
*	the output list to a text file.  Returns 0 on success or -1 otherwise.
*/
const int findAndOutputNameDuplicates(std::string imageDirectory) {
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

	std::cout << "Detected " << allImages.size() <<
		" images to compare.  Comparing...\n" << std::endl;

	// Sorts all image files by filename, not by full filepath.
	std::sort(allImages.begin(), allImages.end(), comparePathObjectFilenames);

	// For each pathObject in allImages, checks it against the last one
	// and prints a message if their filenames match.  Collects each printed
	// message into a single string.
	unsigned int count = 0;
	std::string outputText = "";
	pathObject previous;
	for (unsigned int i = 0; i < allImages.size(); i++) {
		if (i > 0) {
			if (allImages.at(i).filename() == previous.filename()) {
				count++;
				outputText += "#" + std::to_string(count) +
					": The following images have identical filenames: \n\t" +
					previous.string() + "\n\t" +
					allImages.at(i).string() + "\n\n";
			}
		}
		previous = allImages.at(i);
	}

	// Outputs the collected string listing name duplicates and saves it to
	// an output text file.
	std::cout << outputText << "\n" <<
		"Image comparison complete.  Found " << count << " name duplicates."
		"  Saving comparison results to output file 'nameOutput.txt'." <<
		std::endl;
	return saveTextToOutputFile(outputText, "nameOutput.txt");
}


/*
* Name: doublesAreEqual
* Input: a pair of doubles to compare, and another double, default 0.001,
*	stating the difference past which the doubles should be considered not
*	equal
* Output: a bool stating whether the doubles are roughly equal
* Description: Returns true if the passed doubles are within an epsilon value,
*	0.001 by default, of each other, meaning they are effectively equal,
*	and returns false otherwise.
*/
bool doublesAreEqual(double inputA, double inputB, double epsilon) {
	return std::fabs(inputA - inputB) < epsilon;
}


/*
* Name: saveTextToOutputFile
* Input: a string containing text to write to an output file, and a string
*	(default value of "output.txt") containing the filepath of the output file.
* Output: an int representing whether saving was successful; 1 on
*	success or 0 otherwise
* Description: Attempts to open an output text file and write passed text to
*	the file.  Returns 0 on success or -1 on failure.
*/
int saveTextToOutputFile(std::string stringToSave,
	std::string outputFilepath) {

	// Attempts to open the output file.
	int result = 0;
	std::ofstream outputFile(outputFilepath);

	// Prints an error message if the file could not be opened successfully.
	if (!outputFile) {
		std::cerr << "!- " << __func__ << " Error: Could not open file \"" <<
			outputFilepath << "\"" << std::endl;
		result = -1;
	}

	// If the file was opened successfully, writes text in stringToSave to
	// outputFile then closes the file.
	if (outputFile) {
		outputFile << stringToSave;
		outputFile.close();
	}
	return result;
}


/*
* Name: startClock
* Input: a time_point object passed by reference
* Output: none (overwrites time_point object)
* Description: Starts a clock to measure execution time.  Should be followed
*	by a stopClock() call.	The statement
*	"std::chrono::steady_clock::time_point begin;" will initialize the variable
*	to be passed to startClock.
* Code for stopwatch functions copied from
* https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
*/
void startClock(std::chrono::steady_clock::time_point& begin) {
	begin = std::chrono::high_resolution_clock::now();
}


/*
* Name: stopClock
* Input: a time_point object passed by reference, and a bool (default true)
*	stating whether a message should be printed
* Output: a double containing the time in seconds since startClock() was called
* Description: Ends the clock started by a startClock() call, then prints and
*	returns the time measured, accurate to 9 decimal places.  Only prints if
*	bool print is true so that printing can easily be turned on and off.
* Code for stopwatch functions copied from
* https://levelup.gitconnected.com/8-ways-to-measure-execution-time-in-c-c-48634458d0f9
*/
double stopClock(std::chrono::steady_clock::time_point& begin, bool print) {
	// Stops the clock.
	auto end = std::chrono::high_resolution_clock::now();
	// Calculates elapsed time in nanoseconds.
	auto elapsed =
		std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
	// Outputs and returns elapsed time.
	if (print) {
		printf("-- Time measured: %.9f sec\n", elapsed.count() * 1e-9);
	}
	return elapsed.count() * 1e-9;
}



/*
* Name: makeLowercase
* Input: a string
* Output: a string whose alphabetical characters are all lowercase
* Description: For an input string, returns a version whose characters
*	are all lowercase.
*/
std::string makeLowercase(std::string inputString) {
	for (unsigned int i = 0; i < inputString.size(); ++i) {
		inputString.at(i) = tolower(inputString.at(i));
	}
	return inputString;
}


/*
* Name: promptGeneral
* Input: none
* Output: an int stating exit status
* Description: Prompts the user to choose what duplicate searching will take
*	place, and provides a menu for program functions.  Returns 0 on success
*	or -1 on error. 
*/
int promptGeneral() {
	std::string answer = "", lowercaseAnswer = "", directory = "";
	PromptPage currentPage = PromptPage::menu;
	bool saidWelcome = false;

	while (true) {
		if (currentPage == PromptPage::menu) {
			// Prints a welcome when this menu is first accessed, IE at program
			// startup, but at no other time.
			if (!saidWelcome) {
				std::cout << "Welcome to the DupeHunter.  You can input "
					"'exit' at any time to exit program." << std::endl;
				saidWelcome = true;
			}
			std::cout << "\nWhat would you like to do?\n"
				"\t1)\tCheck for images with identical filenames.\n"
				"\t2)\tCheck for image duplicates, including resized images."
				<< std::endl;
			getline(std::cin, answer);
			trimWhitespace(answer);
			lowercaseAnswer = makeLowercase(answer);

			// Checks user input and navigates to different parts of the
			// program accordingly.
			if (answer == "1") {
				currentPage = PromptPage::nameCheck;
				continue;
			}
			if (answer == "2") {
				currentPage = PromptPage::imageCheck;
				continue;
			}
			if (lowercaseAnswer == "exit") {
				currentPage = PromptPage::exit;
				continue;
			}
			if (lowercaseAnswer == "clear" || lowercaseAnswer == "cls") {
				system("cls");
				continue;
			}
			std::cout << "Invalid input." << std::endl;
		}

		// Handles checking for name duplicates.
		if (currentPage == PromptPage::nameCheck) {
			directory.clear();
			int errorStatus = 0;
			std::cout << "\nPlease input a root directory.  I'll check all "
				"images in that directory and its subfolders.  Or leave the "
				"input field blank, and I'll check all images in the "
				"program's working directory.  You can also input 'back' to "
				"return to the menu or 'exit' to exit." << std::endl;
			getline(std::cin, answer);
			trimWhitespace(answer);
			lowercaseAnswer = makeLowercase(answer);

			// Checks user input.
			if (lowercaseAnswer == "exit") {
				currentPage = PromptPage::exit;
				continue;
			}
			if (lowercaseAnswer == "back") {
				currentPage = PromptPage::menu;
				continue;
			}
			if (lowercaseAnswer == "clear" || lowercaseAnswer == "cls") {
				system("cls");
				continue;
			}

			errorStatus = findAndOutputNameDuplicates(answer);
			if (errorStatus == 0) {
				currentPage = PromptPage::menu;
			}
			else {
				std::cout << "\nLet's try again." << std::endl;
			}
		}

		// Handles checking for image duplicates.
		if (currentPage == PromptPage::imageCheck) {
			directory.clear();
			int errorStatus = 0;
			std::cout << "\nPlease input a root directory.  I'll check all "
				"images in that directory and its subfolders.  Or leave the "
				"input field blank, and I'll check all images in the "
				"program's working directory.  You can also input 'back' to "
				"return to the menu or 'exit' to exit." << std::endl;
			getline(std::cin, answer);
			trimWhitespace(answer);
			lowercaseAnswer = makeLowercase(answer);

			// Checks user input.
			if (lowercaseAnswer == "exit") {
				currentPage = PromptPage::exit;
				continue;
			}
			if (lowercaseAnswer == "back") {
				currentPage = PromptPage::menu;
				continue;
			}
			if (lowercaseAnswer == "clear" || lowercaseAnswer == "cls") {
				system("cls");
				continue;
			}

			errorStatus = ImageGrid::findAndOutputImageDuplicates(answer);
			if (errorStatus == 0) {
				currentPage = PromptPage::menu;
			}
			else {
				std::cout << "\nLet's try again." << std::endl;
			}
		}

		// Handles an exit command.
		if (currentPage == PromptPage::exit) {
			std::cout << "\nGoodbye!" << std::endl;
			break;
		}

	}

	return 0;
}

