#include "GLUTDisplay.h"
#include <SOIL.h>
#include <iostream>
#include <fstream>
#include <sstream>


//Camera Class
Camera::Camera(){
	eye_pos = Vector3::ZERO;
	v_dir = Vector3::ZERO;
	u_vec = Vector3::UNIT_Y;
	fovy = 45.0f;
}


//Clamp Color Function

void ClampColor(rgb &color)
{
	if(color.x > 1.0)
		color.x=1.0;
	if(color.y > 1.0)
		color.y=1.0;
	if(color.z > 1.0)
		color.z=1.0;
	if(color.x < 0.0)
		color.x=0.0;
	if(color.y < 0.0)
		color.y=0.0;
	if(color.z < 0.0)
		color.z=0.0;
	return;
}
//Static Var
std::auto_ptr<GDisplay> GDisplay::s_display;

//Static Func
GDisplay* GDisplay::getInstance(){
	if(s_display.get() == NULL)
		s_display.reset(new GDisplay);
	return s_display.get();
}

GDisplay::GDisplay(){
	mSceneMgr = new SceneManager();
	//Needed for RayTracer
	mRayTracer = new RayTracer(mSceneMgr);
	mCamera = new Camera();
	RTfilename = new char[50];
}

GDisplay::~GDisplay(){

}

