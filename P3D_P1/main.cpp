///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2016 by Jo�o Madeiras Pereira
// TEMPLATE: Whitted Ray Tracing NFF scenes and drawing points with Modern OpenGL
//
//You should develop your rayTracing( Ray ray, int depth, float RefrIndex) which returns a color and
// to develop your load_NFF function
//
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <time.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "main.h"
#include "Vec3.h"
#include "Ray.h"
#include "Camera.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"
#include "Triangle.h"
#include "AreaLight.h"
#include "RGBType.h"
#include "Object.h"
#include "BBox.h"
#include "Grid.h"

#define CAPTION "ray tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 6
#define MAX_PLANES 5
#define MAX_LIGHTS 5

#define BIAS 0.01
#define MONTECARLO_THRESHOLD 0.3
#define JITTER_MATRIX 4
#define JITER_MATRIX 2.5
#define RAND_MAX 32767

#define LIGHT_SAMPLES 4

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;

int size_vertices;
int size_colors;

int thisone;

GLfloat m[16];  //projection matrix initialized by ortho function

GLuint VaoId;
GLuint VboId[2];

GLuint VertexShaderId, FragmentShaderId, ProgramId;
GLint UniformId;

/*NFF FILE*/
FILE * nff;

int RES_X, RES_Y;

Camera c;

// Other Helpers
Vec3 background;

// Planes Array
float plane[MAX_PLANES][17];
int num_planes = 0;

// Lights Array
float light[MAX_LIGHTS][6];	// X Y Z R G B
int num_lights = 0;

float latestF[8];

// Grid

Grid grid;

/* Draw Mode: 0 - point by point; 1 - line by line; 2 - full frame */
int draw_mode = 0;

int WindowHandle = 0;


double r2()	// Devolve random entre 0 e 1
{
	return (double)rand() / (double)RAND_MAX;
}

bool rayIntersect(Ray ray) {

	float planeIntersect, sphereIntersect, triangleIntersect;

	//PLANE INTERSECTION CYCLE
	for (int j = 0; j <= num_planes; j++) {
		Plane p(Vec3(plane[j][0], plane[j][1], plane[j][2]), Vec3(plane[j][3], plane[j][4], plane[j][5]), Vec3(plane[j][6], plane[j][7], plane[j][8]), Vec3(plane[j][9], plane[j][10], plane[j][11]), plane[j][12], plane[j][13], plane[j][14], plane[j][15], plane[j][16]);
		//printf("testando planos\n");
		planeIntersect = p.intersect(ray);
		if (planeIntersect != 0) {
			//printf("INTERSECTEI UM PLANOOOO\n");
			return true;

		}
	}

	//SPHERE INTERSECTION CYCLE
	for (int i = 0; i <= num_spheres; i++) {
		Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
		//printf("testando esferas\n");
		sphereIntersect = s.intersect(ray);

		if (sphereIntersect != 0) {
			//printf("INTERSECTEI UMA ESFERAAAAA\n");
			return true;
		}
	}

	//TRIANGLE INTERSECTION CYCLE
	for (int k = 0; k <= num_triangles; k++) {
		Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);

		triangleIntersect = t.intersect(ray);
		//printf("Triangle T: %f \n", triangleIntersect);

		if (triangleIntersect != 0) {
			//printf("INTERSECTEI UM TRIANGULO\n");
			return true;
		}
	}

	return false;
}


///////////////////////////////////////////////////////////////////////  RAY-TRACE SCENE

