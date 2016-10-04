#include "Parser.h"

int line = 1;

// next_c() wraps the getc() function and provides error checking and line
// number maintenance
int next_c(FILE* json) {
	int c = fgetc(json);
	#ifdef DEBUG
	printf("next_c: '%c'\n", c);
	#endif
	if (c == '\n') {
		line += 1;
	}
	if (c == EOF) {
		fprintf(stderr, "Error: Unexpected end of file on line number %d.\n", line);
		exit(1);
	}
	return c;
}


// expect_c() checks that the next character is d.  If it is not it emits
// an error.
void expect_c(FILE* json, int d) {
	int c = next_c(json);
	if (c == d) return;
	fprintf(stderr, "Error: Expected '%c' on line %d.\n", d, line);
	exit(1);    
}


// skip_ws() skips white space in the file.
void skip_ws(FILE* json) {
	int c = next_c(json);
	while (isspace(c)) {
		c = next_c(json);
	}
	ungetc(c, json);
}


// next_string() gets the next string from the file handle and emits an error
// if a string can not be obtained.
char* next_string(FILE* json) {
	char buffer[129];
	int c = next_c(json);
	if (c != '"') {
		fprintf(stderr, "Error: Expected string on line %d.\n", line);
		exit(1);
	}  
	c = next_c(json);
	int i = 0;
	while (c != '"') {
		if (i >= 128) {
			fprintf(stderr, "Error: Strings longer than 128 characters in length are not supported.\n");
			exit(1);      
		}
		if (c == '\\') {
			fprintf(stderr, "Error: Strings with escape codes are not supported.\n");
			exit(1);      
		}
		if (c < 32 || c > 126) {
			fprintf(stderr, "Error: Strings may contain only ascii characters.\n");
			exit(1);
		}
		buffer[i] = c;
		i += 1;
		c = next_c(json);
	}
	buffer[i] = 0;
	return strdup(buffer);
}

//If no value is found, 0 is assumed. therefore, a vector [,,,] is [0,0,0]
double next_number(FILE* json) {
	double value = 0;
	char* temp;
	
	//fscanf(json, "%s", &temp);
	fscanf(json, "%lf", &value);
	//printf("%s\n", &temp);
	// TODO: Error check this..
	//printf("%lf\n",value);
	return value;
}

double* next_vector(FILE* json) {
	double* v = malloc(3*sizeof(double));
	expect_c(json, '[');
	skip_ws(json);
	//printf("num: %d",next_number(json));
	v[0] = next_number(json);
	skip_ws(json);
	expect_c(json, ',');
	skip_ws(json);
	v[1] = next_number(json);
	skip_ws(json);
	expect_c(json, ',');
	skip_ws(json);
	v[2] = next_number(json);
	skip_ws(json);
	expect_c(json, ']');
	return v;
}

Object* read_camera(FILE* filename) {
	Object* object = malloc(sizeof(Object));
	object -> kind = 0;
	int c;
	
	while (1) {
		c = next_c(filename);
		if (c == '}') {
			// stop parsing this object
			break;
		} else if (c == ',') {
			// read another field
			skip_ws(filename);
			char* key = next_string(filename);
			skip_ws(filename);
			expect_c(filename, ':');
			skip_ws(filename);
			if (strcmp(key, "width") == 0){
				object->camera.width = next_number(filename);
			} else if (strcmp(key, "height") == 0){
				object->camera.height = next_number(filename);
			} else {
				fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
				key, line);
				exit(1);
			}

			skip_ws(filename);
		} else {
			fprintf(stderr, "Error: Unexpected value on line %d\n", line);
			exit(1);
		}
	}

	return object;
}

Object* read_sphere(FILE* filename) {
	Object* object = malloc(sizeof(Object));
	object -> kind = 1;
	int c;
	
	while (1) {
	// , }
		c = next_c(filename);
		if (c == '}') {
			// stop parsing this object
			break;
		} else if (c == ',') {
			// read another field
			skip_ws(filename);
			char* key = next_string(filename);
			skip_ws(filename);
			expect_c(filename, ':');
			skip_ws(filename);
			if (strcmp(key, "radius") == 0) {
				object->sphere.radius = next_number(filename);
			} else if (strcmp(key, "color") == 0){
				double* vector = next_vector(filename);
				object->sphere.color[0] = vector[0];
				object->sphere.color[1] = vector[1];
				object->sphere.color[2] = vector[2];
			} else if (strcmp(key, "position") == 0){
				double* vector = next_vector(filename);
				object->sphere.center[0] = vector[0];
				object->sphere.center[1] = vector[1];
				//printf("center %d\n", vector[1]);
				object->sphere.center[2] = vector[2];
			} else {
				fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
				key, line);
				exit(1);
				//char* value = next_string(json);
			}

			skip_ws(filename);
		} else {
			fprintf(stderr, "Error: Unexpected value on line %d\n", line);
			exit(1);
		}
	}
	//printf("type is %d\n, center1 is %lf\n", object->kind, object->sphere.center[1]);
	return object;
}

