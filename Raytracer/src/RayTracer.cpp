#include "RayTracer.h"
#include <cmath>
//Computational Globals
#define INFINITY 1000000000000000000
//Sphere Globals
#define SPHERE_PRIM_CENTER 0.0
#define PI 3.141592653
#define SPHERE_PRIM_RADIUS2 1.0
//Triangle Globals

//Cube/Bounding Box Globals
const Vector3 CUBE_VERT[8] = { Vector3(0.5,0.5,0.5),Vector3(-0.5,0.5,0.5),Vector3(-0.5,0.5,-0.5),Vector3(0.5,0.5,-0.5),
							Vector3(0.5,-0.5,0.5),Vector3(-0.5,-0.5,0.5),Vector3(-0.5,-0.5,-0.5),Vector3(0.5,-0.5,-0.5)
							};

const Vector3 CUBE_FACE_NORM[6] = {Vector3(1,0,0),Vector3(0,1,0),Vector3(0,0,1),
									Vector3(-1,0,0),Vector3(0,-1,0),Vector3(0,0,-1)
									};

const Vector3 CUBE_FACE_POINT[6]  = {Vector3(0.5,0.5,0.5),Vector3(0.5,0.5,0.5),Vector3(0.5,0.5,0.5),
									Vector3(-0.5,-0.5,-0.5),Vector3(-0.5,-0.5,-0.5),Vector3(-0.5,-0.5,-0.5)};

Ray::Ray(){
	p0=Vector3::ZERO;
	d=Vector3::ZERO;
}
Ray::Ray(Vector3 p, Vector3 v){
	p0=Vector3(p.x,p.y,p.z);
	d=Vector3(v.x,v.y,v.z);
}

Vector3 Ray::getPosition(double t)
{
	return Vector3(p0+t*d);
}

Ray Ray::transformRay(const Ogre::Matrix4 &T,const Ogre::Matrix4 &TStar)
{
	Vector3 pNew = T*this->p0;
	Vector3 vNew = TStar*this->d;
	return Ray(pNew,vNew);
}

//Constructor
RayTracer::RayTracer(SceneManager* sm){
	mSceneMgr = sm;
}

RayTracer::RayTracer(){
}

Matrix4 RayTracer::getTStar(Matrix4 const& T)
{
	Matrix4 TStar(T);
	TStar[0][3]=0.0;
	TStar[1][3]=0.0;
	TStar[2][3]=0.0;
	return TStar;
}

Vector3 RayTracer::getNormal(const Ogre::Vector3 &p1, const Ogre::Vector3 &p2, const Ogre::Vector3 &p3)
{
	Vector3 v1 = p2-p1;
	Vector3 v2 = p3-p1;
	return v1.crossProduct(v2).normalisedCopy();
}


double RayTracer::RaySphereIntersect(const Ray &r,Vector3 &n)
{	//We Assume
	//Sphere is In Center
	Vector3 d_norm = r.d.normalisedCopy();
	Ray rT(r.p0,d_norm);
	double t;
	Vector3 o_c = SPHERE_PRIM_CENTER-rT.p0;
	double l2_oc = o_c.dotProduct(o_c);
	if(l2_oc < SPHERE_PRIM_RADIUS2) { //Ray Start is in Sphere
		double t_ca = o_c.dotProduct(rT.d) / rT.d.dotProduct(rT.d);
		double l2_hc = (SPHERE_PRIM_RADIUS2 - l2_oc) / rT.d.dotProduct(rT.d) + t_ca*t_ca;
		t = t_ca + sqrt(l2_hc);
		n = r.p0 + t*rT.d;
		return t/r.d.length();
	} else {
		double t_ca = o_c.dotProduct(rT.d);
		if(t_ca < 0)
			return -1;
		float l2_hc = (SPHERE_PRIM_RADIUS2 - l2_oc) / rT.d.dotProduct(rT.d) + t_ca*t_ca;
		if(l2_hc > 0){
			t = t_ca - sqrt(l2_hc);
			n = r.p0 + t*rT.d;
			return t/r.d.length();
		}
		return -1;
	}
	return -1;
}