Vec3 rayTracing(Ray ray, int depth, float RefrIndex)
{
	Vec3 c = background;
	Vec3 normal;
	float tempT = 0, altT = 0, shortT = 1000;
	float Kdif = 0, Ks = 0, shine = 0, trans = 0, indexRef = 0;
	int fs;

	int tipoIntersect = 0;

	bool intersect = false;

	
	// REGULAR INTERSECTION MODEL - CHECK BELOW FOR GRID MODEL
	//PLANE INTERSECTION CYCLE
	if (num_planes != 0)
	{
		for (int j = 0; j < num_planes; j++) {
			Plane p(Vec3(plane[j][0], plane[j][1], plane[j][2]), Vec3(plane[j][3], plane[j][4], plane[j][5]), Vec3(plane[j][6], plane[j][7], plane[j][8]), Vec3(plane[j][9], plane[j][10], plane[j][11]), plane[j][12], plane[j][13], plane[j][14], plane[j][15], plane[j][16]);

			shortT = p.intersect(ray);

			if (shortT != 0) {
				intersect = true;
				c = p.color;
				normal = p.getNormal();
				Kdif = p.Kdif;
				Ks = p.Ks;
				shine = p.Shine;
				trans = p.Trans;
				indexRef = p.IndexRef;
				tipoIntersect = 0;
			}
			else {

			}
		}
	}

	//SPHERE INTERSECTION CYCLE
	if (num_spheres != 0)
	{
		for (int i = 0; i < num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);

			tempT = s.intersect(ray);

			if (tempT == 0) {

			}
			else {
				intersect = true;
				if (tempT < shortT) {
					shortT = tempT;
					c = s.color;
					normal = s.getNormal(ray, shortT);
					Kdif = s.Kdif;
					Ks = s.Ks;
					shine = s.Shine;
					trans = s.Trans;
					indexRef = s.IndexRef;
					tipoIntersect = 1;
				}
			}
		}
	}

	//TRIANGLE INTERSECTION CYCLE
	if (num_triangles != 0)
	{
		for (int k = 0; k < num_triangles; k++) {
			//printf("TRIANGLE %d p1 %g %g %g p2 %g %g %g p3 %g %g %g\n\n", k, triangle[k][0], triangle[k][1], triangle[k][2], triangle[k][3], triangle[k][4], triangle[k][5], triangle[k][6], triangle[k][7], triangle[k][8]);
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);

			altT = t.intersect(ray);

			if (altT == 0) {

			}
			else
			{
				intersect = true;
				if (altT < shortT) {
					shortT = tempT;
					c = t.color;
					normal = t.getNormal();
					Kdif = t.Kdif;
					Ks = t.Ks;
					shine = t.Shine;
					trans = t.Trans;
					indexRef = t.IndexRef;
					tipoIntersect = 2;
				}
			}
		}
	}

	// END OF REGULAR MODEL!!!
	
	/*
	// UNIFORM GRID MODEL -> Not working
	shortT = grid.hit(ray);
	printf("ShortT: %f\n", shortT);
	*/

	if (intersect)
	{
		Vec3 hitpoint = (ray.origin + ray.direction*shortT).normalize();		// Falta voltar a resolver o self-shadowing

																				// Cor come�a em preto e vamos adicionando a cor de cada objecto
																				// A primeira cor a somar � a do material que intersectou
																				// Depois enviamos raios para cada luz, se n�o houver interese��o com essa luz, ent�o fica apenas assim
																				// Se houver caminho aberto para cada luz, ent�o somamos a contribui��o de cada luz
		Vec3 LightsContribution = Vec3(0, 0, 0);

		for (int h = 0; h < num_lights; h++)
		{
			//printf("Num lights is %d\n", num_lights);
			/* Point Based Lights
			Light ls = Light(Vec3(light[h][0], light[h][1], light[h][2]), Vec3(light[h][3], light[h][4], light[h][5]));

			Vec3 L = (ls.position - hitpoint).normalize();    // Raio da luz para o hitpoint
			Vec3 V = (ray.direction).normalize()*(-1);
			Vec3 H = (L + V).normalize();
			Vec3 r = (normal * 2 * L.dot(normal) - L).normalize();

			Ray shadowRayC = Ray(hitpoint, L);
			Ray shadowRay = Ray(shadowRayC.getPoint(BIAS), L);

			if (normal.dot(L) > 0) {
			if (!rayIntersect(shadowRay)) // Nao ha interseccao com nada - shadow ray not blocked
			{
			Vec3 difuse = (ls.color*Kdif*(std::max(0.f, normal.dot(L))));
			Vec3 specular = (ls.color*Ks*pow(normal.dot(H), shine));

			//printf("DIFUSE: %f %f %f\n", difuse.x, difuse.y, difuse.z);
			//printf("SPECULAR: %f %f %f\n", specular.x, specular.y, specular.z);

			LightsContribution = LightsContribution + difuse + specular;
			}
			else // Caminho obstruido por um objeto, suposto haver sombra?
			{
			//printf("Caminho obstru�do\n");
			}
			}

			//printf("------------\n");
			//printf("KDif: %f\n", Kdif);
			//printf("Cor Luz: %f %f %f\n", ls.color.x, ls.color.y, ls.color.z);
			//printf("Contribui��o Luzes: %f %f %f\n", LightsContribution.x, LightsContribution.y, LightsContribution.z);
			//printf("Cor Anterior: %f %f %f\n", c.x, c.y, c.z);
			*/

			// Area Based Lights
			// A principal alteração é que vamos alterar um pouco o sitio que tomamos como "centro" da luz quando lançamos um raio
			// Fazemos esta alterações com valores aleatórios várias vezes e fazemos a média
			AreaLight ls = AreaLight(Vec3(light[h][0], light[h][1], light[h][2]), Vec3(light[h][3], light[h][4], light[h][5]), Vec3(1, 0, 0), Vec3(1, 0, 0));	// These two last vectors should be reviewed
			Vec3 areaLightContribution = Vec3(0, 0, 0);

			for (int a = 0; a < LIGHT_SAMPLES; a++)
			{
				float s1 = r2();	// Deslocações aleatorias entre 0 e 1
				float s2 = r2();
				// Alterando o "centro"
				Vec3 newPosition = (ls.position + ls.sideA*s1 + ls.sideB*s2).normalize();

				Vec3 L = (newPosition - hitpoint).normalize();    // Raio da luz para o hitpoint
				Vec3 V = (ray.direction).normalize()*(-1);
				Vec3 H = (L + V).normalize();
				Vec3 r = (normal * 2 * L.dot(normal) - L).normalize();

				Ray shadowRayC = Ray(hitpoint, L);
				Ray shadowRay = Ray(shadowRayC.getPoint(BIAS), L);

				if (normal.dot(L) > 0) {
					if (!rayIntersect(shadowRay)) // Nao ha interseccao com nada - shadow ray not blocked
					{
						Vec3 difuse = (ls.color*Kdif*(std::max(0.f, normal.dot(L))));
						Vec3 specular = (ls.color*Ks*pow(normal.dot(H), shine));

						//printf("DIFUSE: %f %f %f\n", difuse.x, difuse.y, difuse.z);
						//printf("SPECULAR: %f %f %f\n", specular.x, specular.y, specular.z);

						areaLightContribution = areaLightContribution + difuse + specular;
					}
					else // Caminho obstruido por um objeto, suposto haver sombra?
					{
						//printf("Caminho obstru�do\n");
					}
				}
			}

			LightsContribution = areaLightContribution / LIGHT_SAMPLES;

		}


		//printf("COR DO MATERIAL");

		c = (c * Kdif + (LightsContribution / num_lights));

		//printf("Cor Final: %f %f %f\n", c.x, c.y, c.z);


		//printf("%g %g %g \n\n ", assistantColor.x, assistantColor.y, assistantColor.z);

		if (depth >= MAX_DEPTH) {
			//printf("depth: %d\n", depth);
			return c;
		}


		//IF REFLECIVE
		if (shine > 0)
		{
			Vec3 V = (ray.direction).normalize()*(-1);

			Vec3 reflectedDirection = (normal * 2 * normal.dot(V) - V);
			Ray reflectedRayC = Ray(hitpoint, reflectedDirection);
			Ray reflectedRay = Ray(reflectedRayC.getPoint(BIAS), reflectedDirection);  //Self-shadowing?

																					   //float angle = (reflectedDirection.dot(V)) / (V.module()*reflectedDirection.module());		 // Já é o coseno

			Vec3 rColor = rayTracing(reflectedRay, depth + 1, 1);

			//Vec3 nColor = Vec3(rColor.x*Ks*pow(angle, shine), rColor.y*Ks*pow(angle, shine), rColor.z*Ks*pow(angle, shine));

			//printf("Ks: %f", Ks);
			c = c + rColor * Ks;
		};


		//IF TRANSLUCID
		if (trans > 0) {
			// Source: https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel

			// Incident Ray:
			Vec3 I = (ray.direction).normalize()*(-1);

			// cos(angle) = I.N
			float angle1 = acos(I.dot(normal));
			// Snell's law (refractionIndex/n2 = sin(angle2)/sin(angle1)
			// RefrIndex vem do meio em que estamos, indexRef do novo material
			float angle2 = asin((RefrIndex / indexRef)*sin(angle1));

			// Vectors Calculations - Check Source Link
			Vec3 N = normal;
			Vec3 C = N * cos(angle1);
			Vec3 M = (I + C) / (sin(angle1));
			Vec3 A = M * sin(angle2);
			Vec3 invN = normal * (-1);
			Vec3 B = (invN)*cos(angle2);
			Vec3 T = A + B;

			// T is the refracted Ray
			Ray refractedRayC = Ray(hitpoint, T);
			Ray refractedRay = Ray(refractedRayC.getPoint(BIAS), T);
			Vec3 rColor = rayTracing(refractedRay, depth + 1, indexRef);

			// Color impact
			c = c + rColor * (1 - Ks);

		}
	};

	return c;
};

