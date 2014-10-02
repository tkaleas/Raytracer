#ifndef __RayTracer_h__
#define __RayTracer_h__

#include <OgreMath.h>
#include <SceneNode.h>
#include "SceneManager.h"
#include <vector>

class Ray {
public:
	Vector3 p0, d;
public:
	Ray();
	Ray(Vector3 p, Vector3 v);
	Vector3 getPosition(double t);//t position on the ray
	Ray transformRay(Matrix4 const& T,const Ogre::Matrix4 &TStar);//Transform the Ray by Matrix T, Matrix T* for the Direction Vector;
};


class RayTracer {
public:
	RayTracer();
	RayTracer(SceneManager* sm);
public:
	//Ray Transform Code
	static Matrix4 getTStar(Matrix4 const& T);
	static Vector3 getNormal(Vector3 const& p1,Vector3 const& p2,Vector3 const& p3); //Assumes Counter Clockwise

	//Intersection Code: Assumes Object is in Object Space when Intersections are computed; 
						//Must transform back to World Space Coordinates.
	//For All:
		//Returns t value on the ray.
		//If there is an intersection

	//Intersection with Sphere of Constant Radius 1 and at the Origin.
	static double RaySphereIntersect(Ray const& r, Vector3 &n);
	//Intersection with Triangle at p1,p2,p3 with normal taken counterclockwise p1->p2->p3
		//Note: this test will only work if ray is shooting at the front: normal is outward facing.
		//Must invert n to work with rays shot at polygons from the Back
	static double RayPolyIntersect(Ray const& r, Vector3 const& p1, Vector3 const& p2, Vector3 const& p3, Vector3 &n); //Triangle(p1,p2,p3) and Normal n
	static double RayMeshIntersect(Ray const& r,Mesh const& m, Vector3 &n);
	static double RayCubeIntersect(Ray const& r, Vector3 &n);//Unit Cube.

	//RayTracer
	//Init
	void cacheObjects(SceneNode* root);
	
	//TraceRay->Lighting
	void TraceRay(Vector3 start, Vector3 direction, int depth,rgb &color, int last_obj);
	rgb ComputeBlinnPhong(Vector3 intersection, Vector3 normal, Vector3 direction,Material m);
	float DiffuseTerm(Vector3 n, Vector3 l);
	float SpecularTerm(Vector3 direction,Vector3 normal, Vector3 light,float specular_co);

	//Intersection
	bool ShadowRayUnblocked(Vector3 p1, Vector3 p2);	//Checks whether ray from p1 to p2 is unblocked.
	Vector3 RayIntersect(Ray r, int &j,Vector3 &n); //Takes in an object index to retrun to and a ray: all are NULL if empty;
	Vector3 ComputeNormal(Vector3 p, int j); //Takes in a ray: in WS and an object compute the normal from
	Vector3 ReflectedRay(Vector3 ri, Vector3 n, Vector3 p0);
	Vector3 RefractedRay(Vector3 i, Vector3 n, Vector3 p0, float eta_1, float eta_2);
	void FresnelCoefficients(float &R, float &T, float eta1, float eta2, Vector3 normal, Vector3 direction, Vector3 transmission); //Computes the Fresnel Coefficients Given all of the angles and refractive indices.

public:
	SceneManager* mSceneMgr;
	std::vector<SceneNode*> objectList;

};

#endif //__RayTracer_h__