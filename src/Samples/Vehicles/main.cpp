/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/Samples/Vehicles/main.cpp

�y�T�v�zSpringhead2�̑����I�ȃf����P�i
 
�y�d�l�z

*/

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
//#include <ctime>
//#include <string>
#include <GL/glut.h>
//#include <sstream>
#include "robot1.h"
#include "robot2.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27

PHSdkIf* phSdk;			// SDK�C���^�t�F�[�X
GRSdkIf* grSdk;
PHSceneIf* scene;		// Scene�C���^�t�F�[�X
GRDebugRenderIf* render;
GRDeviceGLIf* device;

double lookAtY, lookAtZ;

Robot1 car;
Robot2 robot[2];

void CreateFloor(){
	CDBoxDesc desc;
	desc.boxsize = Vec3f(30.0f, 10.0f, 20.0f);
	PHSolidDesc sd;
	PHSolidIf* soFloor = scene->CreateSolid(sd);
	soFloor->AddShape(phSdk->CreateShape(desc));
	soFloor->SetFramePosition(Vec3f(0,-5, 0));
	soFloor->SetDynamical(false);			// ���͊O�͂ɂ���ē����Ȃ��悤�ɂ���
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
	Vec3d pos = robot[0].soBody->GetFramePosition();
	Affinef af;
	af.Pos() = Vec3f(0, 1.5, 4)*1.2;
	af.LookAtGL(Vec3f(0,0,0), Vec3f(0,100,0));
	render->SetViewMatrix(af.inv());
	
	render->ClearBuffer();
	render->DrawScene(scene);
	render->EndScene();
}

/**
 brief     	�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0, light1;
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, 10.0, 1.0);
	render->PushLight(light0);
	render->PushLight(light1);
}

/**
 brief     	����������
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	lookAtY = 1.0;
	lookAtZ = 5.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	Vec3d pos;
	gluLookAt(pos.x, pos.y + lookAtY, pos.z + lookAtZ, 
		      pos.x, pos.y, pos.z,
		 	  0.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	setLight();
}

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		�Ȃ�
 */
void reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	render->Reshape(Vec2f(), Vec2f(w,h));
}

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	unsigned int i = 0;
	switch (key) {
	//�I��
	case ESC:		
	case 'q':
		exit(0);
		break;
	case 'a':
		robot[0].Forward();
		break;
	case 's':
		robot[0].Backward();
		break;
	case 'd':
		robot[0].TurnLeft();
		break;
	case 'f':
		robot[0].TurnRight();
		break;
	case 'g':
		robot[0].Stop();
		break;
	case 'j':
		lookAtY += 0.1;
		break;
	default:
		break;
	}
}	

/**
 brief  	glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
 return 	�Ȃ�
 */
void timer(int id){
	glutTimerFunc(25, timer, 0);
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	for(int i = 0; i < 1; i++)
		scene->Step();
	glutPostRedisplay();
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){

	// SDK�̍쐬�@
	phSdk = CreatePHSdk();
	grSdk = CreateGRSdk();
	// �V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc dscene;
	dscene.timeStep = 0.05;
	scene = phSdk->CreateScene(dscene);				// �V�[���̍쐬
	
	// �V�[���̍\�z
	CreateFloor();
	Posed pose;
	pose.Pos() = Vec3d(3.0, 2.0, 0.0);
	robot[0].Build(pose, scene, phSdk);
	pose.Pos() = Vec3d(0.0, 1.0, 1.0);

//	car.Build(pose, scene, phSdk);
	CDBoxDesc box;
	box.boxsize = Vec3f(1.0, 1.0, 2.0);
	CDBoxIf* boxBody = DCAST(CDBoxIf, phSdk->CreateShape(box));
	PHSolidDesc sd;
	sd.mass *= 0.7;
	sd.inertia *= 0.7;
	PHSolidIf* soBox;
	for(int i=0; i<3;++i){
		soBox = scene->CreateSolid(sd);
		soBox->AddShape(boxBody);
		soBox->SetPose(pose);
		pose.PosY()+=1.0;
		pose.PosX()-=0.1;
	}
	scene->SetGravity(Vec3f(0.0, -9.8, 0.0));
	scene->SetNumIteration(60);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int window = glutCreateWindow("Vehicles");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL(window);
	device->Init();
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�
	Affinef af;
	af.Pos() = Vec3f(5, 2, 0);
	af.LookAtGL(Vec3f(0,0,0), Vec3f(0,100,0));
	render->SetViewMatrix(af);

	glutTimerFunc(10, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(idle);
	
	initialize();

	glutMainLoop();

	delete phSdk;
	delete grSdk;
}
