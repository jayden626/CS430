//#include <stdio.h>
//#include <stdlib.h>
#include <math.h>
#include "Parser.h"
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

int main(int c, char** argv) {

  Object** objects;
  objects = malloc(sizeof(Object*)*2);

  read_scene(objects, argv[1]);
 /* objects[0] = malloc(sizeof(Object));
  objects[0]->kind = 1;
  objects[0]->sphere.radius = 2;
  // object[0]->teapot.handle_length = 2;
  objects[0]->sphere.center[0] = 0;
  objects[0]->sphere.center[1] = 0;
  objects[0]->sphere.center[2] = 0;
  objects[1] = NULL;*/
  
  double cx = 0;
  double cy = 0;
  double h = 0.7;
  double w = 0.7;

  int M = 20;
  int N = 20;

		//normalize(objects[0]->sphere.center);

  double pixheight = h / M;
  double pixwidth = w / N;
  int y;
  for (y = 0; y < M; y += 1) {
  	int x;
    for (x = 0; x < N; x += 1) {
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
      for (i=0; objects[i] != 0; i += 1) {
	double t = 0;

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
	  // Horrible error
	  exit(1);
	}
	if (t > 0 && t < best_t) best_t = t;
      }
      if (best_t > 0 && best_t != INFINITY) {
	printf("#");
      } else {
	printf(".");
      }
      
    }
    printf("\n");
  }
  
  return 0;
}
