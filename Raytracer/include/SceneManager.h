//Scene Node Class

#ifndef __SceneManager_h__
#define __SceneManager_h__

#include "SceneNode.h"

class SceneManager {

private:
	//Scene Things
	SceneNode* mRoot;
	SceneNode* mSelected;
	
public:
	//Ray Trace Things
	rgb ambient_color;
	rgb bkground_color;
	//Materials
	Material mat1;
	Material mat2;
	Material mat3;
	////Lights
	//Only One Light for Now
	Vector3 light_pos;
	rgb light_color;


public:
	SceneManager();
	~SceneManager();
	SceneNode* getRoot(){return mRoot;}
	SceneNode* getSelected(){return mSelected;}
	void setSelected(SceneNode* node);
	//Methods for Different Scene Node Operations
	void selectFirstChild(SceneNode* node);
	void selectParent(SceneNode* node);
	void selectPrevious(SceneNode* node);
	void selectNext(SceneNode* node);
	void eraseNode(SceneNode* node);
	
	//Furniture
	void createChair(SceneNode* node);
	void createTable(SceneNode* node);
	void createCabinet(SceneNode* node);

	//Mesh
	void createMeshNode(SceneNode* node, const char* filename);

	//Materials and Lights
	void setMaterial(unsigned int i, Material mat);
	Material getMaterial(unsigned int i);

	//RayTracing
	void cacheScene(); //Caches T, T^-1, and TStar at Node
};

#endif //#ifndef __SceneManager.h__