#include "CloudGenerator.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>

const char CloudGenerator::gradient[32][4] = {
		{ 1, 1, 1, 0}, { 1, 1, 0, 1}, { 1, 0, 1, 1}, { 0, 1, 1, 1},
		{ 1, 1, -1, 0}, { 1, 1, 0, -1}, { 1, 0, 1, -1}, { 0, 1, 1, -1},
		{ 1, -1, 1, 0}, { 1, -1, 0, 1}, { 1, 0, -1, 1}, { 0, 1, -1, 1},
		{ 1, -1, -1, 0}, { 1, -1, 0, -1}, { 1, 0, -1, -1}, { 0, 1, -1, -1},
		{-1, 1, 1, 0}, {-1, 1, 0, 1}, {-1, 0, 1, 1}, { 0, -1, 1, 1},
		{-1, 1, -1, 0}, {-1, 1, 0, -1}, {-1, 0, 1, -1}, { 0, -1, 1, -1},
		{-1, -1, 1, 0}, {-1, -1, 0, 1}, {-1, 0, -1, 1}, { 0, -1, -1, 1},
		{-1, -1, -1, 0}, {-1, -1, 0, -1}, {-1, 0, -1, -1}, { 0, -1, -1, -1},
	};

CloudGenerator::CloudGenerator(){
	fname = new char[50];
	fillNoise();
}

CloudGenerator::~CloudGenerator(){

}

bool CloudGenerator::load(const char* filename){
	std::ifstream inFile(filename);
	if(inFile.is_open())
	{
		//Temporary Variables
		std::cout << "Loading Config: " << filename << std::endl;
		int args;
		std::string buffer;
		//char* vgrid_fname = new char[50];

		//FILE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"FILE %s", fname);
		if(args != 1){
			std::cout << "FILE is not in correct format" << std::endl;
			return false;
		}
		//PRST
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"PRST %e",&persistence);
		if(args != 1){
			std::cout << "PRST is not in correct format" << std::endl;
			return false;
		}
		//OCTSIZE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"OCTSIZE %e",&octave_num);
		if(args != 1){
			std::cout << "OCTSIZE is not in correct format" << std::endl;
			return false;
		}
		//VSIZE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"VSIZE %e",&v_size);
		if(args != 1){
			std::cout << "VSIZE is not in correct format" << std::endl;
			return false;
		}
		//GSIZE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"GSIZE %d",&grid_size);
		if(args != 1){
			std::cout << "GSIZE is not in correct format" << std::endl;
			return false;
		}
		//TIME
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"TIME %e",&time);
		if(args != 1){
			std::cout << "TIME is not in correct format" << std::endl;
			return false;
		}
		//PAD
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"PAD %d",&pad);
		if(args != 1){
			std::cout << "PAD is not in correct format" << std::endl;
			return false;
		}
		inFile.close();
		return true;
	}
	std::cout << "File Does not Exist" << std::endl;
	return false;
}

void CloudGenerator::fillNoise(){
	for(int i=0; i< 256; i++)
		permutations[i] = rand() & 0xFF;
}

int CloudGenerator::getGradient(const int i, const int j, const int k, const int l){
	return (permutations[(l+permutations[(k+permutations[(j+permutations[i & 0xFF]) & 0xFF]) & 0xFF]) & 0xFF] & 0x1F);
}

float CloudGenerator::lerp(float start, float end, float state){
	return start + state * (end-start);
}

float CloudGenerator::spline5(float state)
{
	float square = state * state;
	float cubic = square * state;
	return cubic * (6 * square - 15 * state + 10);
}

static float Prod (const float a, const char b)
{
 if (b > 0)
  return a;
 if (b < 0)
  return -a;
 return 0;
}

static float Dot_prod (const float x1, const char x2,
            const float y1, const char y2,
            const float z1, const char z2,
            const float t1, const char t2)
{
 return (Prod (x1, x2) + Prod (y1, y2) + Prod (z1, z2) + Prod (t1, t2));
}

static float LinearInterpolation (const float start,
                 const float end,
                 const float state)
{
 return start + state * (end - start);
}

