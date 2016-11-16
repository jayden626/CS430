/**
 * Author: Jayden Urch
 * Date: 11/15/2016
 * Student No: 5388406 / jsu22
 *
 * This program can be used to read in a JSON scene,
 * use raytracing techniques to build an image,
 * and write the image to a P6 PPM file.
 *
 * Correct format is raytrace width height input output
 * Ex: raycast 1000 1000 input.json output.ppm
 */
 
#include "raytrace.h"

/* Returns the square of the passed double. Used to make code easier to read
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

/* Normalizes the given double triplet (Vector 3)
 *
 * Parameters: 		v: Vector to be normalized
 *
 * Preconditions: 	v must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Vector v will be normalized
 */
static inline void normalize(double* v){
	double len = sqrt(sqr(v[0]) + sqr(v[1]) + sqr(v[2]));
	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

/* Inverts the direction of the given double triplet (Vector 3)
 *
 * Parameters: 		v: Vector to be inverted
 *
 * Preconditions: 	v must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Vector v will be inverted
 */
static inline void invert(double* v){
	v[0] *= -1;
	v[1] *= -1;
	v[2] *= -1;
}

/* Clamps the given double to values ranging from 0.0 to 1.0
 *
 * Parameters: 		d: double to be clamped
 *
 * Preconditions: 	None
 *
 * Postconditions:	If d < 0, will return 0. If d > 1, will return 1, else will return d
 */
double clamp(double d){
	if(d < 0){
		return 0;
	}
	else if(d > 1){
		return 1;
	}
	return d;
}

/* Returns the distance between the two given double triplets (Vector 3)
 *
 * Parameters: 		v1, v2: Vectors to compute the distance between
 *
 * Preconditions: 	v1, v2 must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the distance between the two vectors
 */
double distance(double* v1, double* v2){
	return sqrt(sqr(v2[0]-v1[0]) + sqr(v2[1]-v1[1]) + sqr(v2[2]-v1[2]));
}

/* Returns the dot product of the two given double triplets (Vector 3)
 *
 * Parameters: 		v1, v2: Vectors to compute the dot product of
 *
 * Preconditions: 	v1, v2 must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the dot product of the two vectors
 */
double dot (double* v1, double* v2){
	return (v1[0]*v2[0])+(v1[1]*v2[1])+(v1[2]*v2[2]);
}

/* Returns the reflection of the given vector on the given normal vector
 *
 * Parameters: 		v: Vector to be reflected
 *					n: Normal vector to reflect about
 *
 * Preconditions: 	v1, v2 must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the reflection of the given vector about the normal
 */
double* reflect (double* v, double* n){
	double nDotV = dot(n,v);
	double* result = malloc(sizeof(double)*3);

	result[0] = (2*nDotV*n[0]) - v[0];
	result[1] = (2*nDotV*n[1]) - v[1];
	result[2] = (2*nDotV*n[2]) - v[2];

	return result;
}

/* Calculates the diffuse color vector
 *
 * Parameters: 		diffColor: A vector containing 3 values for R,G,B colors
 *					lightColor: A vector containing 3 values for R,G,B colors
 *					N: A vector normal of the surface being reflected
 *					L: A vector of the light direction from the point being calculated
 *
 * Preconditions: 	diffColor, lightColor, N, L must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns a vector containing the new computed diffuse color values
 */
double* diffuse(double* diffColor, double* lightColor, double* N, double* L){
	double* result = malloc(sizeof(double)*3);
	double cosA = dot(N, L);
	if(cosA > 0){
		result[0] = diffColor[0] * lightColor[0] * cosA;
		result[1] = diffColor[1] * lightColor[1] * cosA;
		result[2] = diffColor[2] * lightColor[2] * cosA;
	}
	else{
		result[0] = 0;
		result[1] = 0;
		result[2] = 0;
	}

	return result;
}


/* Calculates the specular color vector
 *
 * Parameters: 		specColor: A vector containing 3 values for R,G,B colors
 *					lightColor: A vector containing 3 values for R,G,B colors
 *					N: A vector normal of the surface being reflected
 *					L: A vector of the light direction from the point being calculated
 *					R: The reflected vector from the light
 *					V: The direction vector towards the camera
 *
 * Preconditions: 	specColor, lightColor, N, L, R, V must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns a vector containing the new computed specular color values
 */
double* specular(double* specColor, double* lightColor, double* L, double* N, double* R, double* V){

	double* result = malloc(sizeof(double)*3);

	double cosA = dot(N, L);
	double cosB = dot(R, V);

	if(cosA > 0 && cosB > 0){
		result[0] = specColor[0] * lightColor[0] * pow(cosB,20);
		result[1] = specColor[1] * lightColor[1] * pow(cosB,20);
		result[2] = specColor[2] * lightColor[2] * pow(cosB,20);
	}
	else{
		result[0] = 0;
		result[1] = 0;
		result[2] = 0;
	}

	return result;
}

/* Calculates the radial attenuation
 *
 * Parameters: 		light: A reference to the light object
 *					distanceToLight: the distance from the light to the object ray intersection
 *
 * Preconditions: 	light must be a pointer to an Object which is a light (kind == 3)
 *
 * Postconditions:	Returns the radial attenuation value
 */
double radial(Object* light, double distanceToLight){
	if(light->kind != 3){
		fprintf(stderr, "Error: Cannot calculate radual attenuation, object kind = %d and is not a light", light->kind);
		exit(1);
	}

	return 1/(light->light.radial[2]*sqr(distanceToLight) + light->light.radial[1]*distanceToLight + light->light.radial[0]);

}

/* Calculates the angular attenuation
 *
 * Parameters: 		light: A reference to the light object
 *					Rd: A vector of the direction from the light to the object ray intersection
 *
 * Preconditions: 	light must be a pointer to an Object which is a light (kind == 3)
 *					Rd must be an array of at least 3 doubles. Will only consider the first 3 values if the array is larger.
 *
 * Postconditions:	Returns the anfular attenuation value
 */
double angular(Object* light, double* Rd){
	if(light->kind != 3){
		fprintf(stderr, "Error: Cannot calculate angular attenuation, object kind = %d and is not a light", light->kind);
		exit(1);
	}

	if(light->light.isSpot == 0){
		return 1;
	}

	double cosA = dot(light->light.direction, Rd);
	double cosT = cos(light->light.theta*(M_PI/180));

	if(cosA > cosT){
		return 0;
	}

	return pow(cosA, light->light.angularA0);
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

	// Rd is normalised, A = 1; 
	double A = (sqr(Rd[0]) + sqr(Rd[1]) + sqr(Rd[2]));
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
	double VdotN = dot(Rd,N);//(Rd[0]*N[0])+(Rd[1]*N[1])+(Rd[2]*N[2]);
	double RoDotN = dot(Ro,N);//(Ro[0]*N[0])+(Ro[1]*N[1])+(Ro[2]*N[2]);

	double t = (-(RoDotN+d))/VdotN;
	
	return t;
}

/* Method to shoot a ray from the origin in the direction given,
 * checking for intersections with any objects.
 *
 * Parameters: 		Ro:  		Ray origin
 *					Rd:  		Ray direction
 *					objects:	An array of Object pointers
 *					best_t:		The best t value for the intersection
 *					best_i:  	The index in the object array of the closest intersection
 *
 * Preconditions:	Rd must be normalized
 *					Ro, Rd must be an array of at least 3 doubles
 *					objects, best_t and best_i must not be NULL
 *
 * Postconditions:	If an intersection is found, best_t value will be set to the t value
 *					of the intersection and best_i will be the index of the closest intersected object
 *
 *					If no intersection, best_t will equal INFINITY and best_i will equal -1
 */
void shoot(double* Ro, double* Rd, Object** objects, double* best_t, int* best_i, int prev_i){
	*best_t = INFINITY;
	*best_i = -1;

	int i;
	for (i=0; objects[i] != NULL; i += 1) {
		if(i == prev_i) continue;
		double t = 0;
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

			case 3:
			break;
			default:
				fprintf(stderr,"Error: Cannot process object of kind %d\n",objects[i]->kind);
				exit(1);
		}
		//Checking if the t value is the smallest (i.e. closest object) and that it is in front of the camera
		if (t > 0 && t < *best_t){
			*best_t = t;
			*best_i = i;
		}
	}
}

