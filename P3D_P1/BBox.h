#include "Vec3.h"

#ifndef _BBOX_
#define _BBOX_

#define BBOX_SHIFT 0.001

struct BBox
{
public:

	Vec3 p0;
	Vec3 p1;

	BBox(Vec3 p0, Vec3 p1) : p0(p0-BBOX_SHIFT), p1(p1+BBOX_SHIFT) {};		// BBOX_SHIFT is to prevent error
	BBox(void) {};

	// Check if the bbox was hit by a ray
	bool hit(Ray r) {
		return true;
	}

	void print()
	{
		printf("BBox from (%f, %f, %f) to (%f, %f, %f)\n", p0.x, p0.y, p0.z, p1.x, p1.y, p1.z);
	}

};

#endif
#pragma once

