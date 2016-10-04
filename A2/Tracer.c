//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include "Parser.h"
#include "ppmrw.h"
//#include "object.h"

static inline double sqr(double v) {
  return v*v;
}


static inline void normalize(double* v) {
  double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
  v[0] /= len;
  v[1] /= len;
  v[2] /= len;
}

double cylinder_intersection(double* Ro, double* Rd,
			     double* C, double r) {
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

double sphere_intersection(double* Ro, double* Rd, double* Cs, double r){

	//printf("ray direction: %lf %lf %lf\n", Rd[0], Rd[1],Rd[2]);
//	printf("center: %lf %lf %lf\n", Cs[0], Cs[1],Cs[2]);
	// Rd is normalised, A = 1; double A = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
	double B = 2*(Rd[0]*(Ro[0] - Cs[0]) + Rd[1]*(Ro[1] - Cs[1]) + Rd[2]*(Ro[2] - Cs[2]));
	double C = sqr(Ro[0]-Cs[0]) + sqr(Ro[1]-Cs[1]) + sqr(Ro[2]-Cs[2]) - sqr(r);

	double det = sqr(B) - 4*C;
	if(det < 0){
		return -1;
	}

	det = sqrt(det);

	double t0 = (-B - det) /2;
	//printf("t0 = %lf\n", t0);
	if(t0 > 0){
		return t0;
	}

	double t1 = (-B + det) /2;
	//printf("t1 = %lf\n", t1);
	if(t1 > 0){
		return t1;
	}

	return -1;
}

double plane_intersection(double* Ro, double* Rd, double* point, double* N){
	double d = ((-point[0])*N[0])+((-point[1])*N[1])+((-point[2])*N[2]);
	double VdotN = (Rd[0]*N[0])+(Rd[1]*N[1])+(Rd[2]*N[2]);
	double RoDotN = (Ro[0]*N[0])+(Ro[1]*N[1])+(Ro[2]*N[2]);

	double t = (-(RoDotN+d))/VdotN;
	//printf("d: %lf  VdotN: %lf  RoDotN: %lf  t: %lf\n",d, VdotN, RoDotN,t);
	return t;
}

int main(int argc, char** argv) {

  Object** objects;
  objects = malloc(sizeof(Object*)*2);
  RGBpixel* pixmap;
  FILE* input;
  FILE* output;
  int convertToNumber;
  int M;
  int N;

  //Reading in arguements and checking for errors
	if(argc != 5){
		fprintf(stderr, "Error: Wrong amount of arguements. Correct format is raycast width height input output");
		return 1;
	}
	convertToNumber = (int) strtol(argv[1],(char **)NULL,10); 
	if(convertToNumber <= 0){
		fprintf(stderr, "Error: Width must be positive");
		return 1;
	}
	N = convertToNumber;
	
	convertToNumber = (int) strtol(argv[2],(char **)NULL,10);
	if(convertToNumber <= 0){
		fprintf(stderr, "Error: Height must be positive");
		return 1;
	}
	M = convertToNumber;
	
	/*input = fopen(argv[3], "r");
	if(!input){
		fprintf(stderr, "Error: Cannot open input file.");
		return 1;
	}*/
	output = fopen(argv[4], "w");
	if(!output){
		fprintf(stderr, "Error: Cannot open output file.");
		return 1;
	}

	pixmap = malloc(sizeof(RGBpixel)*M*N*3);

 	int numObjs = read_scene(objects, argv[3]);
printf("num: %d\n", numObjs);
  double cx = 0;
  double cy = 0;
  //from the camera
  double h;
  double w;
  int camFound = 0;
 	int scanCam;
	for (scanCam=0; scanCam < numObjs; scanCam += 1) {
		if(objects[scanCam]->kind == 0){
			h = objects[scanCam]->camera.height;
			w = objects[scanCam]->camera.width;
			camFound = 1;
			break;
		}
	}
	if(camFound != 1){
		fprintf(stderr,"Error: Camera not found.\n");
		exit(1);
	}
	
			
		//normalize(objects[0]->sphere.center);

  double pixheight = h / M;
  double pixwidth = w / N;
  int y;
  for (y = 0; y < M; y += 1) {
  	int x;
    for (x = 0; x < N; x += 1) {
    	double color[3] = {0,0,0};
      double Ro[3] = {0, 0, 0};
      // Rd = normalize(P - Ro)
      double Rd[3] = {
			cx - (w/2) + pixwidth * (x + 0.5),
			cy - (h/2) + pixheight * (y + 0.5),
			1
      };
      normalize(Rd);

      double best_t = INFINITY;
      int i;
      for (i=0; i < numObjs; i += 1) {
	double t = 0;
	int kind = objects[i]->kind;
	switch(objects[i]->kind) {
	
	case 0:
		break;
	
	case 1:
	  t = sphere_intersection(Ro, Rd,
				    objects[i]->sphere.center,
				    objects[i]->sphere.radius);
	  break;

  	case 2:
	  t = plane_intersection(Ro, Rd,
				    objects[i]->plane.position,
				    objects[i]->plane.normal);
	  break;
	default:
	  fprintf(stderr,"Error: Cannot process object of kind %d\n", objects[i]->kind);
	  exit(1);
	}
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
      if (best_t > 0 && best_t != INFINITY) {
		printf("#");
		//printf("y: %d x: %d position: %d", y, x, y*M*3+x*3);
      } else {
		printf(".");
      }

  		pixmap[y*M*3 + x*3] = color[0];
		pixmap[y*M*3 + x*3+1] = color[1];
		pixmap[y*M*3 + x*3+2] = color[2];
      
    }
    printf("\n");
  }

  writeP3(output, pixmap, N, M, 1);
  
  return 0;
}