double RayTracer::RayPolyIntersect(const Ray &r, const Ogre::Vector3 &p1, const Ogre::Vector3 &p2, const Ogre::Vector3 &p3, Ogre::Vector3 &n)
{
	double t = (n.dotProduct(p2-r.p0))/(n.dotProduct((r.d+r.p0)-r.p0)); //t value of plane intersection
	if(t <= 0){
		return -1;
	}
	Vector3 p = r.p0 + t*r.d; // Point on Plane Intersection
	if(	(p2-p1).crossProduct(p-p1).dotProduct(n) >= 0 &&
		(p3-p2).crossProduct(p-p2).dotProduct(n) >= 0 &&
		(p1-p3).crossProduct(p-p3).dotProduct(n) >= 0){
		return t;
	}
	return -1;
}

double RayTracer::RayCubeIntersect(const Ray &r,Vector3 &n)
{
	////Simple Method
	double close = -1;
	int close_index = -1;

	for(int i=0;i<6;i++){
		Vector3 n = CUBE_FACE_NORM[i];
		Vector3 s = CUBE_FACE_POINT[i];
		double t = (n.dotProduct(s-r.p0))/(n.dotProduct(r.d)); //t value of plane intersection
		if(t<0.001){
			continue;
		}
		Vector3 p = r.p0 + t*r.d;
		//std:: cout << p << std::endl;// Point on Plane Intersection
		if(p.x <= 0.5001f && p.x >= -0.5001f &&
		   p.y <= 0.5001f && p.y >= -0.5001f &&
		   p.z <= 0.5001f && p.z >= -0.5001f && (close == -1 || t < close)){
		   //std::cout << t << std::endl;
		   close = t;
		   close_index = i;
		}
	}
	if(close > 0.0){
		//std::cout << close <<std::endl;
		n = CUBE_FACE_NORM[close_index];
		return close;
	}
	else
		return -1;



	//Ray Convex PolyHedron Intersection with Unit Cube
	//Vector3 ffi = Vector3::ZERO;	 //Furthest Front Intersection
	//Vector3 cbi = Vector3::ZERO;	 //Closest Back Intersection
	//double ffi = NULL;				 //Furthest Front Intersection
	//double cbi = NULL;				 //Closest Back Intersection
	//int ffi_index;
	//int cbi_index;
	//
	//for(int i=0;i<6;i++)
	//{
	//	Vector3 n = CUBE_FACE_NORM[i];
	//	Vector3 s = CUBE_FACE_POINT[i];
	//	double sign = r.d.dotProduct(n);
	//	if (fabs(sign) < 0.0000001){		//Ray is Parallel or on the Face
	//		if(-(r.p0-s).dotProduct(n) < 0) //Ray start is outside the face->outside polygon
	//			return -1;
	//		else							//S on Face or cannot cross it, so ignore.
	//			continue;
	//	}
	//	double t = (n.dotProduct(s-r.p0))/(n.dotProduct((r.d+r.p0)-r.p0)); //t value of plane intersection
	//	if(t>0){
	//		//Back Intersection
	//		if(sign > 0 && ( cbi == NULL || t < cbi)){
	//			cbi = t;
	//			//std::cout <<"Back:" <<cbi << std::endl;
	//			cbi_index = i;
	//		} 
	//		//Front Intersection
	//		else if(sign < 0 && (ffi == NULL || t > ffi)){
	//			ffi = t;
	//			//std::cout <<"Front:"<< ffi << std::endl;
	//			ffi_index = i;
	//		}
	//	}
	//}
	//if(ffi==NULL && cbi != NULL){
	//	n = CUBE_FACE_NORM[cbi_index];
	//	return cbi;
	//}

	//if(ffi < cbi && ffi != NULL && cbi != NULL){
	//	n = CUBE_FACE_NORM[ffi_index];
	//	return ffi;
	//}
	//return -1;
}

double RayTracer::RayMeshIntersect(const Ray &r, const Mesh &m,Vector3 &n)
{
	double t = 100000000000.0;
	double t_temp = -1.0;
	for(int i=0;i<m.mFaceList.size();i++){
		Triangle tri = m.mFaceList[i];
		Vector3 normal = Vector3(tri.norm[VX],tri.norm[VY],tri.norm[VZ]);
		Vector3 p1_ogre = Vector3(tri.p1[VX],tri.p1[VY],tri.p1[VZ]);
		Vector3 p2_ogre = Vector3(tri.p2[VX],tri.p2[VY],tri.p2[VZ]);
		Vector3 p3_ogre = Vector3(tri.p3[VX],tri.p3[VY],tri.p3[VZ]);
		t_temp = RayPolyIntersect(r,p1_ogre,p2_ogre,p3_ogre,normal);
		if(t_temp < t && t_temp > 0){
			t = t_temp;
			n = normal;
		}
	}
	if(t == 100000000000.0)
		return -1;
	return t;
}

