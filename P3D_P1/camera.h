#include "vec3.h"

struct _Camera {
	/* Camera definition*/
	Vec3 eye, at, up;
	float fovy;
	float df;
	//float near, far; //hither and yon planes
	int ResX, ResY; 
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
	c->ResX = ResX;
	c->ResY = ResY;

	//printf("eye: %f %f %f\n", eye.x, eye.y, eye.z);
	//printf("at: %f %f %f\n", at.x, at.y, at.z);
	//printf("ResX: %d ,ResY: %d\n", ResX, ResY);
	//printf("Angulo de visao: %lf\n", fovy);

	Vec3 temp = at - eye;
	//printf("temp: %f %f %f\n", temp.x, temp.y, temp.z);
	//temp = temp.normalize();    //Deve ser normalizado?? E se sim, � aqui que tem de ser?
	float df = temp.module();
	c->df = df;
	printf("Dist�ncia da camara: %f\n", c->df);
	
	c->h = (float)2 * df * tan(fovy / 2);
	printf("Altura: %f\n", c->h);

	c->w = (float)(ResX / ResY) * (c->h);
	printf("Largura: %f\n", c->w);

	float calc = (1 / df);
	Vec3 temp2 = (eye - at);
	temp2.x = temp2.x*calc;
	temp2.y = temp2.y*calc;
	temp2.z = temp2.z*calc;
	c->ze = temp2;
	

	Vec3 tempX = Vec3(up.x * c->ze.x, up.y * c->ze.y, up.z * c->ze.z);
	float calcX = 1 / (tempX.module());
	tempX.x = tempX.x * calcX;
	tempX.y = tempX.y * calcX;
	tempX.z = tempX.z * calcX;
	c->xe = tempX;					//Resultado no input_file dá 0 0 1, é bom sinal?
	

	Vec3 tempY = Vec3(c->ze.x * c->xe.x, c->ze.y * c->xe.y, c->ze.z * c->xe.z);
	c->ye = tempY;
	
	printf("CAMERA VECTORS: \n");
	printf("ze: %f %f %f\n", c->ze.x, c->ze.y, c->ze.z);
	printf("ye: %f %f %f\n", c->ye.x, c->ye.y, c->ye.z);
	printf("xe: %f %f %f\n", c->xe.x, c->xe.y, c->xe.z);
	printf("---------------------------------\n");

	return c;
};



