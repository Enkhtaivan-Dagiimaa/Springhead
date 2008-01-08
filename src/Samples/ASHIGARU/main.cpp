/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/*
 Springhead2/src/Samples/ASHIGARU/main.cpp

�y�T�v�z�R�r���W�����[���{�b�gASHIGARU�̕��s�V�~�����[�^
 
�y�d�l�z���������̐ݒ�y�у��W���[���̏����ݒ��ς��邱�ƂŁu3���W���[������v�u3���W���[����v�u4���W���[�����ˏ�v�ɑΉ�

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

//#include "JoyStick.h"		// JoyStick�N���X
//JoyStick	myjs;			// JoyStick�I�u�W�F�N�g

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

FILE *fd;					//�g�����f���o���p

using namespace Spr;

#define ESC		27
#define module_max 4

double K = 1000.0, D = 0.0;//�����������K�b�`�K�`��

UTRef<PHSdkIf> phSdk;			// SDK
UTRef<GRSdkIf> grSdk;
UTRef<PHSceneIf> scene;			// Scene
UTRef<GRDebugRenderIf> render;
UTRef<GRDeviceGLIf> device;

void GetTrajectory(int module_num, int leg_num, double para_a1, double para_b1, double para_X1, double para_H1, double Phase[], double direction);
void GetLegPosition(int module_num);

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);	// ���C���_�C�A���O

// ���C���_�C�A���O�̃n���h��
HWND		g_hDlgMain;		// �n���h��
HINSTANCE	g_hInstMain;	// �C���X�^���X

Robot robot[50];//�Ƃ肠����ASHIGARU�ő�P�O��

double zoom = 0.2;

double para_X1 = 0.11, para_a1 = 0.03, para_b1 = 0.05, para_H1 = 0.17;
double	L1 = 0.067, L2 = 0.067, L3 = 0.125;	// �֐ߊԒ���
double   direction_theta = 0.0;  //�i�s����

double			NowTime = 0.0;	// ����
double			s_time = 0.0;	// �T���v�����O�^�C��������
double			CycleTime = 1.2;
double			Phase = 0.0;
double			PhaseTemp[50];

bool stepOnTimer = false;
int turn_flag = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateFloor(){
	PHSolidDesc sd;
	PHSolidIf* soFloor = scene->CreateSolid(sd);
	CDBoxDesc bd;
	bd.material.mu = 5.0;
	bd.material.mu0 = 5.0;
	bd.boxsize = Vec3f(10.0f, 0.3f, 10.0f);
	soFloor->AddShape(phSdk->CreateShape(bd));
	soFloor->SetFramePosition(Vec3f( 0, -0.15, 0));
	soFloor->SetDynamical(false);					// ���͊O�͂ɂ���ē����Ȃ��悤�ɂ���
}

/* brief     	glutDisplayFunc�Ŏw�肷��R�[���o�b�N�֐��D��ʂ̕`����s���D
   param	 	�Ȃ�
   return 	    �Ȃ�*/
void Display(){
	Affinef af;
	af.Pos() = Vec3f(0, 3.5, 4)*zoom;
	af.LookAtGL(Vec3f(0.6,0,0.1), Vec3f(0.1,10,0.1));
	render->SetViewMatrix(af.inv());	//	���_�̐ݒ�
	
	render->ClearBuffer();
	render->DrawScene(scene);
	render->EndScene();
	glutSwapBuffers();
}

/*  brief     	�����̐ݒ�
   param	 	�Ȃ�
   return 	    �Ȃ�*/
void setLight() {
	GRLightDesc light0, light1;
	light0.position = Vec4f(20.0, 40.0, 40.0, 0.1);
	//light1.position = Vec4f(-20.0, 20.0, 20.0, 0.1);
	render->PushLight(light0);
	//render->PushLight(light1);
}

/* brief     	����������
   param	 	�Ȃ�
   return 	    �Ȃ�*/
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	setLight();
}

/* brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
   param		<in/--> w�@�@��
   param		<in/--> h�@�@����
   return		�Ȃ�*/
void Reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	render->Reshape(Vec2f(), Vec2f(w,h));
}

/* brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
  param		    <in/--> key�@�@ ASCII�R�[�h
  param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
  param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
  return 	�Ȃ�	*/
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
		NowTime += 0.03;
		//direction_theta += 0.008;
		break;
	case 's':
		NowTime -= 0.03;
		//direction_theta -= 0.008;
		break;
	case 'w':
		turn_flag++ ;
		break;
	case 'z':
		zoom -= 0.01;
		break;
	case 'x':
		zoom += 0.01;
		break;
	case 't':
		stepOnTimer = !stepOnTimer;
		break;
	case 'p':
		stepOnTimer = false;
		scene->Step();
		glutPostRedisplay();
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

/* brief  	    glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
   param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
   return 	    �Ȃ� */
void timer(int id){

	static LARGE_INTEGER	freq, start, now, past;			// ���g���C�J�n���ԁC���ݎ��ԁC�O�񎞊�

	// ���Ԍv������
	start.QuadPart = now.QuadPart = past.QuadPart = 0;
	QueryPerformanceFrequency( &freq );	// �v�Z�@�̎��g���v��
	glutTimerFunc(100, timer, 0);
	direction_theta = NowTime/5;

	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	if (stepOnTimer){
		
		scene->Step();

		//std::cout << robot[0].soBody->GetFramePosition();
		fprintf(fd,"%3f	%3f	%3f	%3f\n", NowTime , robot[0].soBody->GetFramePosition());

		// �ʑ�����
		QueryPerformanceCounter( &now );
		//NowTime = (double)( now.QuadPart - start.QuadPart ) / (double)freq.QuadPart;	// �o�ߎ��Ԃ̌v�Z
		s_time = (double)( now.QuadPart - past.QuadPart ) / (double)freq.QuadPart;
		past.QuadPart = now.QuadPart;

		Phase = (2 * PI / CycleTime) * NowTime;
	
	//3���W���[�����񌋍��̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;//���W���[���̎p���ݒ�

		//robot[0].leg[0].Joint_flag = true;	robot[1].leg[0].Joint_flag = true;//�����r�w��

		//PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase;//�e�r�̈ʑ��w��
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = Phase + PI;
		//PhaseTemp[6] = Phase + PI;	PhaseTemp[7] = Phase;				PhaseTemp[8] = Phase;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//3���W���[���󌋍��̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 2*PI/3;	robot[2].theta = -2*PI/3;//���W���[���̎p���ݒ�

		//robot[0].leg[0].Joint_flag = true;	robot[1].leg[0].Joint_flag = true;	robot[2].leg[0].Joint_flag = true;//�����r�w��

		//PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase;			PhaseTemp[5] = Phase + PI;
		//PhaseTemp[6] = 0.0;				PhaseTemp[7] = Phase;			PhaseTemp[8] = Phase + PI;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//4���W���[�����񌋍��̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;	robot[3].theta = 0.0;//���W���[���̎p���ݒ�

		//robot[0].leg[0].Joint_flag = true;	robot[1].leg[0].Joint_flag = true;	robot[2].leg[0].Joint_flag = true;//�����r�w��

		//PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase;//�e�r�̈ʑ��w��(����]�T�̂�)
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = Phase + PI;
		//PhaseTemp[6] = 0.0;				PhaseTemp[7] = Phase;			PhaseTemp[8] =Phase;
		//PhaseTemp[9] = Phase;			PhaseTemp[10] = Phase + PI;		PhaseTemp[11] = Phase + PI;

		//PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(�S���l��)
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = Phase;
		//PhaseTemp[6] = 0.0;				PhaseTemp[7] = Phase + PI;		PhaseTemp[8] = Phase + PI;
		//PhaseTemp[9] = Phase + PI;		PhaseTemp[10] = Phase;			PhaseTemp[11] = Phase;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//4���W���[��U�^�����̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;	robot[3].theta = 0.0;//���W���[���̎p���ݒ�

		robot[0].leg[0].Joint_flag = true;	robot[2].leg[1].Joint_flag = true;	robot[2].leg[2].Joint_flag = true;//�����r�w��
		robot[1].leg[2].Joint_flag = true;

		PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(����]�T�̂�)
		PhaseTemp[3] = Phase;			PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = 0.0;
		PhaseTemp[6] = Phase + PI;		PhaseTemp[7] = 0.0;				PhaseTemp[8] =0.0;
		PhaseTemp[9] = Phase;			PhaseTemp[10] = Phase + PI;		PhaseTemp[11] = Phase;

		//PhaseTemp[0] = 0.0;				PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(�S���l��)
		//PhaseTemp[3] = Phase;			PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase + PI;		PhaseTemp[7] = 0.0;				PhaseTemp[8] = 0.0;
		//PhaseTemp[9] = Phase;			PhaseTemp[10] = Phase;			PhaseTemp[11] = Phase + PI;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//4���W���[�����ˏ󌋍��̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;	robot[3].theta = 0.0;//���W���[���̎p���ݒ�

		//robot[0].leg[0].Joint_flag = true;	robot[2].leg[1].Joint_flag = true;	robot[3].leg[2].Joint_flag = true;//�����r�w��
		////robot[1].leg[0].Joint_flag = true;	robot[1].leg[1].Joint_flag = true;	robot[1].leg[2].Joint_flag = true;//�����r�w��

		//PhaseTemp[0] = 0.0;					PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(����]�T�̂�)
		//PhaseTemp[3] = Phase + PI;			PhaseTemp[4] = Phase;			PhaseTemp[5] = Phase + PI;
		////PhaseTemp[3] = 0.0;					PhaseTemp[4] = 0.0;				PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase + PI;			PhaseTemp[7] = 0.0;				PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase;				PhaseTemp[10] = Phase + PI;		PhaseTemp[11] = 0.0;

		//PhaseTemp[0] = 0.0;					PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(�S���l��)
		//PhaseTemp[3] = Phase + PI;			PhaseTemp[4] = Phase + PI;			PhaseTemp[5] = Phase;
		////PhaseTemp[3] = 0.0;					PhaseTemp[4] = 0.0;				PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase;			PhaseTemp[7] = 0.0;				PhaseTemp[8] = Phase + PI;
		//PhaseTemp[9] = Phase + PI;				PhaseTemp[10] = Phase;		PhaseTemp[11] = 0.0;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//6���W���[���s��`�̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;//���W���[���̎p���ݒ�
		//robot[3].theta = 0.0;	robot[4].theta = 0.0;	robot[5].theta = 0.0;

		//robot[1].leg[0].Joint_flag = true;	robot[1].leg[2].Joint_flag = true;	robot[3].leg[1].Joint_flag = true;//�����r�w��
		//robot[3].leg[2].Joint_flag = true;	robot[4].leg[2].Joint_flag = true;

		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase;			PhaseTemp[2] = Phase + PI;//�e�r�̈ʑ��w��(����]�T�{�ʐρ{���U���l�������Ƃ��̍œK��)
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase + PI;		PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase;			PhaseTemp[7] = Phase + PI;		PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase + PI;		PhaseTemp[10] = 0.0;			PhaseTemp[11] = 0.0;
		//PhaseTemp[12] = Phase + PI;		PhaseTemp[13] = Phase;			PhaseTemp[14] = 0.0;
		//PhaseTemp[15] = Phase;			PhaseTemp[16] = Phase;			PhaseTemp[17] = Phase + PI;

		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase + PI;		PhaseTemp[2] = Phase;//�e�r�̈ʑ��w��(���[�e�[�V�����@�̉�)
		//PhaseTemp[3] = 0.0;				PhaseTemp[4] = Phase;			PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase;			PhaseTemp[7] = Phase + PI;		PhaseTemp[8] = Phase + PI;
		//PhaseTemp[9] = Phase;			PhaseTemp[10] = 0.0;			PhaseTemp[11] = 0.0;
		//PhaseTemp[12] = Phase + PI;		PhaseTemp[13] = Phase + PI;		PhaseTemp[14] = 0.0;
		//PhaseTemp[15] = Phase;			PhaseTemp[16] = Phase;			PhaseTemp[17] = Phase + PI;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//6���W���[�����̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;//���W���[���̎p���ݒ�
		//robot[3].theta = 0.0;	robot[4].theta = 0.0;	robot[5].theta = 0.0;

		//robot[0].leg[2].Joint_flag = true;	robot[2].leg[1].Joint_flag = true;	robot[3].leg[1].Joint_flag = true;//�����r�w��
		//robot[4].leg[0].Joint_flag = true;	robot[5].leg[0].Joint_flag = true;

		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase + PI;			PhaseTemp[2] = 0.0;//�e�r�̈ʑ��w��(����]�T)
		//PhaseTemp[3] = Phase + PI;				PhaseTemp[4] = Phase;		PhaseTemp[5] = Phase;
		//PhaseTemp[6] = Phase;			PhaseTemp[7] = 0.0;		PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase + PI;		PhaseTemp[10] = 0.0;			PhaseTemp[11] = Phase + PI;
		//PhaseTemp[12] = 0.0;		PhaseTemp[13] = Phase + PI;			PhaseTemp[14] = Phase;
		//PhaseTemp[15] = 0.0;			PhaseTemp[16] = Phase + PI;			PhaseTemp[17] = Phase + PI;

		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase + PI;		PhaseTemp[2] = 0.0;//�e�r�̈ʑ��w��(�S���l��)
		//PhaseTemp[3] = Phase + PI;		PhaseTemp[4] = Phase + PI;			PhaseTemp[5] = Phase;
		//PhaseTemp[6] = Phase;			PhaseTemp[7] = 0.0;				PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase + PI;		PhaseTemp[10] = 0.0;			PhaseTemp[11] = Phase;
		//PhaseTemp[12] = 0.0;			PhaseTemp[13] = Phase;			PhaseTemp[14] = Phase + PI;
		//PhaseTemp[15] = 0.0;			PhaseTemp[16] = Phase;			PhaseTemp[17] = Phase + PI;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//6���W���[���u�[�������̐ݒ�////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//robot[0].theta = 0.0;	robot[1].theta = 0.0;	robot[2].theta = 0.0;//���W���[���̎p���ݒ�
		//robot[3].theta = 0.0;	robot[4].theta = 0.0;	robot[5].theta = 0.0;

		//robot[0].leg[2].Joint_flag = true;	robot[1].leg[2].Joint_flag = true;	robot[3].leg[1].Joint_flag = true;//�����r�w��
		//robot[4].leg[1].Joint_flag = true;	robot[5].leg[1].Joint_flag = true;
		
		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase + PI;		PhaseTemp[2] = 0.0;//�e�r�̈ʑ��w��(����]�T�̂�)
		//PhaseTemp[3] = Phase + PI;		PhaseTemp[4] = Phase;			PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase + PI;		PhaseTemp[7] = Phase + PI;		PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase ;			PhaseTemp[10] = 0.0;			PhaseTemp[11] = Phase + PI;
		//PhaseTemp[12] = Phase;			PhaseTemp[13] = 0.0;			PhaseTemp[14] = Phase;
		//PhaseTemp[15] = Phase + PI;		PhaseTemp[16] = 0.0;			PhaseTemp[17] = Phase;

		//PhaseTemp[0] = Phase;			PhaseTemp[1] = Phase + PI;		PhaseTemp[2] = 0.0;//�e�r�̈ʑ��w��(�S���l��)
		//PhaseTemp[3] = Phase + PI;		PhaseTemp[4] = Phase;			PhaseTemp[5] = 0.0;
		//PhaseTemp[6] = Phase + PI;		PhaseTemp[7] = Phase + PI;		PhaseTemp[8] = Phase;
		//PhaseTemp[9] = Phase ;			PhaseTemp[10] = 0.0;			PhaseTemp[11] = Phase;
		//PhaseTemp[12] = Phase;			PhaseTemp[13] = 0.0;			PhaseTemp[14] = Phase + PI;
		//PhaseTemp[15] = Phase + PI;		PhaseTemp[16] = 0.0;			PhaseTemp[17] = Phase;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//double K = 1000.0, D = 0.0;//�����������K�b�`�K�`��

		GetLegPosition(module_max);//�e���W���[���̎p�������擾
		GetTrajectory(module_max, 3, para_a1, para_b1, para_X1, para_H1, PhaseTemp, direction_theta);//����O���̌v�Z

		for(int i=0; i<module_max; i++){//�����r�̎p����ݒ�
			for(int j=0; j<3; j++){
				if(robot[i].leg[j].Joint_flag == true){
					robot[i].leg[j].jntDX1  -> SetSpringOrigin(Rad(-90.0));robot[i].leg[j].jntDX1  -> SetSpring(K);robot[i].leg[j].jntDX1  -> SetDamper(D);
					robot[i].leg[j].jntDX2  -> SetSpringOrigin(Rad(-90.0));robot[i].leg[j].jntDX2  -> SetSpring(K);robot[i].leg[j].jntDX2  -> SetDamper(D);
					robot[i].leg[j].jntFoot -> SetSpringOrigin(Rad(180.0));robot[i].leg[j].jntFoot -> SetSpring(K);robot[i].leg[j].jntFoot -> SetDamper(D);
				}
			}
		}
		glutPostRedisplay();
	}
}

