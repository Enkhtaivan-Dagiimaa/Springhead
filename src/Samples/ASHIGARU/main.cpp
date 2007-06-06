/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/Samples/Vehicles/main.cpp

�y�T�v�zSpringhead2�̑����I�ȃf����P�i�D8�����{�b�g�̕��s
 
�y�d�l�z

*/

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <GL/glut.h>
#include "robot.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27

UTRef<PHSdkIf> phSdk;			// SDK
UTRef<GRSdkIf> grSdk;
UTRef<PHSceneIf> scene;			// Scene
UTRef<GRDebugRenderIf> render;
UTRef<GRDeviceGLIf> device;

Robot robot;

void CreateFloor(){
	PHSolidDesc sd;
	PHSolidIf* soFloor = scene->CreateSolid(sd);
	CDBoxDesc bd;
	bd.boxsize = Vec3f(30.0f, 10.0f, 20.0f);
	soFloor->AddShape(phSdk->CreateShape(bd));
	soFloor->SetFramePosition(Vec3f(0,-5, 0));
	soFloor->SetDynamical(false);					// ���͊O�͂ɂ���ē����Ȃ��悤�ɂ���
}

/**
 brief     	glutDisplayFunc�Ŏw�肷��R�[���o�b�N�֐��D��ʂ̕`����s���D
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void Display(){
	Affinef af;
	af.Pos() = Vec3f(0, 3, 4)*1.0;
	af.LookAtGL(Vec3f(0,0,0), Vec3f(0,100,0));
	render->SetViewMatrix(af.inv());	//	���_�̐ݒ�
	
	render->ClearBuffer();
	render->DrawScene(scene);
	render->EndScene();
	glutSwapBuffers();
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
	setLight();
}

/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		�Ȃ�
 */
void Reshape(int w, int h){
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
void Keyboard(unsigned char key, int x, int y){
	unsigned int i = 0;
	switch (key) {
	//�I��
	case ESC:		
	case 'q':
		exit(0);
		break;
	case 'a':
		robot.Forward();
		break;
	case 's':
		robot.Backward();
		break;
	case 'd':
		robot.TurnLeft();
		break;
	case 'f':
		robot.TurnRight();
		break;
	case 'g':
		robot.Stop();
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
	glutTimerFunc(50, timer, 0);
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	for(int i = 0; i < 1; i++)
		scene->Step();
	glutPostRedisplay();
}

/**
 brief		���C���֐�
 */
int main(int argc, char* argv[]){

	// SDK�̍쐬�@
	phSdk = PHSdkIf::CreateSdk();
	grSdk = GRSdkIf::CreateSdk();
	// �V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc dscene;
	dscene.timeStep = 0.05;
	dscene.numIteration = 5;
	scene = phSdk->CreateScene(dscene);			// �V�[���̍쐬
	
	// �V�[���̍\�z
	CreateFloor();								//	��
	Posed pose;
	pose.Pos() = Vec3d(0.0, 2.0, 0.0);
	robot.Build(pose, scene, phSdk);			//	���{�b�g
	//pose.Pos() = Vec3d(0.0, 1.0, 1.0);

	/*CDBoxDesc box;								//	�O�d�Ȃ��Ă��锠
	box.boxsize = Vec3f(1.0, 1.0, 2.0);
	CDBoxIf* boxBody = phSdk->CreateShape(box)->Cast();
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
	}*/

	scene->SetGravity(Vec3f(0.0, -9.8, 0.0));	//	�d�͂�ݒ�
	
	//	GLUT�̏�����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int window = glutCreateWindow("ASHIGARU");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL();
	device->Init();
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�

	glutTimerFunc(50, timer, 0);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	
	initialize();

	glutMainLoop();
}
