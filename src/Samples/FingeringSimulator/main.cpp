/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/Samples/Joints/main.cpp

�y�T�v�z�֐ߋ@�\�̃f��
*/

//�^�C�~���O�p�i�֐�,tcount�g�p�j

#include <windows.h>
#include <mmsystem.h>
#include <cstdio>

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <sstream>
#include <GL/glut.h>
#include <stdio.h>
#include <time.h>
  


#pragma comment( lib, "winmm.lib" )
#pragma warning(disable : 4996)

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27
#define k 17.817
#define S 1.0            //��̑傫������1.0�}��
#define s 1.0            //�l�b�N�̒�������1.0�}��
#define sp 10000         //�o�l(10000)
#define dp 1000          //�_���p(1000)

PHSdkIf* phSdk;			// SDK�C���^�t�F�[�X
GRSdkIf* grSdk;
PHConstraintIf* phCon;
PHSceneIf* scene;		// Scene�C���^�t�F�[�X
GRDebugRenderIf* render;
GRDeviceGLIf* device;

GRMaterialDesc matBlock(Vec4f(0.5, 0.5, 0.7, 0.9),		
					Vec4f(0.1, 0.5, 0.8, 0.9),		
					Vec4f(1.0, 1.0, 1.0, 0.9),			
					Vec4f(0.0, 0.0, 0.0, 1.0),		
					10.0);

double simulationPeriod = 32.0;
Vec3d lookAt;

int sceneNo;			// �V�[���ԍ�

PHSolidDesc descFloor;					//�����̂̃f�B�X�N���v�^
PHSolidDesc descBox;					//�����̂̃f�B�X�N���v�^
//CDConvexMeshIf* meshBox;				//���`��̃C���^�t�F�[�X
CDShapeIf* shapeBox;
CDShapeIf* shapeSphere;

std::vector<PHSolidIf*> soBox;			//�����̂̃C���^�t�F�[�X
std::vector<PHJointIf*> jntLink;		//�֐߂̃C���^�t�F�[�X

std::vector<PHSolidIf*> soTarget;		
std::vector<PHSpringIf*> soSpring;

/** �����p�ϐ� **/
const double dt = 0.05	;					//�ϕ���
const int niter = 200;						//LCP������
bool bTimer = true;			                //�^�C�}�[�ŃV�~�����[�V������i�߂邩�ǂ���
double	CameraRotX = 0.0, CameraRotY = Rad(30.0), CameraZoom = 1.0;
bool bLeftButton = false, bRightButton = false;
double J = 0.0;                           //�����W�v�Z�p
double X[26],Y[6],x[26];                  //�t���b�g�A�����W�p




void step(){
	scene->ClearForce();
	scene->GenerateForce();
	scene->Integrate();
	glutPostRedisplay();
}


void set(void){                 //������
	X[0] = s;
	for(int i=1; i<22; i++){                   //�t���b�g�Ԋu�v�Z
		x[i] = (2*s-J)/k;
		J += x[i];
		X[i] = s-J;
	}
	
}