void GDisplay::draw(SceneNode *node)
{
	//Else Push Matrices
	glPushMatrix();
	
	//T
	glTranslated(node->trans[VX],node->trans[VY],node->trans[VZ]);
	//R
		//X
		glRotated(node->rot[VX],1.0f,0.0f,0.0f);
		//Y
		glRotated(node->rot[VY],0.0f,1.0f,0.0f);
		//Z
		glRotated(node->rot[VZ],0.0f,0.0f,1.0f);
	//S
	glScaled(node->scale[VX],node->scale[VY],node->scale[VZ]);

	//Set Color
	glColorMaterial(GL_FRONT,GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	float color[4] = {node->color[RED],node->color[GREEN],node->color[BLUE],node->color[ALPHA]};
	//If Selected brighten the Selected Node
	if(node == mSceneMgr->getSelected()){
		color[RED] += 0.2f; 
		color[GREEN] += 0.2f; 
		color[BLUE] += 0.2f;
	}
	//Set Color
	glColor3f(color[RED],
			color[GREEN],
			color[BLUE]);
	//Draw Geometry
	//If Empty Draw Teapot
	if(node->mPrimitive==NOTHING && node==mSceneMgr->getSelected()){
		glColor4f(color[RED]+0.5f,
			color[GREEN],
			color[BLUE],
			color[ALPHA]);
		glutWireTeapot(0.2f);
	}
	//Draw Cube
	else if(node->mPrimitive==CUBE){
		glutSolidCube(1.0f);
	}
	
	//Draw Mesh
	else if(node->mPrimitive == MESH){
		for(int i=0;i < node->mMesh.mFaceList.size();i++){
			//Faces
			glColor3f(color[RED],
				color[GREEN],
				color[BLUE]);
			draw(node->mMesh.mFaceList[i]);
			//Outlines
			//glColor4f(.8,.3,.8,1);
			//drawWire(node->mMesh.mFaceList[i]);
		}
	}

	//Draw Sphere
	else if(node->mPrimitive==SPHERE){
		glutSolidSphere(1.0f,20,20);
	}
	//Draw Place Marker
	else if(node->mPrimitive==PLACE_MARKER && node==mSceneMgr->getSelected()){
		glColor4f(color[RED],
			color[GREEN],
			color[BLUE]+0.5f,
			color[ALPHA]);
		glutWireSphere(0.2f,5,5);
	}

	//Draw Children
	for(int i=0; i<node->mChildren.size(); i++){
		draw(node->mChildren[i]);
	}

	//Pop Matrices
	glPopMatrix();
}

void GDisplay::draw(Triangle tri)
{
	//Faces
	glNormal3f(tri.norm[VX],tri.norm[VY],tri.norm[VZ]);
	glBegin(GL_TRIANGLES);	
		glVertex3f(tri.p1[VX],tri.p1[VY],tri.p1[VZ]);
		glVertex3f(tri.p2[VX],tri.p2[VY],tri.p2[VZ]);
		glVertex3f(tri.p3[VX],tri.p3[VY],tri.p3[VZ]);
	glEnd();
}
void GDisplay::drawWire(Triangle tri){
	//Faces
	glLineWidth(2.0);
	glBegin(GL_LINE_LOOP);	
		glVertex3f(tri.p1[VX],tri.p1[VY],tri.p1[VZ]);
		glVertex3f(tri.p2[VX],tri.p2[VY],tri.p2[VZ]);
		glVertex3f(tri.p3[VX],tri.p3[VY],tri.p3[VZ]);
	glEnd();
}

void GDisplay::draw()
{
	gluLookAt(10.0, 10.0, 8.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0);		
	//std::cout << "Drawing Scene Graph..." <<  std::endl;
	SceneNode* root = mSceneMgr->getRoot();
	//Draw Scene Graph
	draw(root);
	//Draw Ground
	glColor3f(0.2f,.3f,0.2f);
	glNormal3f(0.0,1.0,0.0);
	glBegin(GL_QUADS);	
		glVertex3f(-7.0,0.0,-7.0);
		glVertex3f(-7.0,0.0,7.0);
		glVertex3f(7.0,0.0,7.0);
		glVertex3f(7.0,0.0,-7.0);
	glEnd();
}

void GDisplay::outputRayTraceImage(const char* filename)
{
	if(loadConfig(filename)){
		//Debug Config - if Loading went Properly, check values
		cout << "Configuration Loaded." << endl;
		
		////Create Temporary "Floor"
		//SceneNode* floor = new SceneNode();
		//floor->setPrim(CUBE);
		//floor->setColor(1);
		//floor->scaled(20.0,0.0,20.0);
		//floor->translate(0.0,-5.0,0.0);
		//mSceneMgr->getRoot()->addChild(floor);
		//Create Viewing Geometry Based on New Camera
		updateViewingGeometry();
		//Cache Transformation Matrices at Scene
		mSceneMgr->cacheScene();
		//Cache Object List in RayTracer
		mRayTracer->objectList.clear();
		mRayTracer->cacheObjects(mSceneMgr->getRoot());
		//SceneNode Debug

		//Intialize Image and RayTrace
		//Each Value will be stored as a char R0-255,G0-255,B0-255 value for each
		//Image File
		unsigned char* image;
		unsigned int c = 3; //RGB value for each entry in the array
		image = new unsigned char[c*resolution_x*resolution_y];
		//Color Val
		rgb color(1.0f,1.0f,1.0f);
		unsigned char red_c;
		unsigned char green_c;
		unsigned char blue_c;
		double res_x_d = resolution_x;
		double res_y_d = resolution_y;
		double percentage_done=0.0;

		//Main RayTrace Loop
		for(int x=0;x<resolution_x;x++){
			for(int y=0;y<resolution_y;y++){
				double x_d = x;
				double y_d = resolution_y-y; //Flip the Y value to be the opposite (Bottom to Top Down Conversion)
				Vector3 P = M+((2*x_d)/(res_x_d-1)-1)*H+((2*y_d)/(res_y_d-1)-1)*V; //Pixel's Point on the Screen in World Space
				Vector3 D = (P-mCamera->eye_pos).normalisedCopy();
				Vector3 E = mCamera->eye_pos;
				mRayTracer->TraceRay(E,D,0,color,-1);
				//Clamp Colors
				ClampColor(color);
				//Convert Color To Char Color Scheme
				red_c = color[RED]*255;
				green_c = color[GREEN]*255;
				blue_c = color[BLUE]*255;
				//Apply Final Color to Pixel
				image[y*resolution_x*c+x*c]  =  red_c;
				image[y*resolution_x*c+x*c+1] = green_c;
				image[y*resolution_x*c+x*c+2] = blue_c;
			}
			percentage_done = (double(x)/res_x_d)*100;
			cout << "Percentage of Scene RayTraced: " << percentage_done << "%" << endl;
		}
		cout << endl;

		if(SOIL_save_image(RTfilename,SOIL_SAVE_TYPE_BMP,resolution_x,resolution_y,3,image)){
			cout << "Raytrace Successful" << endl;
		}

		////Destroy the Floor (Not necessary Anymore)
		//mSceneMgr->eraseNode(floor);
		return;
	}
}

bool GDisplay::loadConfig(const char* filename)
{
	std::ifstream inFile(filename);
	if(inFile.is_open())
	{
		//Temporary Variables
		std::cout << "Loading Config: " << filename << std::endl;
		int args;
		std::string buffer;

		//FILE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"FILE %s",RTfilename);
		if(args != 1){
			std::cout << "FILE is not in correct format" << std::endl;
			return false;
		}
		//RESO
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"RESO %d %d",&resolution_x,&resolution_y);
		if(args != 2){
			std::cout << "RESO is not in correct format" << std::endl;
			return false;
		}
		//EYEP
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"EYEP %e %e %e",&(mCamera->eye_pos.x),&(mCamera->eye_pos.y),&(mCamera->eye_pos.z));
		if(args != 3){
			std::cout << "EYEP is not in correct format" << std::endl;
			return false;
		}
		//VDIR
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"VDIR %e %e %e",&(mCamera->v_dir.x),(&mCamera->v_dir.y),&(mCamera->v_dir.z));
		if(args != 3){
			std::cout << "VDIR is not in correct format" << std::endl;
			return false;
		}
		//UVEC
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"UVEC %e %e %e",&(mCamera->u_vec.x),(&mCamera->u_vec.y),&(mCamera->u_vec.z));
		if(args != 3){
			std::cout << "UVEC is not in correct format" << std::endl;
			return false;
		}
		//FOVY
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"FOVY %e",&(mCamera->fovy));
		if(args != 1){
			std::cout << "FOVY is not in correct format" << std::endl;
			return false;
		}
		//LPOS
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"LPOS %e %e %e",&(mSceneMgr->light_pos.x),&(mSceneMgr->light_pos.y),&(mSceneMgr->light_pos.z));
		if(args != 3){
			std::cout << "LPOS is not in correct format" << std::endl;
			return false;
		}
		//LCOL
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"LCOL %e %e %e",&(mSceneMgr->light_color[RED]),&(mSceneMgr->light_color[GREEN]),&(mSceneMgr->light_color[BLUE]));
		if(args != 3){
			std::cout << "LCOL is not in correct format" << std::endl;
			return false;
		}
		//ACOL
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"ACOL %e %e %e",&(mSceneMgr->ambient_color[RED]),&(mSceneMgr->ambient_color[GREEN]),&(mSceneMgr->ambient_color[BLUE]));
		if(args != 3){
			std::cout << "ACOL is not in correct format" << std::endl;
			return false;
		}

		//Materials Loading
		Material tempMat;
		//MAT1
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"MAT1 %e %e %e %e %e %e %e",
			&(tempMat.C[RED]),
			&(tempMat.C[GREEN]),
			&(tempMat.C[BLUE]),
			&(tempMat.kn),
			&(tempMat.kr),
			&(tempMat.kt),
			&(tempMat.krefrac));
		if(args != 7){
			std::cout << "MAT1 is not in correct format" << std::endl;
			return false;
		}
		mSceneMgr->mat1 = tempMat;
		//MAT2
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"MAT2 %e %e %e %e %e %e %e",
			&(tempMat.C[RED]),
			&(tempMat.C[GREEN]),
			&(tempMat.C[BLUE]),
			&(tempMat.kn),
			&(tempMat.kr),
			&(tempMat.kt),
			&(tempMat.krefrac));
		if(args != 7){
			std::cout << "MAT2 is not in correct format" << std::endl;
			return false;
		}
		mSceneMgr->mat2 = tempMat;
		//MAT3
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"MAT3 %e %e %e %e %e %e %e",
			&(tempMat.C[RED]),
			&(tempMat.C[GREEN]),
			&(tempMat.C[BLUE]),
			&(tempMat.kn),
			&(tempMat.kr),
			&(tempMat.kt),
			&(tempMat.krefrac));
		if(args != 7){
			std::cout << "MAT3 is not in correct format" << std::endl;
			return false;
		}
		mSceneMgr->mat3 = tempMat;
		return true;
	} 
	else {
		std::cout << "File Does not Exist" << std::endl;
	}
	return false;
}

void GDisplay::updateViewingGeometry()
{
	cout << "Updating Viewing Geometry" << endl;
	//Eye Coord
	Vector3 eye = mCamera->eye_pos;
	//View Direction
	Vector3 C = mCamera->v_dir;
	//Up Vector
	Vector3 up = mCamera->u_vec;
	//Half Viewing Angles
	float phi = (mCamera->fovy/2.0)*PI/180.0; //Conversion to Rad
	//Need to Make |C| the focal length; Calculate by
	//float C_length = (float(resolution_y)/2.0)/tan(phi);
	//C.normalise();
	//C = C*C_length;
	//float theta= atan(float(resolution_x)/(2.0*C.length())); //In Radians
	float aspect_ratio = float(resolution_x)/float(resolution_y);
	float theta = asin(aspect_ratio*sin(phi));
	
	float phi_D = phi*(180.0/PI);
	float theta_D = theta*(180.0/PI);
	cout << "Half Angles: " << phi_D << ", "<< theta_D << endl;
	Vector3 A = C.crossProduct(up);
	Vector3 B = A.crossProduct(C);
	M = eye + C;
	H = (A*(C.length()*tan(theta)))/A.length();
	V = (B*(C.length()*tan(phi)))/B.length();
}


