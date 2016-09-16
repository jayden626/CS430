#include "ppmrw.h"

//Main function
int main(int argc, char* argv[]) {
	RGBpixel* pixmap;
	FILE* input;
	FILE* output;
	int convertToNumber; //Magic number to write out to
	char magicNumber[255];
	unsigned char tempInput[255]; //Buffer that temporarily stores input values before storing them
	int height;
	int width;
	int scale;
	int count; //iterator used to initialise the pixmap

	//Reading in arguements and checking for errors
	if(argc != 4){
		fprintf(stderr, "Error: Wrong amount of arguements. Correct format is ppmrw [3|6] input output.");
		return 1;
	}
	convertToNumber = (int) strtol(argv[1],(char **)NULL,10);	
	printf("%d\n",convertToNumber);
	if(convertToNumber != 3 && convertToNumber != 6){
		fprintf(stderr, "Error: Not a correct conversion number. Correct formats are '3' or '6'.");
		return 1;
	}
	input = fopen(argv[2], "r");
	if(!input){
		fprintf(stderr, "Error: Cannot open input file.");
		return 1;
	}
	output = fopen(argv[3], "w");
	if(!input){
		fprintf(stderr, "Error: Cannot open output file.");
		return 1;
	}

	//Read magic number
	fscanf(input,"%s",magicNumber);

	//Read in all comment lines. The last read value will contain the width. Works with zero or many comments
	int commentLine; //boolean used in while loop
	do{
		fscanf(input,"%s",tempInput);
		if(tempInput[0] == '#'){ //TODO: Comments can appear anywhere in header
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, input); //ignore rest of the line. please no lines over 1000 char
			printf("%s\n",str);
		}
		else{
			width = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);

	//Scanning and storing height and scale
	do{
		fscanf(input,"%s",tempInput);
		if(tempInput[0] == '#'){ //TODO: Comments can appear anywhere in header
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, input); //ignore rest of the line. please no lines over 1000 char
			printf("%s\n",str);
		}
		else{
			height = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);
	
	//fscanf(input,"%s",tempInput);
	//height = (int) strtol(tempInput,(char **)NULL,10);

	do{
		fscanf(input,"%s",tempInput);
		if(tempInput[0] == '#'){ //TODO: Comments can appear anywhere in header
			commentLine = 1;
			char str[1000];
			fgets (str, 1000, input); //ignore rest of the line. please no lines over 1000 char
			printf("%s\n",str);
		}
		else{
			scale = (int) strtol(tempInput,(char **)NULL,10);
			commentLine = 0;
		}
	}
	while(commentLine);
	//fscanf(input,"%d",tempInput);
	//scale = (int) strtol(tempInput,(char **)NULL,10);
	fgetc(input);
	
	//Checking that each channel is a single byte
	if(scale < 0 || scale > 255 || width < 0 || height < 0){
		fprintf(stderr, "Error: Scale is more than one byte per channel.");
	}

	//Initialize pixmap (Inspired by the readings)
	//pixmap = malloc(sizeof(RGBpixel*)*height);
	pixmap = malloc(sizeof(RGBpixel)*height*width*3);
	//for(count = 1; count<height; count++){
	//	pixmap[count] = pixmap[count-1] + (sizeof(RGBpixel)*width);
	//}
	
	printf("%s and %u %u %u\n",magicNumber, width, height, scale); //print for debugging purposes

	//Check magicNumber and use appropriate read function
	if(strcmp(magicNumber,"P3") == 0){
		readP3(input, pixmap, magicNumber, width, height, scale);
		printf("%u\n",pixmap[0*width + 1*3 + 2]);
		printPixmap(pixmap, magicNumber, width, height, scale); //debugging
		puts("printed"); //debugging
		
	}
	else if(strcmp(magicNumber,"P6") == 0){
		readP6(input,output, pixmap, width, height, scale);
		printPixmap(pixmap, magicNumber, width, height, scale);
		//puts("printed");
		//writeP6(output, pixmap, width, height, scale);
		puts("written"); //debugging
	}

	//Check conversion arguement and use appropriate output function
	if(convertToNumber == 3){
		puts("writing p3");//debugging
		writeP3(output, pixmap, width, height, scale);
	}
	else if(convertToNumber == 6){
		puts("writing p6");//debugging
		writeP6(output, pixmap, width, height, scale);
	}

	int inputClose = fclose(input);
	int outputClose = fclose(output);
	return 0;
}

/*
 * This funtion prints the given array of RGBpixels to stdout
 */
void printPixmap(RGBpixel* map, char* magicNumber, int width, int height, int scale){
	int countWidth;
	int countHeight;
	int countColor;
	int maxPrintWidth = 3;
	int j = 0;

	//Print header, then loop through pixmap printing each value to stdout
	printf("%s\n%u %u\n%u\n", magicNumber, width, height, scale);
	for (j=0; j<height*width*3; j++) {
	//for(countHeight = 0; countHeight<height; countHeight++){
	//	for(countWidth = 0; countWidth<width; countWidth++){
	//		for(countColor = 0; countColor<3; countColor++){
				printf("%*u ", maxPrintWidth, map[j]);//countHeight*width + countWidth*3 + countColor]);//[countWidth][countColor]);
	//		}
			printf("\t");
	//	}
		printf("\n");
	//}
	}
}

/*
 * This funtion writes the given array of RGBpixels to the given output file in P3 format
 */
