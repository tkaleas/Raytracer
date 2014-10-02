//Scene Node Class

#ifndef __SceneNode_h__
#define __SceneNode_h__

#include <vector>
#include <string>
#include "Mesh.h"
#include <OgreMath.h>



enum{CUBE,SPHERE,NOTHING, PLACE_MARKER, MESH};	//Define enums for geometric primitives, and furniture place-marker

enum{RED,GREEN,BLUE,ALPHA};		//Define enums for Red Green Blue Alpha

using namespace std;
using namespace Ogre;

typedef Vector3 rgb;

class Material {
public:
	Material(); //Constructor
public:
	rgb C;		//Color of the object - combined with the diffuse color;
	float ks;	//Specular Reflectivity
	float kn;	//Specular Phong Exponent - Is this really needed?
	float kr;	//Reflectivity Coefficient
	float ka;	//Ambient Factor - Should be set to 1
	//For Transparency
	float kt;	//Transmittance
	float krefrac;	//Index of Refraction
	string toString();
};

class SceneNode {

public:
	////Transform
	
	//Translation
	float* trans;
	//Rotation
	float* rot;
	//Scale
	float* scale;

	//GeoPrimitive
	int mPrimitive;

	//Mesh (If Applicable, NULL if not in Primitive)
	Mesh mMesh;

	//Color in OpenGL - Preview Viewing Color
	float* color;

	//Material - 1,2,3 Defined by SceneManager
	unsigned int mMat;
	
	//Name
	string mName;
	
	//SceneNodes
	SceneNode* mParent;
	vector<SceneNode*> mChildren;
	//Index: Keep Track of Which Index
	//Within the parent's child list the node is in.
	int parentIndex;

	//Caching
	Matrix4 T;
	Matrix4 T_inv;
	Matrix4 T_star_inv;

//Methods
public:
	SceneNode();
	~SceneNode();
	SceneNode(SceneNode* parent, string name);
	void setIndex(int i){parentIndex=i;};
	void setPrim(int prim){mPrimitive=prim;};
	void addChild(SceneNode* node);
	void removeChild(SceneNode* node);
	void setColor(unsigned int mat);

	//Caching
	void cacheNode(Matrix4 const& t);

	//Transformation (All transforms are additive)
	void translate(float x,float y,float z);
	void scaled(float x, float y, float z);
	void rotate(float x, float y, float z);
};

#endif //#ifndef __SceneNode.h__