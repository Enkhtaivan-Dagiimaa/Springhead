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

#include <windows.h>	// Window�֘A
#include <winuser.h>
#include "resource1.h"
#include <stdio.h>
#include <math.h>
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <GL/glut.h>
#include "robot.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27
#define module_max 2

UTRef<PHSdkIf> phSdk;			// SDK
UTRef<GRSdkIf> grSdk;
UTRef<PHSceneIf> scene;			// Scene
UTRef<GRDebugRenderIf> render;
UTRef<GRDeviceGLIf> device;

Robot robot[10];
//vector<Robot> robots;

double zoom = 0.2;
double shift_LR = 0.1;
double shift_UD = 0.1;

bool stepOnTimer = false;


void CreateFloor(){
	PHSolidDesc sd;
	PHSolidIf* soFloor = scene->CreateSolid(sd);
	CDBoxDesc bd;
	bd.boxsize = Vec3f(30.0f, 10.0f, 20.0f);
	soFloor->AddShape(phSdk->CreateShape(bd));
	soFloor->SetFramePosition(Vec3f( 0, -5, 0));
	soFloor->SetDynamical(false);					// ���͊O�͂ɂ���ē����Ȃ��悤�ɂ���
}

/**
 brief     	glutDisplayFunc�Ŏw�肷��R�[���o�b�N�֐��D��ʂ̕`����s���D
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void Display(){
	Affinef af;
	af.Pos() = Vec3f(0, 3.5, 4)*zoom;
	af.LookAtGL(Vec3f(shift_LR,0,shift_UD), Vec3f(shift_LR,10,shift_UD));
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
	light0.position = Vec4f(20.0, 40.0, 40.0, 0.1);
	//light1.position = Vec4f(-20.0, 20.0, 20.0, 0.1);
	render->PushLight(light0);
	//render->PushLight(light1);
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
		exit(0);
		break;
	case 'q':
		exit(0);
		break;
	case 'a':
		for(int i=0; i<module_max; i++){if(i==1)robot[i].Forward();else robot[i].Backward();}
		break;
	case 'z':
		zoom -= 0.01;
		glutPostRedisplay();
		break;
	case 'x':
		zoom += 0.01;
		glutPostRedisplay();
		break;
	case 's':
		for(int i=0; i<module_max; i++){if(i==1)robot[i].Backward();else robot[i].Forward();}
		break;
	case 't':
		stepOnTimer = !stepOnTimer;
		break;
	case 'p':
		stepOnTimer = false;
		scene->Step();
		glutPostRedisplay();
		break;
	case 'c':
		shift_LR += 0.01;
		break;
	case 'v':
		shift_LR -= 0.01;
		break;
	case 'k':
		shift_UD += 0.01;
		break;
	case 'm':
		shift_UD -= 0.01;
		break;
	/*case 'd':
		robot.TurnLeft();
		break;
	case 'f':
		robot.TurnRight();
		break;*/
	case 'd':
		for(int i=0; i<module_max; i++){robot[i].Stop();}
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
	glutTimerFunc(100, timer, 0);
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	if (stepOnTimer){
		for(int i = 0; i < 5; i++)
			scene->Step();
		glutPostRedisplay();
	}
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
	dscene.timeStep = 0.005;
	dscene.numIteration = 5;
	scene = phSdk->CreateScene(dscene);			// �V�[���̍쐬
	scene->SetGravity(Vec3f(0.0, -9.8, 0.0));	//	�d�͂�ݒ�
	// �V�[���̍\�z
	CreateFloor();								//	��
	Posed pose;
	pose.Ori() = Quaterniond::Rot(Rad(120.0), 'y');
	for(int i=0; i<module_max; i++){
			pose.Pos() = Vec3d(0.2*i, 0.2, 0.0);
			//pose.Pos() = Vec3d(0.0, 0.2, 0.0);
			robot[i].Build(pose, scene, phSdk);			//	���{�b�g�\�z
		}

	//�R���W���[�����񌋍�/////////////////////////////////////////////////////////////////////////

	PHHingeJointDesc Connect;

	for(int i=0; i<module_max; i++){
		robot[i].leg[1].jntDX1 -> SetSpringOrigin(Rad(-60.0));
		robot[i].leg[2].jntDX1 -> SetSpringOrigin(Rad(-120.0));
	}

	std::vector<PHSolidIf*> allSolids;
	for(int i=0; i<module_max; i++){
		allSolids.push_back(robot[i].soBody);
		for(int j=0; j<3; ++j){
			allSolids.push_back(robot[i].leg[j].soDX1);
			allSolids.push_back(robot[i].leg[j].soDX2);
			allSolids.push_back(robot[i].leg[j].soFoot);
			allSolids.push_back(robot[i].leg[j].soSphere);
		}
	}
	scene->SetContactMode(&allSolids[0], allSolids.size(), PHSceneDesc::MODE_NONE);

	for(int i=0; i<module_max-1; i++){
		robot[i].leg[0].jntDX1  -> SetSpringOrigin(Rad(-90.0));//�����r(leg[0])�̎p����ݒ�
		robot[i].leg[0].jntDX1  -> SetSpring(1000);
		robot[i].leg[0].jntDX2  -> SetSpringOrigin(Rad(-90.0));
		robot[i].leg[0].jntDX2  -> SetSpring(1000);
		robot[i].leg[0].jntFoot -> SetSpringOrigin(Rad(180.0));
		robot[i].leg[0].jntFoot -> SetSpring(1000);
	}

	//���������\�z
	Connect.poseSocket.Ori() = Quaterniond::Rot(Rad(60.0), 'y');
	Connect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	Connect.posePlug.Pos() = Vec3d(0.03, 0.025, 0.0);
	for(int i=0; i<module_max-1; i++){
		robot[i].leg[0].jntConnect[0] = scene->CreateJoint(robot[i+1].soBody, robot[i].leg[0].soDX2, Connect)->Cast();
		robot[i].leg[0].jntConnect[0]->SetSpring(10);
		robot[i].leg[0].jntConnect[0]->SetDamper(1);
		robot[i].leg[0].jntConnect[0]->SetSpringOrigin(Rad(180.0));
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	
	//	GLUT�̏�����
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int window = glutCreateWindow("ASHIGARU");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL();
	device->Init();
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�

	glutTimerFunc(100, timer, 0);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	
	initialize();

	glutMainLoop();
}
