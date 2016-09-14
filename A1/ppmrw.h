#ifndef URCH_PPMRW
#define URCH_PPMRW

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char RGBpixel[3];

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')


int main(int argc, char* argv[]);
void printPixmap(RGBpixel** map, char* magicNumber, int width, int height, int scale);
void writeP3(FILE* output, RGBpixel** map, char* magicNumber, int width, int height, int scale);
void writeP6(FILE* output, RGBpixel** map, char* magicNumber, int width, int height, int scale);
void readPPM(FILE* input, RGBpixel** map, char* magicNumber, int width, int height, int scale);


#endif
