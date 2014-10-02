#include "SceneManager.h"
#include <iostream>

SceneManager::SceneManager(){
	mRoot = new SceneNode();
	bkground_color = rgb(0.1,0.1,0.1);
	setSelected(mRoot);
}

SceneManager::~SceneManager(){
	delete mRoot;
}

void SceneManager::setSelected(SceneNode* node)
{
	mSelected = node;
}

void SceneManager::selectFirstChild(SceneNode* node)
{
	if(node->mChildren.size()>0){
		mSelected = node->mChildren[0];
	}
}

void SceneManager::selectParent(SceneNode* node){
	if(node->mParent!= NULL){
		mSelected = node->mParent;
	}
}

void SceneManager::selectPrevious(SceneNode* node)
{
	if(node != mRoot && node->mParent->mChildren.size()>1){
		int index;
		if(node->parentIndex==0){index = node->mParent->mChildren.size()-1;}
		else{index = node->parentIndex-1;}
		mSelected = node->mParent->mChildren[index];
	}
}

void SceneManager::selectNext(SceneNode* node)
{
	if(node != mRoot && node->mParent->mChildren.size()>1){
		int index = (node->parentIndex+1) % node->mParent->mChildren.size();
		mSelected = node->mParent->mChildren[index];
	}
}

void SceneManager::eraseNode(SceneNode* node)
{
	if(node != mRoot){
		SceneNode* parent = node->mParent;
		//Remove from Children List
		parent->mChildren.erase(parent->mChildren.begin()+node->parentIndex);
		//Reset Parent Indices to the New Value
		for(int i=0;i<parent->mChildren.size();i++){
			parent->mChildren[i]->parentIndex = i;
		}
		delete node;
		mSelected = parent;
	}
}

void SceneManager::createChair(SceneNode* node)
{
	//Root
	SceneNode* root = new SceneNode();
	root->translate(0.0,0.6,0.0);
	node->addChild(root);
	//Back
	SceneNode* back = new SceneNode();
	back->setPrim(CUBE);
	back->scaled(-0.7,0.5,0.3);
	back->translate(-0.5,1.3,0.0);
	root->addChild(back);
	//Seat
	SceneNode* seat = new SceneNode();
	seat->setPrim(CUBE);
	seat->translate(0.0,0.7,0.0);
	seat->scaled(0.3,-0.8,0.3);
	root->addChild(seat);
	//Leg1
	SceneNode* leg1 = new SceneNode();
	leg1->setPrim(CUBE);
	leg1->scaled(-0.75,0.25,-0.75);
	leg1->translate(-0.5,0,-0.5);
	root->addChild(leg1);
	//Leg2
	SceneNode* leg2 = new SceneNode();
	leg2->setPrim(CUBE);
	leg2->scaled(-0.75,0.25,-0.75);
	leg2->translate(-0.5,0,0.5);
	root->addChild(leg2);
	//Leg3
	SceneNode* leg3 = new SceneNode();
	leg3->setPrim(CUBE);
	leg3->scaled(-0.75,0.25,-0.75);
	leg3->translate(0.5,0,-0.5);
	root->addChild(leg3);
	//Leg4
	SceneNode* leg4 = new SceneNode();
	leg4->setPrim(CUBE);
	leg4->scaled(-0.75,0.25,-0.75);
	leg4->translate(0.5,0,0.5);
	root->addChild(leg4);
	//PlaceMarker
	SceneNode* pMarker = new SceneNode();
	pMarker->setPrim(PLACE_MARKER);
	pMarker->translate(0.0,1.0,0.0);
	root->addChild(pMarker);

	mSelected = pMarker;
}

void SceneManager::createTable(SceneNode *node)
{
	//Root
		SceneNode* root = new SceneNode();
		root->translate(0.0,1.0,0.0);
		node->addChild(root);
		//Top
		SceneNode* top = new SceneNode();
		top->setPrim(CUBE);
		top->scaled(2.0,-0.6,2.0);
		top->translate(0,1.0,0);
		root->addChild(top);
		//Leg1
		SceneNode* leg1 = new SceneNode();
		leg1->setPrim(CUBE);
		leg1->scaled(-0.5,1.0,-0.5);
		leg1->translate(-1.0,0,-1.0);
		root->addChild(leg1);
		//Leg2
		SceneNode* leg2 = new SceneNode();
		leg2->setPrim(CUBE);
		leg2->scaled(-0.5,1.0,-0.5);
		leg2->translate(-1.0,0,1.0);
		root->addChild(leg2);
		//Leg3
		SceneNode* leg3 = new SceneNode();
		leg3->setPrim(CUBE);
		leg3->scaled(-0.5,1.0,-0.5);
		leg3->translate(1.0,0,-1.0);
		root->addChild(leg3);
		//Leg4
		SceneNode* leg4 = new SceneNode();
		leg4->setPrim(CUBE);
		leg4->scaled(-0.5,1.0,-0.5);
		leg4->translate(1.0,0,1.0);
		root->addChild(leg4);
		//PlaceMarker
		SceneNode* pMarker = new SceneNode();
		pMarker->setPrim(PLACE_MARKER);
		pMarker->translate(0.0,1.3,0.0);
		root->addChild(pMarker);

		mSelected = pMarker;
}

void SceneManager::createCabinet(SceneNode *node)
{
    //Root
    SceneNode* root = new SceneNode();
    node->addChild(root);
        //Main Box
        SceneNode* box = new SceneNode();
        box->setPrim(CUBE);
        box->translate(0.0,1.0,0.0);
        box->scaled(0.0,1.0,0.0);
        root->addChild(box);
        //Drawer1
        SceneNode* drawer1 = new SceneNode();
        drawer1->setPrim(CUBE);
        drawer1->translate(0.0,0.5,0.6);
        drawer1->scaled(-0.3,-0.4,-0.5);
        root->addChild(drawer1);
        //Drawer2
        SceneNode* drawer2 = new SceneNode();
        drawer2->translate(0.0,1.3,0.3);
        drawer2->scaled(-0.3,-0.4,-0.5);
        drawer2->setPrim(CUBE);
        root->addChild(drawer2);
        //PlaceMarker
		SceneNode* pMarker = new SceneNode();
		pMarker->setPrim(PLACE_MARKER);
		pMarker->translate(0.0,2.0,0.0);
		root->addChild(pMarker);

		mSelected = pMarker;
}

void SceneManager::createMeshNode(SceneNode* node, const char* filename){
	SceneNode* nNode = new SceneNode();
	nNode->setPrim(MESH);
	if(nNode->mMesh.load(filename)){
		node->addChild(nNode);
		mSelected = nNode;
	}
}

void SceneManager::setMaterial(unsigned int i, Material mat)
{
	switch(i){
		case 1:
			mat1 = mat;
			break;
		case 2:
			mat2 = mat;
			break;
		case 3:
			mat3 = mat;
			break;
		default:
			break;
	}
}

Material SceneManager::getMaterial(unsigned int i)
{
	switch(i){
		case 1:
			return mat1;
		case 2:
			return mat2;
		case 3:
			return mat3;
		default:
			return mat1;
	}
}

void SceneManager::cacheScene()
{
	mRoot->cacheNode(Matrix4::IDENTITY); //T is the stacking total transform matrix from the last node
}


