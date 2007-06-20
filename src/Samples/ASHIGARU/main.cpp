/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/*
 Springhead2/src/Samples/ASHIGARU/main.cpp

�y�T�v�z�R�r���W�����[���{�b�gASHIGARU�̕��e�����V�~�����[�^
 
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
#include "myheader.h"	// ����֐��C�\����

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27
#define module_max 5

UTRef<PHSdkIf> phSdk;			// SDK
UTRef<GRSdkIf> grSdk;
UTRef<PHSceneIf> scene;			// Scene
UTRef<GRDebugRenderIf> render;
UTRef<GRDeviceGLIf> device;

Robot robot[10];//�Ƃ肠����ASHIGARU�ő�P�O��

double zoom = 0.2;
double shift_LR = 0.1;
double shift_UD = 0.1;

double para_X1 = 0.11, para_a1 = 0.05, para_b1 = 0.07, para_H1 = 0.17;
double	L1 = 0.067, L2 = 0.067, L3 = 0.12;	// �֐ߊԒ���

double			NowTime = 0.0;	// ����
double			s_time = 0.0;					// �T���v�����O�^�C��������
double			CycleTime = 2.0;
double			Phase = 0.0;

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
	af.LookAtGL(Vec3f(shift_LR+0.5,0,shift_UD), Vec3f(shift_LR,10,shift_UD));
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
		for(int i=0; i<module_max; i++){robot[i].Forward();}
		//robot[module_max-1].leg[0].jntDX2->SetSpringOrigin(Rad(80.0));
		//robot[module_max-1].leg[0].jntFoot->SetSpringOrigin(Rad(10.0));
		break;
	case 's':
		for(int i=0; i<module_max; i++){robot[i].Backward();}
		//robot[module_max-1].leg[0].jntDX2->SetSpringOrigin(Rad(-60.0));
		//robot[module_max-1].leg[0].jntFoot->SetSpringOrigin(Rad(150.0));
		break;
	case 'z':
		zoom -= 0.01;
		glutPostRedisplay();
		break;
	case 'x':
		zoom += 0.01;
		glutPostRedisplay();
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
		NowTime += 0.05;
		break;
	case 'm':
		NowTime -= 0.05;
		break;
	case 'd':
		for(int i=0; i<module_max; i++){robot[i].Stop();}
		break;
	case 'o':
		for(int i=0; i<module_max; i++){
			robot[i].soBody->SetDynamical(true);
		}
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

	static LARGE_INTEGER	freq, start, now, past;			// ���g���C�J�n���ԁC���ݎ��ԁC�O�񎞊�

	// ���Ԍv������
	start.QuadPart = now.QuadPart = past.QuadPart = 0;
	QueryPerformanceFrequency( &freq );	// �v�Z�@�̎��g���v��

	glutTimerFunc(100, timer, 0);

	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	if (stepOnTimer){
		for(int i = 0; i < 1; i++)
		scene->Step();

		// �ʑ�����
		/*QueryPerformanceCounter( &now );
		NowTime = (double)( now.QuadPart - start.QuadPart ) / (double)freq.QuadPart;	// �o�ߎ��Ԃ̌v�Z
		s_time = (double)( now.QuadPart - past.QuadPart ) / (double)freq.QuadPart;
		past.QuadPart = now.QuadPart;*/	

		Phase = (2 * PI / CycleTime) * NowTime;
	
		for(int module_num=0; module_num<module_max; module_num++){
				for(int leg_num = 1; leg_num < 3; leg_num++){
				double	PhaseTemp;
				PhaseTemp = Phase + leg_num * 2 * PI / 2.0 + module_num * 2 * PI / 2.0;
				//PhaseTemp[module_num * 2 + leg_num]= Phase[module_num * 2 + leg_num];
				//PhaseTemp[module_num * 2 + leg_num]= Phase[module_num * 2 + leg_num] + leg_num * 2 * PI / 2.0 + module_num * 2 * PI / 2.0;
				//PhaseTemp[module_num * 2 + leg_num]= Phase[module_num * 2 + leg_num] + leg_num * 2 * PI / 2.0;

				// ����O���̌v�Z�i�ȉ~�O���j
				double	xx = para_X1;
				//double	yy = pow(-1, leg_num) * para_a1 * cos(PhaseTemp[module_num * 2 + leg_num]);
				double	yy = pow(-1, leg_num) * para_a1 * cos(PhaseTemp);
				//double	yy = pow(-1, module_num) * para_a1 * cos(PhaseTemp[module_num * 2 + leg_num]);
				//double	yy = para_a1 * cos(PhaseTemp[module_num * 2 + leg_num]);
				double	zz;
					if(sin(PhaseTemp) < 0.0)	zz = - para_H1;
					//if(sin(PhaseTemp[module_num * 2 + leg_num]) < 0.0)	zz = - para_H1;
					else zz = para_b1 * sin(PhaseTemp) - para_H1;
					//else zz = para_b1 * sin(PhaseTemp[module_num * 2 + leg_num]) - para_H1;

				// �e�֐ߊp�x�̌v�Z
				double	theta1 = CalcTheta1(xx, yy);
				if(leg_num)theta1 += pow(-1, leg_num) * PI / 6.0;
				double	theta2 = CalcTheta2(xx, yy, zz, L1, L2, L3);
				double	theta3 = CalcTheta3(xx, yy, zz, L1, L2, L3);

				// �O���t�B�b�N�\���̂��߂Ɋe�l���Z�b�g
				robot[module_num].leg[leg_num].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
				robot[module_num].leg[leg_num].jntDX2  ->  SetSpringOrigin(-theta2);
				robot[module_num].leg[leg_num].jntFoot ->  SetSpringOrigin(-theta3);
			}
		}

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
	dscene.timeStep = 0.05;
	dscene.numIteration = 5;
	scene = phSdk->CreateScene(dscene);			// �V�[���̍쐬
	scene->SetGravity(Vec3f(0.0, -9.8, 0.0));	//	�d�͂�ݒ�
	// �V�[���̍\�z
	CreateFloor();								//	��

	Posed pose;
	pose.Ori() = Quaterniond::Rot(Rad(120.0), 'y');
	for(int i=0; i<module_max; i++){			//ASHIGARU�\�z
		pose.Pos() = Vec3d(0.3*i, 0.17, 0.0);
		robot[i].Build_root(pose, scene, phSdk);
	}