/////////////////////////////////////////////////////////////////////// SUPERSAMPLING

Vec3 monteCarlo(double x, double y, int division)
{
	printf("NEW CALL at (%g, %g) - (%d) --------\n", x, y, division);

	// 4X4 matrix
	Vec3 mcMatrix[4];

	// get colors from each point
	Vec3 color;
	mcMatrix[0] = rayTracing(c.getTopLeft(x, y), 1, 1.0);
	mcMatrix[1] = rayTracing(c.getTopRight(x, y), 1, 1.0);
	mcMatrix[2] = rayTracing(c.getBottomLeft(x, y), 1, 1.0);
	mcMatrix[3] = rayTracing(c.getBottomRight(x, y), 1, 1.0);

	printf("MCMatrix: (%f, %f, %f), (%f, %f, %f)\n          (%f, %f, %f), (%f, %f, %f)\n", mcMatrix[0].x, mcMatrix[0].y, mcMatrix[0].z, mcMatrix[1].x, mcMatrix[1].y, mcMatrix[1].z, mcMatrix[2].x, mcMatrix[2].y, mcMatrix[2].z, mcMatrix[3].x, mcMatrix[3].y, mcMatrix[3].z);

	// if colors are within threshold
	// Question: should the diff be between all colors? Or just the ones on the left and bottom?
	Vec3 diffVec;
	float diff = 0;

	for (int j = 0; j < 3; j++)
	{
		for (int h = j + 1; h < 3; h++)
		{
			//printf("Iter\n");
			diffVec = mcMatrix[j] - mcMatrix[h];
			diff = abs(diffVec.x) + abs(diffVec.y) + abs(diffVec.z);
			printf("Diff: %f\n", diff);
			if (diff > MONTECARLO_THRESHOLD)	// As cores já são bastante diferentes, não é preciso continuar, partimos o pixel
			{
				//printf("Out First\n");
				break;
			}
		}
		if (diff > MONTECARLO_THRESHOLD)	// As cores já são bastante diferentes, não é preciso continuar, partimos o pixel
		{
			//printf("Out Second\n");
			break;
		}
	}

	if (diff < MONTECARLO_THRESHOLD)
	{
		// make average
		color.x = (mcMatrix[0].x + mcMatrix[1].x + mcMatrix[2].x + mcMatrix[3].x) / 4;
		color.y = (mcMatrix[0].y + mcMatrix[1].y + mcMatrix[2].y + mcMatrix[3].y) / 4;
		color.y = (mcMatrix[0].z + mcMatrix[1].z + mcMatrix[2].z + mcMatrix[3].z) / 4;
		printf("Devolver cor %f %f %f\n", color.x, color.y, color.z);
		return color;
	}
	else
	{
		//printf("Repartir\n");
		// Limit Recursion, return mean either way
		if (division >= MAX_DEPTH)
		{
			color.x = (mcMatrix[0].x + mcMatrix[1].x + mcMatrix[2].x + mcMatrix[3].x) / 4;
			color.y = (mcMatrix[0].y + mcMatrix[1].y + mcMatrix[2].y + mcMatrix[3].y) / 4;
			color.y = (mcMatrix[0].z + mcMatrix[1].z + mcMatrix[2].z + mcMatrix[3].z) / 4;
			return color;
		}

		// for cicle to divide the pixel in 4
		for (int i = 0; i < 4; i++)
		{
			// call monteCarlo on each division
			double newPixelX = x + (1 / pow(2, division));		// 40+1/2 -> 40+1/4 -> 40+1/8
			double newPixelY = y + (1 / pow(2, division));
			printf("New Pixels: %f, %f\n", newPixelX, newPixelY);
			color = monteCarlo(newPixelX, newPixelY, division + 1);
		}

		return color;
	}

	//Ray r = c.getPrimaryRay(x, y);
	//Vec3 color = rayTracing(r, 1, 1.0);

	return Vec3();
}

