#include "IK.h"

#include <iostream>
#include <sstream>
#include <ctime>
#include <string>

#include <GL/glut.h>

#include "Physics/PHConstraint.h"
#include "Physics/PHSpatial.h"
#include "Physics/PHIK.h"
#include "Physics/PHScene.h"

#define ESC 27

IK app;

IK::IK(){
	bGravity	= true;
	bDebug		= false;
	bStep		= true;
	bOneStep	= false;
	bIK			= true;

	gravity		= Vec3d(0.0, -9.8, 0.0);
}

void IK::TimerFunc(int id){	
	switch(id){
		case 0:
			Step();
			PostRedisplay();
			break;
	}
}

void IK::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUT);
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬

	// GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.01);

	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "Inverse Kinematics Sample";			// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();
	InitCameraView();										// �J�����r���[�̏�����

	BuildScene(0);
	int timerId = CreateTimer(FWTimer::GLUT);				// �^�C�}�[�̐���
}


void IK::Reset(int sceneNum){
	GetSdk()->GetScene()->GetPHScene()->Clear();

	bGravity	= true;
	bDebug		= false;
	bStep		= true;
	bOneStep	= false;
	bIK			= true;

	BuildScene(sceneNum);
}

void IK::InitCameraView(){
	istringstream iss(
		"((0.9996 0.00141267 -0.0282299 -0.0937266)"
		"(1.59067e-012 0.99875 0.0499792 0.165937)"
		"(0.0282652 -0.0499592 0.998351 3.31465)"
		"(	   0      0      0      1))"
		);
	iss >> cameraInfo.view;
}