//////���W���[�����񌋍��\�z/////////////////////////////////////////////////////////////////////////

	PHHingeJointDesc jdConnect;

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
		}
	}
	scene->SetContactMode(&allSolids[0], allSolids.size(), PHSceneDesc::MODE_NONE);

	double K = 10000.0, D = 0.0;
	for(int i=0; i<module_max; i++){
		robot[i].leg[0].jntDX1  -> SetSpringOrigin(Rad(-90.0));//�����r(leg[0])�̎p����ݒ�
		robot[i].leg[0].jntDX1  -> SetSpring(K);
		robot[i].leg[0].jntDX1  -> SetDamper(D);
		robot[i].leg[0].jntDX2  -> SetSpringOrigin(Rad(-90.0));
		robot[i].leg[0].jntDX2  -> SetSpring(K);
		robot[i].leg[0].jntDX2  -> SetDamper(D);
		robot[i].leg[0].jntFoot -> SetSpringOrigin(Rad(180.0));
		robot[i].leg[0].jntFoot -> SetSpring(K);
		robot[i].leg[0].jntFoot -> SetDamper(D);
	}

	//���������\�z
	jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	for(int i=0; i<module_max-1; i++){
		robot[i].leg[0].jntConnect[0] = scene->CreateJoint(robot[i+1].soBody, robot[i].soBody, jdConnect)->Cast();
		robot[i].leg[0].jntConnect[0]->SetSpring(K);
		robot[i].leg[0].jntConnect[0]->SetDamper(D);
		robot[i].leg[0].jntConnect[0]->SetSpringOrigin(Rad(0.0));
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
