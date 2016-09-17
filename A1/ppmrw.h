#ifndef URCH_PPMRW
#define URCH_PPMRW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char RGBpixel;

int main(int argc, char* argv[]);
void printPixmap(RGBpixel* map, char* magicNumber, int width, int height, int scale);
void writeP3(FILE* output, RGBpixel* map, int width, int height, int scale);
void writeP6(FILE* output, RGBpixel* map, int width, int height, int scale);
void readP3(FILE* input, RGBpixel* map, int width, int height, int scale);
void readP6(FILE* input, RGBpixel* map, int width, int height);

#endif
