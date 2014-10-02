//Vec3 Stuff
#include "Mesh.h"
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>


//Vec3 Stuff
float& vec3::operator [] ( int i)
{
    return v[i];
}

vec3::vec3()
{
	v[VX]=0.0;
	v[VY]=0.0;
	v[VZ]=0.0;
}
vec3::vec3(const float x, const float y, const float z)
{
	v[VX]=x;
	v[VY]=y;
	v[VZ]=z;
}

float vec3::length()
{
	return sqrt(v[VX]*v[VX] + v[VY]*v[VY] + v[VZ]*v[VZ]);
}

vec3& vec3::normalize()
{
	float l = length();
	v[VX] /= l;
	v[VY] /= l;
	v[VZ] /= l;
	return *this;
}

vec3& vec3::yaxis_rotated(float angle)
{
	//To Radians
	float a = (angle*PI)/180.0;
	//Rotate
	v[VX] = cos(a)*v[VX]-sin(a)*v[VZ];
	v[VZ] = sin(a)*v[VX]+cos(a)*v[VZ];
	return *this;
}

vec3 vec3::get_yaxis_rotated(float angle)
{
	//To Radians
	float a = (angle*PI)/180.0;
	//Rotate
	vec3 rotated(cos(a)*v[VX]-sin(a)*v[VZ],v[VY],sin(a)*v[VX]+cos(a)*v[VZ]);
	return rotated;
}

//Operators
vec3 operator - (const vec3& a)
{  return vec3(-a.v[VX],-a.v[VY],-a.v[VZ]); }

vec3 operator + (const vec3& a, const vec3& b)
{ return vec3(a.v[VX]+ b.v[VX], a.v[VY] + b.v[VY], a.v[VZ] + b.v[VZ]); }

vec3 operator - (const vec3& a, const vec3& b)
{ return vec3(a.v[VX]-b.v[VX], a.v[VY]-b.v[VY], a.v[VZ]-b.v[VZ]); }

vec3 operator * (const vec3& a, const double d)
{ return vec3(d*a.v[VX], d*a.v[VY], d*a.v[VZ]); }

vec3 operator * (const double d, const vec3& a)
{ return a*d; }

double operator * (const vec3& a, const vec3& b)
{ return (a.v[VX]*b.v[VX] + a.v[VY]*b.v[VY] + a.v[VZ]*b.v[VZ]); }

vec3 operator / (const vec3& a, const double d)
{ double d_inv = 1.0/d; return vec3(a.v[VX]*d_inv, a.v[VY]*d_inv,
  a.v[VZ]*d_inv); }

vec3 operator ^ (const vec3& a, const vec3& b) {
    return vec3(a.v[VY]*b.v[VZ] - a.v[VZ]*b.v[VY],
		a.v[VZ]*b.v[VX] - a.v[VX]*b.v[VZ],
		a.v[VX]*b.v[VY] - a.v[VY]*b.v[VX]);
}


