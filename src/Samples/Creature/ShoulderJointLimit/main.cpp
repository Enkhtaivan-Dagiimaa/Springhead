/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** \page pageShoulderJointLimit
 Springhead2/src/Samples/Creature/ShoulderJointLimit/main.cpp

\ref ���֐߉��搧���̃f��

\secntion secSpecShoulderJointLimit
���֐߂̉��搧�����X�v���C���Ȑ��Ŏw�肷��f���ł��D
*/

#include <vector>
#include "../../SampleApp.h"

#include <Creature/CRBodyGenerator/CRBallHumanBodyGen.h>

#pragma  hdrstop

using namespace Spr;
using namespace std;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// �A�v���P�[�V�����N���X
class ShoulderJointLimitApp : public SampleApp{
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

	ShoulderJointLimitApp(){
		appName = "Shoulder Joint Limit Sample";
		numScenes   = 1;

		AddAction(MENU_SCENE, ID_UP, "Pointer move Up");
		AddHotKey(MENU_SCENE, ID_UP, 'i');

		AddAction(MENU_SCENE, ID_DOWN, "Pointer move Down");
		AddHotKey(MENU_SCENE, ID_DOWN, 'k');

		AddAction(MENU_SCENE, ID_LEFT, "Pointer move Left");
		AddHotKey(MENU_SCENE, ID_LEFT, 'j');

		AddAction(MENU_SCENE, ID_RIGHT, "Pointer move Right");
		AddHotKey(MENU_SCENE, ID_RIGHT, 'l');
	}
	~ShoulderJointLimitApp(){}

	// ������
	virtual void Init(int argc, char* argv[]) {
		this->argc = argc;
		this->argv = argv;

		SampleApp::Init(argc, argv);
		ToggleAction(MENU_ALWAYS, ID_RUN);
		curScene = 0;

		fwScene->GetPHScene()->GetConstraintEngine()->SetBSaveConstraints(true);
		GetCurrentWin()->GetTrackball()->SetPosition(Vec3f(3,3,5));
		fwScene->EnableRenderAxis(true, false, false);
		fwScene->EnableRenderLimit(true);
	}

	PHSolidIf*         soTarget;

	// �V�[���\�z
	virtual void BuildScene() {
		PHSdkIf* phSdk = phScene->GetSdk();

		PHSolidDesc descSolid;

		CDSphereDesc descSphere;
		descSphere.radius  = 0.1;

		// Pointer
		PHSolidIf* so4 = phScene->CreateSolid(descSolid);
		so4->SetFramePosition(Vec3d(1.2,0,0));
		so4->AddShape(phSdk->CreateShape(descSphere));
		so4->SetDynamical(false);

		soTarget = so4;

		CRSdkIf* crSdk = CRSdkIf::CreateSdk();
		CRCreatureDesc descCreature;
		CRCreatureIf* crCreature = crSdk->CreateCreature(descCreature);
		crCreature->AddChildObject(phScene);
		CRBallHumanBodyGenDesc descBody;
		CRBallHumanBodyGen bodyGen(descBody);
		CRBodyIf* body = bodyGen.Generate(crCreature);

		body->GetSolid(CRBallHumanBodyGenDesc::SO_WAIST)->GetPHSolid()->SetDynamical(false);

		// ----- ----- ----- ----- -----

		PHBallJointSplineLimitDesc descSplineLimit;
		// descSplineLimit.poleTwist = Vec2d(Rad(-90), Rad(0));
		descSplineLimit.limitDir  = Vec3d(0,0,-1);
		descSplineLimit.spring    = 5.0;
		descSplineLimit.damper    = 0.5;

		PHBallJointIf* joLShoulder = body->GetJoint(7)->GetPHJoint()->Cast();
		PHBallJointSplineLimitIf* limitL = joLShoulder->CreateLimit(descSplineLimit)->Cast();
		limitL->AddNode(Rad(0),Rad(90),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(48.0775),Rad(87.8079),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(132.51),Rad(65.1153),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(226.245),Rad(173.518),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(232.853),Rad(55.2006),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(247.286),Rad(62.1539),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(291.705),Rad(147.814),Rad(50),Rad(10),Rad(0),Rad(-90));
		limitL->AddNode(Rad(360),Rad(90),Rad(50),Rad(10),Rad(0),Rad(-90));

		if (joLShoulder) {
			joLShoulder->SetDamper(10.0);
			joLShoulder->SetSpring( 1.0);
		}
	}

	virtual void OnAction(int menu, int id){
		if(menu == MENU_SCENE){
			if(id == ID_UP){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.0,  0.1,  0.0));
			}

			if(id == ID_DOWN){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.0, -0.1,  0.0));
			}

			if(id == ID_LEFT){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d(-0.1,  0.0,  0.0));
			}

			if(id == ID_RIGHT){
				Vec3d currPos = soTarget->GetPose().Pos();
				soTarget->SetFramePosition(currPos + Vec3d( 0.1,  0.0,  0.0));
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
