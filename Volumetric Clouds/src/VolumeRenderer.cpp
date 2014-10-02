#include "VolumeRenderer.h"
#include <iostream>
#include <SOIL.h>

#define PI 3.14159265
#define EPS 0.000001

using namespace std;


enum{RED,GREEN,BLUE,ALPHA};

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

VolumeRenderer::VolumeRenderer() {
		red_vgrid = new VoxelGrid();
		green_vgrid = new VoxelGrid();
		blue_vgrid = new VoxelGrid();
		rho_vgrid = new VoxelGrid();
		output_filename = new char[50];
}

VolumeRenderer::~VolumeRenderer(){
	delete red_vgrid;
	delete green_vgrid;
	delete blue_vgrid;
	delete rho_vgrid;
}

void VolumeRenderer::updateViewingGeometry(){
	cout << "Updating Viewing Geometry" << endl;
	//Eye Coord
	Vector3 eye = eye_pos;
	//View Direction
	Vector3 C = view_dir;
	//Up Vector
	Vector3 up = up_vec;
	//Half Viewing Angles
	float phi = (fov_y/2.0)*PI/180.0; //Conversion to Rad
	float aspect_ratio = float(resolution_x)/float(resolution_y);
	float theta = asin(aspect_ratio*sin(phi));
	float phi_D = phi*(180.0/PI);
	float theta_D = theta*(180.0/PI);
	Vector3 A = C.crossProduct(up);
	Vector3 B = A.crossProduct(C);
	M = eye + C;
	H = (A*(C.length()*tan(theta)))/A.length();
	V = (B*(C.length()*tan(phi)))/B.length();
}

bool VolumeRenderer::load(const char *filename){
	std::ifstream inFile(filename);
	if(inFile.is_open())
	{
		//Temporary Variables
		std::cout << "Loading Config: " << filename << std::endl;
		int args;
		std::string buffer;
		char* vgrid_fname = new char[50];

		//FILE
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"FILE %s",output_filename);
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
		args = sscanf(buffer.c_str(),"EYEP %e %e %e",&(eye_pos.x),&(eye_pos.y),&(eye_pos.z));
		if(args != 3){
			std::cout << "EYEP is not in correct format" << std::endl;
			return false;
		}
		//VDIR
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"VDIR %e %e %e",&(view_dir.x),(&view_dir.y),&(view_dir.z));
		if(args != 3){
			std::cout << "VDIR is not in correct format" << std::endl;
			return false;
		}
		//UVEC
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"UVEC %e %e %e",&(up_vec.x),(&up_vec.y),&(up_vec.z));
		if(args != 3){
			std::cout << "UVEC is not in correct format" << std::endl;
			return false;
		}
		//FOVY
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"FOVY %e",&fov_y);
		if(args != 1){
			std::cout << "FOVY is not in correct format" << std::endl;
			return false;
		}
		//LPOS
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"LPOS %e %e %e",&(l_pos.x),&(l_pos.y),&(l_pos.z));
		if(args != 3){
			std::cout << "LPOS is not in correct format" << std::endl;
			return false;
		}
		//LCOL
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"LCOL %e %e %e",&(l_color[RED]),&(l_color[GREEN]),&(l_color[BLUE]));
		if(args != 3){
			std::cout << "LCOL is not in correct format" << std::endl;
			return false;
		}
		//STEP
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"STEP %e",&step);
		if(args != 1){
			std::cout << "STEP is not in correct format" << std::endl;
			return false;
		}
		//BCOL
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"BCOL %e %e %e",&(b_color[RED]),&(b_color[GREEN]),&(b_color[BLUE]));
		if(args != 3){
			std::cout << "BCOL is not in correct format" << std::endl;
			return false;
		}
		//RCVG
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"RCVG %s",vgrid_fname);
		if(args != 1){
			std::cout << args << endl;
			std::cout << "RCVG is not in correct format" << std::endl;
			return false;
		}
		red_vgrid->load(vgrid_fname);

		//GCVG
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"GCVG %s",vgrid_fname);
		if(args != 1){
			std::cout << "GCVG is not in correct format" << std::endl;
			return false;
		}
		green_vgrid->load(vgrid_fname);
		
		//BCVG
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"BCVG %s",vgrid_fname);
		if(args != 1){
			std::cout << "BCVG is not in correct format" << std::endl;
			return false;
		}
		blue_vgrid->load(vgrid_fname);

		//RHVG
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"RHVG %s",vgrid_fname);
		if(args != 1){
			std::cout << "RHVG is not in correct format" << std::endl;
			return false;
		}
		rho_vgrid->load(vgrid_fname);

		//KAPPA
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"KAPA %e",&kappa);
		if(args != 1){
			std::cout << "KAPA is not in correct format" << std::endl;
			return false;
		}
		std::cout << "Precomputing Q Grid..." << endl;
		q_grid = VoxelGrid::computeQGrid(rho_vgrid,l_pos,step,kappa);

		inFile.close();
		return true;
	}
	else {
		std::cout << "File Does not Exist" << std::endl;
	}
	return false;
}

void VolumeRenderer::clear(){
	delete green_vgrid;
	delete red_vgrid;
	delete blue_vgrid;
	delete rho_vgrid;
	green_vgrid = new VoxelGrid();
	blue_vgrid = new VoxelGrid();
	red_vgrid = new VoxelGrid();
	rho_vgrid = new VoxelGrid();
}