Vec3 jitter(double x, double y)
{
	Vec3 color = Vec3();
	for (int p = 0; p < JITTER_MATRIX - 1; p++)
	{
		for (int q = 0; q < JITTER_MATRIX - 1; q++)
		{
			double jit = r2();
			//printf("JIT: %g\n", jit);
			Vec3 newPixel = rayTracing(c.getPrimaryRay(x + ((p + jit) / JITTER_MATRIX), y + ((q + jit) / JITTER_MATRIX)), 1, 1.0);
			//color = color + (newPixel / (JITTER_MATRIX*JITTER_MATRIX));
			color = color + newPixel;
		}
	}
	color = color / ((JITTER_MATRIX - 1)*(JITER_MATRIX- 1));
	return color;
};

/////////////////////////////////////////////////////////////////////// ERRORS

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const GLubyte *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = gluErrorString(errCode);
		std::cerr << "OpenGL ERROR [" << errString << "]." << std::endl;
	}
	return isError;
}

void checkOpenGLError(std::string error)
{
	if (isOpenGLError()) {
		std::cerr << error << std::endl;
		//exit(EXIT_FAILURE);
	}
}
/////////////////////////////////////////////////////////////////////// SHADERs
const GLchar* VertexShader =
{
	"#version 330 core\n"

	"in vec2 in_Position;\n"
	"in vec3 in_Color;\n"
	"uniform mat4 Matrix;\n"
	"out vec4 color;\n"

	"void main(void)\n"
	"{\n"
	"	vec4 position = vec4(in_Position, 0.0, 1.0);\n"
	"	color = vec4(in_Color, 1.0);\n"
	"	gl_Position = Matrix * position;\n"

	"}\n"
};

const GLchar* FragmentShader =
{
	"#version 330 core\n"

	"in vec4 color;\n"
	"out vec4 out_Color;\n"

	"void main(void)\n"
	"{\n"
	"	out_Color = color;\n"
	"}\n"
};

