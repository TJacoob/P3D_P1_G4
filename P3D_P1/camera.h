#include "vec3.h"
#include "Ray.h"

#ifndef _CAMERAH_
#define _CAMERAH_

struct Camera
{
public:
	
	Vec3 eye, at, up;		// Initializing vectors
	float angle, view;  // Angle of vision and distance from camera to plane

	int ResX, ResY;			// Initial Resolutions
	float width, height;
	
	Vec3 xe, ye, ze;		// Camera vectors in world coordinates
	

	// Construtores
	Camera() : eye(Vec3(0,0,0)), at(Vec3(0,0,0)), up(Vec3(0,0,1)), angle(0), ResX(1), ResY(1) {};
	Camera(Vec3 eye, Vec3 at, Vec3 up, float angle, int ResX, int ResY) : eye(eye), at(at), up(up), angle(angle), ResX(ResX), ResY(ResY) {
		view = (at - eye).module();
		height = 2 * view * tan(angle / 2);	   // Distance * hipotenusa de metade do angulo
		width = height * (ResX / ResY);

		// Conversion to camera coordinates
		ze = (eye - at) * ( 1/ (eye - at).module() );
		xe = (up*ze) * (1 / (up*ze).module());
		ye = ze * xe;
	};
	
	// Methods
	Ray getPrimaryRay(double x, double y)
	{
		Vec3 dx = xe.normalize() * width * ((float)( (x / ResX) +0.5));
		Vec3 dy = ye.normalize() * height * ((float)( (y / ResY) + 0.5));
		Vec3 dz = ze.normalize() * (-view);
		return Ray(eye, dz+dy+dx);
	}


	void print() {
		printf("CAMERA SETTINGS:\n");
		printf("EYE: %f %f %f \n", eye.x, eye.y, eye.z);
		printf("AT: %f %f %f \n", at.x, at.y, at.z);
		printf("UP: %f %f %f \n", up.x, up.y, up.z);
		printf("ResX: %d , ResY: %d\n", ResX, ResY);
		printf("ANGLE: %f, VIEW: %f\n", angle, view);
		printf("Width: %f, Height: %f\n\n", width, height);
		printf("xe: %f %f %f \n", xe.x, xe.y, xe.z);
		printf("ye: %f %f %f \n", ye.x, ye.y, ye.z);
		printf("ze: %f %f %f \n", ze.x, ze.y, ze.z);
		printf("---------------------------\n");
	}

};

#endif