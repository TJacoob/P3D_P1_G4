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
	float Kdif, Ks;		// Difuse and Specular components

	// Construtores
	Sphere() : center(Vec3(0, 0, 0)), radius(1), color(Vec3(0.678, 1.000, 0.184)), Kdif(1), Ks(1) {}; // sphere at (0,0,0) with 1, bright green
	Sphere(Vec3 center, float radius, Vec3 color, float kdif, float ks) : center(center), radius(radius), radius2(radius*radius), color(color), Kdif(kdif), Ks(ks) {};

	// Methods
	float intersect(Ray ray) const {
		Vec3 o = ray.origin;
		Vec3 d = ray.direction;
		Vec3 oc = center - o;
		float b = oc.dot(d);
		float doc2 = oc.dot(oc);

		if (doc2 > (radius*radius)) {
			if (b < 0) return false;
		}
		float c = doc2 - (radius*radius);

		float r = (b * b) - c;

		if (r < 0) return false;

		float raizR = sqrt(r);
		float t0 = b - raizR;
		float t1 = b + raizR;
		float t;

		if (doc2 >(radius * radius)) {
			t = t0;
		}
		else if (doc2 <= (radius * radius)) {
			t = t1;
		}
		else {
			t = 0;
		}
		return t;
	}

	Vec3 getNormal( Ray r, float hitpoint )	// Depende do hitpoint
	{
		float x = (r.origin.x + (r.direction.x * hitpoint) - center.x)/ radius;
		float y = (r.origin.y + (r.direction.y * hitpoint) - center.y)/ radius;
		float z = (r.origin.z + (r.direction.z * hitpoint) - center.z)/ radius;
		return Vec3(x, y, z).normalize();
	}

	void print() {
		printf("ESFERA:\n");
		printf("CENTRO EM: %f %f %f - RAIO: %f\n", center.x, center.y, center.z, radius);
	}


};

#endif
