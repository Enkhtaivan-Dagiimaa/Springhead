/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRFourLegsTinyAnimalBody.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

//�R���X�g���N�^
CRFourLegsTinyAnimalBodyDesc::CRFourLegsTinyAnimalBodyDesc(){
		
	jointOrder = SOCKET_PARENT;

	bodyBreadth   = 2.5;
	bodyHeight    = 3.8;
	bodyThickness = 0.6;

	upperSizes = Vec2d(0.4, 0.2);
	lowerSizes = Vec2d(0.2, 0.1);
	upperLength = 1.0;
	lowerLength = 1.4;


	springFront   = 1.0;  damperFront   =  5.0;
	springRear    = 1.0;  damperRear    =  5.0;

	// Vec2d(lower, upper)  lower>upper�̂Ƃ����搧������
	rangeFrontSwing   = Vec2d(FLT_MAX , FLT_MAX);
	rangeFrontTwist	  = Vec2d(FLT_MAX , FLT_MAX);
	rangeRearSwing    = Vec2d(FLT_MAX , FLT_MAX);
	rangeRearTwist	  = Vec2d(FLT_MAX , FLT_MAX);

	noLegs = false;
	noHead = false;
	onlyOneLeg = false;
	hingeDebug = false;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsTinyAnimalBody


// --- --- ---
void CRFourLegsTinyAnimalBody::Init(){
	CRBody::Init();
}

// --- --- ---
void CRFourLegsTinyAnimalBody::InitBody(){
	CreateBody();
}

void CRFourLegsTinyAnimalBody::CreateBody(){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;

	// Solid
	solidDesc.mass		= massBody;
	solids[SO_BODY]		= phScene->CreateSolid(solidDesc);
	boxDesc.boxsize		= Vec3f(bodyBreadth, bodyHeight, bodyThickness);
	solids[SO_BODY]->AddShape(phSdk->CreateShape(boxDesc));
	solids[SO_BODY]->SetFramePosition(Vec3f(0,0,0));
	solids[SO_BODY]->SetOrientation(Quaterniond::Rot(Rad(-90), 'x'));
	solids[SO_BODY]->SetName("soWaist");
	//solids[SO_BODY]->SetInertia(Matrix3d(100000, 10000, 10000, 10000, 100000, 10000, 10000, 10000, 100000));
	solids[SO_BODY]->SetDynamical(false);
}
void CRFourLegsTinyAnimalBody::InitFrontLeg0(LREnum lr){	
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massFL;
	if(lr == RIGHTPART){
		solids[SO_RIGHT_FRONT_LEG_0] = phScene->CreateSolid(solidDesc);
		rcDesc.radius	 = upperSizes;
		rcDesc.length	 = upperLength;
		solids[SO_RIGHT_FRONT_LEG_0]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_RIGHT_FRONT_LEG_0]->SetName("soRightFrontLeg0");
	}
	else{
		solids[SO_LEFT_FRONT_LEG_0] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = upperSizes;
		rcDesc.length = upperLength;
		solids[SO_LEFT_FRONT_LEG_0]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_LEFT_FRONT_LEG_0]->SetName("soLeftFrontLeg0");
	}
	// �̊��ƑO�r�Ԃ̊֐�
	{
		ballDesc.poseSocket.Pos()	= Vec3d(lr * bodyBreadth / 2.0, -bodyHeight / 2.0, 0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0, upperLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeFrontSwing;
		ballDesc.limitTwist			= rangeFrontTwist;
	}
	if(lr == RIGHTPART){
		joints[JO_BODY_RIGHT_FRONT_LEG_0] = CreateJoint(solids[SO_BODY], solids[SO_RIGHT_FRONT_LEG_0], ballDesc);
		joints[JO_BODY_RIGHT_FRONT_LEG_0]->SetName("joRightShoulder");
		phScene->SetContactMode(solids[SO_BODY], solids[SO_RIGHT_FRONT_LEG_0], PHSceneDesc::MODE_NONE);
	}
	else{
		joints[JO_BODY_LEFT_FRONT_LEG_0] =  CreateJoint(solids[SO_BODY], solids[SO_LEFT_FRONT_LEG_0], ballDesc);
		joints[JO_BODY_LEFT_FRONT_LEG_0]->SetName("joLeftShoulder");
		phScene->SetContactMode(solids[SO_BODY], solids[SO_LEFT_FRONT_LEG_0], PHSceneDesc::MODE_NONE);
	}
}
void CRFourLegsTinyAnimalBody::InitFrontLeg1(LREnum lr){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massFF;
	if(lr == RIGHTPART){
		solids[SO_RIGHT_FRONT_LEG_1] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = lowerSizes;
		rcDesc.length = lowerLength;
		solids[SO_RIGHT_FRONT_LEG_1]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_RIGHT_FRONT_LEG_1]->SetName("soRightFrontLeg1");
	}
	else{
		solids[SO_LEFT_FRONT_LEG_1] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = lowerSizes;
		rcDesc.length = lowerLength;
		solids[SO_LEFT_FRONT_LEG_1]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_LEFT_FRONT_LEG_1]->SetName("soLeftFrontLeg1");
	}
	// �O�r-�O���Ԃ̊֐�
	{
		ballDesc.poseSocket.Pos()	= Vec3d(0, 0, -upperLength/2.0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0, lowerLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeFrontSwing;
		ballDesc.limitTwist			= rangeFrontTwist;
	}
	if(lr == RIGHTPART){
		joints[JO_BODY_RIGHT_FRONT_LEG_1] = CreateJoint(solids[SO_RIGHT_FRONT_LEG_0], solids[SO_RIGHT_FRONT_LEG_1], ballDesc);
		joints[JO_BODY_RIGHT_FRONT_LEG_1]->SetName("joRightElbow");
		phScene->SetContactMode(solids[SO_RIGHT_FRONT_LEG_0], solids[SO_RIGHT_FRONT_LEG_1], PHSceneDesc::MODE_NONE);
	}
	else{
		joints[JO_BODY_LEFT_FRONT_LEG_1] =  CreateJoint(solids[SO_LEFT_FRONT_LEG_0], solids[SO_LEFT_FRONT_LEG_1], ballDesc);
		joints[JO_BODY_LEFT_FRONT_LEG_1]->SetName("joLeftElbow");
		phScene->SetContactMode(solids[SO_LEFT_FRONT_LEG_0], solids[SO_LEFT_FRONT_LEG_1], PHSceneDesc::MODE_NONE);
	}

}
void CRFourLegsTinyAnimalBody::InitRearLeg0(LREnum lr){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massRL;
	if(lr == RIGHTPART){
		solids[SO_RIGHT_REAR_LEG_0] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = upperSizes;
		rcDesc.length = upperLength;
		solids[SO_RIGHT_REAR_LEG_0]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_RIGHT_REAR_LEG_0]->SetName("soRightRearLeg0");
	}
	else{
		solids[SO_LEFT_REAR_LEG_0] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = upperSizes;
		rcDesc.length = upperLength;
		solids[SO_LEFT_REAR_LEG_0]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_LEFT_REAR_LEG_0]->SetName("soLeftRearLeg0");
	}
	// �̊��ƌ�r�Ԃ̊֐�
	{
		ballDesc.poseSocket.Pos()	= Vec3d(lr * bodyBreadth / 2.0, bodyHeight / 2.0, 0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0, upperLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeRearSwing;
		ballDesc.limitTwist			= rangeRearTwist;
	}
	if(lr == RIGHTPART){
		joints[JO_BODY_RIGHT_REAR_LEG_0] = CreateJoint(solids[SO_BODY], solids[SO_RIGHT_REAR_LEG_0], ballDesc);
		joints[JO_BODY_RIGHT_REAR_LEG_0]->SetName("joRightHip");
		phScene->SetContactMode(solids[SO_BODY], solids[SO_RIGHT_REAR_LEG_0], PHSceneDesc::MODE_NONE);
	}
	else{
		joints[JO_BODY_LEFT_REAR_LEG_0] =  CreateJoint(solids[SO_BODY], solids[SO_LEFT_REAR_LEG_0], ballDesc);
		joints[JO_BODY_LEFT_REAR_LEG_0]->SetName("joLeftHip");
		phScene->SetContactMode(solids[SO_BODY], solids[SO_LEFT_REAR_LEG_0], PHSceneDesc::MODE_NONE);
	}
}
void CRFourLegsTinyAnimalBody::InitRearLeg1(LREnum lr){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massRF;
	if(lr == RIGHTPART){
		solids[SO_RIGHT_REAR_LEG_1] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = lowerSizes;
		rcDesc.length = lowerLength;
		solids[SO_RIGHT_REAR_LEG_1]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_RIGHT_REAR_LEG_1]->SetName("soRightRearLeg1");
	}
	else{
		solids[SO_LEFT_REAR_LEG_1] = phScene->CreateSolid(solidDesc);
		rcDesc.radius = lowerSizes;
		rcDesc.length = lowerLength;
		solids[SO_LEFT_REAR_LEG_1]->AddShape(phSdk->CreateShape(rcDesc));
		solids[SO_LEFT_REAR_LEG_1]->SetName("soLeftRearLeg1");
	}
	//��r-�㑫�Ԃ̊֐�
	{
		ballDesc.poseSocket.Pos()	= Vec3d(0, 0, -upperLength / 2.0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0,  lowerLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeRearSwing;
		ballDesc.limitTwist			= rangeRearTwist;
	}
	if(lr == RIGHTPART){
		joints[JO_BODY_RIGHT_REAR_LEG_1] = CreateJoint(solids[SO_RIGHT_REAR_LEG_0], solids[SO_RIGHT_REAR_LEG_1], ballDesc);
		joints[JO_BODY_RIGHT_REAR_LEG_1]->SetName("joRightRearElbow");
		phScene->SetContactMode(solids[SO_RIGHT_REAR_LEG_0], solids[SO_RIGHT_REAR_LEG_1], PHSceneDesc::MODE_NONE);
	}
	else{
		joints[JO_BODY_LEFT_REAR_LEG_1] =  CreateJoint(solids[SO_LEFT_REAR_LEG_0], solids[SO_LEFT_REAR_LEG_1], ballDesc);
		joints[JO_BODY_LEFT_REAR_LEG_1]->SetName("joLeftRearElbow");
		phScene->SetContactMode(solids[SO_LEFT_REAR_LEG_0], solids[SO_LEFT_REAR_LEG_1], PHSceneDesc::MODE_NONE);
	}
}

