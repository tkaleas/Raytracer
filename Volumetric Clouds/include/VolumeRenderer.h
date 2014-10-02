#ifndef _VOLUMERENDERER_H_
#define _VOLUMERENDERER_H_

#include "VoxelGrid.h"
#include <OgreMath.h>


typedef Vector3 rgb;

class VolumeRenderer {

public:
	//Configuration Input
	const char* output_filename;
	int resolution_x;
	int resolution_y;
	
	//Camera
	Vector3 eye_pos;
	Vector3 view_dir;
	Vector3 up_vec;
	float fov_y;
	
	//Lights
	Vector3 l_pos;
	Vector3 l_color;
	
	float step;	//Step Size for Ray March
	
	Vector3 b_color;
	
	//The Grids
	VoxelGrid* red_vgrid;
	VoxelGrid* green_vgrid;
	VoxelGrid* blue_vgrid;
	VoxelGrid* rho_vgrid;
	VoxelGrid* q_grid;
	
	float kappa;
	
	//Viewing Geometry
	Vector3 M;
	Vector3 H;
	Vector3 V;

public:
	VolumeRenderer();
	~VolumeRenderer();
	void updateViewingGeometry();
	bool load(const char* filename);
	void clear();
	void outputImage(const char* filename);
	void outputImage(const char* filename, const char* custom_output);
	rgb rayMarch(Ray r); //Color given Ray through Voxel Grid
	float getQ(Ray r); //Transmittance from Point to Light Source
};

#endif //_VOLUMERENDERER_H_