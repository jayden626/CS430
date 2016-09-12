#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "ppmrw.h"


typedef unsigned char RGBpixel[3];

int main(int argc, char *argv[]) {
	RGBpixel **pixmap;
	char magicNumber[8];
	char tempInput[9];
	int height;
	int width;
	int scale;
	int count = 1;
	
	FILE* input = fopen("test3.data", "r");
	if(input)
	printf("Hello world\n");

	RGBpixel* pix = malloc(sizeof(char)*3);
	*pix[0] = 0b01010000;
	*pix[1] = (char) strtol("80",(char **)NULL,10);
	*pix[2] = *pix[0];


	printf("%c %c %c", *pix[0],*pix[1],*pix[2]);

	height = (int) strtol("101",(char **)NULL,2);
	printf("\nheight: %u\n",height);
	//input = fopen("test2.data", "r");
	//printf("here");
	/*if(!input){
		printf("TO STDERR: File not found");
		return 1;
	}*/

	//Read magic number
	fscanf(input,"%8s",magicNumber);
	//Read width and convert to int, repeat for height and scale
	fscanf(input,"%8s",tempInput);
	width = (int) strtol(tempInput,(char **)NULL,10);

	fscanf(input,"%8s",tempInput);
	height = (int) strtol(tempInput,(char **)NULL,10);

	fscanf(input,"%8s",tempInput);
	scale = (int) strtol(tempInput,(char **)NULL,10);
	//Error checking
	if(scale < 0 || scale > 255 || width < 0 || height < 0){
		printf("Error: Scale invalid (print to stderr)");
	}

	//Initialize pixmap (Inspired by the readings)
	pixmap = malloc(sizeof(RGBpixel)*height);
	pixmap[0] = malloc(sizeof(RGBpixel)*height*width);
	for(count; count<height; count++){
		pixmap[count] = pixmap[count-1] + width;
	}

	printf("%s and %u %u %u\n",magicNumber, width, height, scale);
	if(strcmp(magicNumber,"P3") == 0 || strcmp(magicNumber,"P6") == 0){
		readPPM(input, pixmap, width, height, scale);
		printPixmap(pixmap, magicNumber, width, height, scale);
	}
	/*else if(strcmp(magicNumber,"P6") == 0){
		//readP6(input);
		readP3(input, pixmap, width, height, scale);
		printPixmap(pixmap, magicNumber, width, height, scale);
		puts("Meant to read P6 here");
	}*/

	int close = fclose(input);
	return 0;
}

int printPixmap(RGBpixel** map, char* magicNumber, int width, int height, int scale){
	int countWidth = 0;
	int countHeight = 0;
	int countColor = 0;
	int maxPrintWidth = 3;

	printf("%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for(countHeight; countHeight<height; countHeight++){
		countWidth = 0;
		for(countWidth; countWidth<width; countWidth++){
			countColor = 0;
			for(countColor; countColor<3; countColor++){
				printf("%*d ", maxPrintWidth, map[countHeight][countWidth][countColor]);
			}
			printf("\t");
		}
		printf("\n");
	}
	return 0;
}

int readPPM(FILE* input, RGBpixel** map, int magicNumber, int width, int height, int scale){
	char* tempInput;
	int tempInt;
	RGBpixel* tempPixel;
	int base = 0;
	int countWidth = 0;
	int countHeight = 0;
	int countColor = 0; //loops through 0,1,2 for assigning the correct color values in the RGBpixel

	if(strcmp(magicNumber,"P3") == 0){
		base = 10;
	}
	else if(strcmp(magicNumber,"P6") == 0){
		base = 2;
	}
	
	for(countHeight; countHeight<height; countHeight++){
		countWidth = 0;
		for(countWidth; countWidth<width; countWidth++){
			countColor = 0;

			for(countColor; countColor<3; countColor++){
				//tempPixel = **map[countHeight][countWidth];
				fscanf(input,"%8s",tempInput);
				if((int)strlen(tempInput) > 8){
					printf("Error: Sting greater than 8");
				}
				tempInt = (int)strtol(tempInput,(char **)NULL, base);
				printf("%u",tempInt);
				if(tempInt < 0 || tempInt > 255){
					printf("%u Error: String size out of bounds", tempInt);
				}
				map[countHeight][countWidth][countColor] = (char)tempInt;
			}
			printf("\n");
		}
	}		
	return 0;
}

