#include <stdio.h>
#include <stdlib.h>


typedef unsigned char RGBpixel[3];

int main(int argc, char *argv[]) {
	char magicNumber[8];
	char tempInput[8];
	int height;
	int width;
	int scale;
	
	FILE* input = fopen("test2.data", "r");
	if(input)
	printf("Hello world\n");

	RGBpixel* pix = malloc(sizeof(char)*3);
	*pix[0] = 0b01010000;
	*pix[1] = 'G';
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
	
	fscanf(input,"%8s",magicNumber);
	printf("hi: %s\n", magicNumber);
	if(strcmp(magicNumber,"P3") == 0){
		//if p3 go readp3 if rawbyte for p6 go readp6
		printf("Test1");
		fscanf(input,"%8s",width);
		printf("Test2");
		fscanf(input,"%8s",height);
		printf("Test3");
		fscanf(input,"%8s",scale);
		printf("Test4");
		if(scale < 0 || scale > 255 || width < 0 || height < 0){
			printf("Error: Scale invalid (print to stderr)");
		}
		printf("p3");
				printf("p3 and %u %u %u",width, height, scale);
		readP3(input);
	}
	else if(strcmp(magicNumber,"01010000") == 0){
		printf("p6");
		fscanf(input,"%8s",tempInput);
		width = (int) strtol(*tempInput,(char **)NULL,2);
		fscanf(input,"%8s",tempInput);
		height = (int) strtol(*tempInput,(char **)NULL,2);
		fscanf(input,"%8s",tempInput);
		scale = (int) strtol(*tempInput,(char **)NULL,2);
		if(scale < 0 || scale > 255 || width < 0 || height < 0){
			printf("Error: Scale invalid (print to stderr)");
		}

			printf("P6 and %u %u %u",width, height, scale);
	}
	printf("yo I'm here");

	/*temp  = fgetc(fh);
	printf("%c \n", temp);

	temp  = fgetc(fh);
	printf("%c \n", temp);

	temp  = fgetc(fh);
	printf("%c \n", temp);

	temp  = fgetc(fh);
	printf("%c \n", temp);

	temp  = fgetc(fh);
	printf("%c \n", temp);*/

	readP3(input);
	int close = fclose(input);
	printf("Hello world\n");
	return 0;
}

int readP3(FILE* f){
	char* text;

	fscanf(f,"%8s",text);
	printf("%s\n", text);

	fscanf(f,"%8s",text);
	printf("%s\n", text);

	fscanf(f,"%8s",text);
	printf("%s\n", text);

	fscanf(f,"%8s",text);
	printf("%s\n", text);
	/*text = fgetc(f);
	printf("Yo I'm here and: %u \n", text);

		text = fgetc(f);
	printf("%u \n", text);
	text = fgetc(f);
	printf("%u \n", text);
	text = fgetc(f);
	printf("%u \n", text);*/

	return 0;
}
