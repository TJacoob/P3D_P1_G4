#ifndef _MAINH_ 
#define _MAINH_ 

#define MAX_SPHERES 15000
#define MAX_TRIANGLES 10000
#define MAX_PLANES 5

// Spheres Array
float sphere[MAX_SPHERES][12];
int num_spheres = 0;

// TRIANGLES Array
float triangle[MAX_TRIANGLES][17];
int num_triangles = 0;


float clamp(float v, float lo, float hi)
{
	//printf("Clamp values: %f %f %f - ", v, lo, hi);
	if (v < lo) return lo;
	if (hi < v) return hi;
	else return v;
}

#endif 

#pragma once