void createShaderProgram()
{
	VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(VertexShaderId, 1, &VertexShader, 0);
	glCompileShader(VertexShaderId);

	FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragmentShaderId, 1, &FragmentShader, 0);
	glCompileShader(FragmentShaderId);

	ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);

	glBindAttribLocation(ProgramId, VERTEX_COORD_ATTRIB, "in_Position");
	glBindAttribLocation(ProgramId, COLOR_ATTRIB, "in_Color");

	glLinkProgram(ProgramId);
	UniformId = glGetUniformLocation(ProgramId, "Matrix");

	checkOpenGLError("ERROR: Could not create shaders.");
}

void destroyShaderProgram()
{
	glUseProgram(0);
	glDetachShader(ProgramId, VertexShaderId);
	glDetachShader(ProgramId, FragmentShaderId);

	glDeleteShader(FragmentShaderId);
	glDeleteShader(VertexShaderId);
	glDeleteProgram(ProgramId);

	checkOpenGLError("ERROR: Could not destroy shaders.");
}
/////////////////////////////////////////////////////////////////////// VAOs & VBOs
void createBufferObjects()
{
	glGenVertexArrays(1, &VaoId);
	glBindVertexArray(VaoId);
	glGenBuffers(2, VboId);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);

	/* N�o � necess�rio fazer glBufferData, ou seja o envio dos pontos para a placa gr�fica pois isso
	� feito na drawPoints em tempo de execu��o com GL_DYNAMIC_DRAW */

	glEnableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glVertexAttribPointer(VERTEX_COORD_ATTRIB, 2, GL_FLOAT, 0, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glEnableVertexAttribArray(COLOR_ATTRIB);
	glVertexAttribPointer(COLOR_ATTRIB, 3, GL_FLOAT, 0, 0, 0);

	// unbind the VAO
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);
	checkOpenGLError("ERROR: Could not create VAOs and VBOs.");
}

void destroyBufferObjects()
{
	glDisableVertexAttribArray(VERTEX_COORD_ATTRIB);
	glDisableVertexAttribArray(COLOR_ATTRIB);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, VboId);
	glDeleteVertexArrays(1, &VaoId);
	checkOpenGLError("ERROR: Could not destroy VAOs and VBOs.");
}

void drawPoints()
{
	glBindVertexArray(VaoId);
	glUseProgram(ProgramId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId[0]);
	glBufferData(GL_ARRAY_BUFFER, size_vertices, vertices, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VboId[1]);
	glBufferData(GL_ARRAY_BUFFER, size_colors, colors, GL_DYNAMIC_DRAW);

	glUniformMatrix4fv(UniformId, 1, GL_FALSE, m);
	glDrawArrays(GL_POINTS, 0, RES_X*RES_Y);
	glFinish();

	glUseProgram(0);
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	checkOpenGLError("ERROR: Could not draw scene.");
}

/////////////////////////////////////////////////////////////////////// CALLBACKS


void saveBMP(const char *filename, int x, int y, int dpi, RGBType *data) {
	FILE *f;
	int k = x * y;
	int s = 4 * k;
	int filesize = 54 + s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi * m;

	unsigned char bmpfileheader[14] = { 'B', 'M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
	unsigned char bmpinfoheader[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(x);
	bmpinfoheader[ 5] = (unsigned char)(x>>8);
	bmpinfoheader[ 6] = (unsigned char)(x>>16);
	bmpinfoheader[ 7] = (unsigned char)(x>>24);

	bmpinfoheader[ 8] = (unsigned char)(y);
	bmpinfoheader[ 9] = (unsigned char)(y >> 8);
	bmpinfoheader[ 10] = (unsigned char)(y >> 16);
	bmpinfoheader[ 11] = (unsigned char)(y >> 24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s >> 8);
	bmpinfoheader[23] = (unsigned char)(s >> 16);
	bmpinfoheader[24] = (unsigned char)(s >> 24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm >> 8);
	bmpinfoheader[27] = (unsigned char)(ppm >> 16);
	bmpinfoheader[28] = (unsigned char)(ppm >> 24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm >> 8);
	bmpinfoheader[31] = (unsigned char)(ppm >> 16);
	bmpinfoheader[32] = (unsigned char)(ppm >> 24);

	f = fopen(filename, "wb");

	fwrite(bmpfileheader, 1, 14, f);
	fwrite(bmpinfoheader, 1, 40, f);

	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];

		float red = (data[i].r) * 255;
		float green = (data[i].g) * 255;
		float blue = (data[i].b) * 255;

		unsigned char color[3] = { (int)floor(blue), (int)floor(green) , (int)floor(red) };

		fwrite(color, 1, 3, f);

	}

	fclose(f);

}


// Render function by primary ray casting from the eye towards the scene's objects

