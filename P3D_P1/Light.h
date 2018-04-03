#include "Vec3.h"

#ifndef _LIGHTH_
#define _LIGHTH_

struct Light
{
public:

	Vec3 position;
	Vec3 color;

	Light(Vec3 p, Vec3 c) : position(p), color(c) {};

	void print() {
		printf("Light AT (%f, %f, %f)\n", position.x, position.y, position.z);
		//printf("WITH DIRECTION (%f, %f, %f)\n", direction.x, direction.y, direction.z);
		printf("---------------------------\n");
	}
};

#endif