Vector3 RayTracer::ComputeNormal(Vector3 p, int j)
{
	Vector3 normalOS;
	//Transform intersection to Object Space
	Vector3 pTransformed = objectList[j]->T_inv*p;
	Vector3 p_eps = pTransformed-Vector3(1.0,1.0,1.0);
	Vector3 p_eps_neg = pTransformed+Vector3(1.0,1.0,1.0);
	switch(objectList[j]->mPrimitive){
	//Normal is Origin->P for Sphere
	//Normal is Combination of X,Y,Z
		case CUBE:
			if(p_eps.x < 0.00001)
				normalOS = Vector3(1.0,0.0,0.0);
			if(p_eps_neg.x < 0.00001)
				normalOS = Vector3(-1.0,0.0,0.0);
			if(p_eps.y < 0.00001)
				normalOS = Vector3(0.0,1.0,0.0);
			if(p_eps_neg.y < 0.00001)
				normalOS = Vector3(0.0,-1.0,0.0);
			if(p_eps.z < 0.00001)
				normalOS = Vector3(0.0,0.0,1.0);
			if(p_eps_neg.z < 0.00001)
				normalOS = Vector3(0.0,0.0,-1.0);
			break;
		case SPHERE:
			normalOS = pTransformed;
			break;
		case MESH:
			break;
		default:
			break;
	}
	//Transform back to World Space and return Normal Vector
	Vector3 normalWS = objectList[j]->T_inv.transpose()*normalOS;
	return normalWS.normalisedCopy();
}

Vector3 RayTracer::ReflectedRay(Ogre::Vector3 ri, Ogre::Vector3 n, Vector3 p0)
{	//Flip Directional Ray
	//ri = -ri;
	Vector3 Rr = ri-2*n*(ri.dotProduct(n));
	return Rr.normalisedCopy();
}

Vector3 RayTracer::RefractedRay(Vector3 i, Vector3 n, Vector3 p0, float eta_1, float eta_2)
{
	//Crappy Convulated Way
	i.normalise();
	n.normalise();
	float eta_12 = eta_1/eta_2;
	float n_dot_i = i.dotProduct(n);
	Vector3 T(0.0,0.0,0.0);
	//Critical Angle Fix
	if(acos(-n_dot_i) > asin(eta_2/eta_1))
		return T;
	//New Cool Way
	Vector3 z = eta_12*(i-n*n_dot_i);
	if(z.length() < 1.0){
		T = z-(sqrt(1-z.length()*z.length()))*n;
	}
	return T.normalisedCopy();
}

void RayTracer::FresnelCoefficients(float &R, float &T, float eta1, float eta2, Vector3 normal, Vector3 direction, Vector3 transmission)
{

	float cos_thetai = fabsf(normal.dotProduct(direction));
	float sin_thetai = sqrtf(1-cos_thetai*cos_thetai);
	float sin_thetat = (eta1/eta2) * sin_thetai;
	if(sin_thetat * sin_thetat > 0.9999f){
		R = 1.0;
		T = 0.0;
		return;
	}
	float cos_thetat = sqrtf(1 - sin_thetat * sin_thetat);
	//Coefficient for Parallel Polarized light
	float r_perp = (eta1*cos_thetai-eta2*cos_thetat) / (eta1*cos_thetai+eta2*cos_thetat);
	//Coefficient for Perpendicular Polarized light
	float r_par =  (eta1*cos_thetat-eta2*cos_thetai) / (eta1*cos_thetat+eta2*cos_thetai);
	R = (r_par*r_par+r_perp*r_perp)/2.0;
	T = 1.0-R;
	return;
}

void RayTracer::cacheObjects(SceneNode* root){
	objectList.push_back(root);
	for each (SceneNode* n in root->mChildren)
	{
		cacheObjects(n);
	}
}

