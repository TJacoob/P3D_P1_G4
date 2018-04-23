#ifndef _OBJECTH_ 
#define _OBJECTH_ 

class Object
{
public:

	int index;
	int type;  // 0 for sphere, 1 for triangle 

	Object(int index, int type) : index(index), type(type) {};

	Sphere getSphere(int i)
	{
		return Sphere(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
	};

	Triangle getTriangle(int k)
	{
		return Triangle(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
	}
};

#endif 
#pragma once
