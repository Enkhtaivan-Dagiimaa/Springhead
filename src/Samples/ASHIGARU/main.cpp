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

#define CN 9 
#define CK 4 
typedef unsigned int set; 
#define first(n) ((set) ((1U << (n)) - 1U))

using namespace Spr;

#define ESC		27
#define module_max 4

UTRef<PHSdkIf> phSdk;			// SDK
UTRef<GRSdkIf> grSdk;
UTRef<PHSceneIf> scene;			// Scene
UTRef<GRDebugRenderIf> render;
UTRef<GRDeviceGLIf> device;

INT_PTR CALLBACK MainDlgProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);	// ���C���_�C�A���O

// ���C���_�C�A���O�̃n���h��
HWND		g_hDlgMain;		// �n���h��
HINSTANCE	g_hInstMain;	// �C���X�^���X

Robot robot[50];//�Ƃ肠����ASHIGARU�ő�P�O��

double zoom = 0.2;

//CPG�֘A
double Tr=0.1;//u�̎��萔
double ae=1.5;//�Q�̃j���[�����̌����W��
double af=1.5;//�Q�̃j���[�����̌����W��
double s=1.0;//�O������
double b=2.5;//��J�W��
double Ta=0.5;//v�̎��萔
double U[10];//�j���[�����̓������
double V[10];//�j���[�����̎��ȗ}���x
double WY[10];//�d�݂Əo�͂̐�
double w[10][10];//�����W��
double Y[10], Y_Past[10];//�b�o�f�o��
double smp[10];//�ő�U��
double ue[10], ye[10], ve[10], sume[10];//e�j���[�����Ɋւ���o��
double uf[10], yf[10], vf[10], sumf[10];//f�j���[�����Ɋւ���o��

double para_X1 = 0.11, para_a1 = 0.03, para_b1 = 0.05, para_H1 = 0.17;
double	L1 = 0.067, L2 = 0.067, L3 = 0.125;	// �֐ߊԒ���
double   direction_theta = 0.0;  //�i�s����

double			NowTime = 0.0;	// ����
double			s_time = 0.0;	// �T���v�����O�^�C��������
double			CycleTime = 1.0;
double			Phase = 0.0;

bool stepOnTimer = false;
int turn_flag = 0;


//CPG�v�Z�p�֐�����////////////////////////////////////////////////////////////////////////////////////////////
double _max(double x) {return ((x>0) ? x : 0);}//x���O���傫�����ǂ����𔻕ʂ���֐�

double CalcU(int cpgnum, double u, double a, double y, double v, double sum)//�j���[�����̓�����Ԃ��v�Z����֐�
{
	U[cpgnum] = ( - u - a*y + s - b*v - sum ) / Tr;

	return U[cpgnum];
}

double CalcV(int cpgnum, double v, double y)//�j���[�����̎��ȗ}���x���v�Z����֐�
{
	V[cpgnum] = ( - v + y ) / Ta;
	return V[cpgnum];
}