bool Mesh::load(const char* filename){
	mFaceList.clear();
	mVertexList.clear();
	std::ifstream inFile(filename);
	if(inFile.is_open())
	{	
		std::cout << "Loading " << filename << std::endl;
		std::string buffer;
		std::getline(inFile,buffer);
		std::vector<vec3> baseList;

		//Load Extrusion
		if(!buffer.substr(0,9).compare("extrusion")){
			std::cout << "Loading Extrusion..." << std::endl;
			float height;
			int vNum;
			std::getline(inFile,buffer);
			sscanf(buffer.c_str(),"%e",&height);
			std::getline(inFile,buffer);
			sscanf(buffer.c_str(),"%d",&vNum);
			while (!inFile.eof())
			{
				//Main Reading Loop for Vertices
				float px;
				float pz;
				std::getline(inFile,buffer);
				sscanf(buffer.c_str(),"%e %e",&px,&pz);
				vec3 vertex(px,0.0,pz);
				baseList.push_back(vertex);
			}
			//Debug
			//for each (vec3 v in baseList){
			//	std::cout << v[VX] << ", " << v[VY] << ", " << v[VZ] << std::endl;
			//}
			//std::vector<vec3> baseList(mVertexList);
			inFile.close();
			//Check if Convex
			if(isConvex(baseList)){
				std::cout << "Base is Convex." << std::endl;
				//Bottom Cap
				for(int i = 1;i<baseList.size()-1;i++){
					Triangle tri;
					tri.p1 = baseList[0];
					tri.p2 = baseList[i];
					tri.p3 = baseList[i+1];
					mFaceList.push_back(tri);
				}	
				//Top Cap
				for(int i = 1;i<baseList.size()-1;i++){
					Triangle tri;
					tri.p1 = baseList[0];
					tri.p1[VY] = height;
					tri.p3 = baseList[i];
					tri.p3[VY] = height;
					tri.p2 = baseList[i+1];
					tri.p2[VY] = height;
					mFaceList.push_back(tri);
				}
			} else {std::cout << "Base is Concave." << std::endl;}
			//Build Sides
			int j;
			for(int i=0;i<baseList.size();i++){
				if(i<baseList.size()-1){j=i+1;}
				else {j=0;}
				Triangle tri1;
				Triangle tri2;
				tri1.p1 = baseList[i];
				tri1.p2 = vec3(baseList[i][VX],height,baseList[i][VZ]);
				tri1.p3 = vec3(baseList[j][VX],height,baseList[j][VZ]);
				tri2.p1 = baseList[i];
				tri2.p2 = vec3(baseList[j][VX],height,baseList[j][VZ]);
				tri2.p3 = baseList[j];
				mFaceList.push_back(tri1);
				mFaceList.push_back(tri2);
			}
		}

		//Load SurfaceRev
		else if(!buffer.substr(0,7).compare("surfrev")){
			std::cout << "Loading Surface Revolution..." << std::endl;
			int slices;
			int pNum;
			std::getline(inFile,buffer);
			sscanf(buffer.c_str(),"%d",&slices);
			std::getline(inFile,buffer);
			sscanf(buffer.c_str(),"%d",&pNum);
			while (!inFile.eof())
			{
				//Main Reading Loop for Vertices
				float px;
				float py;
				std::getline(inFile,buffer);
				sscanf(buffer.c_str(),"%e %e",&px,&py);
				if(px<0){
					std::cout << "Contains Negative X - Coordinate: Cannot Revolve" << std::endl;
					return false;
				}
				vec3 vertex(px,py,0.0);
				//std::cout << vertex[VX] << vertex[VY] << std::endl;
				baseList.push_back(vertex);
			}

			//Clamp to the Y Axis
			if(baseList[baseList.size()-1][VX] != 0.0){
				std::cout << "Clamping Top to Y Axis" << std::endl;
				vec3 lastVertex(0.0,baseList[baseList.size()-1][VY],0.0);
				baseList.push_back(lastVertex);
			}
			if(baseList[0][VX] != 0.0){
				std::cout << "Clamping Bottom to Y Axis" << std::endl;
				vec3 lastVertex(0.0,baseList[0][VY],0.0);
				baseList.insert(baseList.begin(),lastVertex);
			}
			//Revolve
			float angle_increment = 360.0f/slices;
			float current_angle = 0.0;
			float last_angle = 0.0;
			//std::cout << angle_increment << std::endl;
			std::vector<vec3> cList(baseList); //Current Points from the "Slice"
			for(int i=0;i<slices;i++){
				last_angle = current_angle;
				current_angle += angle_increment;
				for(int j=0;j<cList.size()-1;j++){
					if(j==0){//Triangle Top
						Triangle bot;
						bot.p1 = cList[j].get_yaxis_rotated(last_angle);
						bot.p2 = cList[j+1].get_yaxis_rotated(current_angle);
						bot.p3 = cList[j].get_yaxis_rotated(current_angle);
						mFaceList.push_back(bot);
					}
					if(j==cList.size()-2){//Triangle Bottom
						Triangle bot;
						bot.p1 = cList[j].get_yaxis_rotated(last_angle);
						bot.p2 = cList[j+1].get_yaxis_rotated(current_angle);
						bot.p3 = cList[j].get_yaxis_rotated(current_angle);
						mFaceList.push_back(bot);
					}
					else {
						Triangle tri1;
						Triangle tri2;
						tri1.p1 = cList[j].get_yaxis_rotated(last_angle);
						tri1.p2 = cList[j+1].get_yaxis_rotated(last_angle);
						tri1.p3 = cList[j+1].get_yaxis_rotated(current_angle);
						tri2.p1 = cList[j].get_yaxis_rotated(last_angle);
						tri2.p2 = cList[j+1].get_yaxis_rotated(current_angle);
						tri2.p3 = cList[j].get_yaxis_rotated(current_angle);
						mFaceList.push_back(tri1);
						mFaceList.push_back(tri2);
					}
				}
			}

		}
		else {
			std::cout << "File is not in the correct format" << std::endl;
			return false;
		}
		computeFaceNormals();
		return true;

	}
	else {
		std::cout << "File Not Found or Does Not Exist" << std::endl;
	}
	return false;
}

bool Mesh::isConvex(std::vector<vec3> vList){
	vec3 f;
	vec3 s;
	bool sign;	//Initially a positive angle
	int j=0;
	int i=0;
	//Check until find Non Zero Value
	for(i=0;i<vList.size();i++){
		if(i==vList.size()-1){j=0;}
		else{j=i+1;}
		f=vList[j]-vList[i];
		if(j+1==vList.size())
			s=vList[0]-vList[j];
		else
			s=vList[j+1]-vList[j];
		float alpha = (f^s)[VY];
		if(alpha > 0.0){
			sign = true;
			break;
		}
		if(alpha < 0.0){
			sign = false;
			break;
		}
	}

	//Check Against Sign
	for(i=i;i<vList.size();i++){
		if(i==vList.size()-1){j=0;}
		else{j=i+1;}
		f=vList[j]-vList[i];
		if(j+1==vList.size())
			s=vList[0]-vList[j];
		else
			s=vList[j+1]-vList[j];
		float alpha = (f^s)[VY];
		if(alpha<0 && sign){
			return false;
		}
		if (alpha>0 && !sign){
			return false;
		}
	}
	return true;
}

void Mesh::computeFaceNormals(){
	for(int i=0;i<mFaceList.size();i++){
		Triangle* tri = &mFaceList[i];
		vec3 temp = ((tri->p2-tri->p1)^(tri->p3-tri->p1)).normalize();
		tri->norm[VX]=temp[VX];tri->norm[VY]=temp[VY];tri->norm[VZ]=temp[VZ];
	}
}

void Mesh::createVertex(float x,float y, float z){
	vec3 vertex(x,y,z);
	mVertexList.push_back(vertex);
	return;
}