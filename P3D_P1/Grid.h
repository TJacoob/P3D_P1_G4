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

	std::vector<std::vector<Object>> cells;
	//std::vector<int> test;
	//std::vector<std::tuple<>> cells;	// (index, type) -> type 0 for spheres, 1 for triangles
	
	//Grid(Vec3 p0, Vec3 p1) : p0(p0), p1(p1) {};
	Grid(void) {};

	void setup_cells()
	{
		Vec3 p0 = get_minP();
		Vec3 p1 = get_maxP();

		int num_objects = num_spheres + num_triangles;
		int multi = 2;    // Scales the number of cells per objects (8-10)

		bbox.p0.x = p0.x; bbox.p0.y = p0.y; bbox.p0.z = p0.z;
		bbox.p1.x = p1.x; bbox.p1.y = p1.y; bbox.p1.z = p1.z;

		// Dimensions of the grid
		double sx = p1.x - p0.x;
		double sy = p1.y - p0.y;
		double sz = p1.z - p0.z;

		double s = pow(sx*sy*sz / num_objects, 0.333333);
		nx = multi * sx / s + 1;
		ny = multi * sy / s + 1;
		nz = multi * sz / s + 1;

		int num_cells = nx * ny* nz;
		int num_obj = num_spheres + num_triangles;

		std::vector<int> counts;

		// Atribuir objetos a cada cell
		for (int i = 0; i <= num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
			BBox obj = s.bbox;

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
						cells[index].push_back(Object(i, 0));

						/*
						if (counts[index] == 0)
						{	// Quando ainda nao existe nenhum objeto no elemento
							//cells[index][0] = Object(i, 0);
							cells[index].push_back(Object(i, 0));
							counts[index] += 1;
						}
						else
						{

						}
						*/
					}

		}
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
		Vec3 p0 = Vec3(10000, 10000, 10000);

		// Spheres
		for (int i = 0; i <= num_spheres; i++) {
			Sphere s(Vec3(sphere[i][0], sphere[i][1], sphere[i][2]), sphere[i][3], Vec3(sphere[i][4], sphere[i][5], sphere[i][6]), sphere[i][7], sphere[i][8], sphere[i][9], sphere[i][10], sphere[i][11]);
			obj = s.bbox;

			if (obj.p0.x > p0.x)
				p0.x = obj.p0.x;
			if (obj.p0.y > p0.y)
				p0.y = obj.p0.y;
			if (obj.p0.z > p0.z)
				p0.z = obj.p0.z;
		}

		// Triangles
		for (int k = 0; k <= num_triangles; k++) {
			Triangle t(Vec3(triangle[k][0], triangle[k][1], triangle[k][2]), Vec3(triangle[k][3], triangle[k][4], triangle[k][5]), Vec3(triangle[k][6], triangle[k][7], triangle[k][8]), Vec3(triangle[k][9], triangle[k][10], triangle[k][11]), triangle[k][12], triangle[k][13], triangle[k][14], triangle[k][15], triangle[k][16]);
			obj = t.bbox;

			if (obj.p0.x > p0.x)
				p0.x = obj.p0.x;
			if (obj.p0.y > p0.y)
				p0.y = obj.p0.y;
			if (obj.p0.z > p0.z)
				p0.z = obj.p0.z;
		}

		p0.x += BBOX_SHIFT; p0.y += BBOX_SHIFT; p0.z += BBOX_SHIFT;

		return p0;
	}

};
	

#endif
#pragma once