void BuildScene(){	// �M�^�[�A�M�^���X�g���f��
	soBox.clear();
	jntLink.clear();
	CDBoxDesc bd;
    PHHingeJointDesc jd;
    CDConvexMeshDesc md;
	CDConvexMeshDesc md1;
	CDConvexMeshDesc md2;
	CDConvexMeshDesc md3;
	PHSliderJointDesc sj;
	PHBallJointDesc bj;
	PHSpringDesc spd;

	//	�M�^�[(�l�b�N) 
	CDConvexMeshIf* meshBlock=NULL;
	md.vertices.push_back(Vec3f(-s,-3*s/120,-s/60));                              
    md.vertices.push_back(Vec3f(-s,-9*s/120, s/60));	
    md.vertices.push_back(Vec3f(-s, 3*s/120,-s/60));	
    md.vertices.push_back(Vec3f(-s, 9*s/120, s/60));
    md.vertices.push_back(Vec3f( s,-3*s/120,-s/60));	
    md.vertices.push_back(Vec3f( s,-6*s/120, s/60));
    md.vertices.push_back(Vec3f( s, 3*s/120,-s/60));
    md.vertices.push_back(Vec3f( s, 6*s/120, s/60));
    meshBlock = DCAST(CDConvexMeshIf, phSdk->CreateShape(md));
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->SetFramePosition(Vec3f(0.0, 0.0, 0.0));
	soBox.back()->AddShape(meshBlock);
	soBox.back()->SetDynamical(false);

	/////////////////////�e�w///////////////////////
	soBox.push_back(scene->CreateSolid(descBox));
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, -0.06*s, -0.02*s);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 60.0;
	jd.spring = 1000.0;
	jd.lower = Rad(15);
	jd.upper = Rad(25);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[1], soBox[0], jd));
	
	
	//	�e�w1 
	bd.boxsize = Vec3f(0.047*S, 0.1*S, 0.047*S);
	CDShapeIf* shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//�X���C�_
	sj = PHSliderJointDesc();
	sj.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.023*S);
	sj.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	sj.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jntLink.push_back(scene->CreateJoint(soBox[2], soBox[1], sj));
	PHSliderJointIf* slider = DCAST(PHSliderJointIf, jntLink.back());
	slider->SetRange(0.0,1.0);
	slider->SetSpring(200);
	slider->SetDamper(400);
	
	//	�e�w2
	bd.boxsize = Vec3f(0.05*S, 0.14*S, 0.05*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
    //	��1�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, -0.05*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, 0.07*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(30), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(30), 'y');
	jd.damper = 60.0;
	jd.spring = 500.0;
	jd.lower = Rad(-10);
	jd.upper = Rad(0);
	jd.origin = Rad(-10);
	jntLink.push_back(scene->CreateJoint(soBox[3], soBox[2], jd));

   
    //	�e�w3
	bd.boxsize = Vec3f(0.05*S, 0.16*S, 0.05*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
    //	��2�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, -0.07*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, 0.08*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(30), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(30), 'y');
	jd.damper = 60.0;
	jd.spring = 500.0;
	jd.lower = Rad(0);
	jd.upper = Rad(70);
	jd.origin = Rad(40);
	jntLink.push_back(scene->CreateJoint(soBox[4], soBox[3], jd));

	//	��
	bd.boxsize = Vec3f(0.15*S, 0.05*S, 0.05*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//����
	//soBox.push_back(scene->CreateSolid(descBox));
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0.0, -0.08*S, 0.025*S);
	jd.poseSocket.Pos() = Vec3d(0.055*S, 0.0, -0.025*S);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.damper = 60.0;
	jd.spring = 500.0;
	jd.lower = Rad(-20);
	jd.upper = Rad(0);
	jd.origin = Rad(-10);
	jntLink.push_back(scene->CreateJoint(soBox[5], soBox[4], jd));

	//	�ڕW�ʒu
	CDSphereDesc sd;
	sd.radius = 0.01;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(0.0, 0.0, -s/60));
	soTarget.push_back(soBox.back());
	//	�o�l(�e�w)
	spd.damper = Vec3f(1,1,1) * dp;
	spd.spring = Vec3f(1,1,1) * sp;
	spd.posePlug.Pos() = Vec3d(0, 0.05*S, 0.023*S);
	jntLink.push_back(scene->CreateJoint(soBox[6], soBox[2], spd));
	
	


	///////////////////////�l�����w////////////////////////
   
    
	//	�l�����w1
	bd.boxsize = Vec3f(0.04*S, 0.18*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	����
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0.065*S, 0.025*S, 0.025*S);
	jd.poseSocket.Pos() = Vec3d(0, -0.09*S, 0.0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.lower = Rad(-52);
	jd.upper = Rad(-48);
	jd.damper = 10.0;
	jd.spring = 300.0;
	jd.origin = Rad(-50);
	jntLink.push_back(scene->CreateJoint(soBox[7], soBox[5], jd));
    

	//  ��3�֐�
    soBox.push_back(scene->CreateSolid(descBox));
    jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.08*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.damper = 20.0;
	jd.spring = 80.0;
	jd.lower = Rad(-10);
	jd.upper = Rad(25);
	jd.origin = Rad(15);
	jntLink.push_back(scene->CreateJoint(soBox[8], soBox[7], jd));

	//	�l�w���w2
	bd.boxsize = Vec3f(0.04*S, 0.12*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��3�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.06*S, 0);
	jd.lower = Rad(-30);
	jd.upper = Rad(100);
	jd.damper = 5.0;
	jd.spring = 40.0;
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[9], soBox[8], jd));

    //	�l�w���w3
	bd.boxsize = Vec3f(0.04*S, 0.07*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��2�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.06*S, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.035*S, 0);
	jd.damper = 4.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(100);
	jd.origin = Rad(30);
	jntLink.push_back(scene->CreateJoint(soBox[10], soBox[9], jd));

	
    //	�l�w���w4
	bd.boxsize = Vec3f(0.037*S, 0.06*S, 0.037*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��1�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.035*S, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.03*S, 0);
	jd.damper = 10.0;
	jd.spring = 15.0;
	jd.lower = Rad(0);
	jd.upper = Rad(90);
	jd.origin = Rad(30);
	jntLink.push_back(scene->CreateJoint(soBox[11], soBox[10], jd));


	//	�ڕW�ʒu
	sd.radius = 0.01;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(0.13, 0.0, 0.02));
	soTarget.push_back(soBox.back());
	//	�o�l(�l�����w)
	spd.damper = Vec3f(1,1,1) * dp;
	spd.spring = Vec3f(1,1,1) * sp;
	spd.posePlug.Pos() = Vec3d(0, 0.03*S, -0.018*S);
	jntLink.push_back(scene->CreateJoint(soBox[12], soBox[11], spd));


	
	/////////////////////////���w///////////////////////////////

	//	���w1
	bd.boxsize = Vec3f(0.04*S, 0.19*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
    //	����
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0.02*S, 0.03*S, 0.025*S);
	jd.poseSocket.Pos() = Vec3d(0, -0.095*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 10.0;
	jd.spring = 300.0;
	jd.lower = Rad(-52);
	jd.upper = Rad(-48);
	jd.origin = Rad(-50);
	jntLink.push_back(scene->CreateJoint(soBox[13], soBox[5], jd));
	 
	//	��3�֐�
	soBox.push_back(scene->CreateSolid(descBox));
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.095*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.damper = 20.0;
	jd.spring = 80.0;
	jd.lower = Rad(-20);
	jd.upper = Rad(20);
	jd.origin = Rad(0);
	jntLink.push_back(scene->CreateJoint(soBox[14], soBox[13], jd));

	//	���w2
	bd.boxsize = Vec3f(0.04*S, 0.12*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��3�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.06*S, 0);
	jd.damper = 5.0;
	jd.spring = 40.0;
	jd.lower = Rad(-30);
	jd.upper = Rad(100);
	jd.origin = Rad(10);
	jntLink.push_back(scene->CreateJoint(soBox[15], soBox[14], jd));

    //	���w3
	bd.boxsize = Vec3f(0.04*S, 0.09*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��2�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.06*S, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.045*S, 0);
	jd.damper = 4.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(110);
	jd.origin = Rad(35);
	jntLink.push_back(scene->CreateJoint(soBox[16], soBox[15], jd));

	//	���w4
	bd.boxsize = Vec3f(0.037*S, 0.06*S, 0.037*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��1�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.045*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, -0.03*S, 0);
	jd.damper = 6.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(90);
	jd.origin = Rad(35);
	jntLink.push_back(scene->CreateJoint(soBox[17], soBox[16], jd));



	//	�ڕW�ʒu
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(0.01, 0.0, 0.02));
	soTarget.push_back(soBox.back());

	//	�o�l(���w)
	spd.damper = Vec3f(1,1,1) * dp;
	spd.spring = Vec3f(1,1,1) * sp;
	spd.posePlug.Pos() = Vec3d(0, 0.03*S, -0.018*S);
	jntLink.push_back(scene->CreateJoint(soBox[18], soBox[17], spd));


	
    ///////////////////////��w//////////////////////////

	//	��w1
	bd.boxsize = Vec3f(0.04*S, 0.18*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	����
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(-0.025*S, 0.03*S, 0.025*S);
	jd.poseSocket.Pos() = Vec3d(0, -0.09*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 20.0;
	jd.spring = 300.0;
	jd.lower = Rad(-53);
	jd.upper = Rad(-47);
	jd.origin = Rad(-50);
	jntLink.push_back(scene->CreateJoint(soBox[19], soBox[5], jd));

	//	��3�֐�
	soBox.push_back(scene->CreateSolid(descBox));
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.09*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.damper = 10.0;
	jd.spring = 80.0;
	jd.lower = Rad(-15);
	jd.upper = Rad(10);
	jd.origin = Rad(-10);
	jntLink.push_back(scene->CreateJoint(soBox[20], soBox[19], jd));

	//	��w2
	bd.boxsize = Vec3f(0.04*S, 0.1*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��3�֐�
	jd = PHHingeJointDesc();
	jd.poseSocket.Pos() = Vec3d(0, -0.05*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 5.0;
	jd.spring = 40.0;
	jd.lower = Rad(-30);
	jd.upper = Rad(100);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[21], soBox[20], jd));

    //	��w3
	bd.boxsize = Vec3f(0.04*S, 0.08*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��2�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.05*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, -0.04*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 4.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(100);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[22], soBox[21], jd));

	//	��w4
	bd.boxsize = Vec3f(0.037*S, 0.06*S, 0.037*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��1�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.04*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, -0.03*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 2.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(90);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[23], soBox[22], jd));

	//	�ڕW�ʒu
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(-0.05, 0.0,0.02));
	soTarget.push_back(soBox.back());

	
	//	�o�l(��w)
	spd.damper = Vec3f(1,1,1) * dp;
	spd.spring = Vec3f(1,1,1) * sp;
	spd.posePlug.Pos() = Vec3d(0, 0.03*S, -0.018*S);
	jntLink.push_back(scene->CreateJoint(soBox[24], soBox[23], spd));


    ///////////////////���w////////////////////

	//	���w1
	bd.boxsize = Vec3f(0.04*S, 0.17*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
    //	����
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(-0.07*S, 0.03*S, 0.025*S);
	jd.poseSocket.Pos() = Vec3d(0, -0.085*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 10.0;
	jd.spring = 500.0;
	jd.lower = Rad(-53);
	jd.upper = Rad(-47);
	jd.origin = Rad(-50);
	jntLink.push_back(scene->CreateJoint(soBox[25], soBox[5], jd));
	
	
    //	��3�֐�
	soBox.push_back(scene->CreateSolid(descBox));
	jd = PHHingeJointDesc();
    jd.posePlug.Pos() = Vec3d(0, 0.085*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'y');
	jd.damper = 20.0;
	jd.spring = 80.0;
	jd.lower = Rad(-30);
	jd.upper = Rad(15);
	jd.origin = Rad(-20);
	jntLink.push_back(scene->CreateJoint(soBox[26], soBox[25], jd));
	

	//	���w2
	bd.boxsize = Vec3f(0.04*S, 0.08*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��3�֐�
	jd = PHHingeJointDesc();
	jd.poseSocket.Pos() = Vec3d(0, -0.04*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 5.0;
	jd.spring = 40.0;
	jd.lower = Rad(-30);
	jd.upper = Rad(100);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[27], soBox[26], jd));

    //	���w3
	bd.boxsize = Vec3f(0.04*S, 0.07*S, 0.04*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��2�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.04*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, -0.035*S, 0);
    jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 6.0;
	jd.spring = 30.0;
	jd.lower = Rad(0);
	jd.upper = Rad(90);
	jd.origin = Rad(10);
	jntLink.push_back(scene->CreateJoint(soBox[28], soBox[27], jd));

    //	���w4
	bd.boxsize = Vec3f(0.037*S, 0.06*S, 0.037*S);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��1�֐�
	jd = PHHingeJointDesc();
	jd.posePlug.Pos() = Vec3d(0, 0.035*S, 0);
	jd.poseSocket.Pos() = Vec3d(0, -0.03*S, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.damper = 6.0;
	jd.spring = 20.0;
	jd.lower = Rad(0);
	jd.upper = Rad(90);
	jd.origin = Rad(20);
	jntLink.push_back(scene->CreateJoint(soBox[29], soBox[28], jd));

	//	�ڕW�ʒu
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(-0.15, 0.0, 0.02));
	soTarget.push_back(soBox.back());
	
	//	�o�l(���w)
	spd.damper = Vec3f(1,1,1) * dp;
	spd.spring = Vec3f(1,1,1) * sp;
	spd.posePlug.Pos() = Vec3d(0.0, 0.03*S, -0.018*S);
	jntLink.push_back(scene->CreateJoint(soBox[30], soBox[29], spd));
  

  
	
	
	
	
	// �d�͂�ݒ�
	scene->SetGravity(Vec3f(0, -9.8, 0));
	//	�ڐG��؂�
	scene->SetContactMode(&soTarget[0], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(&soTarget[1], PHSceneDesc::MODE_NONE);
	scene->SetContactMode(&soTarget[2], PHSceneDesc::MODE_NONE);
    scene->SetContactMode(&soTarget[3], PHSceneDesc::MODE_NONE); 
	scene->SetContactMode(&soTarget[4], PHSceneDesc::MODE_NONE);
	
	
	
	
	
	
	
	////////��//////////

    bd.boxsize = Vec3f(0.01, 12*s/120, 0.01);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(s-0.005, 0.0, s/60));

	bd.boxsize = Vec3f(0.02, 20*s/120, 0.015);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(-19*s/20, 0.0, s/60));
    

	///////////�t���b�g////////////
	for(int i=1;i<22;i++){
			bd.boxsize = Vec3f(0.005, s/10 + (s-X[i])/40, 0.005);
			shape = phSdk->CreateShape(bd);
			soBox.push_back(scene->CreateSolid(descBox));
			soBox.back()->AddShape(shape);
			soBox.back()->SetDynamical(false);
			soBox.back()->SetFramePosition(Vec3f(X[i], 0.0, s/70));
	}

    ////////////�w�b�h////////////
    CDConvexMeshIf* meshBlock1=NULL;
    md1.vertices.push_back(Vec3f(s/4,-3*s/120,-s/80));                              
    md1.vertices.push_back(Vec3f(s/4,-3*s/120, s/80));	
    md1.vertices.push_back(Vec3f(s/4, 3*s/120,-s/80));	
    md1.vertices.push_back(Vec3f(s/4, 3*s/120, s/80));
    md1.vertices.push_back(Vec3f(0.0,-10*s/120,-s/80));	
    md1.vertices.push_back(Vec3f(0.0,-10*s/120, s/80));
    md1.vertices.push_back(Vec3f(0.0, 10*s/120,-s/80));
    md1.vertices.push_back(Vec3f(0.0, 10*s/120, s/80));
    meshBlock1 = DCAST(CDConvexMeshIf, phSdk->CreateShape(md1));
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->SetFramePosition(Vec3f(s, 0.0, 0.0));
	soBox.back()->AddShape(meshBlock1);
	soBox.back()->SetDynamical(false);

    /////////�{�f�B�[/////////
    CDConvexMeshIf* meshBlock2=NULL;
	md2.vertices.push_back(Vec3f(0.0, 0.0, -s/30));                              
	md2.vertices.push_back(Vec3f(0.0, 0.0, s/30));	
	md2.vertices.push_back(Vec3f(3*s/4, -s/120, -s/30));	
    md2.vertices.push_back(Vec3f(3*s/4, -s/120, s/30));
    md2.vertices.push_back(Vec3f(-2*s/3, s/2,-s/30));	
    md2.vertices.push_back(Vec3f(-2*s/3, s/2, s/30));
    md2.vertices.push_back(Vec3f(-2*s/3, 2*s/5,-s/30));
    md2.vertices.push_back(Vec3f(-2*s/3, 2*s/5, s/30));
    meshBlock2 = DCAST(CDConvexMeshIf, phSdk->CreateShape(md2));
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->SetFramePosition(Vec3f(-s, 9*s/120, -s/90));
	soBox.back()->AddShape(meshBlock2);
	soBox.back()->SetDynamical(false);

    
    CDConvexMeshIf* meshBlock3=NULL;
	md3.vertices.push_back(Vec3f(0.0, 0.0, -s/30));                              
	md3.vertices.push_back(Vec3f(0.0, 0.0, s/30));	
	md3.vertices.push_back(Vec3f(3*s/4, s/120, -s/30));	
    md3.vertices.push_back(Vec3f(3*s/4, s/120, s/30));
    md3.vertices.push_back(Vec3f(-2*s/3, -s/2,-s/30));	
    md3.vertices.push_back(Vec3f(-2*s/3, -s/2, s/30));
    md3.vertices.push_back(Vec3f(-2*s/3, -2*s/5,-s/30));
    md3.vertices.push_back(Vec3f(-2*s/3, -2*s/5, s/30));
    meshBlock3 = DCAST(CDConvexMeshIf, phSdk->CreateShape(md3));
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->SetFramePosition(Vec3f(-s, -9*s/120, -s/90));
	soBox.back()->AddShape(meshBlock3);
	soBox.back()->SetDynamical(false);

}

