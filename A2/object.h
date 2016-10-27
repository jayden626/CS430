#ifndef URCH_A2_OBJECT
#define URCH_A2_OBJECT

// Plymorphism in C

typedef struct {
  int kind; // 0 = camera, 1 = cylinder, 2 = sphere
  double color[3];
  union {
  	struct {
  		double width;
  		double height;
  	} camera;
    struct {
    	double color[3];
      double center[3];
      double radius;
    } sphere;
    struct {
		  double color[3];
      double position[3];
      double normal[3];
    } plane;
  };
} Object;

#endif
