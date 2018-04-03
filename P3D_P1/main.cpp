///////////////////////////////////////////////////////////////////////
//
// P3D Course
// (c) 2016 by Joï¿½o Madeiras Pereira
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

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Vec3.h"
#include "Ray.h"
#include "Camera.h"
#include "Sphere.h"
#include "Plane.h"
#include "Light.h"

#define CAPTION "ray tracer"

#define VERTEX_COORD_ATTRIB 0
#define COLOR_ATTRIB 1

#define MAX_DEPTH 6
#define MAX_SPHERES 15000
#define MAX_PLANES 5
#define MAX_LIGHTS 5

// Points defined by 2 attributes: positions which are stored in vertices array and colors which are stored in colors array
float *colors;
float *vertices;

int size_vertices;
int size_colors;

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

// Spheres Array
float sphere[MAX_SPHERES][12];
int num_spheres = 0;

// Planes Array
float plane[MAX_PLANES][17];
int num_planes = 0;

// Lights Array
float light[MAX_LIGHTS][6];	// X Y Z R G B
int num_lights = 0;

float latestF[8];

/* Draw Mode: 0 - point by point; 1 - line by line; 2 - full frame */
int draw_mode = 0;

int WindowHandle = 0;


bool rayIntersect(Ray ray) {

	float planeIntersect, sphereIntersect;

	//PLANE INTERSECTION CYCLE
	for (int j = 0; j <= num_planes; j++) {
		Plane p(Vec3(plane[j][0], plane[j][1], plane[j][2]), Vec3(plane[j][3], plane[j][4], plane[j][5]), Vec3(plane[j][6], plane[j][7], plane[j][8]), Vec3(plane[j][9], plane[j][10], plane[j][11]), plane[j][12], plane[j][13], plane[j][14], plane[j][15], plane[j][16]);

		planeIntersect = p.intersect(ray);
		if (planeIntersect != 0) {
			return true;
		}
	}

	//SPHERE INTERSECTION CYCLE
	for (int i = 0; i <= num_spheres; i++) {
		Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);

		sphereIntersect = s.intersect(ray);

		if (sphereIntersect != 0) {
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
	float tempT, shortT;
	float Kdif, Ks, shine = 0, trans = 0, indexRef = 0;
	int fs;

	bool intersect = false;

	//PLANE INTERSECTION CYCLE
	for (int j = 0; j <= num_planes; j++) {
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
		}
		else {

		}
	}

	//SPHERE INTERSECTION CYCLE
	for (int i = 0; i <= num_spheres; i++) {
		Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);

		tempT = s.intersect(ray);

		if (tempT == 0) {

		}
		else {
			intersect = true;
			if (tempT < shortT) {
				shortT = tempT;
				c = s.color;
				normal = s.getNormal(ray, tempT);
				Kdif = s.Kdif;
				Ks = s.Ks;
				shine = s.Shine;
				trans = s.Trans;
				indexRef = s.IndexRef;
			}
		}

	}

	if (intersect)
	{
		Vec3 hitpoint = (ray.origin + ray.direction*shortT).normalize();
		Vec3 assistantColor;

		for (int h = 0; h < num_lights; h++)
		{
			Light ls = Light(Vec3(light[h][0], light[h][1], light[h][2]), Vec3(light[h][3], light[h][4], light[h][5]));
			ls.print();

			// Normal vem de cima, já normalizada
			Vec3 L = (ls.position - hitpoint).normalize(); // Light Direction: Hitpoint -> Luz
			//Vec3 V = Vec3(-ray.direction.x, -ray.direction.y, -ray.direction.z);  // Viewing Direction: Hitpoint -> Olho
			Vec3 V = (ray.origin - hitpoint).normalize();
			Vec3 R = (normal* (L.dot(normal)))*2 - L; 
			Vec3 H = (L + V).normalize();
		
			Ray shadowRayC = Ray(hitpoint, L);  
			Ray shadowRay = Ray(shadowRayC.getPoint(0.001), L); // Resolve o self-shadowing

			float facingRatio = normal.dot(R);

			if (facingRatio > 0)  // Para fora da esfera, =1 é mesmo de frente
			{

				if (rayIntersect(shadowRay))		// Está a ver se intersecta algum objeto
				{
					// acontece alguma coisa, o que?
				}
				else // Atingir a luz
				{
					assistantColor = assistantColor + (ls.color*Kdif)*(normal.dot(L)) + (ls.color*Ks)*(pow(normal.dot(H), shine));
				}
			}
			
		}		

		c = c * Kdif + assistantColor;  // É suposto ser produto externo? Não está 
		
		if (depth >= MAX_DEPTH) {
			printf("depth: %d\n", depth);
			return c;
		}
	
		/*
		//IF REFLECIVE
		if (shine > 0)
		{
			normal = Vec3(-normal.x, -normal.y, -normal.z).normalize();
			Vec3 V = (ray.origin - hitpoint).normalize();		// Raio do hitpoint atÃ© ao olho

			Vec3 reflectedDirection = (normal * 2 * normal.dot(V) - V).normalize();
			Ray reflectedRay = Ray(hitpoint, reflectedDirection);

			float angle = (reflectedDirection.dot(V)) / (V.module()*reflectedDirection.module());		 // JÃ¡ Ã© o coseno

			Vec3 rColor = rayTracing(reflectedRay, depth + 1, 1);

			int nValue = 5;
			Vec3 nColor = Vec3(rColor.x*Ks*pow(angle, nValue), rColor.x*Ks*pow(angle, nValue), rColor.x*Ks*pow(angle, nValue));

			c = c - nColor;
		};

		//IF TRANSLUCID
		if (trans > 0) {
			normal = Vec3(-normal.x, -normal.y, -normal.z).normalize();
			Vec3 V = (ray.origin - hitpoint).normalize();		// Raio do hitpoint até ao olho

			Vec3 reflectedDirection = (normal * 2 * normal.dot(V) - V).normalize();
			Ray reflectedRay = Ray(hitpoint, reflectedDirection);

			float angle = acos((reflectedDirection.dot(V)) / (V.module()*reflectedDirection.module()));
			float nAngle = asin(indexRef*sin(angle));

			Vec3 vt = normal * (V.dot(normal)) - V;
			Vec3 t = (vt * (1 / vt.module())).normalize();

			Vec3 invNormal = Vec3(-normal.x, -normal.y, -normal.z);
			Vec3 refractedDirection = t * sin(nAngle) + (invNormal)*cos(nAngle);
			Ray refractedRay = Ray(hitpoint, refractedDirection);

			// get color with RayTracing
			Vec3 rColor = rayTracing(refractedRay, depth + 1, 1);

			// reduce color with transmittance coeficient
			int nValue = shine;
			Vec3 nColor = Vec3(rColor.x*trans*pow(nAngle, nValue), rColor.x*trans*pow(nAngle, nValue), rColor.x*trans*pow(nAngle, nValue));
			c = c - nColor;
		}
		*/
	};

	return c;
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

	/* Nï¿½o ï¿½ necessï¿½rio fazer glBufferData, ou seja o envio dos pontos para a placa grï¿½fica pois isso
	ï¿½ feito na drawPoints em tempo de execuï¿½ï¿½o com GL_DYNAMIC_DRAW */

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

