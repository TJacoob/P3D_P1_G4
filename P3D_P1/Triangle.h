#include "Vec3.h"

#ifndef _TRIANGLEH_
#define _TRIANGLEH_


struct Triangle
{
public:

	Vec3 point1, point2, point3;
	Vec3 color;
	float Kdif, Ks, Shine, Trans, IndexRef;		// Difuse and Specular components
	BBox bbox;

	Triangle(Vec3 p1, Vec3 p2, Vec3 p3, Vec3 c, float kdif, float ks, float shine, float trans, float indexRef) : point1(p1), point2(p2), point3(p3), color(c), Kdif(kdif), Ks(ks), Shine(shine), Trans(trans), IndexRef(indexRef) { calcBBox(); };

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

	void calcBBox()
	{
		float x0, x1, y0, y1, z0, z1;

		x0 = std::min(std::min(point1.x, point2.x), point3.x);
		x1 = std::max(std::min(point1.x, point2.x), point3.x);

		y0 = std::min(std::min(point1.y, point2.y), point3.y);
		y1 = std::max(std::min(point1.y, point2.y), point3.y);

		z0 = std::min(std::min(point1.z, point2.z), point3.z);
		z1 = std::max(std::min(point1.z, point2.z), point3.z);

		bbox = BBox(Vec3(x0, y0, z0), Vec3(x1, y1, z1));
	}

	Vec3 getNormal()	// � independente do ponto de interesec��o pq a normal � igual ao longo de todo o triangulo
	{
		Vec3 u = point2 - point1;
		Vec3 v = point3 - point1;
		return (u * v).normalize();
	}

	void print() {
		printf("TRIANGULO - Point1: (%f,%f,%f) - Point2: (%f,%f,%f) - Point3: (%f,%f,%f)\n", point1.x, point1.y, point1.z, point2.x, point2.y, point2.z, point3.x, point3.y, point3.z);
	}
};

#endif