void IK::BuildScene(int sceneNum){
	PHSdkIf*	phSdk	= GetSdk()->GetPHSdk();
	PHSceneIf*	phScene	= GetSdk()->GetScene()->GetPHScene();

	soPosCtl1 = NULL;
	soPosCtl2 = NULL;

	// �{�[���W���C���g�Q�̒����󃊃��N
	if (sceneNum == 0) {
		PHSolidDesc			descSolid;
		CDBoxDesc			descBox;
		PHBallJointDesc		descBallJoint;
		PHIKBallJointDesc	descIKBall;
		PHIKPosCtlDesc		descIKPos;

		/// -- ���[�g����
		///// ����
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		so1->AddShape(phSdk->CreateShape(descBox));

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so2->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.1f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring =   1.0f;
		descBallJoint.damper =   2.0f;
		PHJointIf* jo1 = phScene->CreateJoint(so1, so2, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 1000.0f;
		descIKBall.damper =   20.0f;
		PHIKNodeIf* ikNode1 = phScene->CreateIKNode(descIKBall);
		ikNode1->AddChildObject(jo1->Cast());

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so3 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so3->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring =   1.0f;
		descBallJoint.damper =   2.0f;
		PHJointIf* jo2 = phScene->CreateJoint(so2, so3, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 1000.0f;
		descIKBall.damper =   20.0f;
		PHIKNodeIf* ikNode2 = phScene->CreateIKNode(descIKBall);
		ikNode2->AddChildObject(jo2->Cast());

		/// -- IK����_
		///// ����_�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl1 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl1->AddChildObject(so3->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl1);
		ikNode2->AddControlPoint(ikPosCtl1);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl1 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl1->AddShape(phSdk->CreateShape(descBox));
	}

	// �{�[���W���C���g�T�̒����󃊃��N
	if (sceneNum == 1) {
		PHSolidDesc			descSolid;
		CDBoxDesc			descBox;
		PHBallJointDesc		descBallJoint;
		PHIKBallJointDesc	descIKBall;
		PHIKPosCtlDesc		descIKPos;

		/// -- ���[�g����
		///// ����
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		so1->AddShape(phSdk->CreateShape(descBox));

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so2->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.1f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo1 = phScene->CreateJoint(so1, so2, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode1 = phScene->CreateIKNode(descIKBall);
		ikNode1->AddChildObject(jo1->Cast());

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so3 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so3->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo2 = phScene->CreateJoint(so2, so3, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode2 = phScene->CreateIKNode(descIKBall);
		ikNode2->AddChildObject(jo2->Cast());

		/// -- �O�ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so4->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo3 = phScene->CreateJoint(so3, so4, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode3 = phScene->CreateIKNode(descIKBall);
		ikNode3->AddChildObject(jo3->Cast());

		/// -- �l�ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so5 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so5->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo4 = phScene->CreateJoint(so4, so5, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode4 = phScene->CreateIKNode(descIKBall);
		ikNode4->AddChildObject(jo4->Cast());

		/// -- �܂ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so6 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so6->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo5 = phScene->CreateJoint(so5, so6, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode5 = phScene->CreateIKNode(descIKBall);
		ikNode5->AddChildObject(jo5->Cast());

		/// -- IK����_
		///// ����_�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl1 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl1->AddChildObject(so6->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl1);
		ikNode2->AddControlPoint(ikPosCtl1);
		ikNode3->AddControlPoint(ikPosCtl1);
		ikNode4->AddControlPoint(ikPosCtl1);
		ikNode5->AddControlPoint(ikPosCtl1);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl1 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl1->AddShape(phSdk->CreateShape(descBox));
	}

	// �{�[���W���C���g�T�̒����󃊃��N�i�D��x�t���E�����o�l�_���p���߁j
	if (sceneNum == 2) {
		PHSolidDesc			descSolid;
		CDBoxDesc			descBox;
		PHBallJointDesc		descBallJoint;
		PHIKBallJointDesc	descIKBall;
		PHIKPosCtlDesc		descIKPos;

		/// -- ���[�g����
		///// ����
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		so1->AddShape(phSdk->CreateShape(descBox));

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so2->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.1f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo1 = phScene->CreateJoint(so1, so2, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   200.0f;
		descIKBall.bias = 1.8;
		PHIKNodeIf* ikNode1 = phScene->CreateIKNode(descIKBall);
		ikNode1->AddChildObject(jo1->Cast());

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so3 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so3->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo2 = phScene->CreateJoint(so2, so3, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 90000.0f;
		descIKBall.damper =   200.0f;
		descIKBall.bias = 1.6;
		PHIKNodeIf* ikNode2 = phScene->CreateIKNode(descIKBall);
		ikNode2->AddChildObject(jo2->Cast());

		/// -- �O�ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so4->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo3 = phScene->CreateJoint(so3, so4, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 80000.0f;
		descIKBall.damper =   200.0f;
		descIKBall.bias = 1.4;
		PHIKNodeIf* ikNode3 = phScene->CreateIKNode(descIKBall);
		ikNode3->AddChildObject(jo3->Cast());

		/// -- �l�ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so5 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so5->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo4 = phScene->CreateJoint(so4, so5, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 70000.0f;
		descIKBall.damper =   200.0f;
		descIKBall.bias = 1.2;
		PHIKNodeIf* ikNode4 = phScene->CreateIKNode(descIKBall);
		ikNode4->AddChildObject(jo4->Cast());

		/// -- �܂ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so6 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so6->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo5 = phScene->CreateJoint(so5, so6, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 60000.0f;
		descIKBall.damper =   200.0f;
		descIKBall.bias = 1.0;
		PHIKNodeIf* ikNode5 = phScene->CreateIKNode(descIKBall);
		ikNode5->AddChildObject(jo5->Cast());

		/// -- IK����_
		///// ����_�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl1 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl1->AddChildObject(so6->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl1);
		ikNode2->AddControlPoint(ikPosCtl1);
		ikNode3->AddControlPoint(ikPosCtl1);
		ikNode4->AddControlPoint(ikPosCtl1);
		ikNode5->AddControlPoint(ikPosCtl1);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl1 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl1->AddShape(phSdk->CreateShape(descBox));
	}

	// �V���v���ȂQ���򃊃��N
	if (sceneNum == 3) {
		PHSolidDesc			descSolid;
		CDBoxDesc			descBox;
		PHBallJointDesc		descBallJoint;
		PHIKBallJointDesc	descIKBall;
		PHIKPosCtlDesc		descIKPos;

		/// -- ���[�g����
		///// ����
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		so1->AddShape(phSdk->CreateShape(descBox));

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so2->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.1f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		//descBallJoint.spring =   1.0f;
		//descBallJoint.damper =   2.0f;
		PHJointIf* jo1 = phScene->CreateJoint(so1, so2, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 1000.0f;
		descIKBall.damper = 1520.0f;
		PHIKNodeIf* ikNode1 = phScene->CreateIKNode(descIKBall);
		ikNode1->AddChildObject(jo1->Cast());

		/// -- ��ڂ̃����N�i�`�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so3 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so3->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		//descBallJoint.spring =   1.0f;
		//descBallJoint.damper =   2.0f;
		PHJointIf* jo2 = phScene->CreateJoint(so2, so3, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 1000.0f;
		descIKBall.damper = 1520.0f;
		PHIKNodeIf* ikNode2 = phScene->CreateIKNode(descIKBall);
		ikNode2->AddChildObject(jo2->Cast());

		/// -- ��ڂ̃����N�i�a�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so4->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		//descBallJoint.spring =   1.0f;
		//descBallJoint.damper =   2.0f;
		PHJointIf* jo3 = phScene->CreateJoint(so2, so4, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 1000.0f;
		descIKBall.damper = 1520.0f;
		PHIKNodeIf* ikNode3 = phScene->CreateIKNode(descIKBall);
		ikNode3->AddChildObject(jo3->Cast());

		/// -- IK����_
		///// ����_�P�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl1 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl1->AddChildObject(so3->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl1);
		ikNode2->AddControlPoint(ikPosCtl1);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl1 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl1->AddShape(phSdk->CreateShape(descBox));

		///// ����_�Q�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl2 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl2->AddChildObject(so4->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl2);
		ikNode3->AddControlPoint(ikPosCtl2);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl2 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl2->AddShape(phSdk->CreateShape(descBox));
	}

	// �������߂̂Q���򃊃��N
	if (sceneNum == 4) {
		PHSolidDesc			descSolid;
		CDBoxDesc			descBox;
		PHBallJointDesc		descBallJoint;
		PHIKBallJointDesc	descIKBall;
		PHIKPosCtlDesc		descIKPos;

		/// -- ���[�g����
		///// ����
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		so1->AddShape(phSdk->CreateShape(descBox));

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so2->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.1f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo1 = phScene->CreateJoint(so1, so2, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode1 = phScene->CreateIKNode(descIKBall);
		ikNode1->AddChildObject(jo1->Cast());

		/// -- ��ڂ̃����N
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so3 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so3->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo2 = phScene->CreateJoint(so2, so3, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode2 = phScene->CreateIKNode(descIKBall);
		ikNode2->AddChildObject(jo2->Cast());

		/// -- �O�ڂ̃����N�i�`�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so4->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo3 = phScene->CreateJoint(so3, so4, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode3 = phScene->CreateIKNode(descIKBall);
		ikNode3->AddChildObject(jo3->Cast());

		/// -- �l�ڂ̃����N�i�`�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so5 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so5->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo4 = phScene->CreateJoint(so4, so5, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode4 = phScene->CreateIKNode(descIKBall);
		ikNode4->AddChildObject(jo4->Cast());

		/// -- �܂ڂ̃����N�i�`�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so6 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so6->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo5 = phScene->CreateJoint(so5, so6, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode5 = phScene->CreateIKNode(descIKBall);
		ikNode5->AddChildObject(jo5->Cast());

		/// -- IK����_
		///// ����_�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl1 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl1->AddChildObject(so6->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl1);
		ikNode2->AddControlPoint(ikPosCtl1);
		ikNode3->AddControlPoint(ikPosCtl1);
		ikNode4->AddControlPoint(ikPosCtl1);
		ikNode5->AddControlPoint(ikPosCtl1);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl1 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl1->AddShape(phSdk->CreateShape(descBox));


		/// -- �O�ڂ̃����N�i�a�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so7 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so7->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo6 = phScene->CreateJoint(so3, so7, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode6 = phScene->CreateIKNode(descIKBall);
		ikNode6->AddChildObject(jo6->Cast());

		/// -- �l�ڂ̃����N�i�a�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so8 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so8->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo7 = phScene->CreateJoint(so7, so8, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode7 = phScene->CreateIKNode(descIKBall);
		ikNode7->AddChildObject(jo7->Cast());

		/// -- �܂ڂ̃����N�i�a�j
		///// ����
		descSolid.dynamical = true;
		descSolid.mass = 1.0;
		PHSolidIf* so9 = phScene->CreateSolid(descSolid);
		///// �`��
		descBox.boxsize = Vec3f(0.2f, 1.0f, 0.2f);
		so9->AddShape(phSdk->CreateShape(descBox));
		///// �֐�
		descBallJoint.poseSocket.Pos() = Vec3f(0.0f, +0.5f, 0.0f);
		descBallJoint.posePlug.Pos() = Vec3f(0.0f, -0.5f, 0.0f);
		descBallJoint.spring = 1000.0f;
		descBallJoint.damper =   20.0f;
		PHJointIf* jo8 = phScene->CreateJoint(so8, so9, descBallJoint);
		///// IK�m�[�h
		descIKBall.spring = 100000.0f;
		descIKBall.damper =   2000.0f;
		PHIKNodeIf* ikNode8 = phScene->CreateIKNode(descIKBall);
		ikNode8->AddChildObject(jo8->Cast());

		/// -- IK����_
		///// ����_�̍쐬
		descIKPos.pos = Vec3d(0.0, 0.5, 0.0);
		ikPosCtl2 = phScene->CreateIKControlPoint(descIKPos)->Cast();
		ikPosCtl2->AddChildObject(so9->Cast());
		///// �m�[�h�ւ̓o�^
		ikNode1->AddControlPoint(ikPosCtl2);
		ikNode2->AddControlPoint(ikPosCtl2);
		ikNode6->AddControlPoint(ikPosCtl2);
		ikNode7->AddControlPoint(ikPosCtl2);
		ikNode8->AddControlPoint(ikPosCtl2);
		///// ����_���w����������
		descSolid.dynamical = false;
		descSolid.mass = 1.0;
		soPosCtl2 = phScene->CreateSolid(descSolid);
		descBox.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		soPosCtl2->AddShape(phSdk->CreateShape(descBox));
	}

	// Scene�̐ݒ�
	/// IK�̐ݒ�
	phScene->GetIKEngine()->SetNumIter(50);
	phScene->GetIKEngine()->Enable(bIK);
	/// ���̑��̐ݒ�
	phScene->SetContactMode(PHSceneDesc::MODE_NONE);
	phScene->SetGravity(gravity);
	phScene->SetTimeStep(0.01);
	phScene->SetNumIteration(15);
}

void IK::Step(){
	if(bStep) {
		OneStep();
	} else if (bOneStep) {
		OneStep();
		bOneStep = false;
	}

	glutPostRedisplay();
}

void IK::OneStep(){
	PHSceneIf* phScene = GetSdk()->GetScene()->GetPHScene();

	phScene->Step();

	soPosCtl1->SetFramePosition(ikPosCtl1->GetTargetPosition());
	if (soPosCtl2) {
		soPosCtl2->SetFramePosition(ikPosCtl2->GetTargetPosition());
	}
}

void IK::Display(){
	// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(bDebug);

	// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}

void IK::Keyboard(int key, int x, int y){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	switch (key) {
		case ESC:

		case 'q':
			exit(0);
			break;

		case 'r':
			Reset();
			break;

		case 'w':
			InitCameraView();
			DSTR << "InitCameraView" << endl;
			break;

		case 'd':
			if(bDebug){
				bDebug = false;
				DSTR << "Debug Mode OFF" << endl;
			}else{
				bDebug = true;
				DSTR << "Debug Mode ON" << endl;
			}
			break;

		case 'g':
			if(bGravity){
				bGravity = false;
				phscene->SetGravity(Vec3d(0,0,0));
				DSTR << "Gravity OFF" << endl;
			}else{
				bGravity = true;
				phscene->SetGravity(gravity);
				DSTR << "Gravity ON" << endl;
			}
			break;

		case 's':
			bStep = false;
			bOneStep = true;
			break;

		case 'a':
			bStep = true;
			break;

		case 'f':
			{
				bIK = !bIK;
				DSTR << (bIK ? "IK ON" : "IK OFF") << endl;
				phscene->GetIKEngine()->Enable(bIK);
			}
			break;

		case 'l':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f(+0.2f,  0.0f,  0.0f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f(+0.2f,  0.0f,  0.0f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;

		case 'j':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f(-0.2f,  0.0f,  0.0f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f(-0.2f,  0.0f,  0.0f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;

		case 'i':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f( 0.0f, +0.2f,  0.0f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f( 0.0f, +0.2f,  0.0f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;

		case 'k':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f( 0.0f, -0.2f,  0.0f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f( 0.0f, -0.2f,  0.0f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;

		case 'u':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f( 0.0f,  0.0f, +0.2f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f( 0.0f,  0.0f, +0.2f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;

		case 'o':
			{
				Vec3d goal = ikPosCtl1->GetTargetPosition();
				goal += Vec3f( 0.0f,  0.0f, -0.2f);
				ikPosCtl1->SetTargetPosition(goal);
				if (soPosCtl2) {
					Vec3d goal = ikPosCtl2->GetTargetPosition();
					goal += Vec3f( 0.0f,  0.0f, -0.2f);
					ikPosCtl2->SetTargetPosition(goal);
				}
			}
			break;


		case 'L':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f(+0.2f,  0.0f,  0.0f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;

		case 'J':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f(-0.2f,  0.0f,  0.0f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;

		case 'I':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f( 0.0f, +0.2f,  0.0f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;

		case 'K':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f( 0.0f, -0.2f,  0.0f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;

		case 'U':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f( 0.0f,  0.0f, +0.2f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;

		case 'O':
			{
				Vec3d relGoal = ikPosCtl2->GetTargetPosition() - ikPosCtl1->GetTargetPosition();
				relGoal += Vec3f( 0.0f,  0.0f, -0.2f);
				ikPosCtl2->SetTargetPosition(relGoal + ikPosCtl1->GetTargetPosition());
			}
			break;


		case '1': case '2': case '3': case '4': case '5':
			Reset(key - '1');
			break;

			// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
			// 
			// �Ǝ��̃L�[�t�@���N�V�����͂����Œ�`���܂��D
			// 
			// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---

		default:
			break;
	}
}
