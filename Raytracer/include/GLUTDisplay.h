//Class for interacting with OpenGL

#ifndef __GLUTDisplay_h__
#define __GLUTDisplay_h__

#include "SceneManager.h"
#include "RayTracer.h"
#include <gl/glut.h>


class Camera {

public:
	Camera();
	Vector3 eye_pos;
	Vector3 v_dir;
	Vector3 u_vec;
	float fovy;
};

class GDisplay {

private:
	GDisplay();

public:
	~GDisplay();
	static GDisplay* getInstance();
	void draw(SceneNode* node);
	void draw(Triangle tri);
	void drawWire(Triangle tri);
	Camera* getCam(){return mCamera;};
	SceneManager* getSceneManager(){return mSceneMgr;};
	
	//RayTrace
	bool loadConfig(const char* filename);
	void updateViewingGeometry();
	void outputRayTraceImage(const char* filename);
	

	void draw();

protected:
	SceneManager* mSceneMgr;
	static std::auto_ptr<GDisplay> s_display;

	
	//RayTrace Configuration
	RayTracer* mRayTracer;
	const char* RTfilename;
	unsigned int resolution_x;
	unsigned int resolution_y;
	Camera* mCamera;

	//Viewing Geometry Information, cached for faster ray production
	Vector3 M;
	Vector3 H;
	Vector3 V;
	Ray R;	//Current Ray Being Shot into the scene at given pixel coord.

};

#endif //#ifndef __GLUTDisplay.h__