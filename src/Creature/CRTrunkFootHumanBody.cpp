#include "CRTrunkFootHumanBody.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

//�R���X�g���N�^
CRTrunkFootHumanBodyDesc::CRTrunkFootHumanBodyDesc(){

	jointOrder = PLUG_PARENT;

	waistHeight    = 0.2298;
	waistBreadth   = 0.3067;
	waistThickness = 0.2307;

	chestHeight    = 1.4020 - 1.2253 + 1.2253 - 1.0142;
	chestBreadth   = 0.2887;
	chestThickness = 0.2118;

	neckLength   = 1.7219 - 1.4564 - 0.1732;
	headDiameter = 0.2387;

	footLength       = 0.2544;
	footBreadth      = 0.0994;
	footThickness    = 0.0619;

	springWaistChest   = 100.0;  damperWaistChest   =  50.0;
	springChestHead    = 100.0;  damperChestHead    =  50.0;

	// Vec2d(lower, upper)  lower>upper�̂Ƃ����搧������
	rangeWaistChest   = Vec2d(Rad(0.0) , Rad(0.01));
	rangeChestHead    = Vec2d(Rad(0.0) , Rad(0.01));
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRTrunkFootHumanBody
IF_OBJECT_IMP(CRTrunkFootHumanBody, CRBody);

// --- --- ---
void CRTrunkFootHumanBody::Init(){
	CRBody::Init();
}

// --- --- ---
void CRTrunkFootHumanBody::InitBody(){
	CreateWaist();
	CreateChest();
	CreateHead();
}

void CRTrunkFootHumanBody::CreateWaist(){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;

	// Solid
	solidDesc.mass     = 0.17;
	solids[SO_WAIST]   = phScene->CreateSolid(solidDesc);
	boxDesc.boxsize    = Vec3f(waistBreadth, waistHeight, waistThickness);
	// boxDesc.boxsize = Vec3f(0.2307, 0.2298, 0.3067);
	solids[SO_WAIST]->AddShape(phSdk->CreateShape(boxDesc));
	solids[SO_WAIST]->SetFramePosition(Vec3f(0,0,0));
	// solids[SO_WAIST]->SetOrientation(Quaternionf::Rot(Rad(0), 'y'));
}

void CRTrunkFootHumanBody::CreateChest(){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;
	PHHingeJointDesc   hingeDesc;

	// Solid
	// solidDesc.mass   = 0.252;
	solidDesc.mass   = 0.44;
	solids[SO_CHEST] = phScene->CreateSolid(solidDesc);
	// boxDesc.boxsize  = Vec3f(0.2, 0.3879, 0.2749);
	boxDesc.boxsize  = Vec3f(chestBreadth, chestHeight, chestThickness);
	solids[SO_CHEST]->AddShape(phSdk->CreateShape(boxDesc));

	//�����ʂƏ㔼�g�̊Ԃ̊֐�
	{
		PHHingeJointDesc hingeDesc;
		// hingeDesc.posePlug.Pos() = Vec3d(0,0,0);
		hingeDesc.posePlug.Pos() = Vec3d(0,waistHeight / 2.0,0);
		hingeDesc.posePlug.Ori() = Quaterniond::Rot(Rad(0), 'x');
		// hingeDesc.poseSocket.Pos() = Vec3d(0, -(chestHeight/2.0 + waistHeight/2.0), 0);
		hingeDesc.poseSocket.Pos() = Vec3d(0, -chestHeight/2.0, 0);
		hingeDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(0), 'x');

		hingeDesc.origin =   0.0;
		hingeDesc.spring = 500.0;
		hingeDesc.damper =  50.0;

		joints[JO_WAIST_CHEST] = CreateJoint(solids[SO_CHEST], solids[SO_WAIST], hingeDesc);
	}

	/*
	PHBallJointDesc ballDesc;
    ballDesc.posePlug.Pos() = Vec3d(0.0, 0.0, 0.0);
	ballDesc.posePlug.Ori() = Quaternionf::Rot(Rad(0), 'x');
	//ballDesc.poseSocket.Pos() = Vec3d(0, -0.32, 0.0);
	ballDesc.poseSocket.Pos() = Vec3d(0, -(chestHeight/2.0 + waistHeight/2.0), 0);
	ballDesc.poseSocket.Ori() = Quaternionf::Rot(Rad(0), 'x');
	joints[JO_WAIST_CHEST] = CreateJoint(solids[SO_CHEST], solids[SO_WAIST], ballDesc);
	DCAST(PHBallJointIf, joints[JO_WAIST_CHEST])->SetSwingRange(0.01);
	DCAST(PHBallJointIf, joints[JO_WAIST_CHEST])->SetTwistRange(0.0, 0.01);
	*/

	/*
	// Joint -- [p]Waist-[c]Chest
	hingeDesc                  = PHHingeJointDesc();
	hingeDesc.posePlug.Pos()   = Vec3d(0,  chestHeight / 2.0, 0);
	hingeDesc.posePlug.Ori()   = Quaternionf::Rot(Rad(90), 'y');
	hingeDesc.poseSocket.Pos() = Vec3d(0, -chestHeight / 2.0, 0);
	hingeDesc.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	hingeDesc.spring           = springWaistChest;
	hingeDesc.damper           = damperWaistChest;
	hingeDesc.origin           = Rad(0);
	hingeDesc.lower            = rangeWaistChest[0];
	hingeDesc.upper            = rangeWaistChest[1];
	joints[JO_WAIST_CHEST] = CreateJoint(solids[SO_CHEST], solids[SO_WAIST], hingeDesc);
	*/

	phScene->SetContactMode(solids[SO_CHEST], solids[SO_WAIST], PHSceneDesc::MODE_NONE);
}