void OnKey(char key){
	PHSpringDesc spd;
	switch(key){    //�e�w�̖ڕW�ʒu
		case '0':  jntLink[5]->Enable(true);
			jntLink[11]->Enable(false);        //�S�J��(�o�loff)
			jntLink[17]->Enable(false);
			jntLink[23]->Enable(false);
			jntLink[29]->Enable(false);
			soTarget[0]->SetFramePosition(Vec3f(0.8, 0.0, -s/60));
			break;
		
		case '1': jntLink[5]->Enable(true);
			jntLink[11]->Enable(true);        
			jntLink[17]->Enable(true);
			jntLink[23]->Enable(true);
			jntLink[29]->Enable(true);
			soTarget[0]->SetFramePosition(Vec3f(0.7, 0.0, -s/60));     //�S�J��(����)
			soTarget[1]->SetFramePosition(Vec3f(0.8, 0.0, 0.05));
			soTarget[2]->SetFramePosition(Vec3f(0.7, 0.0, 0.05));
			soTarget[3]->SetFramePosition(Vec3f(0.6, 0.0, 0.05));
			soTarget[4]->SetFramePosition(Vec3f(0.5, 0.0, 0.05));
			break;
			  
		case '2': jntLink[5]->Enable(false);
			jntLink[11]->Enable(true);        
			jntLink[17]->Enable(true);
			jntLink[23]->Enable(true);
			jntLink[29]->Enable(true);
			soTarget[1]->SetFramePosition(Vec3f(0.8, 0.0, 0.015));
			soTarget[2]->SetFramePosition(Vec3f(0.7, 0.0, 0.015));
			soTarget[3]->SetFramePosition(Vec3f(0.6, 0.0, 0.015));
			soTarget[4]->SetFramePosition(Vec3f(0.5, 0.0, 0.015));
			break;		
		
		case '3': jntLink[5]->Enable(false);
			jntLink[11]->Enable(true);        
			jntLink[17]->Enable(false);
			jntLink[23]->Enable(false);
			jntLink[29]->Enable(false);
			soTarget[1]->SetFramePosition(Vec3f(0.13, 0.0, 0.015));     
			break;
		
		case '4': jntLink[5]->Enable(false);
			jntLink[11]->Enable(true);       
			jntLink[17]->Enable(true);
			jntLink[23]->Enable(true);
			jntLink[29]->Enable(true);
			soTarget[1]->SetFramePosition(Vec3f(-0.3, 0.0, 0.015));
			soTarget[2]->SetFramePosition(Vec3f(-0.4, 0.0, 0.015));
			soTarget[3]->SetFramePosition(Vec3f(-0.5, 0.0, 0.015));
			soTarget[4]->SetFramePosition(Vec3f(-0.6, 0.0, 0.015));
			break;

		case 'o':jntLink[5]->Enable(false);
			jntLink[11]->Enable(false);       
			jntLink[17]->Enable(false);
			jntLink[23]->Enable(false);
			jntLink[29]->Enable(false);
			break;    
				
				

		case 's': 
		bTimer = false;
		step();
		break;
		case 'S': 
		bTimer = true;
		break;


		}
}

		