float CloudGenerator::getNoise(const float x, const float y, const float z,const float t)
{
	//fillNoise();
	//Hypercube with the current point
	int x1 = (int) (x > 0 ? x : x-1);
	int y1 = (int) (y > 0 ? y : y - 1);
	int z1 = (int) (z > 0 ? z : z - 1);
	int t1 = (int) (t > 0 ? t : t - 1);
	int x2 = x1 + 1;
	int y2 = y1 + 1;
	int z2 = z1 + 1;
	int t2 = t1 + 1;

	//Gradients
	char * g0000 = (char *)gradient[getGradient(x1, y1, z1, t1)];
	char * g0001 = (char *)gradient[getGradient(x1, y1, z1, t2)];
	char * g0010 = (char *)gradient[getGradient(x1, y1, z2, t1)];
	char * g0011 = (char *)gradient[getGradient(x1, y1, z2, t2)];
	char * g0100 = (char *)gradient[getGradient(x1, y2, z1, t1)];
	char * g0101 = (char *)gradient[getGradient(x1, y2, z1, t2)];
	char * g0110 = (char *)gradient[getGradient(x1, y2, z2, t1)];
	char * g0111 = (char *)gradient[getGradient(x1, y2, z2, t2)];
	char * g1000 = (char *)gradient[getGradient(x2, y1, z1, t1)];
	char * g1001 = (char *)gradient[getGradient(x2, y1, z1, t2)];
	char * g1010 = (char *)gradient[getGradient(x2, y1, z2, t1)];
	char * g1011 = (char *)gradient[getGradient(x2, y1, z2, t2)];
	char * g1100 = (char *)gradient[getGradient(x2, y2, z1, t1)];
	char * g1101 = (char *)gradient[getGradient(x2, y2, z1, t2)];
	char * g1110 = (char *)gradient[getGradient(x2, y2, z2, t1)];
	char * g1111 = (char *)gradient[getGradient(x2, y2, z2, t2)];

	//Deltas
	float dx1 = x - x1;
	float dx2 = x - x2;
	float dy1 = y - y1;
	float dy2 = y - y2;
	float dz1 = z - z1;
	float dz2 = z - z2;
	float dt1 = t - t1;
	float dt2 = t - t2;

// The 16 dot products
 const float b0000 = Dot_prod(dx1, g0000[0], dy1, g0000[1],
                dz1, g0000[2], dt1, g0000[3]);
 const float b0001 = Dot_prod(dx1, g0001[0], dy1, g0001[1],
                dz1, g0001[2], dt2, g0001[3]);
 const float b0010 = Dot_prod(dx1, g0010[0], dy1, g0010[1],
                dz2, g0010[2], dt1, g0010[3]);
 const float b0011 = Dot_prod(dx1, g0011[0], dy1, g0011[1],
                dz2, g0011[2], dt2, g0011[3]);
 const float b0100 = Dot_prod(dx1, g0100[0], dy2, g0100[1],
                dz1, g0100[2], dt1, g0100[3]);
 const float b0101 = Dot_prod(dx1, g0101[0], dy2, g0101[1],
                dz1, g0101[2], dt2, g0101[3]);
 const float b0110 = Dot_prod(dx1, g0110[0], dy2, g0110[1],
                dz2, g0110[2], dt1, g0110[3]);
 const float b0111 = Dot_prod(dx1, g0111[0], dy2, g0111[1],
                dz2, g0111[2], dt2, g0111[3]);
 const float b1000 = Dot_prod(dx2, g1000[0], dy1, g1000[1],
                dz1, g1000[2], dt1, g1000[3]);
 const float b1001 = Dot_prod(dx2, g1001[0], dy1, g1001[1],
                dz1, g1001[2], dt2, g1001[3]);
 const float b1010 = Dot_prod(dx2, g1010[0], dy1, g1010[1],
                dz2, g1010[2], dt1, g1010[3]);
 const float b1011 = Dot_prod(dx2, g1011[0], dy1, g1011[1],
                dz2, g1011[2], dt2, g1011[3]);
 const float b1100 = Dot_prod(dx2, g1100[0], dy2, g1100[1],
                dz1, g1100[2], dt1, g1100[3]);
 const float b1101 = Dot_prod(dx2, g1101[0], dy2, g1101[1],
                dz1, g1101[2], dt2, g1101[3]);
 const float b1110 = Dot_prod(dx2, g1110[0], dy2, g1110[1],
                dz2, g1110[2], dt1, g1110[3]);
 const float b1111 = Dot_prod(dx2, g1111[0], dy2, g1111[1],
                dz2, g1111[2], dt2, g1111[3]);

	//Interp
	dx1 = spline5(dx1);
	dy1 = spline5(dy1);
	dz1 = spline5(dz1);
	dt1 = spline5(dt1);

	float b111 = lerp(b1110,b1111,dt1);
	float b110 = lerp(b1100, b1101, dt1);
	float b101 = lerp(b1110, b1111, dt1);
	float b100 = lerp(b1000, b1001, dt1);
	float b011 = lerp(b0110, b0111, dt1);
	float b010 = lerp(b0100, b0101, dt1);
	float b001 = lerp(b0010, b0011, dt1);
	float b000 = lerp(b0000, b0001, dt1);

	float b11 = lerp(b110, b111, dz1);
	float b10 = lerp(b100, b101, dz1);
	float b01 = lerp(b010, b011, dz1);
	float b00 = lerp(b000, b001, dz1);

	float b1 = lerp(b10, b11, dy1);
	float b0 = lerp(b00, b01, dy1);
	
	float b = lerp(b0,b1,dx1);
	//if(b < 0)
	//	return 0;
	return b;
}

