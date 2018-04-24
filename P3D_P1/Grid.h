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
		nx = n.x; ny = n.y; nz = n.z;
		//printf("ns: %f %f %f\n", n.x, n.y, n.z);

		int num_cells = n.x * n.y * n.z;
		//printf("cells: %d\n", num_cells);
		int num_obj = num_spheres + num_triangles;

		cells.reserve(num_cells);			// Espaço para todos os objetos, ou para todas as cells?
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
			obj.print();		// Shows working Bounding Box
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
						//printf("Index: %d\n", index);
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
						{	// Quando já existe um objeto, adicionamos-lhe um elemento
							// Mas que não seja repetido!!
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

		// Same but for triangles
		for (int k = 0; k < num_triangles; k++) {
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
			BBox obj = t.bbox;
			obj.print();		// Shows working Bounding Box
								//s.print();
			int index;

			//printf("erro aqui: obj.p0: %f p0: %f p1: %f ny: %f\n", obj.p0.y, p0.y, p1.y, n.y);

			int ixmin = (int)clamp((obj.p0.x - p0.x) * n.x / (p1.x - p0.x), 0, n.x - 1);
			int iymin = (int)clamp((obj.p0.y - p0.y) * n.y / (p1.y - p0.y), 0, n.y - 1);
			int izmin = (int)clamp((obj.p0.z - p0.z) * n.z / (p1.z - p0.z), 0, n.z - 1);
			int ixmax = (int)clamp((obj.p1.x - p0.x) * n.x / (p1.x - p0.x), 0, n.x - 1);
			int iymax = (int)clamp((obj.p1.y - p0.y) * n.y / (p1.y - p0.y), 0, n.y - 1);
			int izmax = (int)clamp((obj.p1.z - p0.z) * n.z / (p1.z - p0.z), 0, n.z - 1);

			//printf("ixmin: %d, iymin: %d, izmin: %d\n", ixmin, iymin, izmin);
			for (int iz = izmin; iz <= izmax; iz++)
				for (int iy = iymin; iy <= iymax; iy++)
					for (int ix = ixmin; ix <= ixmax; ix++)
					{
						index = ix * n.x + iy * n.y + iz * n.z;
						//printf("Index: %d\n", index);
						//cells[index].push_back(Object(i, 0));

						if (cells[index].isEmpty())
						{	// Quando ainda nao existe nada na cell, criamos um objeto
							Object o = Object();
							o.addObject(k, 1);		// Type 1 for triangles
							cells.at(index) = o;
							//printf("Adicionada a esfera de centro (%f, %f, %f) e raio %d", s.center.x, s.center.y, s.center.z, s.radius);
							//printf(" no index: %d\n", index);
							//counts[index] += 1;
						}
						else
						{	// Quando já existe um objeto, adicionamos-lhe um elemento
							// Mas que não seja repetido!!
							if (!cells[index].alreadyExists(k))
							{
								//printf("Adicionada a esfera de centro (%f, %f, %f) e raio %d", s.center.x, s.center.y, s.center.z, s.radius);
								//printf(" no index: %d\n", index);
								cells[index].addObject(k, 1);
							}
							//counts[index] += 1;
						}
					}
		}

		// Correr cada célula para mostrar os objetos lá existentes
		for (int index = 0; index < num_cells; index++)
		{
			if (cells[index].isNotEmpty())
			{
				printf("CELL %d\n", index);
				for (int unit = 0; unit < cells[index].units; unit++)
				{
					int i = cells[index].getIndex(unit);
					if (cells[index].getType(unit) == 0)
					{
						Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
						s.print();
					}
					if (cells[index].getType(unit) == 1)
					{
						Triangle t(Vec3(triangle[i][0], triangle[i][1], triangle[i][2]), Vec3(triangle[i][3], triangle[i][4], triangle[i][5]), Vec3(triangle[i][6], triangle[i][7], triangle[i][8]), Vec3(triangle[i][9], triangle[i][10], triangle[i][11]), triangle[i][12], triangle[i][13], triangle[i][14], triangle[i][15], triangle[i][16]);
						t.print();
					}
				}
			}
			else
			{
				printf("Cell %d is empty\n", index);
			}
		};
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
		Vec3 p1 = bbox.p1;		// (Vmax) Hitpoint saída?

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

		// Correr a grelha
		Vec3 dt;
		dt.x = (tMax.x - tMin.x) / nx;
		dt.y = (tMax.y - tMin.y) / ny;
		dt.z = (tMax.z - tMin.z) / nz;

		Vec3 tNext, iStep, iStop;
		
		if (dir.x > 0)
		{
			tNext.x = tMin.x + (ix)* dt.x;
			iStep.x = +1;
			iStop.x = nx;
		}
		else
		{
			tNext.x = tMin.x + (nx - ix)* dt.x;
			iStep.x = -1;
			iStop.x = -1;
		}

		if (dir.x = 0.0)
		{
			tNext.x = 10000;
			iStep.x = -1;
			iStop.x = -1;
		}

		if (dir.y > 0)
		{
			tNext.y = tMin.y + (iy)* dt.y;
			iStep.y = +1;
			iStop.y = ny;
		}
		else
		{
			tNext.y = tMin.y + (ny - iy)* dt.y;
			iStep.y = -1;
			iStop.y = -1;
		}

		if (dir.y = 0.0)
		{
			tNext.y = 10000;
			iStep.y = -1;
			iStop.y = -1;
		}

		if (dir.z > 0)
		{
			tNext.z = tMin.z + (iz)* dt.z;
			iStep.z = +1;
			iStop.z = nz;
		}
		else
		{
			tNext.z = tMin.z + (nz - iz)* dt.z;
			iStep.z = -1;
			iStop.z = -1;
		}

		if (dir.z = 0.0)
		{
			tNext.z = 10000;
			iStep.z = -1;
			iStop.z = -1;
		}

		// Correr a grelha
		while (true)
		{
			// obter o objeto dessa posição
			float shortT = 10000, tempT;

			if (tNext.x < tNext.y && tNext.x < tNext.z)		// Avança no X
			{
				printf("tNext.x %g\n", tNext.x);

				if (shortT < tNext.x)
					return shortT;

				tNext.x += dt.x;
				ix += iStep.x;

				if (ix == iStop.x)
					return -2;
			}
			else
			{
				if (tNext.y < tNext.z)		// Avança no Y
				{
					if (not(cells[ix + nx * iy + nx * ny * iz].isEmpty()))
					{
						for (int i = 0; i < cells[ix + nx * iy + nx * ny * iz].units; i++)
						{
							Sphere s = cells[ix + nx * iy + nx * ny * iz].getSphere(i);
							tempT = s.intersect(ray);
							if (tempT < shortT)
								shortT = tempT;
						}

						if (shortT < tNext.y)
							return shortT;
					}

					tNext.y += dt.y;
					iy += iStep.y;

					if (iy == iStop.y)
						return -3;
				}
				else						// Avança no Z
				{
					if (not(cells[ix + nx * iy + nx * ny * iz].isEmpty()))
					{
						for (int i = 0; i < cells[ix + nx * iy + nx * ny * iz].units; i++)
						{
							Sphere s = cells[ix + nx * iy + nx * ny * iz].getSphere(i);
							tempT = s.intersect(ray);
							if (tempT < shortT)
								shortT = tempT;
						}

						if (shortT < tNext.z)
							return shortT;
					}

					tNext.z += dt.z;
					iz += iStep.z;

					if (iz == iStop.z)
						return -4;
				}
			}
		}

		return -5;
	}
};
	

#endif
#pragma once