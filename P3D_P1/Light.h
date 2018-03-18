#include "Vec3.h"

#ifndef _LIGHTH_
#define _LIGHTH_

struct Light
{
public:

	Vec3 position;
	Vec3 color;

	Light(Vec3 p, Vec3 c) : position(p), color(c) {};

};

#endif