void renderScene()
{
	clock_t begin = clock();

	grid = Grid();

	int dpi = 72;

	RGBType *pixels = new RGBType[RES_Y*RES_X];
	int n = RES_X * RES_Y;

	int index_pos = 0;
	int index_col = 0;

	for (int y = 0; y < RES_Y; y++)
	{
		for (int x = 0; x < RES_X; x++)
		{
			thisone = y * RES_X + x;

			


			// ANTI ALIASING
			//Vec3 color = monteCarlo(x, y, 1);
			Vec3 color = jitter(x, y);

			//YOUR 2 FUNTIONS:
			Ray r = c.getPrimaryRay(x, y);
			//Vec3 color = rayTracing(r, 1, 1.0);

			//Ray cameraSpaceRay = Ray(c.eye, Vec3(px, py, 1.0f));

			//printf("px value is %g and py is %g \n" , px, py);

			//Vec3 color;

			int depth = 0;


			Vec3 focalVecs = c.at - c.eye;
			float focaldistance = focalVecs.module();
			//Color blend(0, 0, 0, 0);


			//float halfWidth = focaldistance * tan(c.angle / 2);

			Vec3 blend = color;
			//para nff test focusPoint = focaldistance-0.55;
			float focusPoint = focaldistance -0.65;

			int samples = 36;
			float discRadius = 1;


			//Stratified Sampling i.e. Random sampling (with 16 samples) inside each pixel to add DOF
			for (int i = 0; i < samples; i++)
			{
				//random values between [-1,1]
				float rw = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1)))); //funciona
				float rh = -1 + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1 - (-1)))); //funciona
				Vec3 P = c.eye + r.direction * focusPoint;

				//SbVec3f start = camera.getCameraPosition() - (r / 2)*u - (r / 2)*v + r * (du)*u + r * (dv)*v;

				Vec3 start = c.eye - Vec3((discRadius / 2) - discRadius + rw, (discRadius / 2) - discRadius + rh, 0.0f);

				Vec3 dir = P - start;

				Ray ray = Ray(start, dir.normalize());
				//ray = c.getPrimaryRay(x+dx, y+dy);

				//Calling the phong shader to render the scene
				//blend += phongShader(scene, ray, depth, output);
				blend = blend + rayTracing(ray, 1, 1.0);
			}

			blend = blend / samples;
			//printf("\n%g value of blend\n", blend);
			color = blend;
			//color = (color + blend)/20;


			// YOUR 2 FUNTIONS:
			//Ray r = c.getPrimaryRay(x, y);
			//Vec3 color = rayTracing(r, 1, 1.0);

			vertices[index_pos++] = (float)x;
			vertices[index_pos++] = (float)y;
			colors[index_col++] = (float)color.x;
			colors[index_col++] = (float)color.y;
			colors[index_col++] = (float)color.z;
			
			pixels[thisone].r = color.x;
			pixels[thisone].g = color.y;
			pixels[thisone].b = color.z;

			if (draw_mode == 0) {  // desenhar o conte�do da janela ponto a ponto
				drawPoints();
				index_pos = 0;
				index_col = 0;
			}
		}
		//printf("line %d\n", y);
		if (draw_mode == 1) {  // desenhar o conte�do da janela linha a linha
			drawPoints();
			index_pos = 0;
			index_col = 0;
		}
	}

	

	if (draw_mode == 2) //preenchar o conte�do da janela com uma imagem completa
		drawPoints();

	printf("Terminou!\n");
	
	saveBMP("scene.bmp", RES_X, RES_Y, dpi, pixels);

	clock_t end = clock();
	double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Tempo de execucao: %lf s\n", time_spent);
}

void cleanup()
{
	destroyShaderProgram();
	destroyBufferObjects();
}

void ortho(float left, float right, float bottom, float top,
	float nearp, float farp)
{
	m[0 * 4 + 0] = 2 / (right - left);
	m[0 * 4 + 1] = 0.0;
	m[0 * 4 + 2] = 0.0;
	m[0 * 4 + 3] = 0.0;
	m[1 * 4 + 0] = 0.0;
	m[1 * 4 + 1] = 2 / (top - bottom);
	m[1 * 4 + 2] = 0.0;
	m[1 * 4 + 3] = 0.0;
	m[2 * 4 + 0] = 0.0;
	m[2 * 4 + 1] = 0.0;
	m[2 * 4 + 2] = -2 / (farp - nearp);
	m[2 * 4 + 3] = 0.0;
	m[3 * 4 + 0] = -(right + left) / (right - left);
	m[3 * 4 + 1] = -(top + bottom) / (top - bottom);
	m[3 * 4 + 2] = -(farp + nearp) / (farp - nearp);
	m[3 * 4 + 3] = 1.0;
}

void reshape(int w, int h)
{
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, w, h);
	ortho(0, (float)RES_X, 0, (float)RES_Y, -1.0, 1.0);
}

/////////////////////////////////////////////////////////////////////// SETUP
void setupCallbacks()
{
	glutCloseFunc(cleanup);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshape);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	GLenum err_code = glGetError();
	printf("Vendor: %s\n", glGetString(GL_VENDOR));
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
}

