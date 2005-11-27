#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <gl/glut.h>
#include <stdlib.h>
#include <stdio.h>
#pragma hdrstop
using namespace Spr;

#define ESC 27

PHSdkIf* sdk;
PHSceneIf* scene;
PHSolidIf* solid1, *solid2;

static GLfloat light_position[] = { 15.0, 30.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

static GLfloat mat_red[]        = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_blue[]       = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };

void Display(){
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glMaterialf(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, (1.f,1.f,1.f,1.f));

	Affinef af;
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
	
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);
	glPushMatrix();
	solid1->GetOrientation().to_matrix(af);
	af.Pos() = solid1->GetFramePosition();
	glMultMatrixf(af);
	
	glutSolidTeapot(1.0);
	glPopMatrix();

	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);
	glPushMatrix();
	solid2->GetOrientation().to_matrix(af);
	af.Pos() = solid2->GetFramePosition();
	glMultMatrixf(af);
	//glTranslatef(5.0, 0.0, 0.0);
	glutSolidTeapot(1.0);

	glPopMatrix();

	glutSwapBuffers();
}
void setLight() {
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
}
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(2.0, 3.0, 10.0, 
		      2.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	setLight();
}
void Reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)(w/h), 1.0, 500);
	glMatrixMode(GL_MODELVIEW);
}
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}
void Idle(){
	//	���̂̏d�S��1m����E�ɉ����D
	solid1->AddForce( Vec3f(1,0,0), Vec3f(0,1,0) +  solid1->GetCenterPosition());
	solid2->AddForce( Vec3f(1,0,0), Vec3f(0,1,0) +  solid2->GetCenterPosition());
	scene->Step();
	glutPostRedisplay();
	std::cout << solid1->GetFramePosition();
	std::cout << solid2->GetFramePosition() << std::endl;
}
int main(int argc, char* argv[]){
	sdk = CreatePHSdk();				//	SDK�̍쐬
	scene = sdk->CreateScene();			//	�V�[���̍쐬
	
	PHSolidDesc desc;					// ����teapot
	desc.mass = 2.0;					// ����	
	desc.inertia *=2.0;					// �����e���\��
	desc.center = Vec3f(0,0,0);			// ���ʒ��S�̈ʒu
	solid1 = scene->CreateSolid(desc);	// ���̂�desc�Ɋ�Â��č쐬
	
	desc.center = Vec3f(1,0,0);		//	�d�S�̈ʒu��Solid�̌��_����1m�E�ɂ��炷�D
	solid2 = scene->CreateSolid(desc);	

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("PHSimpleGL");

	initialize();

	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(Idle);

	glutMainLoop();
}