//�^�C�}������
void timer(int id){
	if (!bTimer) return;
	glutTimerFunc(simulationPeriod, timer, 0);
	step();
	
}





/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */

void display(){
	Affinef view;
	double yoffset = 0.11;
	view.Pos() = CameraZoom * Vec3f(
		cos(CameraRotX) * cos(CameraRotY),
		sin(CameraRotX),
		cos(CameraRotX) * sin(CameraRotY));
	view.PosY() += yoffset;
	view.LookAtGL(Vec3f(0.0, yoffset, 0.0), Vec3f(0.0f, 100.0f, 0.0f));
	render->SetViewMatrix(view.inv());
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	lookAt.x = 0.3;
	lookAt.y = -0.2;
	lookAt.z = 1.0;
	lookAt.x = 0.03;
	lookAt.y = 0.1;
	lookAt.z = 0.3;
	gluLookAt(lookAt.x, lookAt.y, lookAt.z, 
		      0.0, lookAt.y, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	setLight();
}

int xlast, ylast;
void mouse(int button, int state, int x, int y){
	xlast = x, ylast = y;
	if(button == GLUT_LEFT_BUTTON)
		bLeftButton = (state == GLUT_DOWN);
	if(button == GLUT_RIGHT_BUTTON)
		bRightButton = (state == GLUT_DOWN);
}

void motion(int x, int y){
	static bool bFirst = true;
	int xrel = x - xlast, yrel = y - ylast;
	xlast = x;
	ylast = y;
	if(bFirst){
		bFirst = false;
		return;
	}
	// ���{�^��(���_�ύX)
	if(bLeftButton){      
		CameraRotY += xrel * 0.01;
		CameraRotY = Spr::max(Rad(-270.0), Spr::min(CameraRotY, Rad(270.0)));
		CameraRotX += yrel * 0.01;
		CameraRotX = Spr::max(Rad(-270.0), Spr::min(CameraRotX, Rad(270.0)));
	}
	// �E�{�^��(�g��k��)
	if(bRightButton){
		CameraZoom *= exp(yrel/10.0);
		CameraZoom = Spr::max(0.1, Spr::min(CameraZoom, 10.0));
	}
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
			exit(0);
			break;
		default:
			OnKey(key);
			break;
	}
}	

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	set();
	// SDK�̍쐬�@
	phSdk = PHSdkIf::CreateSdk();
	grSdk = GRSdkIf::CreateSdk();
	// �V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc dscene;
	dscene.timeStep = dt;
	dscene.numIteration = niter;
	scene = phSdk->CreateScene(dscene);				// �V�[���̍쐬
	BuildScene();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1200, 1000);
	int window = glutCreateWindow("Fingering Simulator");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL();

	// �����ݒ�
	device->Init();
   	glutTimerFunc(20, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�
	
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�
	initialize();
	glutMainLoop();

}