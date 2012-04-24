/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** \page pageIKSample
 Springhead2/src/Samples/IK/main.cpp

\ref pagePhysics �̋t�^���w�iIK�j�@�\�̃f���v���O�����B

\secntion secSpecIKSample �d�l
�A�[���̐�[���̂̒��S�ʒu�����|�C���^�i���́j�̈ʒu�ɂȂ�悤���B�^������D
���E���֐߂Ƃ��ɉ��搧�����������Ă���D
- �X�y�[�X�L�[�ŃV�~�����[�V�����J�n�D
- 'i'���^�C�v����ƃ|�C���^�ʒu����ɓ����D
- 'k'�ŁA����
- 'j'�ŁA����
- 'l'�ŁA�E��
*/

#include "../../SampleApp.h"

#pragma  hdrstop

using namespace Spr;
using namespace std;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// �A�v���P�[�V�����N���X
class IKSampleApp : public SampleApp{
public:
	/// �y�[�WID
	enum {
		MENU_MAIN = MENU_SCENE,
	};
	/// �A�N�V����ID
	enum {
		ID_RESET,
		ID_UP,
		ID_DOWN,
		ID_LEFT,
		ID_RIGHT,
	};

	int argc;
	char** argv;

	IKSampleApp(){
		appName = "IK Sample";
		numScenes   = 1;

		AddAction(MENU_SCENE, ID_UP, "Target move Up");
		AddHotKey(MENU_SCENE, ID_UP, 'i');

		AddAction(MENU_SCENE, ID_DOWN, "Target move Down");
		AddHotKey(MENU_SCENE, ID_DOWN, 'k');

		AddAction(MENU_SCENE, ID_LEFT, "Target move Left");
		AddHotKey(MENU_SCENE, ID_LEFT, 'j');

		AddAction(MENU_SCENE, ID_RIGHT, "Target move Right");
		AddHotKey(MENU_SCENE, ID_RIGHT, 'l');
	}
	~IKSampleApp(){}

	// ������
	virtual void Init(int argc, char* argv[]) {
		this->argc = argc;
		this->argv = argv;

		SampleApp::Init(argc, argv);
		ToggleAction(MENU_ALWAYS, ID_RUN);
		curScene = 0;

		fwScene->GetPHScene()->GetConstraintEngine()->SetBSaveConstraints(true);
		GetCurrentWin()->GetTrackball()->SetPosition(Vec3f(6.5,6,20));

		GetCurrentWin()->GetTrackball()->SetPosition(Vec3d(0,0,50));
	}

	PHSolidIf*         soTarget;
	PHIKEndEffectorIf* ikeTarget;

	// �V�[���\�z
	virtual void BuildScene() {
		PHSdkIf* phSdk = phScene->GetSdk();

		PHSolidDesc descSolid;

		CDRoundConeDesc descCapsule;
		descCapsule.radius = Vec2f(0.3, 0.3);
		descCapsule.length = 4;

		CDSphereDesc descSphere;
		descSphere.radius  = 0.5;

		CDBoxDesc descBox;
		descBox.boxsize = Vec3d(1.0, 0.5, 1.0);

		Posed shapePose; shapePose.Ori() = Quaterniond::Rot(Rad(90), 'x');

		// Base Link
		PHSolidIf* so0 = phScene->CreateSolid(descSolid);
		so0->SetDynamical(false);
		so0->AddShape(phSdk->CreateShape(descCapsule));
		so0->SetShapePose(0, shapePose);

		// Link 1
		PHSolidIf* so1 = phScene->CreateSolid(descSolid);
		so1->SetFramePosition(Vec3d(0,4,0));
		so1->AddShape(phSdk->CreateShape(descCapsule));
		so1->SetShapePose(0, shapePose);

		// Link 2
		PHSolidIf* so2 = phScene->CreateSolid(descSolid);
		so2->SetFramePosition(Vec3d(0,8,0));
		so2->AddShape(phSdk->CreateShape(descCapsule));
		so2->SetShapePose(0, shapePose);

		// Pointer
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		so4->SetFramePosition(Vec3d(-10,0,0));
		so4->AddShape(phSdk->CreateShape(descSphere));
		so4->SetDynamical(false);

		soTarget = so4;

		// ----- ----- ----- ----- -----

		PHHingeJointDesc descJoint;
		descJoint.poseSocket = Posed(1,0,0,0, 0, 2,0);
		descJoint.posePlug   = Posed(1,0,0,0, 0,-2,0);
		descJoint.spring =     10.0;
		descJoint.damper =   1000.0;
		descJoint.fMax   =   1000.0;

		PH1DJointLimitDesc descLimit;
		descLimit.spring = 1000000.0;
		descLimit.damper =   10000.0;

		PHIKHingeActuatorDesc descIKA;
		descIKA.velocityGain = 10.0;

		PHIKEndEffectorDesc descIKE;

		// Base <-> Link 1
		PHHingeJointIf* jo1 = phScene->CreateJoint(so0, so1, descJoint)->Cast();
		descLimit.range  = Vec2d(Rad(-90), Rad(90));
		jo1->CreateLimit(descLimit);
		PHIKHingeActuatorIf* ika1 = phScene->CreateIKActuator(descIKA)->Cast();
		ika1->AddChildObject(jo1);

		// Link 1 <-> Link 2
		PHHingeJointIf* jo2  = phScene->CreateJoint(so1, so2, descJoint)->Cast();
		descLimit.range  = Vec2d(Rad(0), Rad(180));
		jo2->CreateLimit(descLimit);
		PHIKHingeActuatorIf* ika2 = phScene->CreateIKActuator(descIKA)->Cast();
		ika2->AddChildObject(jo2);

		// Link2 = End Effector
		PHIKEndEffectorIf* ike1 = phScene->CreateIKEndEffector(descIKE);
		ike1->AddChildObject(so2);
		ika1->AddChildObject(ike1);
		ika2->AddChildObject(ike1);
		ike1->SetTargetPosition(Vec3d(-5,0,0));

		ikeTarget = ike1;

		// ----- ----- ----- ----- -----

		ike1->SetTargetPosition(soTarget->GetPose().Pos());
		phScene->GetIKEngine()->Enable(true);

		phScene->SetContactMode(PHSceneDesc::MODE_NONE);
	}

	virtual void OnAction(int menu, int id){
		if(menu == MENU_SCENE){
			if(id == ID_UP){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.0,  0.1,  0.0));
				ikeTarget->SetTargetPosition(soTarget->GetPose().Pos());
			}

			if(id == ID_DOWN){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.0, -0.1,  0.0));
				ikeTarget->SetTargetPosition(soTarget->GetPose().Pos());
			}

			if(id == ID_LEFT){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d(-0.1,  0.0,  0.0));
				ikeTarget->SetTargetPosition(soTarget->GetPose().Pos());
			}

			if(id == ID_RIGHT){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.1,  0.0,  0.0));
				ikeTarget->SetTargetPosition(soTarget->GetPose().Pos());
			}
		}

		SampleApp::OnAction(menu, id);
	}
} app;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char *argv[]) {
	app.Init(argc, argv);
	app.StartMainLoop();
	return 0;
}
