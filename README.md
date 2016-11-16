# CS430
## Project 1 - Images

This program can be used to read a P3 or P6 PPM file into memory and write the memory to a file in either P3 or P6 format.

It's proper usage should be "ppmrw [3|6] inputFile.extension outputFile.extension". The first value after the program name is the output type, either in P3 or P6.

NOTE: If height or width is less than 1, program will throw an error.

NOTE: Will not throw an error if the width * height is less than or greater than the number of input pixels
      i.e. a 3 * 3 file having 4 * 4 input pixels or a 5 * 5 file having 4 * 4 input pixels

Author: Jayden Urch
Date: 09/16/2016
Student No: 5388406 / jsu22

## Project 2 - Raycaster

This program can be used to read in a JSON scene, uses raycasting techniques to build an image, and write the image to a P6 PPM file. A valid input JSON file is provided as well as the corresponding output. Test image is size 100 x 100

Correct format is raycast width height input output
* Ex: raycast 100 100 input.json output.ppm

Author: Jayden Urch
Date: 10/04/2016
Student No: 5388406 / jsu22

## Project 3 - Raycaster

This program can be used to read in a JSON scene, uses raycasting techniques to build an image, and write the image to a P6 PPM file. A valid input JSON file is provided as well as the corresponding output. Test image is size 1000 x 1000

Correct format is raycast width height input output
* Ex: raycast 1000 1000 input.json output.ppm

Author: Jayden Urch
Date: 10/20/2016
Student No: 5388406 / jsu22

## Project 4 - Raytracer

This program can be used to read in a JSON scene, uses recursive raytracing techniques (including reflectivity and refractivity) to build an image, and write the image to a P6 PPM file. A valid input JSON file is provided as well as the corresponding output. Test image is size 1000 x 1000

Correct format is raycast width height input output
* Ex: raycast 1000 1000 input.json output.ppm

Author: Jayden Urch
Date: 11/15/2016
Student No: 5388406 / jsu22