void CRTrunkFootHumanBody::CreateHead(){
	CDSphereDesc       sphereDesc;
	PHSolidDesc        solidDesc;
	PHHingeJointDesc   hingeDesc;

	// Solid
	// solidDesc.mass    = 0.07;
	solidDesc.mass    = 0.178;
	solids[SO_HEAD]   = phScene->CreateSolid(solidDesc);
	sphereDesc.radius = headDiameter / 2.0;
	// sphereDesc.radius = 0.2387/2.0;
	solids[SO_HEAD]->AddShape(phSdk->CreateShape(sphereDesc));

	//�㔼�g�Ɠ����̊Ԃ̊֐�
	hingeDesc                  = PHHingeJointDesc();
    // hingeDesc.posePlug.Pos()   = Vec3d(0.0, 0.0, 0.0);
    hingeDesc.posePlug.Pos()   = Vec3d(0.0, chestHeight/2.0 + neckLength/2.0, 0.0);
	hingeDesc.posePlug.Ori()   = Quaterniond::Rot(Rad(0), 'x');
	// hingeDesc.poseSocket.Pos() = Vec3d(0, -0.36, 0.0);
	// hingeDesc.poseSocket.Pos() = Vec3d(0, -(chestHeight/2.0 + neckLength + headDiameter/2.0), 0.0);
	hingeDesc.poseSocket.Pos() = Vec3d(0, -headDiameter/2.0 - neckLength/2.0, 0.0);
	hingeDesc.poseSocket.Ori() = Quaterniond::Rot(Rad(0), 'x');

	hingeDesc.origin =   0.0;
	hingeDesc.spring = 500.0;
	hingeDesc.damper =  50.0;
	/*
    hingeDesc.lower = 0.0;
	hingeDesc.upper = 0.01;
	*/
    joints[JO_CHEST_HEAD] = CreateJoint(solids[SO_HEAD], solids[SO_CHEST], hingeDesc);

	/*
	// Joint -- [p]Chest-[c]Head
	hingeDesc                  = PHHingeJointDesc();
	hingeDesc.posePlug.Pos()   = Vec3d(0, chestHeight / 2.0 + neckLength, 0);
	hingeDesc.posePlug.Ori()   = Quaternionf::Rot(Rad(90), 'y');
	hingeDesc.poseSocket.Pos()   = Vec3d(0, -headDiameter, 0);
	hingeDesc.poseSocket.Ori() = Quaternionf::Rot(Rad(90), 'y');
	hingeDesc.spring           = springChestHead;
	hingeDesc.damper           = damperChestHead;
	hingeDesc.origin           = Rad(0);
	hingeDesc.lower            = rangeChestHead[0];
	hingeDesc.upper            = rangeChestHead[1];
	joints[JO_CHEST_HEAD] = CreateJoint(solids[SO_HEAD], solids[SO_CHEST], hingeDesc);
	*/

	phScene->SetContactMode(solids[SO_HEAD], solids[SO_CHEST], PHSceneDesc::MODE_NONE);
}

// --- --- ---
void CRTrunkFootHumanBody::InitLegs(){
	CreateFoot(LEFTPART);
	CreateFoot(RIGHTPART);
}

void CRTrunkFootHumanBody::CreateFoot(LREnum lr){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;
	PHHingeJointDesc   hingeDesc;

	CRTrunkFootHumanBodyDesc::CRHumanSolids soNFoot;
	if (lr==LEFTPART) {
		soNFoot = SO_LEFT_FOOT;
	} else {
		soNFoot = SO_RIGHT_FOOT;
	}

	// Solid
	solidDesc.mass   = 0.01;
	solids[soNFoot]  = phScene->CreateSolid(solidDesc);
	boxDesc.boxsize  = Vec3f(footBreadth, footThickness, footLength);
    // boxDesc.boxsize = Vec3f(0.2544, 0.0619, 0.0994);
	solids[soNFoot]->AddShape(phSdk->CreateShape(boxDesc));
}

// --- --- ---
void CRTrunkFootHumanBody::InitContact(){
	// �����ɑ����鍄�̓��m�̐ڐG��Off�i�܂����Ȃ����邩���H�Œ���̐ڐG�͎c�������i07/09/25, mitake�j�j
	for (unsigned int i=0; i<solids.size(); ++i) {
		for (unsigned int j=0; j<solids.size(); ++j) {
			if (i!=j) {
				phScene->SetContactMode(solids[i], solids[j], PHSceneDesc::MODE_NONE);
			}
		}
	}

	// �����ȊO�ɂ��ł�Body������΂���Body�ɑ����鍄�̂Ƃ�Contact���؂�
	for (int i=0; i<creature->NBodies(); ++i) {
		CRBodyIf* body = creature->GetBody(i);
		if (DCAST(CRTrunkFootHumanBodyIf,body)!=(this->Cast())) {
			for (int s=0; s<body->NSolids(); ++s) {
				for (unsigned int j=0; j<solids.size(); ++j) {
					phScene->SetContactMode(body->GetSolid(s), solids[j], PHSceneDesc::MODE_NONE);
				}
			}
		}
	}
}
}
