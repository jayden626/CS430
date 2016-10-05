/**
 * Author: Jayden Urch
 * Date: 10/04/2016
 * Student No: 5388406 / jsu22
 *
 * This program can be used to read in a JSON scene,
 * use raytracing techniques to build an image,
 * and write the image to a P6 PPM file.
 *
 * Correct format is raycast width height input output
 * Ex: raycast 100 200 input.json output.ppm
 */
 
#include <math.h>
#include "Parser.h"
#include "ppmrw.h"

/* Method to return the square of the passed double. Used to make code easier to read
 *
 * Parameters:		v: double value to be squared
 *
 * Preconditions:	None
 *
 * Postconditions:	Will return v*v
 */
static inline double sqr(double v) {
	return v*v;
}

/* Method to normalize the given double triplet (Vector 3)
 *
 * Parameters: 		v: Vector to be normalized
 *
 * Preconditions: 	v must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Vector v will be normalized
 */
static inline void normalize(double* v) {
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

/* Method to return the t value of a ray-cylinder intersection
 * Parameters: 		Ro: Ray origin
 *					Rd: Ray direction
 *					Cs: Center of sphere
 *					r:  Radius of sphere
 *
 * Preconditions:	Rd must be normalized
 *					Ro, Rd and Cs must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the smallest t value (point of intersection) if found
 *					otherwise returns -1
 */
double cylinderIntersection(double* Ro, double* Rd, double* C, double r) {
	  // Step 1. Find the equation for the object you are
	  // interested in..  (e.g., cylinder)
	  //
	  // x^2 + z^2 = r^2
	  //
	  // Step 2. Parameterize the equation with a center point
	  // if needed
	  //
	  // (x-Cx)^2 + (z-Cz)^2 = r^2
	  //
	  // Step 3. Substitute the eq for a ray into our object
	  // equation.
	  //
	  // (Rox + t*Rdx - Cx)^2 + (Roz + t*Rdz - Cz)^2 - r^2 = 0
	  //
	  // Step 4. Solve for t.
	  //
	  // Step 4a. Rewrite the equation (flatten).
	  //
	  // -r^2 +
	  // t^2 * Rdx^2 +
	  // t^2 * Rdz^2 +
	  // 2*t * Rox * Rdx -
	  // 2*t * Rdx * Cx +
	  // 2*t * Roz * Rdz -
	  // 2*t * Rdz * Cz +
	  // Rox^2 -
	  // 2*Rox*Cx +
	  // Cx^2 +
	  // Roz^2 -
	  // 2*Roz*Cz +
	  // Cz^2 = 0
	  //
	  // Steb 4b. Rewrite the equation in terms of t.
	  //
	  // t^2 * (Rdx^2 + Rdz^2) +
	  // t * (2 * (Rox * Rdx - Rdx * Cx + Roz * Rdz - Rdz * Cz)) +
	  // Rox^2 - 2*Rox*Cx + Cx^2 + Roz^2 - 2*Roz*Cz + Cz^2 - r^2 = 0
	  //
	  // Use the quadratic equation to solve for t..
	  double a = (sqr(Rd[0]) + sqr(Rd[2]));
	  double b = (2 * (Ro[0] * Rd[0] - Rd[0] * C[0] + Ro[2] * Rd[2] - Rd[2] * C[2]));
	  double c = sqr(Ro[0]) - 2*Ro[0]*C[0] + sqr(C[0]) + sqr(Ro[2]) - 2*Ro[2]*C[2] + sqr(C[2]) - sqr(r);

	  double det = sqr(b) - 4 * a * c;
	  if (det < 0) return -1;

	  det = sqrt(det);
	  
	  double t0 = (-b - det) / (2*a);
	  if (t0 > 0) return t0;

	  double t1 = (-b + det) / (2*a);
	  if (t1 > 0) return t1;

	  return -1;
}

/* Method to return the t value of a ray-sphere intersection
 * Parameters: 		Ro: Ray origin
 *					Rd: Ray direction
 *					Cs: Center of sphere
 *					r:  Radius of sphere
 *
 * Preconditions:	Rd must be normalized
 *					Ro, Rd and Cs must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the smallest t value (point of intersection) if found
 *					otherwise returns -1
 */
double sphereIntersection(double* Ro, double* Rd, double* Cs, double r){

	// Rd is normalised, A = 1; double A = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
	double B = 2*(Rd[0]*(Ro[0] - Cs[0]) + Rd[1]*(Ro[1] - Cs[1]) + Rd[2]*(Ro[2] - Cs[2]));
	double C = sqr(Ro[0]-Cs[0]) + sqr(Ro[1]-Cs[1]) + sqr(Ro[2]-Cs[2]) - sqr(r);

	double det = sqr(B) - 4*C;
	if(det < 0){
		return -1;
	}

	det = sqrt(det);

	double t0 = (-B - det) /2;
	if(t0 > 0){
		return t0;
	}

	double t1 = (-B + det) /2;
	if(t1 > 0){
		return t1;
	}

	return -1;
}

/* Method to return the t value of a ray-plane intersection
 * Parameters: 		Ro:    Ray origin
 *					Rd:    Ray direction
 *					point: A point on the plane
 *					N:     Normal vector of the plane
 *
 * Preconditions:	Rd must be normalized
 *					Ro, Rd, point and N must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the smallest t value (point of intersection) if found
 *					otherwise returns -1
 */
double planeIntersection(double* Ro, double* Rd, double* point, double* N){

	double d = ((-point[0])*N[0])+((-point[1])*N[1])+((-point[2])*N[2]);
	double VdotN = (Rd[0]*N[0])+(Rd[1]*N[1])+(Rd[2]*N[2]);
	double RoDotN = (Ro[0]*N[0])+(Ro[1]*N[1])+(Ro[2]*N[2]);

	double t = (-(RoDotN+d))/VdotN;
	
	return t;
}

/* Main method. Will read in the arguements, use the parser to get a list of objects,
 * then perform ray tracing techniques to build an image and write it to a P6 PPM file.
 * Parameters: 		argc: Number of arguements
 *					argv: array of argument values
 *
 * Preconditions:	argc must equal 5
 *					argv[1] and argv[2] must be positive integers
 *					argv[3] must be a compatible JSON file
 *
 * Postconditions:	Outputs a P6 PPM image of the scene, traced from camera position {0,0,0}
 */
int main(int argc, char** argv) {

	Object** objects; //Array of objects in the scene
	objects = malloc(sizeof(Object*)*2*128);
	RGBpixel* pixmap; //The pixel map used for storing pixel values
	FILE* input;
	FILE* output;
	int imgHeight; //Height of the image in pixels
	int imgWidth; //Width of the image in pixels

	//Reading in arguements and checking for errors
	if(argc != 5){
		fprintf(stderr, "Error: Wrong amount of arguements. Correct format is raycast width height input output");
		return 1;
	}
	imgWidth = (int) strtol(argv[1],(char **)NULL,10); 
	if(imgWidth <= 0){
		fprintf(stderr, "Error: Width must be positive");
		return 1;
	}

	imgHeight = (int) strtol(argv[2],(char **)NULL,10);
	if(imgHeight <= 0){
		fprintf(stderr, "Error: Height must be positive");
		return 1;
	}

	output = fopen(argv[4], "w");
	if(!output){
		fprintf(stderr, "Error: Cannot open output file.");
		return 1;
	}

	//Initializing the pixel map
	pixmap = malloc(sizeof(RGBpixel)*imgHeight*imgWidth*3);

	//Using the parser to read the scene, store all objects in the objects array and return the number of objects
	int numObjs = read_scene(objects, argv[3]);
	double cx = 0; //Center of the x pixel
	double cy = 0; //Center of the y pixel

	double camHeight;
	double camWidth;

	//Scanning the objects list for the first camera and setting the view plane width and height
	int scanCam;
	int camFound = 0; //boolean used to specify if the camera has been found
	for (scanCam = 0; scanCam < numObjs; scanCam++) {
		if(objects[scanCam]->kind == 0){
			camHeight = objects[scanCam]->camera.height;
			camWidth = objects[scanCam]->camera.width;
			camFound = 1;
			break;
		}
	}
	if(camFound != 1){
		fprintf(stderr,"Error: Camera not found.\n");
		exit(1);
	}

	//Setting the height and width of each pixel in world space
	double pixheight = camHeight / imgHeight;
	double pixwidth = camWidth / imgWidth;

	//Looping over each pixel in the viewplane, checking for intersections and storing the color value
	int y;
	for (y = 0; y < imgHeight; y++) {
		int x;
		for (x = 0; x < imgWidth; x++) {
			double color[3] = {0,0,0}; //Initialized to black. Is set when an intersection is found
			double Ro[3] = {0, 0, 0}; //Origin of the ray from the camera. Set to {0,0,0} for this assignment

			// Rd = normalize(P - Ro)
			double Rd[3] = {cx - (camWidth/2) + pixwidth * (x + 0.5), cy - (camHeight/2) + pixheight * (y + 0.5), 1};
			normalize(Rd);

			double best_t = INFINITY; //Used to find the closest intersection

			//Looping over all objects, checking for intersections with the current ray direction from the origin
			int i;
			for (i=0; i < numObjs; i += 1) {
				double t = 0;
				int kind = objects[i]->kind;
				switch(objects[i]->kind) {
					case 0:
					break;

					case 1:
						t = sphereIntersection(Ro, Rd,
						objects[i]->sphere.center,
						objects[i]->sphere.radius);
					break;

					case 2:
						t = planeIntersection(Ro, Rd,
						objects[i]->plane.position,
						objects[i]->plane.normal);
					break;
					
					default:
						fprintf(stderr,"Error: Cannot process object of kind %d\n",objects[i]->kind);
						exit(1);
				}
				//Checking if the t value is the smallest (i.e. closest object) and that it is in front of the camera
				//Then sets the color vector to that objects color.
				if (t > 0 && t < best_t){
					best_t = t;
					if(kind == 1){
						color[0] = objects[i]->sphere.color[0];
						color[1] = objects[i]->sphere.color[1];
						color[2] = objects[i]->sphere.color[2];
						//printf("color: %lf %lf %lf \n",color[0], color[1], color[2]);
					}
					else if(kind == 2){
						color[0] = objects[i]->plane.color[0];
						color[1] = objects[i]->plane.color[1];
						color[2] = objects[i]->plane.color[2];
					}
				}
			}

			//Can print the scene pixel by pixel to the terminal.
			if (best_t > 0 && best_t != INFINITY) {
				printf("#");
			} else {
				printf(".");
			}

			//Setting the color of the pixel to the color vector. Flips the y-axis
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3] = color[0]*255;
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3+1] = color[1]*255;
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3+2] = color[2]*255;

		}
		printf("\n");
	}

	//Writes the pixel map to the output file.
	writeP6(output, pixmap, imgWidth, imgHeight, 255);

	return 0;
}