void CRFourLegsTinyAnimalBody::CreateFrontLegs(LREnum lr){
	InitFrontLeg0(lr);
	InitFrontLeg1(lr);
}

void CRFourLegsTinyAnimalBody::CreateRearLegs(LREnum lr){
	InitRearLeg0(lr);
	InitRearLeg1(lr);
}

// --- --- ---
void CRFourLegsTinyAnimalBody::InitLegs(){
	if(! noLegs){
		CreateFrontLegs(RIGHTPART);
		if(! onlyOneLeg){
			CreateFrontLegs(LEFTPART);
			CreateRearLegs(RIGHTPART);
			CreateRearLegs(LEFTPART);
		}
	}
}

// --- --- ---
void CRFourLegsTinyAnimalBody::InitContact(){
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
		if (DCAST(CRFourLegsTinyAnimalBodyIf,body)!=(this->Cast())) {
			for (int s=0; s<body->NSolids(); ++s) {
				for (unsigned int j=0; j<solids.size(); ++j) {
					phScene->SetContactMode(body->GetSolid(s), solids[j], PHSceneDesc::MODE_NONE);
				}
			}
		}
	}
}

void CRFourLegsTinyAnimalBody::InitControlMode(PHJointDesc::PHControlMode m){
	int njoints = joints.size();
	for(int i = 0; i < njoints; i++){
		if(joints[i]){joints[i]->SetMode(m);}
	}
}
}