Object* read_plane(FILE* filename) {
	Object* object = malloc(sizeof(Object));
	object -> kind = 2;
	int c;
	
	while (1) {
	// , }
		c = next_c(filename);
		if (c == '}') {
			// stop parsing this object
			break;
		} else if (c == ',') {
			// read another field
			skip_ws(filename);
			char* key = next_string(filename);
			skip_ws(filename);
			expect_c(filename, ':');
			skip_ws(filename);
			if (strcmp(key, "position") == 0) {
				double* vector = next_vector(filename);
				object->plane.position[0] = vector[0];
				object->plane.position[1] = vector[1];
				object->plane.position[2] = vector[2];
			} else if (strcmp(key, "color") == 0){
				double* vector = next_vector(filename);
				object->plane.color[0] = vector[0];
				object->plane.color[1] = vector[1];
				object->plane.color[2] = vector[2];
			} else if (strcmp(key, "normal") == 0){
				double* vector = next_vector(filename);
				object->plane.normal[0] = vector[0];
				object->plane.normal[1] = vector[1];
				object->plane.normal[2] = vector[2];
			} else {
				fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
				key, line);
				exit(1);
				//char* value = next_string(json);
			}

			skip_ws(filename);
		} else {
			fprintf(stderr, "Error: Unexpected value on line %d\n", line);
			exit(1);
		}
	}

	return object;
}


int read_scene(Object** objects, char* filename) {
	int c;
	FILE* json = fopen(filename, "r");

	if (json == NULL) {
		fprintf(stderr, "Error: Could not open file \"%s\"\n", filename);
		exit(1);
	}

	skip_ws(json);

	// Find the beginning of the list
	expect_c(json, '[');

	skip_ws(json);

	int numObjs = 0;
	// Find the objects

	while (1) {
		c = fgetc(json);
		if (c == ']') {
			fprintf(stderr, "Error: This is the worst scene file EVER.\n");
			fclose(json);
			return;
		}
		if (c == '{') {
			skip_ws(json);

			// Parse the object
			char* key = next_string(json);
			if (strcmp(key, "type") != 0) {
				fprintf(stderr, "Error: Expected \"type\" key on line number %d.\n", line);
			exit(1);
			}

			skip_ws(json);

			expect_c(json, ':');

			skip_ws(json);

			char* value = next_string(json);

			objects[numObjs] = malloc(sizeof(Object));

			skip_ws(json);
			//STORE STUFF HERE
			if (strcmp(value, "camera") == 0) {
				//printf("storing cam in: %d\n",numObjs);
				objects[numObjs] = read_camera(json);
	
			} else if (strcmp(value, "sphere") == 0) {
				objects[numObjs] = read_sphere(json);
				//printf("storing sph in: %d\n",numObjs);
	
			} else if (strcmp(value, "plane") == 0) {
				objects[numObjs] = read_plane(json);
			//	printf("storing pla in: %d\n",numObjs);
	
			} else {
				fprintf(stderr, "Error: Unknown type, \"%s\", on line number %d.\n", value, line);
				exit(1);
			}

			numObjs++;

			/*while (1) {
			// , }
				c = next_c(json);
				if (c == '}') {
					// stop parsing this object
					break;
				} else if (c == ',') {
					// read another field
					skip_ws(json);
					char* key = next_string(json);
					skip_ws(json);
					expect_c(json, ':');
					skip_ws(json);
					if ((strcmp(key, "width") == 0) ||
					(strcmp(key, "height") == 0) ||
					(strcmp(key, "radius") == 0)) {
						double value = next_number(json);
					} else if ((strcmp(key, "color") == 0) ||
						(strcmp(key, "position") == 0) ||
						(strcmp(key, "normal") == 0)) {
						double* value = next_vector(json);
					} else {
						fprintf(stderr, "Error: Unknown property, \"%s\", on line %d.\n",
						key, line);
						exit(1);
						//char* value = next_string(json);
					}
		
					skip_ws(json);
				} else {
					fprintf(stderr, "Error: Unexpected value on line %d\n", line);
					exit(1);
				}
			}*/
			skip_ws(json);
			c = next_c(json);
			if (c == ',') {
				// noop
				skip_ws(json);
			} else if (c == ']') {
				fclose(json);
				return numObjs;
			} else {
				fprintf(stderr, "Error: Expecting ',' or ']' on line %d.\n", line);
				exit(1);
			}
		}
	}
}

/*int main(int c, char** argv) {
	Object** objects;
	objects = malloc(sizeof(Object*)*2);
	read_scene(objects, argv[1]);
	//puts("reading");
	//printf("type: %d width %lf height %lf\n", objects[0]->kind, objects[0]->camera.width, objects[0]->camera.height);
	//printf("type: %d color %lf %lf %lf center %lf %lf %lf radius %lf\n", objects[1]->kind, objects[1]->sphere.color[0], objects[1]->sphere.color[1], objects[1]->sphere.color[2], objects[1]->sphere.center[0], objects[1]->sphere.center[1], objects[1]->sphere.center[2], objects[1]->sphere.radius);
	//printf("type: %d color %lf %lf %lf position %lf %lf %lf normal %lf %lf %lf\n", objects[2]->kind, objects[2]->plane.color[0], objects[2]->plane.color[1], objects[2]->plane.color[2], objects[2]->plane.position[0], objects[2]->plane.position[1], objects[2]->plane.position[2], objects[2]->plane.normal[0], objects[2]->plane.normal[1], objects[2]->plane.normal[2]);
	return 0;
}*/