Vector3 RayTracer::RayIntersect(Ray r, int &j,Vector3 &n)
{
	j=-1;
	double t = 1000000000000000.0;
	double t_temp = -1;
	double t_ws = -1;
	Vector3 normal_temp(0.0,0.0,0.0);
	//Cycle through all Objects
	for(int i=0;i < objectList.size();i++){
		SceneNode* sn = objectList[i];
		Ray rayT = r.transformRay(sn->T_inv,sn->T_star_inv);
		switch(sn->mPrimitive){
			case CUBE:
				t_temp = RayCubeIntersect(rayT,normal_temp);
				break;
			case SPHERE:
				t_temp = RaySphereIntersect(rayT,normal_temp);
				break;
			case MESH:
				t_temp = RayMeshIntersect(rayT,sn->mMesh,normal_temp);
				break;
			default:
				break;
		}
		//If temp t < current t and is not behind the ray or outside of it.
		//We set the frontmost intersected object to this one and update t value.
		if(t_temp < t && t_temp > 0.0){
			t = t_temp;
			n = sn->T_inv.transpose()*normal_temp;
			j = i; 
		}
	}
	return r.getPosition(t);
}

rgb RayTracer::ComputeBlinnPhong(Vector3 intersection, Vector3 normal, Vector3 direction,Material m)
{
	Vector3 Lpos = mSceneMgr->light_pos;
	Vector3 L = (Lpos-intersection).normalisedCopy();
	Vector3 V = -direction.normalisedCopy();
	normal.normalise();
	rgb l_color = mSceneMgr->light_color;
	rgb a_color = mSceneMgr->ambient_color;
	rgb lit_color(0.0,0.0,0.0);
	//Retrieve Outward Facing Normal
	if(normal.dotProduct(V) < 0 && normal.dotProduct(L) < 0)
		normal = -normal;
	if(ShadowRayUnblocked(intersection+normal*0.05,Lpos)){
		lit_color += m.C * l_color * DiffuseTerm(normal,L);
		lit_color += l_color * SpecularTerm(V,normal,L,m.kn);
	}
	lit_color += m.C * a_color;

	return lit_color;
}

float RayTracer::DiffuseTerm(Vector3 n, Vector3 l)
{
	float dt = l.dotProduct(n);
	if(dt<0){
		return 0;
	}
	return dt;
}

float RayTracer::SpecularTerm(Vector3 direction,Vector3 normal, Vector3 light,float specular_co)
{
	//Blinn-Phong
	Vector3 H = (light+direction).normalisedCopy();
	Vector3 N = normal;
	float base = N.dotProduct(H);
	float specular_term = pow(base,specular_co);
	return specular_term;
	//Phong
	//Vector3 R = 2*N*(light.dotProduct(N))-light;
	//specular_term = R.dotProduct(direction);
	//return pow(specular_term,specular_co);
}

bool RayTracer::ShadowRayUnblocked(Vector3 p1, Vector3 p2)
{
	Vector3 direction = p2-p1;
	Ray r(p1,direction);
	Vector3 normal_dummy;
	double t = -1;
	double t_temp = -1;
	for(int i=0;i < objectList.size();i++){
		SceneNode* sn = objectList[i];
		Ray rayT = r.transformRay(sn->T_inv,sn->T_star_inv);	
		switch(sn->mPrimitive){
			case CUBE:
				t_temp = RayCubeIntersect(rayT,normal_dummy);
				break;
			case SPHERE:
				t_temp = RaySphereIntersect(rayT,normal_dummy);
				break;
			case MESH:
				t_temp = RayMeshIntersect(rayT,sn->mMesh,normal_dummy);
				break;
			default:
				break;
		}
		if(t_temp > 0.0 && t_temp < direction.length())
			return false;
	}
	return true;
}

