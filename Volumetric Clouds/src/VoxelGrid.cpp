#include "VoxelGrid.h"

//Cube/Bounding Box Globals
const Vector3 CUBE_VERT[8] = { Vector3(0.5,0.5,0.5),Vector3(-0.5,0.5,0.5),Vector3(-0.5,0.5,-0.5),Vector3(0.5,0.5,-0.5),
							Vector3(0.5,-0.5,0.5),Vector3(-0.5,-0.5,0.5),Vector3(-0.5,-0.5,-0.5),Vector3(0.5,-0.5,-0.5)
							};

const Vector3 CUBE_FACE_NORM[6] = {Vector3(1,0,0),Vector3(0,1,0),Vector3(0,0,1),
									Vector3(-1,0,0),Vector3(0,-1,0),Vector3(0,0,-1)
									};

const Vector3 CUBE_FACE_POINT[6]  = {Vector3(0.5,0.5,0.5),Vector3(0.5,0.5,0.5),Vector3(0.5,0.5,0.5),
									Vector3(-0.5,-0.5,-0.5),Vector3(-0.5,-0.5,-0.5),Vector3(-0.5,-0.5,-0.5)};

//Ray Class Function
Ray::Ray(){
	p0=Vector3::ZERO;
	d=Vector3::ZERO;
}
Ray::Ray(Vector3 p, Vector3 v){
	p0=p;
	d=v;
}

Vector3 Ray::getPosition(double t)
{
	return p0+t*d;
}

Ray Ray::transformRay(const Ogre::Matrix4 &T,const Ogre::Matrix4 &TStar)
{
	Vector3 pNew = T*this->p0;
	Vector3 vNew = TStar*this->d;
	return Ray(pNew,vNew);
}

Matrix4 Ray::getTStar(Matrix4 const& T)
{
	Matrix4 TStar(T);
	TStar[0][3]=0.0;
	TStar[1][3]=0.0;
	TStar[2][3]=0.0;
	return TStar;
}

//VoxelGrid Class Functions

VoxelGrid::VoxelGrid()
{
	default_density=0.0;
	grid = 0;
	delta = 0.0;
	xCount = 0;
	yCount = 0;
	zCount = 0;
	max_x = 0;
	max_y = 0;
	max_z = 0;
}
VoxelGrid::~VoxelGrid(){
	for(int i = 0;i< xCount; i++){
		for(int j =0; j<yCount; j++){
			delete [] grid[i][j];
		}
		delete [] grid[i];
	}
	delete [] grid;
}

bool VoxelGrid::load(const char *filename){
	std::ifstream inFile(filename);
	if(inFile.is_open())
	{
		//Temporary Variables
		std::cout << "Loading Voxel Grid: " << filename << std::endl;
		int args;
		std::string buffer;

		//Delta
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"%e",&delta);
		if(args != 1){
			std::cout << "Delta is not in correct format" << std::endl;
			return false;
		}

		//Count
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"%d %d %d",&xCount,&yCount,&zCount);
		if(args != 3){
			std::cout << "Count is not in correct format" << std::endl;
			return false;
		}

		//Default Density
		std::getline(inFile,buffer);
		args = sscanf(buffer.c_str(),"%e",&default_density);
		if(args != 1){
			std::cout << "Default Density is not in correct format" << std::endl;
			return false;
		}

		//Intialized Voxel 3-D array to the right size
		grid = new float**[xCount];
		for(int i=0;i<xCount;i++){
			grid[i] = new float*[yCount];
			for(int j=0;j<yCount;j++){
				grid[i][j] = new float[zCount];
			}
		}
		
		//Fill in density values for each thing in the grid
		for(int i=0;i<zCount;i++){
			for(int j=0;j<yCount;j++){
				for(int k=0;k<xCount;k++){
					//Grid is grid[X(k)][Y(j)][Z(i)]
					std::getline(inFile,buffer);
					float d;
					args = sscanf(buffer.c_str(),"%e",&d);
					if(args != 1){
						std::cout << "Density Value at line " << 3+(i+1)*(j+1)*(k+1) <<" is not in correct format" << std::endl;
							return false;
					}
					grid[k][j][i] = d;
				}
			}
		}
		
		//Set Max World Space Values for Voxel Grid
		max_x = xCount*delta;
		max_y = yCount*delta;
		max_z = zCount*delta;
		inFile.close();
		return true;

	} else {
		std::cout << "File Does Not Exist" << std::endl;
	}
	return false;
}


float VoxelGrid::getVoxelDensity(float x, float y, float z){
	return getVoxelDensity(Vector3(x,y,z));
}

