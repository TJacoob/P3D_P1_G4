#include "Vec3.h"

#ifndef _RAYH_
#define _RAYH_

struct Ray
{
public:

	Vec3 origin;		//(x,y,z) point!
	Vec3 direction;		//(x,y,z) unit vector (normalized)

						// Construtores
	Ray() : origin(Vec3()), direction(Vec3()) {};
	Ray(Vec3 o, Vec3 d) : origin(o), direction(d) {};

	// Methods
	Vec3 getPoint(float t) const { return origin + (direction.normalize() * t); };

	void print() {
		printf("RAY AT (%f, %f, %f)\n", origin.x, origin.y, origin.z);
		printf("WITH DIRECTION (%f, %f, %f)\n", direction.x, direction.y, direction.z);
		printf("---------------------------\n");
	}

};

#endif