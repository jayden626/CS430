#ifndef URCH_A3_PARSER
#define URCH_A3_PARSER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "object.h"



int next_c(FILE* json);
void expect_c(FILE* json, int d);
int check_c(FILE* json, int d);
void skip_ws(FILE* json);
char* next_string(FILE* json);
double next_number(FILE* json);
double* next_vector(FILE* json);
Object* read_camera(FILE* filename);
Object* read_sphere(FILE* filename);
Object* read_plane(FILE* filename);
Object* read_light(FILE* filename);
void read_scene(Object** objects, Object** lights, char* filename);

#endif