void setupGLUT(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(3, 3);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	glutInitWindowPosition(640, 100);
	glutInitWindowSize(RES_X, RES_Y);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
	glDisable(GL_DEPTH_TEST);
	WindowHandle = glutCreateWindow(CAPTION);
	if (WindowHandle < 1) {
		std::cerr << "ERROR: Could not create a new rendering window." << std::endl;
		//exit(EXIT_FAILURE);
	}
}

// NFF Parser Helpers
void setBackground() {
	float holder[3];
	if (fscanf(nff, " %g %g %g", &holder[0], &holder[1], &holder[2]) != 0) {
		background = Vec3(holder[0], holder[1], holder[2]);
	}
}

void setF() {
	float f[3];
	if (fscanf(nff, "rom %g %g %g", &f[0], &f[1], &f[2]) != 0) {
		printf("FROM: %g %g %g\n", f[0], f[1], f[2]);
		c.setEye(Vec3(f[0], f[1], f[2]));
	}
	else {
		if (fscanf(nff, " %g %g %g %g %g %g %g %g", &latestF[0], &latestF[1], &latestF[2], &latestF[3], &latestF[4], &latestF[5], &latestF[6], &latestF[7]) != 0) {
			printf("FILL SHADE %d: %g %g %g %g %g %g %g %g\n", latestF[0], latestF[1], latestF[2], latestF[3], latestF[4], latestF[5], latestF[6], latestF[7]);
		}
	}
}

void setUp() {
	float up[3];
	if (fscanf(nff, "p %g %g %g", &up[0], &up[1], &up[2]) != 0) {
		printf("UP: %g %g %g\n", up[0], up[1], up[2]);
		c.setUp(Vec3(up[0], up[1], up[2]));
	}
}

void setA() {
	float at[3], angle;
	if (fscanf(nff, "t %g %g %g", &at[0], &at[1], &at[2]) != 0) {
		printf("AT: %g %g %g\n", at[0], at[1], at[2]);
		c.setAt(Vec3(at[0], at[1], at[2]));
	}
	else if (fscanf(nff, "ngle %g", &angle) != 0) {
		printf("ANGLE: %g\n", angle);
		c.setAngle(angle);
	}
}

void setResolution() {
	if (fscanf(nff, "esolution %d %d", &RES_X, &RES_Y) != 0) {
		printf("RESOLUTION: %d %d\n", RES_X, RES_Y);
		c.setRes(RES_X, RES_Y);
	}
}

void setSphere() {

	printf("NUM ESFERAS: %d\n", num_spheres);
	if (fscanf(nff, "%g %g %g %g", &sphere[num_spheres][0], &sphere[num_spheres][1], &sphere[num_spheres][2], &sphere[num_spheres][3]) != 0) {
		sphere[num_spheres][4] = latestF[0];
		sphere[num_spheres][5] = latestF[1];
		sphere[num_spheres][6] = latestF[2];
		sphere[num_spheres][7] = latestF[3];
		sphere[num_spheres][8] = latestF[4];
		sphere[num_spheres][9] = latestF[5];
		sphere[num_spheres][10] = latestF[6];
		sphere[num_spheres][11] = latestF[7];
		printf("SPHERE %d: %g %g %g %g  SHINE %g\n", num_spheres, sphere[num_spheres][0], sphere[num_spheres][1], sphere[num_spheres][2], sphere[num_spheres][3], sphere[num_spheres][9]);
		num_spheres++;

	}
}

void setPlane() {

	if (fscanf(nff, " 3 %g %g %g", &triangle[num_triangles][0], &triangle[num_triangles][1], &triangle[num_triangles][2]) != 0) {
		fscanf(nff, "%g %g %g", &triangle[num_triangles][3], &triangle[num_triangles][4], &triangle[num_triangles][5]);
		fscanf(nff, "%g %g %g", &triangle[num_triangles][6], &triangle[num_triangles][7], &triangle[num_triangles][8]);
		triangle[num_triangles][9] = latestF[0];
		triangle[num_triangles][10] = latestF[1];
		triangle[num_triangles][11] = latestF[2];
		triangle[num_triangles][12] = latestF[3];
		triangle[num_triangles][13] = latestF[4];
		triangle[num_triangles][14] = latestF[5];
		triangle[num_triangles][15] = latestF[6];
		triangle[num_triangles][16] = latestF[7];
		printf("FOUND TRIANGLE NUMBER - %d\n", num_triangles);
		num_triangles++;
	}

	if (fscanf(nff, "l %g %g %g %g %g %g %g %g %g", &plane[num_planes][0], &plane[num_planes][1], &plane[num_planes][2], &plane[num_planes][3], &plane[num_planes][4], &plane[num_planes][5], &plane[num_planes][6], &plane[num_planes][7], &plane[num_planes][8]) != 0) {
		plane[num_planes][9] = latestF[0];
		plane[num_planes][10] = latestF[1];
		plane[num_planes][11] = latestF[2];
		plane[num_planes][12] = latestF[3];
		plane[num_planes][13] = latestF[4];
		plane[num_planes][14] = latestF[5];
		plane[num_planes][15] = latestF[6];
		plane[num_planes][16] = latestF[7];
		printf("PLANE:\np1: %g %g %g\np2: %g %g %g\np3: %g %g %g  SHINE %g\n", plane[num_planes][0], plane[num_planes][1], plane[num_planes][2], plane[num_planes][3], plane[num_planes][4], plane[num_planes][5], plane[num_planes][6], plane[num_planes][7], plane[num_planes][8], plane[num_planes][14]);
		num_planes++;
	}

}

