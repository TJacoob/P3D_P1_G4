#include "vec3.h"

struct _Camera {
	/* Camera definition*/
	Vec3 eye, at, up;
	float fovy;
	//float near, far; //hither and yon planes
	//int ResX, ResY; 
	float w, h;
	Vec3 xe, ye, ze; //uvn frame
	//Vec3 uvn; //Already in xyz mode
};
typedef struct _Camera Camera;

_Camera* startCam(Camera *c, Vec3 eye, Vec3 at, Vec3 up, double fovy, double near, double far, int ResX, int ResY)
{
	c->eye = eye;
	c->at = at;
	c->up = up;

	//printf("eye: %f %f %f\n", eye.x, eye.y, eye.z);
	//printf("at: %f %f %f\n", at.x, at.y, at.z);
	//printf("ResX: %d ,ResY: %d\n", ResX, ResY);
	//printf("�ngulo de vis�o: %lf\n", fovy);

	Vec3 temp = at-eye;
	//printf("temp: %f %f %f\n", temp.x, temp.y, temp.z);
	//temp = temp.normalize();    //Deve ser normalizado?? E se sim, � aqui que tem de ser?
	double df = temp.module();
	printf("Dist�ncia da camara: %lf\n", df);
	// Falta dar assign � camera deste valor, se for preciso

	c->h = (float) 2 * df * tan(fovy / 2);
	printf("Altura: %f\n", c->h);

	c->w = (float) (ResX/ResY) * (c->h) ;
	printf("Largura: %f\n", c->w);
	
	float calc = (1 / df);
	Vec3 temp2 = (eye - at);
	temp2.x = temp2.x*calc;
	temp2.y = temp2.y*calc;
	temp2.z = temp2.z*calc;
	c->ze = temp2;
	printf("ze: %f %f %f\n", c->ze.x, c->ze.y, c->ze.z);

	//double dist = 				//Field of vision size, norma de vetores
	//c->h = 
	//c->w = (ResX/ResY)*c->h
	
	return c;
};

