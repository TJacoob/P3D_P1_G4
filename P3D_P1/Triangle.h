#include "Vec3.h"

#ifndef _TRIANGLEH_
#define _TRIANGLEH_

struct Triangle
{
public:

	Vec3 point1, point2, point3;
	Vec3 color;
	float Kdif, Ks, Shine, Trans, IndexRef;		// Difuse and Specular components

	Triangle(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 c, float kdif, float ks, float shine, float trans, float indexRef) : point1(p1), point2(p2), point3(p3), color(c), Kdif(kdif), Ks(ks), Shine(shine), Trans(trans), IndexRef(indexRef) {};

	float intersect(Ray ray) const {
		
	}

	Vec3 getNormal()	// � independente do ponto de interesec��o pq a normal � igual ao longo de todo o triangulo
	{
		Vec3 u = point2 - point1;
		Vec3 v = point3 - point1;
		return (u * v).normalize();
	}

};

#endif
