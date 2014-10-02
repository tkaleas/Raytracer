#include "SceneNode.h"
#include "RayTracer.h"
#include <iostream>
#include <sstream>

Material::Material(){
	C[RED]=0.5;
	C[BLUE]=0.5;
	C[GREEN]=0.5;;
	ks = 0.0;
	kn = 1.0;
	ka = 1.0;
	kr = 0.0;
	kt = 0.0;
	krefrac = 0.0;
}

string Material::toString(){
	std::string string;
	std::stringstream ss;
	ss << endl
		<<"\tC:" << C << endl
		<<"\tKs:" << ks << endl
		<<"\tKn:" << kn << endl
		<<"\tKr:" << kr << endl
		<<"\tKa:" << ka << endl
		<<"\tKt:" << kt << endl
		<<"\tKRefrac:" << krefrac << endl;
	return ss.str();
}

SceneNode::SceneNode(){
	//Defaults
	trans = new float[3];
	rot = new float[3];
	scale = new float[3];
	color = new float[4];
	trans[VX]=0.0;trans[VY]=0.0;trans[VZ]=0.0;
	rot[VX]=0.0;rot[VY]=0.0;rot[VZ]=0.0;
	scale[VX]=1.0f;scale[VY]=1.0f;scale[VZ]=1.0;
	mPrimitive = NOTHING;
	color[RED]=0.5f;color[GREEN]=0.5f;color[BLUE]=0.5f;color[ALPHA] = 1.0f;
	mParent = NULL;
	setIndex(-1);
	//Caching Defaults
	T = Matrix4::IDENTITY;
	T_inv = T.inverse();
	T_star_inv = T;
}

SceneNode::~SceneNode(){
	for(int i=0;i<mChildren.size();i++){
		delete mChildren[i];
	}
	mChildren.clear();
}

SceneNode::SceneNode(SceneNode* parent, string name){
	//Defaults
	//Translation
	trans = new float[3];
	rot = new float[3];
	scale = new float[3];
	color = new float[4];
	trans[VX]=0.0;trans[VY]=0.0;trans[VZ]=0.0;
	rot[VX]=0.0;rot[VY]=0.0;rot[VZ]=0.0;
	scale[VX]=1.0f;scale[VY]=1.0f;scale[VZ]=1.0;
	mPrimitive = NOTHING;
	color[RED]=0.5f;color[GREEN]=0.5f;color[BLUE]=0.5f;color[ALPHA] = 1.0f;
	mParent = parent;
	setIndex(-1);
}

void SceneNode::addChild(SceneNode *node){
	mChildren.push_back(node);
	node->mParent = this;
	node->parentIndex = mChildren.size()-1;
}

void SceneNode::removeChild(SceneNode *node){

}

void SceneNode::setColor(unsigned int mat)
{	//Set Previewing Color
	switch(mat){
		case 1:
			color[RED]=1;
			color[GREEN]=0;
			color[BLUE]=0;
			break;
		case 2:
			color[RED]=0;
			color[GREEN]=1;
			color[BLUE]=0;
			break;
		case 3:
			color[RED]=0;
			color[GREEN]=0;
			color[BLUE]=1;
			break;
		default:
			break;
	}
	//Set Material
	mMat = mat;
	for each(SceneNode* n in mChildren){
		n->setColor(mat);
	}
}

//Caching
void SceneNode::cacheNode(Matrix4 const& T)
{
	//Reset Matrices
	this->T = Matrix4::IDENTITY;
	//T_inv = T.inverse();
	//T_star = T;

	//Translate
	Vector3 tr(trans[VX],trans[VY],trans[VZ]);
	//Scale
	Vector3 s(scale[VX],scale[VY],scale[VZ]);
	//Rotate
	float rot_x  = (rot[VX]*PI)/180.0;
	float rot_y  = (rot[VY]*PI)/180.0;
	float rot_z  = (rot[VZ]*PI)/180.0;
	cout << "Rotation: " << rot_x << ", " << rot_y << ", " << rot_z << endl;
	Quaternion rx(rot_x,Vector3(1.0f,0.0f,0.0f));
	Quaternion ry(rot_y,Vector3(0.0f,1.0f,0.0f));
	Quaternion rz(rot_z,Vector3(0.0f,0.0f,1.0f));
	Quaternion r = rx*ry*rz;

	//Create Local Transformation Matrix
	Matrix4 localTrans;
	this->T.makeTransform(tr,s,r);
	
	//Update Local Cached Transformed Matrices
	this->T = T*this->T;
	T_inv = this->T.inverse(); //C^-1
	T_star_inv = RayTracer::getTStar(T_inv); //(C*)^-1s

	//Debug
	//std::cout << "NEW NODE" << endl;
	std::cout << "TMatrix-Scale: " << this->T << std::endl;
	//std::cout << "TMatrix-Translate: " << this->T << std::endl;
	//std::cout << "TMatrix Inverse-Scale: " << this->T_inv << std::endl;
	//std::cout << "TMatrix Inverse-Translate: " << this->T_inv << std::endl;
	//std::cout << "T Star Inverse: " << this->T_star_inv << std::endl;
	
	//Recurse for Children
	for each(SceneNode* n in mChildren){
		n->cacheNode(this->T);
	}
}

//Tranformation

void SceneNode::translate(float x, float y, float z){
	trans[VX] += x;
	trans[VY] += y;
	trans[VZ] += z;
}

void SceneNode::scaled(float x, float y, float z){
	scale[VX] += x;
	scale[VY] += y;
	scale[VZ] += z;
}

void SceneNode::rotate(float x, float y, float z){
	rot[VX] += x;
	rot[VY] += y;
	rot[VZ] += z;
}