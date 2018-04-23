#include "Vec3.h"

#ifndef _GRID_
#define _GRID_

#define BBOX_SHIFT 0.001

#include <algorithm>
#include <vector>

struct Grid
{
public:

	int nx, ny, nz;  //Number of cells (z?)
	BBox bbox;

	std::vector<Object> cells;
	//std::vector<int> test;
	//std::vector<std::tuple<>> cells;	// (index, type) -> type 0 for spheres, 1 for triangles
	
	//Grid(Vec3 p0, Vec3 p1) : p0(p0), p1(p1) {};
	Grid(void) { setup_cells(); };

	void setup_cells()
	{
		Vec3 p0 = get_minP();
		Vec3 p1 = get_maxP();

		int num_objects = num_spheres + num_triangles;
		int multi = 2;    // Scales the number of cells per objects (8-10)

		// Setup the grid bouding box
		bbox.p0.x = p0.x; bbox.p0.y = p0.y; bbox.p0.z = p0.z;
		bbox.p1.x = p1.x; bbox.p1.y = p1.y; bbox.p1.z = p1.z;
		//bbox.print();

		// Dimensions of the grid (tamanho de cada lado)
		Vec3 ss = p1 - p0;
		//printf("s: %g %g %g\n", ss.x, ss.y, ss.z);
		
		double s = pow((ss.x*ss.y*ss.z) / num_objects, 0.333333);
		//printf("s: %g\n", s);
		Vec3 n = ((ss*multi)/s) + 1;
		//printf("ns: %f %f %f\n", n.x, n.y, n.z);

		int num_cells = n.x * n.y * n.z;
		//printf("cells: %d\n", num_cells);
		int num_obj = num_spheres + num_triangles;

		cells.reserve(num_cells);			// Espa�o para todos os objetos, ou para todas as cells?
		for (int j = 0; j < num_cells; j++)
		{
			Object o = Object();
			cells.push_back(o);
		}

		std::vector<int> counts;
		counts.reserve(num_cells);

		for ( int i=0; i<num_cells; i++)
		{
			counts.push_back(0);
		}

		// Atribuir objetos a cada cell
		for (int i = 0; i < num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
			BBox obj = s.bbox;
			//obj.print();
			//s.print();
			int index;

			//printf("erro aqui: obj.p0: %f p0: %f p1: %f ny: %f\n", obj.p0.y, p0.y, p1.y, n.y);

			int ixmin = (int) clamp((obj.p0.x - p0.x) * n.x / (p1.x - p0.x), 0, n.x - 1);
			int iymin = (int) clamp((obj.p0.y - p0.y) * n.y / (p1.y - p0.y), 0, n.y - 1);
			int izmin = (int) clamp((obj.p0.z - p0.z) * n.z / (p1.z - p0.z), 0, n.z - 1);
			int ixmax = (int) clamp((obj.p1.x - p0.x) * n.x / (p1.x - p0.x), 0, n.x - 1);
			int iymax = (int) clamp((obj.p1.y - p0.y) * n.y / (p1.y - p0.y), 0, n.y - 1);
			int izmax = (int) clamp((obj.p1.z - p0.z) * n.z / (p1.z - p0.z), 0, n.z - 1);

			//printf("ixmin: %d, iymin: %d, izmin: %d\n", ixmin, iymin, izmin);
			for (int iz = izmin; iz <= izmax; iz++) 					
				for (int iy = iymin; iy <= iymax; iy++)					
					for (int ix = ixmin; ix <= ixmax; ix++)
					{
						index = ix * n.x + iy * n.y + iz * n.z;
						printf("Index: %d\n", index);
						//cells[index].push_back(Object(i, 0));
						
						if (cells[index].isEmpty())
						{	// Quando ainda nao existe nada na cell, criamos um objeto
							Object o = Object();
							o.addObject(i, 0);
							cells.at(index) = o;
							//printf("Adicionada a esfera de centro (%f, %f, %f) e raio %d", s.center.x, s.center.y, s.center.z, s.radius);
							//printf(" no index: %d\n", index);
							//counts[index] += 1;
						}
						else
						{	// Quando j� existe um objeto, adicionamos-lhe um elemento
							// Mas que n�o seja repetido!!
							if (!cells[index].alreadyExists(i))
							{
								//printf("Adicionada a esfera de centro (%f, %f, %f) e raio %d", s.center.x, s.center.y, s.center.z, s.radius);
								//printf(" no index: %d\n", index);
								cells[index].addObject(i, 0);
							}
							//counts[index] += 1;
						}
					}
		}

		
		

		/*
		// Repetir para os triangulos :|
		for (int k = 0; k <= num_triangles; k++) {
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
			BBox obj = t.bbox;

			int ixmin = clamp((obj.p0.x - p0.x) * nx / (p1.x - p0.x), 0, nx - 1);
			int iymin = clamp((obj.p0.y - p0.y) * ny / (p1.y - p0.y), 0, ny - 1);
			int izmin = clamp((obj.p0.z - p0.z) * nz / (p1.z - p0.z), 0, nz - 1);
			int ixmax = clamp((obj.p1.x - p0.x) * nx / (p1.x - p0.x), 0, nx - 1);
			int iymax = clamp((obj.p1.y - p0.y) * ny / (p1.y - p0.y), 0, ny - 1);
			int izmax = clamp((obj.p1.z - p0.z) * nz / (p1.z - p0.z), 0, nz - 1);

			for (int iz = izmin; iz <= izmax; iz++)
				for (int iy = iymin; iy <= iymax; iy++)
					for (int ix = ixmin; ix <= ixmax; ix++)
					{
						int index = ix + nx * iy + nx * ny * iz;
						cells[index].push_back(Object(k, 0));
					}

		}
		*/


	}

