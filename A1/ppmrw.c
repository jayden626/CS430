//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
#include "ppmrw.h"


//typedef unsigned char RGBpixel[3];

/*#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') */


int main(int argc, char* argv[]) {
	RGBpixel **pixmap;
	FILE* input;
	FILE* output;
	int convertToNumber;
	char magicNumber[8];
	unsigned char tempInput[9];
	int height;
	int width;
	int scale;
	int count = 1;

	if(argc == 4){
		convertToNumber = (int) strtol(argv[1],(char **)NULL,10);
		input = fopen(argv[2], "r");
		output = fopen(argv[3], "w");
	}
	else{
		printf("ERROR: Wrong arguements");
		return 1;
	}

	/*if(input)
	printf("Hello world\n");

	RGBpixel* pix = malloc(sizeof(char)*3);
	*pix[0] = 0b01010000;
	*pix[1] = (char) strtol("80",(char **)NULL,10);
	*pix[2] = *pix[0];


	printf("%c %c %c", *pix[0],*pix[1],*pix[2]);

	height = (int) strtol("101",(char **)NULL,2);
	printf("\nheight: %u\n",height);*/
	//input = fopen("test2.data", "r");
	//printf("here");
	if(!input){
		printf("TO STDERR: File not found");
		return 1;
	}

	//Read magic number
	fscanf(input,"%8s",magicNumber);
	
	//Read width and convert to int, repeat for height and scale
	int commentLine = 1;
	do{
		fscanf(input,"%8s",tempInput);
		if(tempInput[0] == '#'){
			char str[1000];
			fgets (str, 1000, input);
			printf("%s\n",str);
		}
		else{
			width = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);
	//fscanf(input,"%8s",tempInput);
	//return 1;
	//width = (int) strtol(tempInput,(char **)NULL,10);

	fscanf(input,"%8s",tempInput);
	height = (int) strtol(tempInput,(char **)NULL,10);

	fscanf(input,"%8s",tempInput);
	scale = (int) strtol(tempInput,(char **)NULL,10);
	//Error checking
	if(scale < 0 || scale > 255 || width < 0 || height < 0){
		printf("Error: Scale invalid (print to stderr)");
	}

	//Initialize pixmap (Inspired by the readings)
	pixmap = malloc(sizeof(RGBpixel*)*height);
	pixmap[0] = malloc(sizeof(RGBpixel)*height*width);
	for(count = 1; count<height; count++){
		pixmap[count] = pixmap[count-1] + (sizeof(RGBpixel)*width);
	}

	printf("%s and %u %u %u\n",magicNumber, width, height, scale);
	if(strcmp(magicNumber,"P3") == 0 || strcmp(magicNumber,"P6") == 0){
		readPPM(input, pixmap, magicNumber, width, height, scale);


		/*int countWidth;
		int countColor;
		for(countWidth = 0; countWidth<width; countWidth++){
			for(countColor = 0; countColor<3; countColor++){
				printf("%*d ", 3, pixmap[height-1][countWidth][countColor]);
			}
			printf("\t");
		}
		printf("\n");*/
		printf("last value is %u\n", pixmap[4][4][2]);
		printPixmap(pixmap, magicNumber, width, height, scale);
		puts("printed");
		writeP3(output, pixmap, magicNumber, width, height, scale);
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

void printPixmap(RGBpixel** map, char* magicNumber, int width, int height, int scale){
	int countWidth;
	int countHeight;
	int countColor;
	int maxPrintWidth = 3;

	//printf("%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for(countHeight = 0; countHeight<height; countHeight++){
		for(countWidth = 0; countWidth<width; countWidth++){
			for(countColor = 0; countColor<3; countColor++){
				printf("%*d ", maxPrintWidth, map[countHeight][countWidth][countColor]);
			}
			printf("\t");
		}
		printf("\n");
	}
	//return 0;
}

void writeP3(FILE* output, RGBpixel** map, char* magicNumber, int width, int height, int scale){
	int countWidth = 0;
	int countHeight = 0;
	int countColor = 0;
	int maxPrintWidth = 3;
	unsigned char* tempOutput;

	fprintf(output, "%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for(countHeight; countHeight<height; countHeight++){
		countWidth = 0;
		for(countWidth; countWidth<width; countWidth++){
			countColor = 0;
			for(countColor; countColor<3; countColor++){
				//fwrite(map[countHeight][countWidth], 1, 1, tempOutput);
				fprintf(output, "%*d ", maxPrintWidth, map[countHeight][countWidth][countColor]);
			}
			fprintf(output, "\t");
		}
		fprintf(output, "\n");
	}
	//return 0;
}

void writeP6(FILE* output, RGBpixel** map, char* magicNumber, int width, int height, int scale){
	int countWidth = 0;
	int countHeight = 0;
	int countColor = 0;
	int maxPrintWidth = 3;
	//char* tempOutput;

	fprintf(output, "%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for(countHeight; countHeight<height; countHeight++){
		countWidth = 0;
		for(countWidth; countWidth<width; countWidth++){
			countColor = 0;
			for(countColor; countColor<3; countColor++){
				//fwrite(*map[countHeight][countWidth], 1, 3, output);
				fprintf(output, BYTE_TO_BINARY_PATTERN" ", BYTE_TO_BINARY(map[countHeight][countWidth][countColor]));
			}
			fprintf(output, "\t");
		}
		fprintf(output, "\n");
	}
	//return 0;
}

void readPPM(FILE* input, RGBpixel** map, char* magicNumber, int width, int height, int scale){
	unsigned char* tempInput;
	int tempInt;
	RGBpixel* tempPixel;
	int base;
	int countWidth;
	int countHeight;
	int countColor; //loops through 0,1,2 for assigning the correct color values in the RGBpixel

	if(strcmp(magicNumber,"P3") == 0){
		base = 10;
	}
	else if(strcmp(magicNumber,"P6") == 0){
		base = 2;
	}
	else{
		printf("ERROR HERE");
		//return 1;
	}
	
	for(countHeight = 0; countHeight<height; countHeight++){
		for(countWidth = 0; countWidth<width; countWidth++){
			for(countColor = 0; countColor<3; countColor++){
				//tempPixel = **map[countHeight][countWidth];
				fscanf(input,"%8s",tempInput);
				if((int)strlen(tempInput) > 8){
					printf("Error: Sting greater than 8");
				}
				tempInt = (int)strtol(tempInput,(char **)NULL, base);
				printf("%u\t%u\n",tempInt, scale);
				if(tempInt < 0 || tempInt > 255){// || tempInt > scale){
					printf("%u Error: String size out of bounds", tempInt);
				}
				printf("Storing %u at height %u width %u color %u\n", tempInt, countHeight, countWidth, countColor);
				map[countHeight][countWidth][countColor] = (unsigned char)tempInt;
				printf("Stored %u at height %u width %u color %u\n", map[countHeight][countWidth][countColor], countHeight, countWidth, countColor);
			}
			//printf("last value is %u\n", map[4][4][2]);
		}
		//printf("last value is %u\n", map[4][4][2]);
	}
	printf("return value is %u\n", map[4][4][2]);
	//return 0;
}