/* brief		���C���֐� */
int main(int argc, char* argv[]){

	// SDK�̍쐬�@
	phSdk = PHSdkIf::CreateSdk();
	grSdk = GRSdkIf::CreateSdk();

	if ((fd = fopen("data1.txt", "w")) < 0) {printf("Can't open %s", "data1.txt");exit(1);}//�e�L�X�g�������݃t�@�C���I�[�v��

	// �V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc dscene;
	dscene.timeStep = 0.05;
	dscene.numIteration = 5;
	scene = phSdk->CreateScene(dscene);			// �V�[���̍쐬
	scene->SetGravity(Vec3f(0.0, -9.8, 0.0));	//	�d�͂�ݒ�

	// �V�[���̍\�z
	CreateFloor();								//	��

	//�����Ō�����Ԃ̐ݒ���s��
	Posed pose;

	//�����`��ʒu/////////////////////////////////////////////////////////////////////////�����`��ʒu������ɂ��Ȃ��ƃG������ۂ�

	pose.Ori() = Quaterniond::Rot(Rad(120.0), 'y');
	for(int i=0; i<module_max; i++){			//ASHIGARU�\�z
		pose.Pos() = Vec3d(0.3*i, 0.17, 0.0);
		robot[i].Build_root(pose, scene, phSdk);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////////

	PHHingeJointDesc jdConnect;

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

//////���W���[�����񌋍��\�z/////////////////////////////////////////////////////////////////////////

	//���������\�z
	/*jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	for(int i=0; i<module_max-1; i++){
		robot[i].jntConnect = scene->CreateJoint(robot[i+1].soBody, robot[i].soBody, jdConnect)->Cast();
		robot[i].jntConnect->SetSpring(K);
		robot[i].jntConnect->SetDamper(D);
		robot[i].jntConnect->SetSpringOrigin(Rad(0.0));
	}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////�R���W���[���󌋍�////////////////////////////////////////////////////////////////////////////////////////////

	//���������\�z
	/*jdConnect.poseSocket.Ori() = Quaterniond::Rot(Rad(-120.0), 'y');
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[0].jntConnect = scene->CreateJoint(robot[1].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Ori() = Quaterniond::Rot(Rad(-120.0), 'y');
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[1].jntConnect = scene->CreateJoint(robot[2].soBody, robot[1].soBody, jdConnect)->Cast();
	robot[1].jntConnect->SetSpring(K);
	robot[1].jntConnect->SetDamper(D);
	robot[1].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Ori() = Quaterniond::Rot(Rad(-120.0), 'y');
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);*/
	/*robot[2].jntConnect = scene->CreateJoint(robot[0].soBody, robot[2].soBody, jdConnect)->Cast();//����̐ݒ肪���������B����Ȃ��Ă����������l�ɂ͂Ȃ��Ă�B
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////4���W���[��U�^����////////////////////////////////////////////////////////////////////////////////////////////

	//���������\�z
	jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[0].jntConnect = scene->CreateJoint(robot[1].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[2].jntConnect = scene->CreateJoint(robot[1].soBody, robot[2].soBody, jdConnect)->Cast();
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.1, 0.0, 0.173);
	robot[3].jntConnect = scene->CreateJoint(robot[2].soBody, robot[3].soBody, jdConnect)->Cast();
	robot[3].jntConnect->SetSpring(K);
	robot[3].jntConnect->SetDamper(D);
	robot[3].jntConnect->SetSpringOrigin(Rad(0.0));

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////4���W���[�����ˏ󌋍�////////////////////////////////////////////////////////////////////////////////////////////

	//���������\�z
	/*jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[0].jntConnect = scene->CreateJoint(robot[1].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[2].jntConnect = scene->CreateJoint(robot[1].soBody, robot[2].soBody, jdConnect)->Cast();
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[3].jntConnect = scene->CreateJoint(robot[1].soBody, robot[3].soBody, jdConnect)->Cast();
	robot[3].jntConnect->SetSpring(K);
	robot[3].jntConnect->SetDamper(D);
	robot[3].jntConnect->SetSpringOrigin(Rad(0.0));*/

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////6���W���[���s��`///////////////////////////////////////////////////////////////////////////////////////////////////

	/*jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.1, 0.0, -0.173);
	robot[2].jntConnect = scene->CreateJoint(robot[1].soBody, robot[2].soBody, jdConnect)->Cast();
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[1].jntConnect = scene->CreateJoint(robot[0].soBody, robot[1].soBody, jdConnect)->Cast();
	robot[1].jntConnect->SetSpring(K);
	robot[1].jntConnect->SetDamper(D);
	robot[1].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.2, 0.0, 0.0);
	robot[0].jntConnect = scene->CreateJoint(robot[3].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[3].jntConnect = scene->CreateJoint(robot[4].soBody, robot[3].soBody, jdConnect)->Cast();
	robot[3].jntConnect->SetSpring(K);
	robot[3].jntConnect->SetDamper(D);
	robot[3].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[4].jntConnect = scene->CreateJoint(robot[5].soBody, robot[4].soBody, jdConnect)->Cast();
	robot[4].jntConnect->SetSpring(K);
	robot[4].jntConnect->SetDamper(D);
	robot[4].jntConnect->SetSpringOrigin(Rad(0.0));*/
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////6���W���[��r///////////////////////////////////////////////////////////////////////////////////////////////////

	/*jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[0].jntConnect = scene->CreateJoint(robot[1].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[2].jntConnect = scene->CreateJoint(robot[1].soBody, robot[2].soBody, jdConnect)->Cast();
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[4].jntConnect = scene->CreateJoint(robot[1].soBody, robot[4].soBody, jdConnect)->Cast();
	robot[4].jntConnect->SetSpring(K);
	robot[4].jntConnect->SetDamper(D);
	robot[4].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[3].jntConnect = scene->CreateJoint(robot[2].soBody, robot[3].soBody, jdConnect)->Cast();
	robot[3].jntConnect->SetSpring(K);
	robot[3].jntConnect->SetDamper(D);
	robot[3].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	robot[5].jntConnect = scene->CreateJoint(robot[4].soBody, robot[5].soBody, jdConnect)->Cast();
	robot[5].jntConnect->SetSpring(K);
	robot[5].jntConnect->SetDamper(D);
	robot[5].jntConnect->SetSpringOrigin(Rad(0.0));*/
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////6���W���[���u�[������///////////////////////////////////////////////////////////////////////////////////////////////////

	/*jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[0].jntConnect = scene->CreateJoint(robot[1].soBody, robot[0].soBody, jdConnect)->Cast();
	robot[0].jntConnect->SetSpring(K);
	robot[0].jntConnect->SetDamper(D);
	robot[0].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, -0.173);
	robot[1].jntConnect = scene->CreateJoint(robot[2].soBody, robot[1].soBody, jdConnect)->Cast();
	robot[1].jntConnect->SetSpring(K);
	robot[1].jntConnect->SetDamper(D);
	robot[1].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[3].jntConnect = scene->CreateJoint(robot[2].soBody, robot[3].soBody, jdConnect)->Cast();
	robot[3].jntConnect->SetSpring(K);
	robot[3].jntConnect->SetDamper(D);
	robot[3].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[4].jntConnect = scene->CreateJoint(robot[3].soBody, robot[4].soBody, jdConnect)->Cast();
	robot[4].jntConnect->SetSpring(K);
	robot[4].jntConnect->SetDamper(D);
	robot[4].jntConnect->SetSpringOrigin(Rad(0.0));
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(0.2, 0.0, 0.0);
	robot[5].jntConnect = scene->CreateJoint(robot[4].soBody, robot[5].soBody, jdConnect)->Cast();
	robot[5].jntConnect->SetSpring(K);
	robot[5].jntConnect->SetDamper(D);
	robot[5].jntConnect->SetSpringOrigin(Rad(0.0));*/
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//�֐ߊp�x�v�Z�p�֐�
void GetTrajectory(int module_num, int leg_num, double para_a1, double para_b1, double para_X1, double para_H1, double Phase[], double direction){

	double	PhaseTemp[100];
	double	L1 = 0.067, L2 = 0.067, L3 = 0.12;	// �֐ߊԒ���

	for(int i = 0; i < module_num; i++){
		for(int j = 0; j < leg_num; j++){
			PhaseTemp[i * 3 + j] = Phase[i * 3 + j];
			// ����O���̌v�Z�i�ȉ~�O���j
			double	xx = 0.0;
			double	yy = - para_a1 * cos(PhaseTemp[i * 3 + j]);
			double	zz;
				if(sin(PhaseTemp[i * 3 + j]) < 0.0)	zz = - para_H1;
				else zz = para_b1 * sin(PhaseTemp[i * 3 + j]) - para_H1;

			double  YY = xx * cos(direction + robot[i].leg[j].direction) - yy * sin(direction + robot[i].leg[j].direction);
			double  XX = xx * sin(direction + robot[i].leg[j].direction) + yy * cos(direction + robot[i].leg[j].direction) + para_X1;

			// �e�֐ߊp�x�̌v�Z
			double	theta1 = atan(YY / XX);
			double	m = sqrt(XX*XX + YY*YY) - L1;
			double	theta2 = atan(zz / m) + acos( (L2*L2 + m*m + zz*zz - L3*L3) / (2 * L2 * sqrt(m*m + zz*zz)) );
			double	theta3 = - PI + acos( (L2*L2 + L3*L3 - m*m - zz*zz) / (2 * L2 * L3) );

			// �O���t�B�b�N�\���̂��߂Ɋe�l���Z�b�g
			if(robot[i].leg[j].Joint_flag == true)continue;
			else{
				robot[i].leg[j].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
				robot[i].leg[j].jntDX2  ->  SetSpringOrigin(-theta2);
				robot[i].leg[j].jntFoot ->  SetSpringOrigin(-theta3);
			}	
		}
	}
}

//�r�ʒu�擾
void GetLegPosition(int module_num){
	for(int i=0; i<module_num; i++){
		robot[i].leg[0].direction = robot[i].theta;
		robot[i].leg[1].direction = robot[i].theta + 2*PI/3;
		robot[i].leg[2].direction = robot[i].theta - 2*PI/3;
	}
}
