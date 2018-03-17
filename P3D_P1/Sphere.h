#include "Vec3.h"
#include "Ray.h"

#ifndef _SPHEREH_
#define _SPHEREH_

struct Sphere
{
public:
	Vec3 center;
	float radius, radius2;
	Vec3 color;

	// Construtores
	Sphere() : center(Vec3(0, 0, 0)), radius(1), color(Vec3(0.678, 1.000, 0.184)) {}; // sphere at (0,0,0) with 1, bright green
	Sphere(Vec3 center, float radius, Vec3 color) : center(center), radius(radius), radius2(radius*radius), color(color) {};

	// Methods
	bool intersect(Ray r)
	{
		Vec3 l = center - r.origin;
		float tca = l.dot(r.direction);
		if (tca < 0) return false;
		float d2 = l.dot(l) - (tca * tca);
		if (d2 > (radius2)) return false;
		float thc = sqrt((radius2) - d2);
		//t0 = tca - thc;		// Isto não está aqui só pra encher?
		//t1 = tca + thc;
		return true;
	};

};

#endif