void VolumeRenderer::outputImage(const char* filename){
	if(load(filename)){
		cout << "Configuration Loaded." << endl;

		updateViewingGeometry();
		unsigned char* image;
		unsigned int c = 3; //RGB value for each entry in the array
		image = new unsigned char[c*resolution_x*resolution_y];
		rgb color(0.0f,0.0f,0.0f);
		unsigned char red_c;
		unsigned char green_c;
		unsigned char blue_c;
		double res_x_d = resolution_x;
		double res_y_d = resolution_y;
		double percentage_done=0.0;
		
		for(int x=0;x<resolution_x;x++){
			for(int y=0;y<resolution_y;y++){
				color = rgb(0.0f,0.0f,0.0f);
				double x_d = x;
				double y_d = resolution_y-y; //Flip the Y value to be the opposite (Bottom to Top Down Conversion)
				Vector3 P = M+((2*x_d)/(res_x_d-1)-1)*H+((2*y_d)/(res_y_d-1)-1)*V; //Pixel's Point on the Screen in World Space
				Vector3 D = (P-eye_pos).normalisedCopy();
				Vector3 E = eye_pos;
				//Volume Render the Pixel
				Ray r(E,D);
				color = rayMarch(r);
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
			cout << "Percentage of Scene Rendered: " << percentage_done << "%" << endl;
		}

		if(SOIL_save_image(output_filename,SOIL_SAVE_TYPE_BMP,resolution_x,resolution_y,3,image)){
			cout << endl << "Volume Render Successful" << endl;
		}
	}
}

void VolumeRenderer::outputImage(const char* filename, const char* custom_output)
{
	if(load(filename)){
		cout << "Configuration Loaded." << endl;

		updateViewingGeometry();
		unsigned char* image;
		unsigned int c = 3; //RGB value for each entry in the array
		image = new unsigned char[c*resolution_x*resolution_y];
		rgb color(0.0f,0.0f,0.0f);
		unsigned char red_c;
		unsigned char green_c;
		unsigned char blue_c;
		double res_x_d = resolution_x;
		double res_y_d = resolution_y;
		double percentage_done=0.0;
		
		for(int x=0;x<resolution_x;x++){
			for(int y=0;y<resolution_y;y++){
				color = rgb(0.0f,0.0f,0.0f);
				double x_d = x;
				double y_d = resolution_y-y; //Flip the Y value to be the opposite (Bottom to Top Down Conversion)
				Vector3 P = M+((2*x_d)/(res_x_d-1)-1)*H+((2*y_d)/(res_y_d-1)-1)*V; //Pixel's Point on the Screen in World Space
				Vector3 D = (P-eye_pos).normalisedCopy();
				Vector3 E = eye_pos;
				//Volume Render the Pixel
				Ray r(E,D);
				color = rayMarch(r);
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
			cout << "Percentage of Scene Rendered: " << percentage_done << "%" << endl;
		}

		if(SOIL_save_image(custom_output,SOIL_SAVE_TYPE_BMP,resolution_x,resolution_y,3,image)){
			cout << endl << "Volume Render Successful" << endl;
		}
	}
}

rgb VolumeRenderer::rayMarch(Ray r){
	//Ray March Variables
	rgb color(0.0,0.0,0.0);
	float deltaT;
	float T = 1.0;
	rgb Ci(0.0,0.0,0.0);
	float deltaS = step; //Same as step size
	float Q;

	Vector3 xi;
	
	//Get t for intersection with the grid
	//Transform Matrix T for the Cube
	Matrix4 Tmat;
	Vector3 trans(rho_vgrid->max_x/2.0,rho_vgrid->max_y/2.0,rho_vgrid->max_z/2.0);
	Vector3 scale(rho_vgrid->max_x,rho_vgrid->max_y,rho_vgrid->max_z);
	Tmat.makeInverseTransform(trans,scale,Quaternion::IDENTITY);
	Matrix4 Tstar = r.getTStar(Tmat);
	//TInverse + TStar Inverse -> transform ray - RTransform
	Ray rT = r.transformRay(Tmat,Tstar);
	//Check Intersection
	float t = rho_vgrid->RayGridIntersect(rT);
	//If it intersected (t>0), 
	if(t > 0){
		//Raymarch through until starting when point enters gridspace (inGridspace=true).
		//When We've reached first thing in gridspace
		xi = r.getPosition(0.0);
		int counter = 0;

		//Loop Without Adding until we get to the edge of the Grid
		//while(!rho_vgrid->inGridspace(xi)){
		//	xi += r.d*deltaS;
		//}

		if(rho_vgrid->inGridspace(r.getPosition(0.0)))//Start AT the Grid
			xi = r.getPosition(0.0);
		else
			xi = r.getPosition(t+EPS);

		//Start Adding up Gridspace Color
		while(T > EPS){
			if(!rho_vgrid->inGridspace(xi)){
				color += b_color*T;
				break;
			}
			deltaT = exp(-kappa*deltaS*rho_vgrid->getVoxelDensity(xi));
			T *= deltaT;
			Ci = rgb(red_vgrid->getVoxelDensity(xi),green_vgrid->getVoxelDensity(xi),blue_vgrid->getVoxelDensity(xi));
			//Ray xi_to_light(xi,(l_pos-xi).normalisedCopy());
			//Q = getQ(xi_to_light);
			Q = q_grid->getVoxelDensity(xi);
			color += ((1.0-deltaT)/kappa)*(Ci*l_color)*T*Q;
			xi += r.d*deltaS;
		}
		//End when the current point is outside the gridspace or when Transmittance is 0;
	}
	else
	{
		//Return Background Color;
		color = b_color;
	}
	//Else if (-1), return -1
	
	return color;
}

float VolumeRenderer::getQ(Ray r){
	float T = 1.0;			   //Q value
	Vector3 xi = r.getPosition(0.0);
	float deltaS = step;
	while(rho_vgrid->inGridspace(xi)){
		T *= exp(-kappa*deltaS*rho_vgrid->getVoxelDensity(xi));
		xi += r.d*deltaS;
	}
	return T;
}