VoxelGrid* CloudGenerator::fillVoxelGrid(float voxel_size,int grid_size, float t)
{

	std::cout << "Filling Voxel Grid.." << std::endl;

	VoxelGrid* vg = new VoxelGrid();
	vg->default_density = 0.0;
	vg->delta = voxel_size;
	vg->xCount = grid_size;
	vg->yCount = grid_size;
	vg->zCount = grid_size;

	//Fill in Voxel Values
	//Intialized Voxel 3-D array to the right size
		vg->grid = new float**[vg->xCount];
		for(int i=0;i<vg->xCount;i++){
			vg->grid[i] = new float*[vg->yCount];
			for(int j=0;j<vg->yCount;j++){
				vg->grid[i][j] = new float[vg->zCount];
			}
		}
		
		//Fill in density values for each thing in the grid
		for(int i=0;i<vg->zCount;i++){
			for(int j=0;j<vg->yCount;j++){
				for(int k=0;k<vg->xCount;k++){
					//Grid is grid[X(k)][Y(j)][Z(i)]
					if(i < pad || j < pad || k < pad || 
						i > vg->zCount-pad || j>vg->yCount-pad || k > vg->xCount-pad)
						vg->grid[k][j][i] = 0.0;
					else
						vg->grid[k][j][i] = getCloudiness(((float)k)*voxel_size,((float)j)*voxel_size,((float)i)*voxel_size,t);
				}
			}
		}

		return vg;
}

bool CloudGenerator::outputVoxelGrid(VoxelGrid* vg, const char* filename){
	std::ofstream outFile(filename);
	if(outFile.is_open())
	{
		outFile <<  vg->delta << std::endl;
		outFile << vg->xCount << " " << vg->yCount << " " << vg->zCount << std::endl;
		outFile <<  vg->default_density << std::endl;

		//Print out Voxel Values
		for(int i=0;i<vg->zCount;i++){
			for(int j=0;j<vg->yCount;j++){
				for(int k=0;k<vg->xCount;k++){
					//Grid is grid[X(k)][Y(j)][Z(i)]
					outFile << vg->grid[k][j][i] << std::endl;
				}
			}
		}
		outFile.close();
		return true;
	}
	std::cout << "File Cannot Be Created" << std::endl;
	return false;
}

bool CloudGenerator::generateCloud(const char* filename){
	if(load(filename)){
		std::cout << "Filling Voxel Grid With Cloud..." << std::endl;
		VoxelGrid* vg = fillVoxelGrid(v_size,grid_size,time);
		std::cout << "Ouputting Voxel Grid to: " << fname << std::endl;
		outputVoxelGrid(vg,fname);
		return true;
	}
	return false;
}

bool CloudGenerator::generateCloudAnim(const char* filename,float start, float end, float interval,VolumeRenderer* vr)
{ 
	if(load(filename)){
		std::cout << "Generating Cloud Animation. Might take a while..." << std::endl;
		std::string temp(filename);
		std::string pre;
		char* fstring = new char[5];
		int pos = temp.find(".");
		pre = temp.substr(0,pos);
		int frame = 1;
		for(float t = start;t<end;t+=interval){
			temp.clear();
			temp+=pre;
			temp+= itoa(frame,fstring,10); 
			temp+=".bmp";
			VoxelGrid* vg = fillVoxelGrid(v_size,grid_size,t);
			outputVoxelGrid(vg,fname);
			vr->outputImage("test.txt",temp.c_str());
			delete vg;
			vg = NULL;
			frame++;
		}
		return true;
	}
	return false;

}

float CloudGenerator::getCloudiness(Vector4 p)
{
	float t = 0.0;
	float scale = 1.0;
	Vector4 temp_p(p);
	float beta = 1/persistence;
	float frequency;
    float amplitude;
	for(int i=0;i<octave_num;i++){
		//t += fabs(getNoise(temp_p/scale) * scale);
		//scale /= beta;
		//temp_p *= beta;
		float frequency = pow(2.0f,(float)i);
        float amplitude = pow(beta,(float)i);
		t += getNoise(Vector4(p.x * frequency,p.y * frequency,p.z * frequency,p.w)) * amplitude;
	}
	if(t < 0)
		return 0.0;
	//std::cout << abs(getNoise(p)) << std::endl;
	//return abs(getNoise(p*0.2));
	return fabs(t);
}