void writeP3(FILE* output, RGBpixel* map, int width, int height, int scale){
	int countWidth;
	int countHeight;
	int countColor;
	int maxPrintWidth = 3;
	int j = 0;
	//Print header, then loop through pixmap printing each value to output file
	//TODO: Fix.
	fprintf(stdout, "P3\n%u %u\n%u\n", width, height, scale);
	fprintf(output, "P3\n%u %u\n%u\n", width, height, scale);
	//for(countHeight = 0; countHeight<height; countHeight++){
	//	for(countWidth = 0; countWidth<width; countWidth++){
	//		for(countColor = 0; countColor<3; countColor++){
	for (j=0; j<height*width*3; j++) {
				fprintf(stdout, "%*u ", maxPrintWidth, map[j]);//countHeight*width + countWidth*3 + countColor]);
				//fprintf(stdout, "%u\n", map[countHeight][countWidth][countColor]);
				fprintf(output, "%*u ", maxPrintWidth, map[j]);//countHeight*width + countWidth*3 + countColor]);
	//		}
			fprintf(stdout, "\t");
			//fprintf(stdout, "\n");
			fprintf(output, "\t");
	//	}
		fprintf(stdout, "\n");
		fprintf(output, "\n");
	//}
	}
}

/*
 * This funtion writes the given array of RGBpixels to the given output file in P6 format
 */
void writeP6(FILE* output, RGBpixel* map, int width, int height, int scale){
	int countWidth = 0;
	int countHeight = 0;
	int countColor = 0;

	//Print header, write the pixmap to the output file.
	fprintf(output, "P6\n%u %u\n%u\n", width, height, scale);
	fwrite(map, sizeof(RGBpixel), height*width*3, output);
}

/*
 * This funtion reads the given P3 input file and stored it in the buffer
 */
void readP3(FILE* input, RGBpixel* map, char* magicNumber, int width, int height, int scale){
	unsigned char tempInput[8];
	int tempInt;
	int countWidth;
	int countHeight;
	int countColor; //loops through 0,1,2 for assigning the correct color values in the RGBpixel
	int i = 0;
	int j = 0;
	//for(countHeight = 0; countHeight<height; countHeight++){
	//	for(countWidth = 0; countWidth<width; countWidth++){
	//		for(countColor = 0; countColor<3; countColor++){
			for (j=0; j<height*width*3; j++) {
				fscanf(input,"%d",&tempInt);
				//if((int)strlen(tempInput) > 8){
				//	printf("Error: Sting greater than 8");
				//}
				//tempInt = (int)strtol(tempInput,(char **)NULL, 10);
				//printf("%u\n",tempInt);
				if(tempInt < 0 || tempInt > 255){// || tempInt > scale){
					printf("%u Error: String size out of bounds", tempInt);
				}
				map[j] = (unsigned char)tempInt; // countHeight*width + countWidth*3 + countColor] = i(unsigned char)tempInt;
				i += 1;
				printf("%u\n",map[j]); // countHeight*width + countWidth*3 + countColor]);
		//	}
			printf("\n");
		}
	//}
			printf("%u\n",map[0*width + 1*3 + 2]);
}

/*
 * This funtion reads the given P6 input file and stored it in the buffer
 * May or may not be working properly. Either this or the output is broken, or there's weird errors again
 */
int readP6(FILE* input, FILE* output, RGBpixel* map, int width, int height, int scale){
	unsigned char* tempInput;
	int tempInt;
	//RGBpixel* tempPixel;
	//int base = 2;
	//char tempOut[255];
	int countWidth;
	int countHeight;
	int countColor; //loops through 0,1,2 for assigning the correct color values in the RGBpixel
	printf("reading P6\n");
	fread(map, sizeof(RGBpixel), height*width*3, input);
	int j = 0;
	
	//tempInput = &(map[9][9][2]);
	//printf("%d\n",tempInput);
	//printf("%8u\n", map[0][0][0]);
	//puts("printed");
		//itoa(map[0][0][0], tempOut, 2);
		//sprintf(tempOut, "1","%d",map[0][0][0]);
		//printf("%s\n", tempOut);

	//fprintf(output, "P6\n%u %u\n%u\n", width, height, scale);
	//fwrite(map, sizeof(RGBpixel)*width, height, output);
	//for(countHeight = 0; countHeight<height; countHeight++){
	//	for(countWidth = 0; countWidth<width; countWidth++){
	//		for(countColor = 0; countColor<3; countColor++){
	/*for (j=0; j<height*width*3; j++) {
		printf("j = %d\n", j);
		fread(tempInput, sizeof(RGBpixel), 1, input);
		printf("tempInput: %d\n", tempInput);*/
		//tempInt = *tempInput;
		//printf("tempInt: %d\n", tempInt);
		//map[j] = tempInt;//countHeight*width + countWidth*3 + countColor] = (unsigned char)tempInt;
		//printf("%u\n", map[j]);
				//countHeight*width + countWidth*3 + countColor]);
				//strcpy(tempInput, map[countHeight][countWidth][countColor]);
				//printf("%u yo yo\n",map[countHeight][countWidth][countColor]);
				//tempInt = (int)strtol(tempInput,(char **)NULL, base);
				//printf("Storing %u at height %u width %u color %u\n", tempInt, countHeight, countWidth, countColor);..0
				//map[countHeight][countWidth][countColor] = (unsigned char)tempInt;
				//puts("conv");
				//printf("Stored %u at height %u width %u color %u\n", map[countHeight][countWidth][countColor], countHeight, countWidth, countColor);
	//		}
	//	}
	//}
	//}
	puts("read");
	return 0;
}