double CalcWY(int cpgnum, int othercpg, double y)//�d�݂Əo�͂̐ς��v�Z����֐�
{	
	WY[cpgnum] = w[othercpg][cpgnum] * y;
	return WY[cpgnum];
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void CreateFloor(){
	PHSolidDesc sd;
	PHSolidIf* soFloor = scene->CreateSolid(sd);
	CDBoxDesc bd;
	bd.boxsize = Vec3f(3.0f, 0.3f, 3.0f);
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
		NowTime += 0.05;
		break;
	case 's':
		NowTime -= 0.05;
		break;
	case 'w':
		turn_flag++ ;
		break;
	case 'z':
		zoom -= 0.01;
		//glutPostRedisplay();
		break;
	case 'x':
		zoom += 0.01;
		//glutPostRedisplay();
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

	glutTimerFunc(50, timer, 0);

	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	if (stepOnTimer){
		//for(int i = 0; i < 1; i++)
		scene->Step();

		// �ʑ�����
		QueryPerformanceCounter( &now );
		NowTime = (double)( now.QuadPart - start.QuadPart ) / (double)freq.QuadPart;	// �o�ߎ��Ԃ̌v�Z
		s_time = (double)( now.QuadPart - past.QuadPart ) / (double)freq.QuadPart;
		past.QuadPart = now.QuadPart;

		Phase = (2 * PI / CycleTime) * NowTime;
	
	//���񌋍�
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
			double	yy; 
					if(turn_flag%2 == 0)yy = pow(-1, leg_num) * para_a1 * cos(PhaseTemp);
			        else if(turn_flag%2 == 1)yy = para_a1 * cos(PhaseTemp);
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

	//�o�g���h���C�h
		// module0�i�r�j
			/*for(int leg_num = 1; leg_num < 3; leg_num++){

				double	PhaseTemp = Phase + leg_num * 2 * PI / 2.0;

				// ����O���̌v�Z�i�ȉ~�O���j
				double	zz = - para_a1 * cos(PhaseTemp);
				double	yy = 0.0;
				double	xx;
					if(sin(PhaseTemp) < 0.0)	xx = - para_H1;
					else						xx = para_b1 * sin(PhaseTemp) - para_H1;

				// �e�֐ߊp�x�̌v�Z
				double	theta1 = 0.0;
				if(leg_num)
					theta1 -= pow(-1, leg_num) * PI / 3.0;
				double	theta2 = atan(zz / xx) + acos( (L2*L2 + xx*xx + zz*zz - L3*L3) / (2 * L2 * sqrt(xx*xx + zz*zz)) );
				double	theta3 = - PI + acos( (L2*L2 + L3*L3 - xx*xx - zz*zz) / (2 * L2 * L3) );

				// �O���t�B�b�N�\���̂��߂Ɋe�l���Z�b�g
				robot[0].leg[leg_num].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
				robot[0].leg[leg_num].jntDX2  ->  SetSpringOrigin(-theta2);
				robot[0].leg[leg_num].jntFoot ->  SetSpringOrigin(-theta3);

			}

			// module1�i�r�C��j
			double	theta0 = -20.0 * PI / 180.0 * sin(Phase);
			robot[1].leg[0].jntDX1  ->  SetSpringOrigin(-theta0-Rad(90));

			for(int leg_num = 1; leg_num < 3; leg_num++){

				double	PhaseTemp = Phase + leg_num * 2 * PI / 2.0 + 2 * PI / 2.0;

				// ��O���̌v�Z�i�ȉ~�O���j
				double	xx = para_X1;
				double	yy = pow(-1, leg_num) * para_a1 * cos(PhaseTemp);
				double	zz = para_b1 * sin(PhaseTemp) - 0.10;
					//if(sin(PhaseTemp) < 0.0)	zz = - 0.12;
					//else						zz = para_b1 * sin(PhaseTemp) - 0.12;

				// �e�֐ߊp�x�̌v�Z
				double	theta1 = 0.0;
				//if(leg_num)
				//	theta1 += pow(-1, leg_num) * PI / 3.0;
				double	theta2 = CalcTheta2(xx, yy, zz, L1, L2, L3);
				double	theta3 = CalcTheta3(xx, yy, zz, L1, L2, L3);

				// �O���t�B�b�N�\���̂��߂Ɋe�l���Z�b�g
				robot[1].leg[leg_num].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
				robot[1].leg[leg_num].jntDX2  ->  SetSpringOrigin(-theta2);
				robot[1].leg[leg_num].jntFoot ->  SetSpringOrigin(-theta3);

			}*/

	//�R���W���[����
		//module0//leg2
		/*double	PhaseTemp = Phase;

		// ����O���̌v�Z�i�ȉ~�O���j
		double	xx = 0.0;
		double	yy = pow(-1, 2) * para_a1 * cos(PhaseTemp);
		double	zz;
			if(sin(PhaseTemp) < 0.0)	zz =  - para_H1;
			else						zz =  para_b1 * sin(PhaseTemp) - para_H1;
		double  YY = xx * cos(direction_theta + PI/2.0) - yy * sin(direction_theta + PI/2.0);
		double  XX = xx * sin(direction_theta + PI/2.0) + yy * cos(direction_theta + PI/2.0) + para_X1;

		// �e�֐ߊp�x�̌v�Z
		double	theta1 = CalcTheta1(XX, YY);
		double	theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		double	theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[0].leg[2].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[0].leg[2].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[0].leg[2].jntFoot ->  SetSpringOrigin(-theta3);

		//leg1
		PhaseTemp = Phase + 1 * 2 * PI / 2.0;

		// ����O���̌v�Z�i�ȉ~�O���j
		xx = 0.0;
		yy = pow(-1, 2) * para_a1 * cos(PhaseTemp);
		zz;
			if(sin(PhaseTemp) < 0.0)	zz = - para_H1;
			else						zz = para_b1 * sin(PhaseTemp) - para_H1;
		YY = xx * cos(direction_theta - PI/6) - yy * sin(direction_theta - PI/6);
		XX = xx * sin(direction_theta - PI/6) + yy * cos(direction_theta - PI/6) + para_X1;

		// �e�֐ߊp�x�̌v�Z
		theta1 = CalcTheta1(XX, YY);
		theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[0].leg[1].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[0].leg[1].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[0].leg[1].jntFoot ->  SetSpringOrigin(-theta3);

	//module1//leg2
		PhaseTemp = Phase;

		// ����O���̌v�Z�i�ȉ~�O���j
			xx = 0.0;
			yy = pow(-1, 1) * para_a1 * cos(PhaseTemp);
			zz;
				if(sin(PhaseTemp) < 0.0)	zz = - para_H1;
				else						zz = para_b1 * sin(PhaseTemp) - para_H1;
			YY = xx * cos(direction_theta + PI/6) - yy * sin(direction_theta + PI/6);
			XX = xx * sin(direction_theta + PI/6) + yy * cos(direction_theta + PI/6) + para_X1;

		// �e�֐ߊp�x�̌v�Z
		theta1 = CalcTheta1(XX, YY);
		theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[1].leg[2].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[1].leg[2].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[1].leg[2].jntFoot ->  SetSpringOrigin(-theta3);

		//leg1
		PhaseTemp = Phase + 1 * 2 * PI / 2.0;

		// ����O���̌v�Z�i�ȉ~�O���j
		xx = 0.0;
		yy = pow(-1, 0) * para_a1 * cos(PhaseTemp);
		zz;
			if(sin(PhaseTemp) < 0.0)	zz =  - para_H1;
			else						zz =  para_b1 * sin(PhaseTemp) - para_H1;
		YY = xx * cos(direction_theta + PI/2.0) - yy * sin(direction_theta + PI/2.0);
		XX = xx * sin(direction_theta + PI/2.0) + yy * cos(direction_theta + PI/2.0) + para_X1;

		// �e�֐ߊp�x�̌v�Z
		theta1 = CalcTheta1(XX, YY);
		theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[1].leg[1].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[1].leg[1].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[1].leg[1].jntFoot ->  SetSpringOrigin(-theta3);

	// module2//leg2
		PhaseTemp = Phase;

		// ����O���̌v�Z�i�ȉ~�O���j
			xx = 0.0;
			yy = pow(-1, 1) * para_a1 * cos(PhaseTemp);
			zz;
				if(sin(PhaseTemp) < 0.0)	zz = - para_H1;
				else						zz = para_b1 * sin(PhaseTemp) - para_H1;
			YY = xx * cos(direction_theta - PI/6) - yy * sin(direction_theta - PI/6);
			XX = xx * sin(direction_theta - PI/6) + yy * cos(direction_theta - PI/6) - para_X1;

		// �e�֐ߊp�x�̌v�Z
		theta1 = CalcTheta1(XX, YY);
		theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[2].leg[2].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[2].leg[2].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[2].leg[2].jntFoot ->  SetSpringOrigin(-theta3);

	//leg1
		PhaseTemp = Phase + 2 * PI / 2.0;

		// ����O���̌v�Z�i�ȉ~�O���j
		xx = 0.0;
		yy = pow(-1, 1) * para_a1 * cos(PhaseTemp);
		zz;
			if(sin(PhaseTemp) < 0.0)	zz = - para_H1;
			else						zz = para_b1 * sin(PhaseTemp) - para_H1;
		YY = xx * cos(direction_theta + PI/6) - yy * sin(direction_theta + PI/6);
		XX = xx * sin(direction_theta + PI/6) + yy * cos(direction_theta + PI/6) + para_X1;

		// �e�֐ߊp�x�̌v�Z
		theta1 = CalcTheta1(XX, YY);
		theta2 = CalcTheta2(XX, YY, zz, L1, L2, L3);
		theta3 = CalcTheta3(XX, YY, zz, L1, L2, L3);

		robot[2].leg[1].jntDX1  ->  SetSpringOrigin(-theta1-Rad(90));
		robot[2].leg[1].jntDX2  ->  SetSpringOrigin(-theta2);
		robot[2].leg[1].jntFoot ->  SetSpringOrigin(-theta3);*/

		glutPostRedisplay();
	}
}

/* brief		���C���֐� */
int main(int argc, char* argv[]){


	//CPG�n������
	for(int i=1; i<7; i++){
		ue[i]=0.0; uf[i]=0.0;
		ve[i]=ue[i]; vf[i]=uf[i];
		sume[i]=0.0; sumf[i]=0.0;
	}

	ue[1]=0.5;	//CPG�������G�l���M�[�H���ꂪ�Ȃ��ƐU�����܂���B

	for(int i=1; i<7; i++){//�����W����������
		for(int j=1; j<7; j++){
			w[i][j]=0.0;
		}
	}

	for(int i=1; i<7; i++)smp[i] = 0;
	//for(int i=1; i<7; i++)n[i] = 0;

	////�����W����ݒ�@�Q�ڂ̐������e�����󂯂�CPG�̔ԍ�
	w[1][3] = 0.5; w[3][1] = 0.5;
	w[2][4] = 0.5; w[4][2] = 0.5;
	w[1][2] = 0.5; w[2][1] = 0.5;
	w[3][4] = 0.5; w[4][3] = 0.5;

	w[3][5] = 0.5; w[5][3] = 0.5;
	w[4][6] = 0.5; w[6][4] = 0.5;
	w[6][5] = 0.5; w[5][6] = 0.5;

	w[1][6] = 0.5; w[6][1] = 0.5;
	w[2][5] = 0.5; w[5][2] = 0.5;

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

	//�R���W���[����̂Ƃ��͂��̏����`���
	/*pose.Pos() = Vec3d(0.0, 0.17, 0.0);
	robot[0].Build_root(pose, scene, phSdk);
	pose.Ori() = Quaterniond::Rot(Rad(240.0), 'y');
	pose.Pos() = Vec3d(0.2, 0.17, 0.0);
	robot[1].Build_root(pose, scene, phSdk);
	pose.Ori() = Quaterniond::Rot(Rad(360.0), 'y');
	pose.Pos() = Vec3d(0.1, 0.17, -0.173);
	robot[2].Build_root(pose, scene, phSdk);*/

	PHHingeJointDesc jdConnect;

	double K = 10.0, D = 0.0;//�����������K�b�`�K�`��

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
	/*robot[1].leg[0].jntDX2  -> SetSpringOrigin(Rad(0.0));
	robot[1].leg[0].jntDX2  -> SetSpring(K);
	robot[1].leg[0].jntDX2  -> SetDamper(D);
	robot[1].leg[0].jntFoot -> SetSpringOrigin(Rad(120.0));
	robot[1].leg[0].jntFoot -> SetSpring(K);
	robot[1].leg[0].jntFoot -> SetDamper(D);*/

//////���W���[�����񌋍��\�z/////////////////////////////////////////////////////////////////////////

	for(int i=0; i<module_max; i++){
		robot[i].leg[1].jntDX1 -> SetSpringOrigin(Rad(-60.0));
		robot[i].leg[2].jntDX1 -> SetSpringOrigin(Rad(-120.0));
	}

	//���������\�z
	jdConnect.poseSocket.Ori() = Quaterniond();
	jdConnect.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jdConnect.posePlug.Pos() = Vec3d(-0.1, 0.0, 0.173);
	for(int i=0; i<module_max-1; i++){
		//robot[i].leg[0].jntConnect[0] = scene->CreateJoint(robot[i+1].soBody, robot[i].soBody, jdConnect)->Cast();
		//robot[i].leg[0].jntConnect[0]->SetSpring(K);
		//robot[i].leg[0].jntConnect[0]->SetDamper(D);
		//robot[i].leg[0].jntConnect[0]->SetSpringOrigin(Rad(0.0));
		robot[i].jntConnect = scene->CreateJoint(robot[i+1].soBody, robot[i].soBody, jdConnect)->Cast();
		robot[i].jntConnect->SetSpring(K);
		robot[i].jntConnect->SetDamper(D);
		robot[i].jntConnect->SetSpringOrigin(Rad(0.0));
	}

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
	/*robot[2].jntConnect = scene->CreateJoint(robot[0].soBody, robot[2].soBody, jdConnect)->Cast();
	robot[2].jntConnect->SetSpring(K);
	robot[2].jntConnect->SetDamper(D);
	robot[2].jntConnect->SetSpringOrigin(Rad(0.0));*/


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
