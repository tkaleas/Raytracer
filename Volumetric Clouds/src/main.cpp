//Leftheris Kaleas
//CIS460 Project 4

#include "VolumeRenderer.h"
#include "CloudGenerator.h"
#include <gl/glut.h>
#include <iostream>
#include <vector>

using namespace std;
using namespace Ogre;

int main (int argc, char **argv)
{
	std::string fname;
	CloudGenerator* cg = new CloudGenerator();
	VolumeRenderer* vr = new VolumeRenderer();
	while (1){
		cout << "Welcome to Puffy Cloud Maker v0.1!" << endl;
		cout << "Configuration Filename:";
		getline(cin,fname);
		if(fname == "anim")
			cg->generateCloudAnim("cg_test.txt",0.0,5.0,0.07,vr);
		if(fname == "exit")
			break;
		else if(fname == "gen")
			while(1){
				cout << "Welcome to Perlin Voxel Cloud Generator" << endl;
				cout << "Configuration FileName:";
				getline(cin,fname);
				if(fname == "exit")
					break;
				cg->generateCloud(fname.c_str());
				vr->outputImage("test.txt");
			}
		else
			vr->outputImage(fname.c_str());
	}
}

