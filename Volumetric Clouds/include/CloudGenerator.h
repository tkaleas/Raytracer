#ifndef _CLOUDGEN_H_
#define _CLOUDGEN_H_

#include <OgreMath.h>
#include "VolumeRenderer.h"
#include "VoxelGrid.h"

//4 Dimensional Cloud Generator

class CloudGenerator {
public:
	//Perlin Noise Var
	//HyperCube (4 Dimensional Grid)
	static const char gradient[32][4];
	int permutations[256];
	float persistence;
	float octave_num;

	//Voxel Grid Var
	char * fname;
	float v_size;
	int grid_size;
	int pad;
	float time;		//If Time is Static

public:
	CloudGenerator();
	~CloudGenerator();
	bool load(const char* filename);
	void fillNoise();
	int getGradient(const int i, const int j, const int k, const int l);
	float lerp(float start, float end, float state);
	float spline5(float state); //Dropoff Function
	float getNoise(const float x, const float y, const float z,const float t);
	float getNoise(Vector4 p){
		return getNoise(p.x,p.y,p.z,p.w);
	};
	float getCloudiness(Vector4 p);
	float getCloudiness(const float x, const float y, const float z,const float t){
		return getCloudiness(Vector4(x,y,z,t));
	};
	VoxelGrid* fillVoxelGrid(float voxel_size,int grid_size, float t);
	bool outputVoxelGrid(VoxelGrid* vg, const char* filename);

	bool generateCloud(const char* filename);
	bool generateCloudAnim(const char* filename,float start, float end, float interval,VolumeRenderer* vr);
};

#endif //#ifndef _CLOUDGEN_H_