/* Method to shoot a ray from the origin in the direction given,
 * checking for intersections with any objects.
 * Used to determine if an object is in shadow.
 *
 * Parameters: 		Ro:  					Ray origin
 *					Rd:  					Ray direction
 *					objects:				An array of Object pointers
 *					distanceToLight:		The distance to the light source from the object
 *					best_i:  				The index in the object array of the closest intersection
 *
 * Preconditions:	Rd must be normalized
 *					Ro, Rd must be an array of at least 3 doubles
 *					objects and best_i must not be NULL
 *
 * Postconditions:	If an intersection is found (and is between the light and the object),
 *					the object is in shadow and will return 1.
 *
 *					If no intersection is found, will return 0.
 */
int checkShadow(double* Ro, double* Rd, Object** objects, double distanceToLight, int best_i){
	int k;
	for (k=0; objects[k] != NULL; k++) {
		//skip if the current object is the one we are getting the color for
		if(objects[k] == objects[best_i]) continue;
		
		double lightT = -1;
		switch(objects[k]->kind) {
			case 0:
			break;

			case 1:
				lightT = sphereIntersection(Ro, Rd,
				objects[k]->sphere.center,
				objects[k]->sphere.radius);
			break;

			case 2:
				lightT = planeIntersection(Ro, Rd,
				objects[k]->plane.position,
				objects[k]->plane.normal);
			break;

			case 3:
			break;
			default:
				fprintf(stderr,"Error: Cannot process object of kind %d\n",objects[k]->kind);
				exit(1);
		}
		//check if object is in shadow
		if(lightT > 0 && lightT < distanceToLight){
			return 1;
			break;
		}
	}
	return 0;
}