void setLight() {

	int i = 0;

	while (i < MAX_LIGHTS) {
		if (light[i][0] == NULL && light[i][1] == NULL && light[i][2] == NULL && light[i][3] == NULL && light[i][4] == NULL && light[i][5] == NULL) {
			break;
		}
		i++;
	}

	if (fscanf(nff, "%g %g %g %g %g %g", &light[i][0], &light[i][1], &light[i][2], &light[i][3], &light[i][4], &light[i][5]) != 0) {
		printf("LIGHT %d: %g %g %g %g %g %g\n", i, light[i][0], light[i][1], light[i][2], light[i][3], light[i][4], light[i][5]);
		num_lights++;
	}
}

void init(int argc, char* argv[])
{
	setupGLUT(argc, argv);
	setupGLEW();
	std::cerr << "CONTEXT: OpenGL v" << glGetString(GL_VERSION) << std::endl;
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	createShaderProgram();
	createBufferObjects();
	setupCallbacks();
}

int main(int argc, char* argv[])
{
	char ch;
	srand(time(NULL));

	nff = fopen("mount_low.nff", "r");
	//nff = fopen("input_file_test.nff", "r");
	//nff = fopen("mount_medium.nff", "r");
	//nff = fopen("balls_medium.nff", "r");
	if (nff == NULL) {
		return 0;
	}

	while ((ch = getc(nff)) != EOF) {
		switch (ch) {
		case ' ':
		case '\t':
		case '\n':
		case '\f':
		case '\r':
			continue;
		case '#':
			break;
			/*Comment*/
		case 'b':
			setBackground();
			printf("BACKGROUND: %f %f %f\n", background.x, background.y, background.z);
			break;
			/* Background color. */
		case 'v':
			printf("VIEWPOINT SECTION\n");
			break;
			/* Viewpoint Section */
		case 'f':
			setF();
			c.reCalc();
			break;
			/* From and F Section*/
		case 'a':
			setA();
			c.reCalc();
			break;
			/* Angle and at Section*/
		case 'u':
			setUp();
			c.reCalc();
			break;
			/* Up Vector Section*/
		case 'h':
			//setHither();
			break;
			/* Hither Section*/
		case 'r':
			setResolution();
			c.reCalc();
			break;
			/* Resolution Section*/
		case 'l':
			setLight();
			break;
			/* Light Section*/
		case 'p':
			setPlane();
			break;
			/* Plane Section*/
		case 's':
			setSphere();
			break;
			/* Sphere Section*/
		default:
			break;
			//exit(1);
		}
	}

	//c = Camera(Vec3(2.1, 1.3, 1.7),Vec3(0,0,0),Vec3(0,0,1), (double)45, RES_X, RES_Y);
	c.print();

	if (draw_mode == 0) { // desenhar o conte�do da janela ponto a ponto
		size_vertices = 2 * sizeof(float);
		size_colors = 3 * sizeof(float);
		printf("DRAWING MODE: POINT BY POINT\n");
	}
	else if (draw_mode == 1) { // desenhar o conte�do da janela linha a linha
		size_vertices = 2 * RES_X * sizeof(float);
		size_colors = 3 * RES_X * sizeof(float);
		printf("DRAWING MODE: LINE BY LINE\n");
	}
	else if (draw_mode == 2) { // preencher o conte�do da janela com uma imagem completa
		size_vertices = 2 * RES_X*RES_Y * sizeof(float);
		size_colors = 3 * RES_X*RES_Y * sizeof(float);
		printf("DRAWING MODE: FULL IMAGE\n");
	}
	else {
		printf("Draw mode not valid \n");
		exit(0);
	}
	printf("resx = %d  resy= %d.\n", RES_X, RES_Y);

	vertices = (float*)malloc(size_vertices);
	if (vertices == NULL) exit(1);

	colors = (float*)malloc(size_colors);
	if (colors == NULL) exit(1);

	init(argc, argv);
	glutMainLoop();

	exit(EXIT_SUCCESS);
}
///////////////////////////////////////////////////////////////////////