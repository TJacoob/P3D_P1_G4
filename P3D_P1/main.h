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

// Compiler doesn't recognize the clamp function arguments, so it's defined here 
float clamp(float n, float lower, float upper) {
	return std::max(lower, std::min(n, upper));
}

#endif 

#pragma once