double* recursiveRaytrace(double* Ro, double* Rd, Object** objects, Object** lights, int recurseNo, int prev_i){
	double* color = malloc(sizeof(double)*3); //Initialized to black. Is set when an intersection is found
	recurseNo++;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	if(recurseNo >= 7){
		return color;
	}
		
	double best_t = INFINITY; //Used to find the closest intersection
	int best_i = -1;

	shoot(Ro, Rd, objects, &best_t, &best_i, prev_i); //Finds the best t and intersected object
	if(best_i > -1 && best_t > 0 && best_t < INFINITY){

		double Ron[3];//New ray origin (point of intersection
		Ron[0] = (best_t*Rd[0] + Ro[0]);
		Ron[1] = (best_t*Rd[1] + Ro[1]);
		Ron[2] = (best_t*Rd[2] + Ro[2]);

		double V[3];//direction from intersection back to the camera (once inverted). Used for specular light calculations.
		V[0] = Rd[0];
		V[1] = Rd[1];
		V[2] = Rd[2];
		invert(V);
		normalize(V);
		
		//Loop over all lights looking for objects in the way
		int j;
		for(j=0; lights[j]!=NULL; j++){
		
			double Rdn[3]; //Ray direction from the object intersection towards the camera
			Rdn[0] = lights[j]->light.position[0] - Ron[0];
			Rdn[1] = lights[j]->light.position[1] - Ron[1];
			Rdn[2] = lights[j]->light.position[2] - Ron[2];
				
			//normalize(Ron);  <-- here was the cause of a lot of pain. Pro Tip: Don't normalize points.
			normalize(Rdn);
			double distanceToLight = distance(Ron, lights[j]->light.position);
			int isInShadow = checkShadow(Ron, Rdn, objects, distanceToLight, best_i);
			
			if(isInShadow == 0){
				double* N = malloc(sizeof(double)*3); //Normal to the intersection point
				double* objectDiffuse; //Diffuse color of the object
				double* objectSpecular; //Specular color of the object
				double reflectivity; //of the object
				double refractivity; //of the object
				double ior; //of the object

				//Getting normal and color vectors, reflectivity, refractivity and ior
				if(objects[best_i]->kind == 1){
					N[0] = Ron[0] - objects[best_i]->sphere.center[0];
					N[1] = Ron[1] - objects[best_i]->sphere.center[1];
					N[2] = Ron[2] - objects[best_i]->sphere.center[2];
					objectDiffuse = objects[best_i]->sphere.diffuseColor;
					objectSpecular = objects[best_i]->sphere.specularColor;
					reflectivity = objects[best_i]->sphere.reflectivity;
					refractivity = objects[best_i]->sphere.refractivity;
					ior = objects[best_i]->sphere.ior;
				
				}
				else if(objects[best_i]->kind == 2){
					N = objects[best_i]->plane.normal;
					objectDiffuse = objects[best_i]->plane.diffuseColor;
					objectSpecular = objects[best_i]->plane.specularColor;
					reflectivity = objects[best_i]->plane.reflectivity;
					refractivity = objects[best_i]->plane.refractivity;
					ior = objects[best_i]->plane.ior;
				
				}
				else{
					fprintf(stderr, "Error: Cannot calculate lighting.");
					exit(1);
				}
				
				normalize(N);
				double* R = reflect(Rdn,N); //Reflected vector of the new ray direction about the light
				normalize(R);
				double fang = angular(lights[j], Rdn); //angular attenuation
				double frad = radial(lights[j], distanceToLight); //radial attenuation
				double* diff = diffuse(objectDiffuse, lights[j]->light.color, N, Rdn); //diffuse color
				double* spec = specular(objectSpecular, lights[j]->light.color, Rdn, N, R, V); //specular color
				
				color[0] = color[0] + (fang*frad*(diff[0] + spec[0]));
				color[1] = color[1] + (fang*frad*(diff[1] + spec[1]));
				color[2] = color[2] + (fang*frad*(diff[2] + spec[2]));

				if(reflectivity > 0){
					double* reflectedColor = recursiveRaytrace(Ron, R, objects, lights, recurseNo, best_i);
					color[0] = color[0] + reflectedColor[0]*reflectivity;
					color[1] = color[1] + reflectedColor[1]*reflectivity;
					color[2] = color[2] + reflectedColor[2]*reflectivity;
				}

				if(refractivity > 0){

					double n = AMBIENT_IOR/ior;
					double c1 = -dot(N,Rd);
					double c2 = 1-n*n*(1-c1*c1);
					double* refractedRay = malloc(sizeof(double)*3);

					refractedRay[0] = (n*Rd[0]) + (n*c1-sqrt(c2))*N[0];
					refractedRay[1] = (n*Rd[1]) + (n*c1-sqrt(c2))*N[1];
					refractedRay[2] = (n*Rd[2]) + (n*c1-sqrt(c2))*N[2];
					
					double* refractedColor = recursiveRaytrace(Ron, refractedRay, objects, lights, recurseNo, best_i);
					color[0] = color[0] + refractedColor[0]*refractivity;
					color[1] = color[1] + refractedColor[1]*refractivity;
					color[2] = color[2] + refractedColor[2]*refractivity;
				}

				free(N);
				free(R);
				free(diff);
				free(spec);
				
			}
		}//End of light loop
	}
	return color;
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
	Object** lights; //Array of the lights in the scene
	lights = malloc(sizeof(Object*)*2*128);
	//Object* cam = NULL;// malloc(sizeof(Object));
	RGBpixel* pixmap; //The pixel map used for storing pixel values
	FILE* input;
	
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

	//Initializing the pixel map
	pixmap = malloc(sizeof(RGBpixel)*imgHeight*imgWidth*3);

	//Using the parser to read the scene, store all objects in the objects array and return the number of objects
	read_scene(objects, lights, argv[3]);
	
	double cx = 0; //Center of the x pixel
	double cy = 0; //Center of the y pixel

	double camHeight;// = cam->camera.height;
	double camWidth;// = cam->camera.width;


	//Scanning the objects list for the first camera and setting the view plane width and height
	int scanCam;
	int camFound = 0; //boolean used to specify if the camera has been found
	for (scanCam = 0; objects[scanCam] != NULL; scanCam++) {
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
			
			double Ro[3] = {0, 0, 0}; //Origin of the ray from the camera. Set to {0,0,0} for this assignment
			// Rd = normalize(P - Ro)
			double Rd[3] = {cx - (camWidth/2) + pixwidth * (x + 0.5), cy - (camHeight/2) + pixheight * (y + 0.5), 1};
			normalize(Rd);
			double* color = recursiveRaytrace(Ro, Rd, objects, lights, 0, -1);
			
		
			//Setting the color of the pixel to the color vector. Flips the y-axis
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3] = (unsigned char) (clamp(color[0])*255);
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3+1] = (unsigned char) (clamp(color[1])*255);
			pixmap[imgHeight*imgHeight*3-(y+1)*imgHeight*3 + x*3+2] = (unsigned char) (clamp(color[2])*255);
			
			free(color);
		}
	}

	FILE* output = fopen(argv[4], "w");
	if(!output){
		fprintf(stderr, "Error: Cannot open output file.");
		return 1;
	}
	//Writes the pixel map to the output file.
	writeP6(output, pixmap, imgWidth, imgHeight, 255);

	int freeObjects = 0;
	while(objects[freeObjects] != NULL){
		free(objects[freeObjects]);
		freeObjects++;
	}
	free(objects);
	
	int freeLights= 0;
	while(lights[freeLights] != NULL){
		free(lights[freeLights]);
		freeLights++;
	}
	free(lights);

	free(pixmap);
	
	return 0;
}