void RayTracer::TraceRay(Vector3 start, Vector3 direction, int depth,rgb &color, int last_obj)
{
	if(depth > 5){
		//cout << "WTF" << endl;
		color = mSceneMgr->bkground_color;
		return;
	}

	//Local Initializations
	Ray mRay(start,direction);
	int j; //Keep Track of Object Hit Distance
	Vector3 normal;
	Material surface_material;
	Vector3 IntersectionPoint = RayIntersect(mRay,j,normal);

	//If No Object Hit: Return BackGround Color
	if(j < 0)
	{
		color = mSceneMgr->bkground_color;
	}else{

		//Transparency and Refractivity Function (Do not Treat as Blinn-Phong)
		surface_material = mSceneMgr->getMaterial(objectList[j]->mMat);
		rgb refrac_color(0.0,0.0,0.0);
		rgb refl_color(0.0,0.0,0.0);
		Vector3 refl_ray(0.0,0.0,0.0);
		Vector3 refr_ray(0.0,0.0,0.0);
		//Retrieve Outward Facing Normal
		if(normal.dotProduct(-direction.normalisedCopy()) < 0 && normal.dotProduct(mSceneMgr->light_pos-IntersectionPoint) < 0)
			normal = -normal;

		//If No Transparency, Compute BlinnPhong of Surface
		if(surface_material.kt < 1.0) {
			color = ComputeBlinnPhong(IntersectionPoint,normal,mRay.d,surface_material);
		}
		
		//Reflectivity
		if(surface_material.kr > 0.0 && surface_material.kt < 1.0)
		{
			//Vector3 refl_ray = ReflectedRay(direction.normalisedCopy(),normal.normalisedCopy(),IntersectionPoint);
			//TraceRay(IntersectionPoint,refl_ray,depth+1,refl_color);
			refl_ray = ReflectedRay(direction.normalisedCopy(),normal.normalisedCopy(),IntersectionPoint);
			Vector3 IP_Reflection = IntersectionPoint+refl_ray*0.05;
			TraceRay(IP_Reflection,refl_ray,depth+1,refl_color,-1);
		
		} else {
			refl_color = rgb(0.0,0.0,0.0);
		}

		//Refractivity
		if(surface_material.kt > 0.0){
			//cout << "How are we In here Now?" << endl;
			float eta_1;
			float eta_2;
			float new_obj;
			//Check to See Inside or Outside of object
			if(normal.dotProduct(direction) > 0.0)
					normal = -normal;
			if(last_obj < 0){
				//Outside Object
				eta_1 = 1.0;
				eta_2 = surface_material.krefrac;
				new_obj = j; //Make Object We are Inside Object Currently Raytraced
			}
			else if(last_obj != j){
				//Entering A Different Object
				//cout << "Houston Is Inside..." << endl;
				eta_1 = mSceneMgr->getMaterial(objectList[last_obj]->mMat).krefrac;
				eta_2 = surface_material.krefrac;
				new_obj= j; //We Exited the Object we are in a new Object now
			} 
			else {
				//Inside Object - Exiting Object
				//cout << "Houston Is Inside..." << endl;
				eta_1 = mSceneMgr->getMaterial(objectList[last_obj]->mMat).krefrac;
				eta_2 = 1.0;
				new_obj= -1; //We Exited the Object and we are now Outside, Hopefully...
			}

			//Fresnel Refraction/Reflection
			float R = 0.0;
			float T = 0.0;
			
			direction.normalise();
			normal.normalise();

			//Reflected Color
			refl_ray = ReflectedRay(direction,normal,IntersectionPoint);
			Vector3 IP_Reflection = IntersectionPoint+refl_ray*0.05;
			TraceRay(IP_Reflection,refl_ray,depth+1,refl_color,last_obj);
			
			//Refracted Color
			refr_ray = RefractedRay(direction,normal,IntersectionPoint,eta_1,eta_2);
			if(refr_ray.isZeroLength()){
				refr_ray = ReflectedRay(direction,normal,IntersectionPoint);
			}
			//Compute Transmission/Reflection Amounts
			FresnelCoefficients(R,T,eta_1,eta_2,normal,direction,refr_ray);
			Vector3 IP_Refraction = IntersectionPoint+refr_ray*0.05;
			TraceRay(IP_Refraction,refr_ray,depth+1,refrac_color,new_obj);

			//Compute Fresnel Color
			color = refl_color*R+refrac_color*T;
			//color = refl_color*surface_material.kr+refrac_color*(1.0-surface_material.kr);

			//Modulate Final Color By Surface Material
			color = surface_material.C*color;
			return;
		}

		color = (refl_color*surface_material.kr) + (color*(1-surface_material.kr));
	}
	return;
}
