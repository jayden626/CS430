#ifndef URCH_A3_RAYTRACER
#define URCH_A3_RAYTRACER

#include <math.h>
#include "parser.h"
#include "ppmrw.h"

#define AMBIENT_IOR 1.0 //Index of refraction for the external medium

static inline double sqr(double v);
static inline void normalize(double* v);
static inline void invert(double* v);
static inline void vector3Addition(double* v1, double* v2);
double clamp(double d);
double distance(double* v1, double* v2);
double dot (double* v1, double* v2);
double* reflect (double* v, double* n);
double* diffuse(double* diffColor, double* lightColor, double* N, double* L);
double* specular(double* specColor, double* lightColor, double* L, double* N, double* R, double* V);
double radial(Object* light, double distanceToLight);
double angular(Object* light, double* Rd);
double cylinderIntersection(double* Ro, double* Rd, double* C, double r);
double sphereIntersection(double* Ro, double* Rd, double* Cs, double r);
double planeIntersection(double* Ro, double* Rd, double* point, double* N);
void shoot(double* Ro, double* Rd, Object** objects, double* best_t, int* best_i);
int checkShadow(double* Ro, double* Rd, Object** objects, double distanceToLight, int best_i);

#endif
