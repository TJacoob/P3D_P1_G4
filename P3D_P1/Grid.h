#include "Vec3.h"

#ifndef _GRID_
#define _GRID_

struct Grid
{
public:

	int nx, ny, nz;  //Number of cells (z?)
	BBox bbox;
	
	//Grid(Vec3 p0, Vec3 p1) : p0(p0), p1(p1) {};

	

};

#endif
#pragma once