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
		// Source: http://www.lighthouse3d.com/tutorials/maths/ray-triangle-intersection/

		Vec3 d = ray.direction;
		Vec3 p = ray.origin;
		Vec3 e1 = point2 - point1;
		Vec3 e2 = point3 - point1;
		
		Vec3 h = d * e2;
		float a = e1.dot(h);

		if (a > -0.00001 && a < 0.00001)
			//why?
			return(0);

		float f = 1 / a;
		Vec3 s = p - point1;

		float u = f * (s.dot(h));

		if (u < 0.0 || u > 1.0)
			//why?
			return(0);

		Vec3 q = s * e1;
		float v = f * (d.dot(q));

		if (v < 0.0 || u + v > 1.0)
			//why?
			return(0);

		float shortT = f * e2.dot(q);

		if (shortT > 0.00001) // ray intersection
		{
			//printf("Hà intersceçaõ: %f\n", shortT);
			return(shortT);
		}
		else  //Não há interseção
			return(0);


	}

	Vec3 getNormal()	// � independente do ponto de interesec��o pq a normal � igual ao longo de todo o triangulo
	{
		Vec3 u = point2 - point1;
		Vec3 v = point3 - point1;
		return (u * v).normalize();
	}

};

#endif
