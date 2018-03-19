#include "Vec3.h"

#ifndef _PLANEH_
#define _PLANEH_

struct Plane
{
public:

	Vec3 point1, point2, point3;
	Vec3 color;
	float Kdif, Ks, Shine, Trans, IndexRef;		// Difuse and Specular components

	Plane(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 c, float kdif, float ks, float shine, float trans, float indexRef) : point1(p1), point2(p2), point3(p3), color(c), Kdif(kdif), Ks(ks), Shine(shine), Trans(trans), IndexRef(indexRef) {};

	float intersect(Ray ray) const {
		Vec3 u = point2 - point1;
		Vec3 v = point3 - point1;
		Vec3 normal = u * v;

		if (normal.dot(ray.direction) == 0) {
			return 0;
		}
		float ti = -((normal.dot(ray.origin - point1)) / normal.dot(ray.direction));

		if (ti < 0) {
			return 0;
		}
		else {
			return ti;
		}
	}

	Vec3 getNormal()	// É independente do ponto de interesecção pq a normal é igual ao longo de todo o plano
	{
		Vec3 u = point2 - point1;
		Vec3 v = point3 - point1;
		return (u * v).normalize();
	}

};

#endif
