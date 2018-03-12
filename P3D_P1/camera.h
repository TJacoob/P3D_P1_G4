#include "vec3.h"

struct _Camera {
	/* Camera definition*/
	Vec3 eye, at, up;
	float fovy;
	//float near, far; //hither and yon planes
	//int ResX, ResY; 
	float w, h;
	Vec3 xe, ye, ze; //uvn frame
};
typedef struct _Camera Camera;

_Camera* startCam(Camera *c, Vec3 eye, Vec3 at, Vec3 up, double fovy, double near, double far, int ResX, int ResY)
{
	c->eye = eye;
	c->at = at;
	c->up = up;

	//double dist = 				//Field of vision size, norma de vetores
	//c->h = 
	//c->w = (ResX/ResY)*c->h
};

Ray camGetPrimaryRay(Camera camera, double x, double y)
{
	//Calculate Primary Rays and return
}