#ifndef _VOXELGRID_H_
#define _VOXELGRID_H_

#include "OgreMath.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

class Ray {
public:
	Vector3 p0, d;
public:
	Ray();
	Ray(Vector3 p, Vector3 v);
	Vector3 getPosition(double t); //t position on the ray
	Ray transformRay(const Ogre::Matrix4 &T,const Ogre::Matrix4 &TStar); //Transformed ray given T and TStar
	Matrix4 getTStar(Matrix4 const& T);
};

class VoxelGrid {
public:
	VoxelGrid();
	~VoxelGrid();
	static VoxelGrid* computeQGrid(VoxelGrid* src, Vector3 light, float deltaS, float kappa);
	bool load(const char* filename);		//Loads VoxelGrid file
	float getVoxelDensity(float x, float y, float z);
	float getVoxelDensity(Vector3 p);
	std::vector<float> rayMarch(float step,Ray r);
	bool inGridspace(Vector3 p);
	double RayGridIntersect(const Ray &r);

//Variables
public:
	float default_density; //Default Density of Each Voxel
	int xCount;		   //X,Y,Z total voxel count in each direction
	int yCount;
	int zCount;
	float delta;		   //Size of Each Seperate Voxel
	float ***grid;		   //Density of Voxel Grid size thing
	float max_x;
	float max_y;
	float max_z;
};

#endif //#ifndef _VOXELGRID_H_