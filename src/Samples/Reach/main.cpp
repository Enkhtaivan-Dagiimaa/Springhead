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

�y�e�X�g�p�^�[���z
  �E�V�[��0�F�q���W�̍�
  �E�V�[��1�F�`�F�r�V�F�t�����N
�@�E�V�[��2�F�{�[���W���C���g�̍�
�@�E�V�[��3�F�X���C�_�̍�
  �E�V�[��4�F�p�X�W���C���g
 
�y�d�l�z
	���ʂ̑���F
		�E�V�[���ԍ�0, 1, ...���^�C�v����ƑΉ�����V�[���ɐ؂�ւ��i�f�t�H���g�F�V�[��0)
		�E'x'���^�C�v����Ɗ֐߂̗L��/�������؂�ւ��
		�E'z'�ŁA����������{�b�N�X���΂��A�Փ˂�����		
�@�@�V�[��0�F
		�E'0'�ŁA�q���W�V�[���̐ݒ���s���i�V�[���؊����j
		�E�X�y�[�X�L�[�Ńq���W�p��box�𐶐�
	�V�[��1�F
		�E'1'�ŁA�`�F�r�V�F�t�����N�𐶐�����i�V�[���؊����j
		�E'a'�ŁA���[�^�[�g���N��1.0�ɐݒ肷��
		�E's'�ŁA���[�^�[�g���N��0.0�ɐݒ肷��
		�E'd'�ŁA���[�^�[�g���N��-1.0�ɐݒ肷��
		�E'f'�ŁA�ڕW���x��90.0�ɐݒ肷��
		�E'g'�ŁA�ڕW���x��0.0�ɐݒ肷��
		�E'h'�ŁA�ڕW���x��-90.0�ɐݒ肷��
		�E'j'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��1.0�ɐݒ肷��
		�E'k'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��0.0�ɐݒ肷��
		�E'l'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��-1.0�ɐݒ肷��
	�V�[��2�F
		�E'2'�ŁA�{�[���W���C���g�V�[���̐ݒ���s���i�V�[���؊����j
		�E�X�y�[�X�L�[�Ń{�[���W���C���g�p��box�𐶐�
*/

#include <ctime>
#include <string>
#include <GL/glut.h>
#include <sstream>
#include <windows.h>

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X

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

double simulationPeriod = 32.0;
int sceneNo;			// �V�[���ԍ�

PHSolidDesc descFloor;					//�����̂̃f�B�X�N���v�^
PHSolidDesc descBox;					//�����̂̃f�B�X�N���v�^
//CDConvexMeshIf* meshBox;				//���`��̃C���^�t�F�[�X
CDShapeIf* shapeBox;
CDShapeIf* shapeSphere;

PHSolidIf* soFloor;						//�����̂̃C���^�t�F�[�X
std::vector<PHSolidIf*> soBox;			//�����̂̃C���^�t�F�[�X
std::vector<PHJointIf*> jntLink;		//�֐߂̃C���^�t�F�[�X

/** �����p�ϐ� **/
const double dt = 0.1;					//�ϕ���
const int niter = 10;					//LCP�͂�Ղ���������
const double springOrigin = Rad(90.0);	//�o�l�̌��_
const double Kexp = 200, Dexp = 10;		//explicit�o�l�_���p�̌W��
const double Kimp = 200, Dimp = 10;		//implicit�o�l�_���p�̌W��
const double Kimp2 = 0.01, Dimp2 = 0.01;
double anglePendulum = Rad(60.0);				//�U��q�̏����p�x
double lengthPendulum = 10.0;							//�U��q�̒���
bool bExplicit = false;					//�ǂ����Ńo�l�_���p���邩
/**/

void CreateFloor(bool s=true){
	CDBoxDesc desc;
	desc.boxsize = Vec3f(30.0f, 5.0f, 20.0f);
	soFloor = scene->CreateSolid(descFloor);
	if (s) soFloor->AddShape(phSdk->CreateShape(desc));
	soFloor->SetFramePosition(Vec3f(0,-2,0));
	soFloor->SetDynamical(false);			// ���͊O�͂ɂ���ē����Ȃ��悤�ɂ���
}

// �V�[��0 : ���̃f���Bspace key�Ŕ���������
void BuildScene0(){	
//	CreateFloor();
	soBox.clear();
	jntLink.clear();
	//	��
	CDBoxDesc bd;
	bd.boxsize = Vec3f(0.19, 0.077, 0.137);
	CDShapeIf* shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//�󒆂ɌŒ肷��
	soBox.back()->SetFramePosition(Vec3f(0.0, 0.0, 0.0));
	soBox.back()->SetDynamical(false);

	//	��
	bd.boxsize = Vec3f(0.15, 0.08, 0.108);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	�W���C���g
	PHHingeJointDesc jd;
	jd.poseSocket.Pos() = Vec3d(0,0.04, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'x');
	jd.posePlug.Pos() = Vec3d(0, -0.04, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'x');
	jd.damper = 1.0;
	jd.spring = 1.0;
	jntLink.push_back(scene->CreateJoint(soBox[1], soBox[0], jd));

	//	��
	bd.boxsize = Vec3f(0.18, 0.2, 0.1);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	�W���C���g
	jd.poseSocket.Pos() = Vec3d(0, 0.04, 0);
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.posePlug.Pos() = Vec3d(0, -0.1, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'z');
	jd.damper = 1.0;
	jd.spring = 1.0;
	jntLink.push_back(scene->CreateJoint(soBox[2], soBox[1], jd));

	//	�p��
	soBox.push_back(scene->CreateSolid(descBox));

	//	��Z
	jd.poseSocket.Pos() = Vec3d(0.13, 0.1, 0);
	jd.posePlug.Pos() = Vec3d(0, 0, 0);
	jd.damper = 1.0;
	jd.spring = 1.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[3], soBox[2], jd));

	//	�E��r
	bd.boxsize = Vec3f(0.049, 0.16, 0.048);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);

	//	��X
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, 0, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.09, 0);
	jd.damper = 1.0;
	jd.spring = 1.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[4], soBox[3], jd));

	//	�E�O�r
	bd.boxsize = Vec3f(0.046, 0.12, 0.046);
	shape = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	�Ђ�
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, -0.09, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, 0.07, 0);
	jd.damper = 1.0;
	jd.spring = 1.0;
	jd.origin = Rad(-30);
	jntLink.push_back(scene->CreateJoint(soBox[5], soBox[4], jd));

	//	��
	CDSphereDesc sd;
	sd.radius = 0.06;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	//	��
	jd.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.poseSocket.Pos() = Vec3d(0, 0.1, 0);
	jd.posePlug.Ori() = Quaternionf::Rot(Rad(90), 'y');
	jd.posePlug.Pos() = Vec3d(0, -0.08, 0);
	jd.damper = 1.0;
	jd.spring = 1.0;
	jd.origin = Rad(0);
	jntLink.push_back(scene->CreateJoint(soBox[6], soBox[2], jd));

	//	�ڕW�ʒu
	sd.radius = 0.02;
	shape = phSdk->CreateShape(sd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shape);
	soBox.back()->SetDynamical(false);
	soBox.back()->SetFramePosition(Vec3f(0.2, 0.2, -0.2));

	//	�o�l
	PHSpringDesc spd;
	spd.damper = Vec3f(1,1,1);
	spd.spring = Vec3f(1,1,1) * 10;
	spd.posePlug.Pos() = Vec3d(0, -0.07, 0);
	jntLink.push_back(scene->CreateJoint(soBox[7], soBox[5], spd));


	// �d�͂�ݒ�
	scene->SetGravity(Vec3f(0, -9.8, 0));
//	scene->SetContactMode(PHSceneDesc::MODE_NONE);
}

// �V�[��1 : �A�N�`���G�[�^�̃f��
void BuildScene1(){
	CreateFloor();
	CDBoxDesc bd;
	soBox.resize(3);
	bd.boxsize = Vec3f(1.0, 2.0, 1.0);
	soBox[0] = scene->CreateSolid(descBox);
	soBox[0]->AddShape(phSdk->CreateShape(bd));
	soBox[0]->SetFramePosition(Vec3f(0.0, 20.0, 0.0));

	bd.boxsize = Vec3f(1.0, 5.0, 1.0);
	soBox[1] = scene->CreateSolid(descBox);
	soBox[1]->AddShape(phSdk->CreateShape(bd));
	soBox[1]->SetFramePosition(Vec3f(0.0, 20.0, 0.0));

	bd.boxsize = Vec3f(1.0, 10.0, 1.0);
	soBox[2] = scene->CreateSolid(descBox);
	soBox[2]->AddShape(phSdk->CreateShape(bd));
	soBox[2]->SetFramePosition(Vec3f(0.0, 20.0, 0.0));

	PHHingeJointDesc jd;
	jntLink.resize(4);
	jd.posePlug.Pos() = Vec3d(0.0, 10.0, 0.0);
	jd.poseSocket.Pos() = Vec3d(0.0, -1.0, 0.0);
	jntLink[0] = scene->CreateJoint(soFloor, soBox[0], jd);

	jd.posePlug.Pos() = Vec3d(4.0, 10.0, 0.0);
	jd.poseSocket.Pos() = Vec3d(0.0, -2.5, 0.0);
	jntLink[1] = scene->CreateJoint(soFloor, soBox[1], jd);

	jd.posePlug.Pos() = Vec3d(0.0, 1.0, 0.0);
	jd.poseSocket.Pos() = Vec3d(0.0, -5.0, 0.0);
	jntLink[2] = scene->CreateJoint(soBox[0], soBox[2], jd);

	jd.posePlug.Pos() = Vec3d(0.0, 2.5, 0.0);
	jd.poseSocket.Pos() = Vec3d(0.0, 0.0, 0.0);
	jntLink[3] = scene->CreateJoint(soBox[1], soBox[2], jd);

	scene->SetContactMode(&soBox[0], 3, PHSceneDesc::MODE_NONE);
	scene->SetGravity(Vec3f(0, 0.0, 0));
}

void BuildScene2(){
	CreateFloor();
	CDBoxDesc bd;
	bd.boxsize = Vec3f(2.0, 2.0, 2.0);
	shapeBox = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shapeBox);
	soBox.back()->SetFramePosition(Vec3f(0.0, 20.0, 0.0));
	soBox.back()->SetDynamical(false);
	scene->SetGravity(Vec3f(0, -9.8, 0));	
}

void BuildScene3(){
	CreateFloor();
	CDBoxDesc bd;
	bd.boxsize = Vec3f(2.0, 2.0, 2.0);
	shapeBox = phSdk->CreateShape(bd);
	descBox.mass=10.0;
	descBox.inertia = 10 * Matrix3d::Unit();
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shapeBox);
	soBox.back()->SetFramePosition(Vec3f(0.0, 20.0, 0.0));
	soBox.back()->SetDynamical(false);
	scene->SetGravity(Vec3f(0, -9.8, 0));	
}

void BuildScene4(){
	CreateFloor();
	CDBoxDesc bd;
	bd.boxsize = Vec3f(2.0, 2.0, 2.0);
	shapeBox = phSdk->CreateShape(bd);
	soBox.push_back(scene->CreateSolid(descBox));
	soBox.back()->AddShape(shapeBox);
	soBox.back()->SetFramePosition(Vec3f(0.0, 20.0, 0.0));

	PHPathDesc desc;
	PHPathIf* path = scene->CreatePath(desc);
	double s;
	double radius = 5.0;
	double pitch = 4.0;
	Posed pose;
	for(s = 0.0; s < 4 * (2 * M_PI); s += Rad(1.0)){
		double stmp = s;
		while(stmp > M_PI) stmp -= 2 * M_PI;
		pose.Pos() = Vec3d(radius * cos(stmp), 5.0 + pitch * s / (2 * M_PI), radius * sin(stmp));
		pose.Ori().FromMatrix(Matrix3d::Rot(-3*stmp, 'y'));
		path->AddPoint(s, pose);
	}
	PHPathJointDesc descJoint;
	jntLink.push_back(scene->CreateJoint(soFloor, soBox[0], descJoint));
	PHPathJointIf* joint = DCAST(PHPathJointIf, jntLink[0]);
	joint->AddChildObject(path);
	joint->SetPosition(2 * 2 * M_PI);
	
	scene->SetGravity(Vec3f(0, -9.8, 0));
}

void BuildScene5(){
	// �o�l�_���p��3�߃����N�ƁA���̐�[���o�l�_���p�ň�������
	scene->SetNumIteration(30);
	CreateFloor(false);
	
	CDBoxDesc bd;
	bd.boxsize = Vec3f(2.0, 6.0, 2.0);
	shapeBox = phSdk->CreateShape(bd);
	
	soBox.resize(6);

	soBox[0] = scene->CreateSolid(descBox);
	soBox[0]->AddShape(shapeBox);

	soBox[1] = scene->CreateSolid(descBox);
	soBox[1]->AddShape(shapeBox);

	soBox[2] = scene->CreateSolid(descBox);
	soBox[2]->AddShape(shapeBox);

	soBox[3] = scene->CreateSolid(descBox);
	soBox[3]->AddShape(shapeBox);

	soBox[4] = scene->CreateSolid(descBox);
	soBox[4]->AddShape(shapeBox);

	CDSphereDesc descSphere;
	descSphere.radius = 1.0;
	soBox[5] = scene->CreateSolid(descBox);
	soBox[5]->AddShape(scene->CreateShape(descSphere));

	jntLink.resize(6);
	PHHingeJointDesc descHinge;
	descHinge.poseSocket.Pos() = Vec3d(0.0, -3.0, 0.0);
	jntLink[0] = scene->CreateJoint(soFloor, soBox[0], descHinge);

	descHinge.posePlug.Pos() = Vec3d(0.0, 3.0, 0.0);
	descHinge.poseSocket.Pos() = Vec3d(0.0, -3.0, 0.0);
	jntLink[1] = scene->CreateJoint(soBox[0], soBox[1], descHinge);
	jntLink[2] = scene->CreateJoint(soBox[1], soBox[2], descHinge);
	jntLink[3] = scene->CreateJoint(soBox[2], soBox[3], descHinge);
	jntLink[4] = scene->CreateJoint(soBox[3], soBox[4], descHinge);

//	double K = 50, D = 5;
	double K = 5, D = 3;
//	DCAST(PHHingeJointIf, jntLink[0])->SetSpring(K);
//	DCAST(PHHingeJointIf, jntLink[0])->SetDamper(D);
	DCAST(PHHingeJointIf, jntLink[1])->SetSpring(K);
	DCAST(PHHingeJointIf, jntLink[1])->SetDamper(D);
	DCAST(PHHingeJointIf, jntLink[2])->SetSpring(K);
	DCAST(PHHingeJointIf, jntLink[2])->SetDamper(D);
	DCAST(PHHingeJointIf, jntLink[3])->SetSpring(K);
	DCAST(PHHingeJointIf, jntLink[3])->SetDamper(D);
	DCAST(PHHingeJointIf, jntLink[4])->SetSpring(K);
	DCAST(PHHingeJointIf, jntLink[4])->SetDamper(D);

	K = 5, D = 1;
//	K = 14, D = 3;
	PHSpringDesc descSpring;
	descSpring.poseSocket.Pos() = Vec3d(0.0, 3.0, 0.0);
	descSpring.spring = Vec3d(1.0, 1.0, 1.0) * K;
	descSpring.damper = Vec3d(1.0, 1.0, 1.0) * D;
	jntLink[5] = scene->CreateJoint(soBox[5], soBox[4], descSpring);

	soBox[5]->SetFramePosition(Vec3d(10.0, 5.0, 0.0));
	soBox[5]->SetDynamical(false);
	
	scene->SetContactMode(PHSceneDesc::MODE_NONE);	// �ڐG��؂�
	scene->SetGravity(Vec3f(0, -9.8, 0));	
}


void BuildScene(){
	switch(sceneNo){
	case 0: BuildScene0(); break;
	case 1: BuildScene1(); break;
	case 2: BuildScene2(); break;
	case 3: BuildScene3(); break;
	case 4: BuildScene4(); break;
	case 5: BuildScene5(); break;
	}
}

void OnKey0(char key){
	switch(key){
	case 'a': soBox.back()->SetFramePosition(Vec3f(0.0, 0.2, -0.2)); break;
	case 's': soBox.back()->SetFramePosition(Vec3f(0.1, 0.2, -0.2)); break;
	case 'd': soBox.back()->SetFramePosition(Vec3f(0.2, 0.2, -0.2)); break;
	case 'f': soBox.back()->SetFramePosition(Vec3f(0.3, 0.2, -0.2)); break;
	case 'w': soBox.back()->SetFramePosition(Vec3f(0.1, 0.2, -0.4)); break;
	case 'e': soBox.back()->SetFramePosition(Vec3f(0.2, 0.2, -0.4)); break;
	case 'r': soBox.back()->SetFramePosition(Vec3f(0.3, 0.2, -0.4)); break;
	case ' ':{
		soBox.push_back(scene->CreateSolid(descBox));
		soBox.back()->AddShape(shapeBox);
		soBox.back()->SetFramePosition(Vec3f(10.0, 10.0, 0.0));
		PHHingeJointDesc jdesc;
		jdesc.posePlug.Pos() = Vec3d( 1.1,  1.1,  0);
		jdesc.poseSocket.Pos() = Vec3d(-1.1, -1.1,  0);
		jdesc.lower = Rad(-30.0);
		jdesc.upper = Rad( 30.0);

		size_t n = soBox.size();
		jntLink.push_back(scene->CreateJoint(soBox[n-2], soBox[n-1], jdesc));
		}break;
	}
}
void display();
void OnKey1(char key){
	const double K = 30.0;
	const double B = 10.0;
	PHHingeJointIf* hinge = DCAST(PHHingeJointIf, jntLink[0]);
	PHPathJointIf* path = (jntLink.size() == 5 ? DCAST(PHPathJointIf, jntLink[4]) : NULL); 
	switch(key){
	case 'a': hinge->SetMotorTorque(1.0);	break;
	case 's': hinge->SetMotorTorque(0.0);	break;
	case 'd': hinge->SetMotorTorque(-1.0);	break;
	case 'f':
		hinge->SetDesiredVelocity(Rad(180));
		if(path)
			path->SetDesiredVelocity(Rad(90.0));
		break;
	case 'g':
		hinge->SetDesiredVelocity(Rad(0.0));
		if(path)
			path->SetDesiredVelocity(Rad(0.0));
		break;
	case 'h': hinge->SetDesiredVelocity(Rad(-90.0));	break;
	case 'j':
		hinge->SetSpring(K);
		hinge->SetSpringOrigin(1.0);
		hinge->SetDamper(B);
		break;
	case 'k':
		hinge->SetSpring(K);
		hinge->SetSpringOrigin(0.0);
		hinge->SetDamper(B);
		break;
	case 'l':
		hinge->SetSpring(K);
		hinge->SetSpringOrigin(-1.0);
		hinge->SetDamper(B);
		break;
	case 'c':{
		//�`�F�r�V�F�t�����N������̋O�Ղ��L�������ăp�X�W���C���g���쐬
		PHPathDesc descPath;
		descPath.bLoop = true;
		PHPathIf* trajectory = scene->CreatePath(descPath);

		hinge->SetSpring(K);
		hinge->SetDamper(B);
		double theta = -Rad(180.0);
		hinge->SetSpringOrigin(theta);
		for(int i = 0; i < 50; i++)
			scene->Step();
		for(; theta < Rad(180.0); theta += Rad(1.0)){
			hinge->SetSpringOrigin(theta);
			for(int i = 0; i < 5; i++)
				scene->Step();
			Posed pose = soFloor->GetPose().Inv() * soBox[2]->GetPose();
			//pose.Ori() = Quaterniond();
			trajectory->AddPoint(theta, pose);
			display();
		}
	
		soBox.resize(4);
		soBox[3] = scene->CreateSolid(descBox);
		soBox[3]->AddShape(soBox[2]->GetShape(0));
		soBox[3]->SetFramePosition(Vec3f(10.0, 20.0, 0.0));

		PHPathJointDesc descJoint;
		descJoint.posePlug.Pos().x = 15.0;
		jntLink.resize(5);
		jntLink[4] = scene->CreateJoint(soFloor, soBox[3], descJoint);
		PHPathJointIf* joint = DCAST(PHPathJointIf, jntLink[4]);
		joint->AddChildObject(trajectory);
		joint->SetPosition(0);
	
		}break;
	}
}

void OnKey2(char key){
	switch(key){
	case ' ':{
		soBox.push_back(scene->CreateSolid(descBox));
		soBox.back()->AddShape(shapeBox);
		soBox.back()->SetFramePosition(Vec3f(10.0, 10.0, 0.0));
		PHBallJointDesc jdesc;
		jdesc.posePlug.Pos() = Vec3d(-1.01, -1.01, -1.01);
		jdesc.poseSocket.Pos() = Vec3d(1.01, 1.01, 1.01);
		size_t n = soBox.size();
		jntLink.push_back(scene->CreateJoint(soBox[n-2], soBox[n-1], jdesc));
		}break;
	}
}

void OnKey3(char key){
	switch(key){
	case ' ':{
		soBox.push_back(scene->CreateSolid(descBox));
		soBox.back()->AddShape(shapeBox);
		soBox.back()->SetFramePosition(Vec3f(10.0, 10.0, 0.0));
		PHSliderJointDesc jdesc;
		jdesc.posePlug.Pos() = Vec3d(0, -1.1, 0);
		jdesc.posePlug.Ori() = Quaterniond::Rot(Rad(90.0), 'y');
		jdesc.poseSocket.Pos() = Vec3d(0,  1.1, 0);
		jdesc.poseSocket.Ori() = Quaterniond::Rot(Rad(90.0), 'y');
		size_t n = soBox.size();
		jntLink.push_back(scene->CreateJoint(soBox[n-2], soBox[n-1], jdesc));
		PHSliderJointIf* slider = DCAST(PHSliderJointIf, jntLink.back());
		slider->SetRange(-0.3, 0.3);
		//slider->SetSpring(1000.0);
		//slider->SetDamper(300);
		//slider->SetSpringOrigin(0.0);
		}break;
	case 'a': scene->SetGravity(Vec3f(5.0, -5, 0.0)); break;
	case 'd': scene->SetGravity(Vec3f(-5.0, -5, 0.0)); break;
	}
}

void OnKey4(char key){
	switch(key){
	case 'a': scene->SetGravity(Vec3f(0.0, -9.8, 0.0)); break;
	case 'd': scene->SetGravity(Vec3f(0.0,  9.8, 0.0)); break;
	}
}

void OnKey5(char key){
	switch(key){
	case 'a': soBox[5]->SetFramePosition(Vec3d(-20.0, 30.0, 0.0)); break;
	case 's': soBox[5]->SetFramePosition(Vec3d(-10.0, 20.0, 0.0)); break;
	case 'd': soBox[5]->SetFramePosition(Vec3d( -5.0, 10.0, 0.0)); break;
	case 'f': soBox[5]->SetFramePosition(Vec3d(  0.0, 10.0, 0.0)); break;
	case 'g': soBox[5]->SetFramePosition(Vec3d(  5.0, 10.0, 0.0)); break;
	case 'h': soBox[5]->SetFramePosition(Vec3d( 10.0, 20.0, 0.0)); break;
	case 'j': soBox[5]->SetFramePosition(Vec3d( 20.0, 30.0, 0.0)); break;
	case '.': 
		simulationPeriod /= 2.0;
		if (simulationPeriod < 1) simulationPeriod = 1.0;
		break;
	case ',': 
		simulationPeriod *= 2.0;
		break;
	case ' ':{
		soBox.push_back(scene->CreateSolid(descBox));
		soBox.back()->AddShape(shapeBox);
		soBox.back()->SetFramePosition(Vec3f(10.0, 10.0, 0.0));
		PHSpringDesc jdesc;
		jdesc.posePlug.Pos() = Vec3d(-1.1, -1.1, -1.1);
		jdesc.poseSocket.Pos() = Vec3d( 1.1,  1.1,  1.1);
		jdesc.spring = 10.0 * Vec3d(1, 1, 1);
		jdesc.damper = 2.0 * Vec3d(1, 1, 1);
		size_t n = soBox.size();
		jntLink.push_back(scene->CreateJoint(soBox[n-2], soBox[n-1], jdesc));
		}break;
	}
}

void OnKey(char key){
	switch(sceneNo){
	case 0: OnKey0(key); break;
	case 1: OnKey1(key); break;
	case 2: OnKey2(key); break;
	case 3: OnKey3(key); break;
	case 4: OnKey4(key); break;
	case 5: OnKey5(key); break;
	}
}

void OnTimer0(){
	//���O�Ńo�l�_���p�̃g���N���v�Z�������
	if(bExplicit){
		PHHingeJointIf* hinge = DCAST(PHHingeJointIf, jntLink[0]);
		double pos = hinge->GetPosition();
		double vel = hinge->GetVelocity();
		hinge->SetMotorTorque(-Kexp * (pos - springOrigin) - Dexp * vel);
	}
}

void OnTimer(){
	switch(sceneNo){
	case 0: OnTimer0(); break;
	/*case 1: OnTimer1(); break;
	case 2: OnTimer2(); break;
	case 3: OnTimer3(); break;
	case 4: OnTimer4(); break;
	case 5: OnTimer5(); break;*/
	}
}	

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void display(){
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
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

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
		//�V�[���؂�ւ�
		case '0': case '1': case '2': case '3': case '4': case '5':
			scene->Clear();
			soFloor = NULL;
			soBox.clear();
			jntLink.clear();
			sceneNo = key - '0';
			BuildScene();
			break;
		case 'x':
			{
				static bool bEnable = true;
				bEnable = !bEnable;
				for(i = 0; i < jntLink.size(); i++)
					jntLink[i]->Enable(bEnable);
			}break;
		case 'z':{
			soBox.push_back(scene->CreateSolid(descBox));
			soBox.back()->AddShape(shapeBox);
			soBox.back()->SetFramePosition(Vec3f(15.0, 15.0, 0.0));
			soBox.back()->SetVelocity(Vec3d(-10.0, 0.0, 0.0));
			soBox.back()->SetMass(2.0);
			}break;	
		default:
			OnKey(key);
			break;
	}
}	

/**
 brief  	glutTimerFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> id�@�@ �^�C�}�[�̋�ʂ����邽�߂̏��
 return 	�Ȃ�
 */
void timer(int id){
	glutTimerFunc(simulationPeriod, timer, 0);
	/// �����̃`�F�b�N�Ɖ�ʂ̍X�V���s��
	static DWORD last = timeGetTime();
	DWORD time = timeGetTime();
	DWORD period = time - last;
	last = time;
	static double realTime;
	static double simTime;
	realTime += period;
	while(realTime > simTime){
		simTime += simulationPeriod;
		OnTimer();
		scene->ClearForce();
		scene->GenerateForce();
		scene->Integrate();
	}
	glutPostRedisplay();
}
void idle(){
	scene->ClearForce();
	scene->GenerateForce();
	scene->Integrate();
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
	dscene.timeStep = dt;
	dscene.numIteration = niter;
	scene = phSdk->CreateScene(dscene);				// �V�[���̍쐬
	// �V�[���̍\�z
	sceneNo = 0;
	BuildScene();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	int window = glutCreateWindow("Joints");

	render = grSdk->CreateDebugRender();
	device = grSdk->CreateDeviceGL(window);

	// �����ݒ�
	device->Init();

	glutTimerFunc(20, timer, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	//glutIdleFunc(idle);
	
	render->SetDevice(device);	// �f�o�C�X�̐ݒ�

	
	initialize();

	// ���_��ݒ肷��
	Affinef view;
	view.Pos() = Vec3f(0.0, 1.2, 0.1);								// eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));			// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);

	glutMainLoop();

}
