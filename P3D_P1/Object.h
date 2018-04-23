#ifndef _OBJECTH_ 
#define _OBJECTH_ 

#include <vector>
#include <algorithm>

class Object
{
public:

	//int index;
	//int type;  // 0 for sphere, 1 for triangle 
	std::vector<int> index;
	std::vector<int> type;
	int units=0;

	Object() {};
	
	void addObject(int index, int type)
	{
		this->index.push_back(index);
		this->type.push_back(type);
		units += 1;
	}


	Sphere getSphere(int i)
	{
		return Sphere(Vec3(sphere[index[i]][0], sphere[index[i]][1], sphere[index[i]][2]), sphere[index[i]][3], Vec3(sphere[index[i]][4], sphere[index[i]][5], sphere[index[i]][6]), sphere[index[i]][7], sphere[index[i]][8], sphere[index[i]][9], sphere[index[i]][10], sphere[index[i]][11]);
	};

	Triangle getTriangle(int k)
	{
		return Triangle(Vec3(triangle[index[k]][0], triangle[index[k]][1], triangle[index[k]][2]), Vec3(triangle[index[k]][3], triangle[index[k]][4], triangle[index[k]][5]), Vec3(triangle[index[k]][6], triangle[index[k]][7], triangle[index[k]][8]), Vec3(triangle[index[k]][9], triangle[index[k]][10], triangle[index[k]][11]), triangle[index[k]][12], triangle[index[k]][13], triangle[index[k]][14], triangle[index[k]][15], triangle[index[k]][16]);
	}

	bool isEmpty()
	{
		return ((index.size() == 0) && (type.size() == 0));
	}

	bool alreadyExists(int i)
	{
		if (std::find(index.begin(), index.end(), i) != index.end())
			return true;
		else
			return false;
	}
};

#endif 
#pragma once