float VoxelGrid::getVoxelDensity(Ogre::Vector3 p)
{
	Vector3 p_norm = p/delta; //Reduce point to a normalized space where each voxel is of size 1. 
	//Out Of Bounds->Default Density
	if(p_norm.x >= xCount || p_norm.x < 0 ||
	   p_norm.y >= yCount || p_norm.y < 0 ||
	   p_norm.z >= zCount || p_norm.z < 0)
	{
	   return default_density;
	}
	int x = floor(p_norm.x);
	int y = floor(p_norm.y);
	int z = floor(p_norm.z);
	//return grid[x][y][z];
	//Tri Linear Interpolation
	float x_d = p_norm.x-x;
	float y_d = p_norm.y-y;
	float z_d = p_norm.z-z;
	int xt;
	int yt;
	int zt;
	if(x == xCount-1)
		xt = x;
	else
		xt = x+1;
	if(y == yCount-1)
		yt = y;
	else
		yt = y+1;	
	if(z == zCount-1)
		zt = z;
	else
		zt = z+1;
	float i_1 = grid[x][y][z]*(1-z_d)+grid[x][y][zt]*z_d;
	float i_2 = grid[x][yt][z]*(1-z_d)+grid[x][yt][zt]*z_d;
	float j_1 = grid[xt][y][z]*(1-z_d)+grid[xt][y][zt]*z_d;
	float j_2 = grid[xt][yt][z]*(1-z_d)+grid[xt][yt][zt]*z_d;
	float w_1 = i_1*(1-y_d)+i_2*y_d;
	float w_2 = j_1*(1-y_d)+j_2*y_d;

	return w_1*(1-x_d)+w_2*(x_d);
}

std::vector<float> VoxelGrid::rayMarch(float step,Ray r)
{
	std::vector<float> dlist;
	float t_curr;
	Vector3 p_curr;
	//Get t for intersection with the grid
	//Transform Matrix T for the Cube
	Matrix4 T;
	Vector3 trans(max_x/2.0,max_y/2.0,max_z/2.0);
	Vector3 scale(max_x,max_y,max_z);
	T.makeInverseTransform(trans,scale,Quaternion::IDENTITY);
	Matrix4 Tstar = r.getTStar(T);
	//TInverse + TStar Inverse -> transform ray - RTransform
	Ray rT = r.transformRay(T,Tstar);
	//Check Intersection
	float t = RayGridIntersect(rT);
	//If it intersected (t>0), 
	if(t > 0){
		//Raymarch through until starting when point enters gridspace (inGridspace=true).
		//When We've reached first thing in gridspace
		t_curr=0;
		p_curr = r.getPosition(t_curr);
		//Loop Without Adding until we get to the end of the thing
		while(!inGridspace(p_curr)){
			t_curr += step;
			p_curr = r.getPosition(t_curr);
		}

		//Start Adding up Gridspace Things
		while(inGridspace(p_curr)){
			dlist.push_back(getVoxelDensity(p_curr));
			t_curr += step;
			p_curr = r.getPosition(t_curr);
		}
		//End when the current point is outside the gridspace. (inGridspace=false)
	}
	else
	{
		dlist.push_back(-1);
	}
	//Else if (-1), return -1
	
	return dlist;
}
bool VoxelGrid::inGridspace(Vector3 p)
{
	//Use Max X,Y,Z to check if a point is in bounds of the Voxel Grid
	if(p.x < max_x && p.x >=0 &&
		p.y < max_y && p.y >=0 &&
		p.z < max_z && p.z >=0){
			return true;
	}
	return false;
}

double VoxelGrid::RayGridIntersect(const Ray &r)
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
		if(p.x <= 0.500001f && p.x >= -0.500001f &&
		   p.y <= 0.500001f && p.y >= -0.500001f &&
		   p.z <= 0.500001f && p.z >= -0.500001f && (close == -1 || t < close)){
		   //std::cout << t << std::endl;
		   close = t;
		   close_index = i;
		}
	}
	if(close > 0.0){
		return close;
	}
	else
		return -1;
}

VoxelGrid* VoxelGrid::computeQGrid(VoxelGrid* src, Vector3 light, float deltaS, float kappa){
	VoxelGrid* q_grid = new VoxelGrid();
	q_grid->default_density = src->default_density; //Default Density of Each Voxel
	q_grid->xCount = src->xCount;		   //X,Y,Z total voxel count in each direction
	q_grid->yCount = src->yCount; 
	q_grid->zCount = src->zCount;
	q_grid->delta = src->delta;			//Size of Each Seperate Voxel
	q_grid->max_x = src->max_x;	
	q_grid->max_y = src->max_y;
	q_grid->max_z = src->max_z;

	//Intialized Voxel 3-D array to the right size
		q_grid->grid = new float**[q_grid->xCount];
		for(int i=0;i<q_grid->xCount;i++){
			q_grid->grid[i] = new float*[q_grid->yCount];
			for(int j=0;j<q_grid->yCount;j++){
				q_grid->grid[i][j] = new float[q_grid->zCount];
			}
		}

	//Fill in Q values for each thing in the grid
		for(int i=0;i<q_grid->xCount;i++){
			for(int j=0;j<q_grid->yCount;j++){
				for(int k=0;k<q_grid->zCount;k++){
					Vector3 p_curr(((float)i)*q_grid->delta+q_grid->delta/2.0,((float)j)*q_grid->delta+q_grid->delta/2.0,((float)k)*q_grid->delta+q_grid->delta/2.0);
					Ray r(p_curr,(light-p_curr).normalisedCopy());
					
					float T = 1.0;			   //Q value
					Vector3 xi = r.getPosition(0);
					while(q_grid->inGridspace(xi)){
						T *= exp(-kappa*deltaS*src->getVoxelDensity(xi));
						xi += r.d*deltaS;
					}
					q_grid->grid[i][j][k] = T;
				}
			}
		}
	return q_grid;
}