// Render function by primary ray casting from the eye towards the scene's objects

void renderScene()
{
	int index_pos = 0;
	int index_col = 0;

	for (int y = 0; y < RES_Y; y++)
	{
		for (int x = 0; x < RES_X; x++)
		{

			//YOUR 2 FUNTIONS:
			Ray r = c.getPrimaryRay(x, y);
			Vec3 color = rayTracing(r, 1, 1.0);

			vertices[index_pos++] = (float)x;
			vertices[index_pos++] = (float)y;
			colors[index_col++] = (float)color.x;
			colors[index_col++] = (float)color.y;
			colors[index_col++] = (float)color.z;

			if (draw_mode == 0) {  // desenhar o conteï¿½do da janela ponto a ponto
				drawPoints();
				index_pos = 0;
				index_col = 0;
			}
		}
		printf("line %d\n", y);
		if (draw_mode == 1) {  // desenhar o conteï¿½do da janela linha a linha
			drawPoints();
			index_pos = 0;
			index_col = 0;
		}
	}

	if (draw_mode == 2) //preenchar o conteï¿½do da janela com uma imagem completa
		drawPoints();

	printf("Terminou!\n");
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

	while (num_spheres < MAX_SPHERES) {
		if (sphere[num_spheres][0] == NULL && sphere[num_spheres][1] == NULL && sphere[num_spheres][2] == NULL && sphere[num_spheres][3] == NULL && sphere[num_spheres][4] == NULL && sphere[num_spheres][5] == NULL && sphere[num_spheres][6] == NULL) {
			break;
		}
		num_spheres++;
	}

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
	}
}

void setPlane() {
	while (num_planes < MAX_PLANES) {
		if (plane[num_planes][0] == NULL) {
			break;
		}
		num_planes++;
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
	// Test Sphere
	//s = Sphere(Vec3( 0 , 0 , 0), 1, Vec3(0.078, 1, 0.207));

	char ch;

	nff = fopen("input_file_test.nff", "r");
	//nff = fopen("input_file_test.nff", "r");
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

	if (draw_mode == 0) { // desenhar o conteï¿½do da janela ponto a ponto
		size_vertices = 2 * sizeof(float);
		size_colors = 3 * sizeof(float);
		printf("DRAWING MODE: POINT BY POINT\n");
	}
	else if (draw_mode == 1) { // desenhar o conteï¿½do da janela linha a linha
		size_vertices = 2 * RES_X * sizeof(float);
		size_colors = 3 * RES_X * sizeof(float);
		printf("DRAWING MODE: LINE BY LINE\n");
	}
	else if (draw_mode == 2) { // preencher o conteï¿½do da janela com uma imagem completa
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