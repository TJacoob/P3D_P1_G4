#include "Vec3.h"

#ifndef _AREALIGHTH_
#define _AREALIGHTH_

struct AreaLight
{
public:

	Vec3 position;
	Vec3 sideA, sideB;
	Vec3 color;

	AreaLight(Vec3 p, Vec3 c, Vec3 a, Vec3 b) : position(p), color(c), sideA(a), sideB(b)  {};

	void print() {
		printf("Area Light AT (%f, %f, %f)\n", position.x, position.y, position.z);
		printf("Color (%f, %f, %f)\n", color.x, color.y, color.z);
		printf("SideA: (%f, %f, %f)\n", sideA.x, sideA.y, sideA.z);
		printf("SideB: (%f, %f, %f)\n", sideB.x, sideB.y, sideB.z);
		printf("---------------------------\n");
	}
};

#endif
#pragma once
