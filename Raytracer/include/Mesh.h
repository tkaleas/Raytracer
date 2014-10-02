//Mesh Class

#ifndef __Mesh_h__
#define __Mesh_h__

#include <vector> 

#define PI 3.1415926536

enum{VX,VY,VZ};		//Define enums for axes, X Y Z.

//Forward Dec
class vec3;
struct Triangle;

class Mesh {
public:
	std::vector<Triangle> mFaceList;
	std::vector<vec3> mVertexList;	//Counter-Clockwise directional load

	//Functions to Create Meshes
	bool load(const char* filename);
	void createExtrusion(float height);
	void createSurfRev(int slices);
	void createVertex(float x,float y, float z);
	bool isConvex(std::vector<vec3> vList);
	void computeFaceNormals();
	void printVList();
};

class vec3 {
protected:
	float v[3];
public:
	//Constructors
	vec3();
	vec3(const float x, const float y, const float z);
	//Functions
	float length();
	vec3& normalize();
	vec3& yaxis_rotated(float angle); //In degrees -> change vector
	vec3 get_yaxis_rotated(float angle); //In degrees -> returns new vector
	//Operators
	float& operator [] ( int i);
	//Friend Operators
	friend vec3 operator - (const vec3& v);
	friend vec3 operator + (const vec3& a, const vec3& b);
	friend vec3 operator - (const vec3& a, const vec3& b);
	friend vec3 operator * (const vec3& a, const double d);	    
	friend vec3 operator * (const double d, const vec3& a);	       	    
	friend double operator * (const vec3& a, const vec3& b);  //Dot Product  
	friend vec3 operator / (const vec3& a, const double d);	    
	friend vec3 operator ^ (const vec3& a, const vec3& b);	  //Cross Product
};

struct Triangle {
	vec3 p1, p2, p3;
	vec3 norm;
};

#endif //#ifndef __SceneNode_h__