#ifndef URCH_A2_RAYCASTER
#define URCH_A2_RAYCASTER

#include <math.h>
#include "parser.h"
#include "ppmrw.h"

static inline double sqr(double v);
static inline void normalize(double* v);
double cylinderIntersection(double* Ro, double* Rd, double* C, double r);
double sphereIntersection(double* Ro, double* Rd, double* Cs, double r);
double planeIntersection(double* Ro, double* Rd, double* point, double* N);

#endif