	Vec3 get_minP()
	{
		BBox obj;
		Vec3 p0 = Vec3(10000, 10000, 10000);

		// Spheres
		for (int i = 0; i <= num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
			obj = s.bbox;

			if (obj.p0.x < p0.x)
				p0.x = obj.p0.x;
			if (obj.p0.y < p0.y)
				p0.y = obj.p0.y;
			if (obj.p0.z < p0.z)
				p0.z = obj.p0.z;
		}

		// Triangles
		for (int k = 0; k <= num_triangles; k++) {
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
			obj = t.bbox;

			if (obj.p0.x < p0.x)
				p0.x = obj.p0.x;
			if (obj.p0.y < p0.y)
				p0.y = obj.p0.y;
			if (obj.p0.z < p0.z)
				p0.z = obj.p0.z;
		}

		p0.x -= BBOX_SHIFT; p0.y -= BBOX_SHIFT; p0.z -= BBOX_SHIFT;

		return p0;
	}

	Vec3 get_maxP()
	{
		BBox obj;
		Vec3 p1 = Vec3(-10000, -10000, -10000);

		// Spheres
		for (int i = 0; i <= num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
			obj = s.bbox;

			if (obj.p1.x > p1.x)
				p1.x = obj.p1.x;
			if (obj.p1.y > p1.y)
				p1.y = obj.p1.y;
			if (obj.p1.z > p1.z)
				p1.z = obj.p1.z;
		}

		// Triangles
		for (int k = 0; k <= num_triangles; k++) {
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
			obj = t.bbox;

			if (obj.p1.x > p1.x)
				p1.x = obj.p1.x;
			if (obj.p1.y > p1.y)
				p1.y = obj.p1.y;
			if (obj.p1.z > p1.z)
				p1.z = obj.p1.z;
		}

		p1.x += BBOX_SHIFT; p1.y += BBOX_SHIFT; p1.z += BBOX_SHIFT;

		return p1;
	}

	int hit(Ray ray)		// return -1 if not hit, return shortestT if hit
	{
		Vec3 ori = ray.origin;
		Vec3 dir = ray.direction;
		Vec3 p0 = bbox.p0;		// (Vmin) Hitpoint entrada?
		Vec3 p1 = bbox.p1;		// (Vmax) Hitpoint sa�da?

		Vec3 tMin; Vec3 tMax;	

		tMin = (p0 - ori)/dir;
		tMax = (p1 - ori) / dir;

		float temp0, temp1;

		// no temp0 fica o valor menor do vetor
		if (tMin.x > tMin.y) temp0 = tMin.x;
		else temp0 = tMin.y;
		if (tMin.z > temp0) temp0 = tMin.z;

		// no temp1 fica o valor maior do vetor
		if (tMin.x > tMin.y) temp1 = tMin.x;
		else temp1 = tMin.y;
		if (tMin.z > temp1) temp1 = tMin.z;

		if (temp0 > temp1)
			temp1 = temp0;

		// Coordenadas iniciais
		int ix, iy, iz;

		if (bbox.isInside(ori))
		{
			ix = clamp((ori.x - p0.x)*nx / (p1.x-p0.x), 0, nx - 1);
			iy = clamp((ori.y - p0.y)*ny / (p1.y - p0.y), 0, ny - 1);
			iz = clamp((ori.z - p0.z)*nz / (p1.z - p0.z), 0, nz - 1);
		}
		else
		{	// Se o raio estiver fora da caixa, encontrar ponto de entrada e alterar coordenadas iniciais
			Vec3 point = ori + dir *temp0;
			ix = clamp((point.x - p0.x)*nx / (p1.x - p0.x), 0, nx - 1);
			iy = clamp((point.y - p0.y)*ny / (p1.y - p0.y), 0, ny - 1);
			iz = clamp((point.z - p0.z)*nz / (p1.z - p0.z), 0, nz - 1);
		}

		return -1;
	}
};
	

#endif
#pragma once