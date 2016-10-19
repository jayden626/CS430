#ifndef URCH_A2_OBJECT
#define URCH_A2_OBJECT

// Plymorphism in C

typedef struct {
	int kind; // 0 = camera, 1 = sphere, 2 = plane, 3 = light
	double color[3];
	union {
		struct {
			double width;
			double height;
		} camera;
		struct {
			double diffuseColor[3];
			double specularColor[3];
			double center[3];
			double radius;
		} sphere;
		struct {
			double diffuseColor[3];
			double specularColor[3];
			double position[3];
			double normal[3];
		} plane;
		struct {
			double color[3];
			double position[3];
			double direction[3];
			double radial[3];
			double angularA0;
			double theta;
			int isSpot;
		} light;
	};
} Object;

#endif
