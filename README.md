# dupe-hunter

The DupeHunter is a command line C++ project for identifying image duplicates
by name or by color.  Plenty of excellent programs like WinDirStat will let
you check for file duplicates--files that reoccur at different paths--but the
DupeHunter actually performs color analysis to identify images that are mostly
but not entirely identical--resizes, dimensional distortions, stretching and
shrinking, slight variants, and similar.  I developed it as a personal project
to help me with archiving a folder with many subfolders and thousands of
images, though the color analysis is based on code I wrote at UNT for an
assignment on image compression analysis.

This program is largely finished, but I haven't implemented it for its purpose
yet and its color comparison thresholds likely need fine-tuning.

The DupeHunter doesn't currently include an installer; to run it, download its
files into a code repository then use G++, Visual Studio, or similar to compile
and run it.

The DupeHunter will check files in its working directory or in a
different folder that you specify, and it creates log files "imageOutput.txt"
and "nameOutput.txt".  It does not modify or delete any files but will
overwrite older log files.

The DupeHunter has a command line menu, and on startup, it asks whether you'd
like to check for name duplicates (images with identical filenames in different
folders) or image duplicates (images that are completely or mostly identical
even if their dimensions are different).  Whichever option you choose, it asks
you to input a root directory for it to operate in.  If you leave the input
blank, the DupeHunter will use its working directory as the root directory.

If you choose to check for name duplicates and give the DupeHunter a root
directory, it collects all image files in the root and subfolders into a vector.
Afterwards, it simply sorts the vector by filename (IE an image at
"Pictures/Photos/graduation.png" will have filename "graduation.png") and
identifies any recurring filenames.  It then outputs a list of filename matches
both to the output window and to "nameOutput.txt".  Name duplicate checking
usually only takes a few seconds at most.

If you choose to check for image duplicates and the give DupeHunter a root
directory, it collects all image files in the root and subfolders into a
vector.  For each image, class ImageGrid breaks the image into a 10 by 10 grid
of image sections, finds the average color of each section, and saves those
averages.  This means that any two images can be compared, no matter their
dimensions, because each image is reduced to a 10 by 10 grid of averages.  
For each image, the DupeHunter then compares the image's ImageGrid to every
other ImageGrid that it hasn't already been compared to, counting how many
image segments have slightly or sharply different colors.  Comparison includes
whether the images' dimensions are identical or have roughly identical
width / height ratios, whether the images' segments are identical, and how many
similar or extremely dissimilar segments were found.  It then outputs a list of
image duplicate relationships and suspected relationships both to the output
window and to "imageOutput.txt".  Image duplicate checking is a slow process
and can take several minutes for many images.

Note that image duplication checking is imprecise in order to keep runtime
short; it is meant for operating on libraries of thousands or even tens of
thousands of images.  It outputs a list of suspected duplication relationships,
including filenames, but you will need to check each listed image yourself to
decide whether each suggested image is truly a duplicate and should be deleted
from your library.  This seemingly odd set of parameters is exactly what I
needed for my image library, and though I question how useful the DupeHunter
may be for anyone else, I'm posting it here regardless.


The outputted text for image duplication checking uses the following number
keys.  Note that the value "distance" mentioned below is the calculated
difference in color between two pixels, to a minimum distance of 0 (identical
pixels) and maximum distance of 764.833 (opposite pixels).  A pure red and pure
blue pixel will have a distance of 569.974, while a pure white and pure black
pixel will have a distance of 764.833, and even seemingly identical pixels may
have a distance ranging from 0 to 200.

Dimensions
	6) The images' dimensions are identical.
	5) The images' dimensional ratios are identical.
	4) The images have one identical dimension and their dimensional ratios
		are <5% different.
	3) The images' dimensional ratios are <5% different.
	2) The images have one identical dimension but their dimensional ratios are
		>=5% different.
	1) The images have no dimensions in common and their dimensional ratios are
		>=5% different.
	0) Comparison error.
 
Sections and colors
	6) All sections have a distance of <5; the ImageGrids have identical
		colors and so come from the same image.
	5) All sections have a distance of <15 but some have >=5; the ImageGrids
		have nearly identical colors and most likely come from the same image.
	4) All sections have a distance of <50 but some have >=15; the ImageGrids
		have generally identical colors and possibly come from the same image.
	3) <10 sections have a distance of <15 and <20% have >=50; the ImageGrids
		have slightly identical colors and possibly come from the same image.
	2) >=10 sections have a distance of <15 but <20% have >=50; the ImageGrids
		have barely identical colors and likely describe different images.
	1) >=20% of sections have a distance of >=50; the ImageGrids almost
		certainly describe different images.


Note that the header references file "tests.cpp", which includes unit testing
and which I have not uploaded to this repository.  File tests.cpp references
many paths from my own PC a, paths that I don't wish to expose to the public.
The file also references numerous test images from my personal library that I
don't have permission to share or don't want to share.  I may eventually
upload a sanitized version of "tests.cpp" and my Test Images folder.
