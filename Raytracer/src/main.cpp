//Leftheris Kaleas
//CIS460 Project 1

#include "GLUTDisplay.h"
#include <iostream>

//Main Method for starting up GLUT.


//Forward Dec
void init(void);
void display(void);
void keypressed(unsigned char key, int x, int y);

int main (int argc, char **argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600,600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Project 2 Mesh Objects");
	init();

	glutDisplayFunc(display);
	glutKeyboardFunc(keypressed);

	glutMainLoop();

	return 0;
}

void init(void)
{
	// This code sets up some simple lighting
	float l0_dif[] = {1.0f, 1.0f, 1.0f, 1.0f};
	float l0_amb[] = {0.2f, 0.2f, 0.2f, 0.2f};
	float l0_pos[] = {0.5f, 0.6f, 0.624f, 0.0f};

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, l0_dif);
	glLightfv(GL_LIGHT0, GL_AMBIENT, l0_amb);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLightfv(GL_LIGHT0, GL_POSITION, l0_pos);
	//GDisplay::getInstance()->getSceneManager()->createMeshNode(GDisplay::getInstance()->getSceneManager()->getRoot(), "test.extrude");
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, 1.0, 0.5, 30.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GDisplay::getInstance()->draw();

	glutSwapBuffers();
}

void keypressed(unsigned char key, int x, int y)
{
	GDisplay* dis = GDisplay::getInstance();
	SceneNode* selected = dis->getSceneManager()->getSelected();
	std::string fname;
	
	switch(key){
		case 's':
			dis->getSceneManager()->selectFirstChild(selected);
			break;
		case 'w':
			dis->getSceneManager()->selectParent(selected);
			break;
		case 'a':
			dis->getSceneManager()->selectNext(selected);
			break;
		case 'd':
			dis->getSceneManager()->selectPrevious(selected);
			break;
		case 'c':
			{
			SceneNode* nNode = new SceneNode();
			selected->addChild(nNode);
			dis->getSceneManager()->setSelected(nNode);
			break;
			}
		case 'e':
			dis->getSceneManager()->eraseNode(selected);
			break;
		case 'b':
			selected->setPrim(SPHERE);
			break;
		case 'g':
			selected->setPrim(CUBE);
			break;
		case 'n':
			selected->setPrim(NOTHING);
			break;
		case 'x':
			selected->trans[VX]++;
			break;
		case 'X':
			selected->trans[VX]--;
			break;
		case 'y':
			selected->trans[VY]++;
			break;
		case 'Y':
			selected->trans[VY]--;
			break;
		case 'z':
			selected->trans[VZ]++;
			break;
		case 'Z':
			selected->trans[VZ]--;
			break;
		case 'u':
			selected->rot[VX]+=30.0f;
			break;
		case 'U':
			selected->rot[VX]-=30.0f;
			break;
		case 'i':
			selected->rot[VY]+=30.0f;
			break;
		case 'I':
			selected->rot[VY]-=30.0f;
			break;
		case 'o':
			selected->rot[VZ]+=30.0f;
			break;
		case 'O':
			selected->rot[VZ]-=30.0f;
			break;
		case 'j':
			selected->scale[VX]+=0.2;
			break;
		case 'J':
			selected->scale[VX]-=0.2;
			break;
		case 'k':
			selected->scale[VY]+=0.2;
			break;
		case 'K':
			selected->scale[VY]-=0.2;
			break;
		case 'l':
			selected->scale[VZ]+=0.2;
			break;
		case 'L':
			selected->scale[VZ]-=0.2;
			break;
		case '1':
			dis->getSceneManager()->createTable(selected);
			break;
		case '2':
			dis->getSceneManager()->createChair(selected);
			break;
		case '3':
			dis->getSceneManager()->createCabinet(selected);
			break;
		case '4':
			cout << "Mesh Filename:"; 
			getline(cin,fname);
			dis->getSceneManager()->createMeshNode(selected,fname.c_str());
			break;
		case '5':
			cout << "Node Set to Material 1" << endl;
			dis->getSceneManager()->getSelected()->setColor(1);
			break;
		case '6':
			cout << "Node Set to Material 2" << endl;
			dis->getSceneManager()->getSelected()->setColor(2);
			break;
		case '7':
			cout << "Node Set to Material 3" << endl;
			dis->getSceneManager()->getSelected()->setColor(3);
			break;
		case 'r':
			cout << "RayTrace Config Filename:";
			getline(cin,fname);
			dis->outputRayTraceImage(fname.c_str());
			break;
		default:
			break;
	}
	display();
}