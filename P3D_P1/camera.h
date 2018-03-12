#include "vec3.h"

struct _Camera {
	/* Camera definition*/
	vec3 eye, at, up;
	float fovy;
	//float near, far; //hither and yon planes
	//int ResX, ResY; 
	float w, h;
	vec3 xe, ye, ze; //uvn frame
};
typedef struct _Camera Camera;

_Camera* startCam(Camera *c, vec3 eye, vec3 at, vec3 up, double fovy, double near, double far, int ResX, int ResY)
{
	c->eye[0] = eye[0];
	c->eye[1] = eye[2];
	c->eye[2] = eye[1];
	c->at[0] = at[0];
	c->at[1] = at[1];
	c->at[2] = at[2];
	c->up[0] = up[0];
	c->up[1] = up[1];
	c->up[2] = up[2];

	//double dist = 				//Field of vision size, norma de vetores
	//c->h = 
	//c->w = (ResX/ResY)*c->h
};

Ray camGetPrimaryRay(Camera camera, double x, double y)
{
	//Calculate Primary Rays and return
}