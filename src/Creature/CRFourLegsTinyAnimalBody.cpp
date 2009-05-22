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

	// �̊��Ɋւ���p�����[�^
	bodyBreadth   = 2.5;
	bodyHeight    = 3.8;
	bodyThickness = 0.6;

	// �r��RounCone�Ɋւ���p�����[�^
	upperSizes = Vec2d(0.4, 0.2);
	lowerSizes = Vec2d(0.2, 0.1);
	upperLength = 1.0;
	lowerLength = 1.4;

	// ���ʂɊւ���p�����[�^
	massFF		= 1.5;
	massFL		= 1.5;
	massRF		= 1.5;
	massRL		= 1.5;
	massBody	= 10;

	// �o�l�E�_���p�W��
	springFront   = 1.0;  damperFront   =  5.0;
	springRear    = 1.0;  damperRear    =  5.0;

	jointType = HINGE_MODE;

	// IF (jointType == BALL_MODE) THEN
	// Vec2d(lower, upper)  lower>upper�̂Ƃ����搧������
	rangeFrontSwing   = Vec2d(FLT_MAX, FLT_MAX);
	rangeFrontTwist	  = Vec2d(FLT_MAX, FLT_MAX);
	rangeRearSwing    = Vec2d(FLT_MAX, FLT_MAX);
	rangeRearTwist	  = Vec2d(FLT_MAX, FLT_MAX);

	// ELSE IF(jointType == HINGE_MODE) THEN
	// Vec2d(lower, upper)  lower>upper�̂Ƃ����搧������
	rangeUpperFront = Vec2d(FLT_MAX, FLT_MAX);
	rangeLowerFront = Vec2d(FLT_MAX, FLT_MAX);
	rangeUpperRear  = Vec2d(FLT_MAX, FLT_MAX);
	rangeLowerRear  = Vec2d(FLT_MAX, FLT_MAX);

	// �f�t�H���g�Ŏl�r���
	rfLeg = true;
	lfLeg = true;
	rrLeg = true;
	lrLeg = true;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRFourLegsTinyAnimalBody


// --- --- ---
void CRFourLegsTinyAnimalBody::Init(){
	CRBody::Init();
}

// --- --- ---
PHSolidIf* CRFourLegsTinyAnimalBody::InitBody(){
	return CreateBody();
}

PHSolidIf* CRFourLegsTinyAnimalBody::CreateBody(){
	CDBoxDesc          boxDesc;
	PHSolidDesc        solidDesc;

	// Solid
	solidDesc.mass	= massBody;
	solids.push_back(phScene->CreateSolid(solidDesc));
	boxDesc.boxsize	= Vec3f(bodyBreadth, bodyHeight, bodyThickness);
	solids.back()->AddShape(phSdk->CreateShape(boxDesc));
	solids.back()->SetFramePosition(Vec3f(0,0,0));
	solids.back()->SetOrientation(Quaterniond::Rot(Rad(-90), 'x'));
	solids.back()->SetName("soWaist");
	solids.back()->SetDynamical(false);

	return solids.back();
}
PHSolidIf* CRFourLegsTinyAnimalBody::InitFrontLeg0(LREnum lr, PHSolidIf* sBody){	
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;

	// Solid
	solidDesc.mass   = massFL;
	solids.push_back(phScene->CreateSolid(solidDesc));

	rcDesc.radius	 = upperSizes;
	rcDesc.length	 = upperLength;

	solids.back()->AddShape(phSdk->CreateShape(rcDesc));

	// �̊��ƑO�r�Ԃ̊֐�
	if(jointType == HINGE_MODE){
		PHHingeJointDesc hingeDesc;
		hingeDesc.poseSocket.Pos()	= Vec3d(lr * bodyBreadth / 2.0, -bodyHeight / 2.0, 0);
		hingeDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.posePlug.Pos()	= Vec3d(0, 0, upperLength / 2.0);
		hingeDesc.posePlug.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.origin			= 0;
		hingeDesc.spring			= springFront;
		hingeDesc.damper			= damperFront;
		hingeDesc.upper				= rangeUpperFront.upper;
		hingeDesc.lower				= rangeUpperFront.lower;
		joints.push_back(CreateJoint(sBody, solids.back(), hingeDesc));
	} else if(jointType == BALL_MODE){
		PHBallJointDesc		ballDesc;
		ballDesc.poseSocket.Pos()	= Vec3d(lr * bodyBreadth / 2.0, -bodyHeight / 2.0, 0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0, upperLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeFrontSwing;
		ballDesc.limitTwist			= rangeFrontTwist;
		joints.push_back(CreateJoint(sBody, solids.back(), ballDesc));
	}
	phScene->SetContactMode(sBody, solids.back(), PHSceneDesc::MODE_NONE);

	if(lr == RIGHTPART){
		solids.back()->SetName("soRightFrontLeg0");
		joints.back()->SetName("joRightShoulder");
	} else{
		solids.back()->SetName("soLeftFrontLeg0");
		joints.back()->SetName("joLeftShoulder");
	}

	return solids.back();
}
void CRFourLegsTinyAnimalBody::InitFrontLeg1(LREnum lr, PHSolidIf* sLeg0){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massFF;

	solids.push_back(phScene->CreateSolid(solidDesc));
	rcDesc.radius = lowerSizes;
	rcDesc.length = lowerLength;
	solids.back()->AddShape(phSdk->CreateShape(rcDesc));

	// �O�r-�O���Ԃ̊֐�
	if(jointType == HINGE_MODE){
		PHHingeJointDesc hingeDesc;
		hingeDesc.poseSocket.Pos()	= Vec3d(0, 0, -upperLength/2.0);
		hingeDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.posePlug.Pos()	= Vec3d(0, 0, lowerLength / 2.0);
		hingeDesc.posePlug.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.origin			= 0;
		hingeDesc.spring			= springFront;
		hingeDesc.damper			= damperFront;
		hingeDesc.upper				= rangeLowerFront.upper;
		hingeDesc.lower				= rangeLowerFront.lower;
		joints.push_back(CreateJoint(sLeg0, solids.back(), hingeDesc));
	} else if(jointType == BALL_MODE){
		ballDesc.poseSocket.Pos()	= Vec3d(0, 0, -upperLength/2.0);
		ballDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.posePlug.Pos()		= Vec3d(0, 0, lowerLength / 2.0);
		ballDesc.posePlug.Ori()		= Quaterniond::Rot(Rad(-90), 'x');
		ballDesc.goal				= Quaterniond::Rot(Rad(90), 'x');
		ballDesc.spring				= springFront;
		ballDesc.damper				= damperFront;
		ballDesc.limitSwing			= rangeFrontSwing;
		ballDesc.limitTwist			= rangeFrontTwist;
		joints.push_back(CreateJoint(sLeg0, solids.back(), ballDesc));
	}
	phScene->SetContactMode(sLeg0, solids.back(), PHSceneDesc::MODE_NONE);

	if(lr == RIGHTPART){
		solids.back()->SetName("soRightFrontLeg1");
		joints.back()->SetName("joRightElbow");
	} else{
		solids.back()->SetName("soLeftFrontLeg1");
		joints.back()->SetName("joLeftElbow");
	}

}
PHSolidIf* CRFourLegsTinyAnimalBody::InitRearLeg0(LREnum lr, PHSolidIf* sBody){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massRL;

	solids.push_back(phScene->CreateSolid(solidDesc));
	rcDesc.radius = upperSizes;
	rcDesc.length = upperLength;
	solids.back()->AddShape(phSdk->CreateShape(rcDesc));

	// �̊��ƌ�r�Ԃ̊֐�
		if(jointType == HINGE_MODE){
		PHHingeJointDesc hingeDesc;
		hingeDesc.poseSocket.Pos()	= Vec3d(lr * bodyBreadth / 2.0, bodyHeight / 2.0, 0);
		hingeDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.posePlug.Pos()	= Vec3d(0, 0, upperLength / 2.0);
		hingeDesc.posePlug.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.origin			= 0;
		hingeDesc.spring			= springFront;
		hingeDesc.damper			= damperFront;
		hingeDesc.upper				= rangeUpperRear.upper;
		hingeDesc.lower				= rangeUpperRear.lower;
		joints.push_back(CreateJoint(sBody, solids.back(), hingeDesc));
	}else if(jointType == BALL_MODE){
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
	joints.push_back(CreateJoint(sBody, solids.back(), ballDesc));
	phScene->SetContactMode(sBody, solids.back(), PHSceneDesc::MODE_NONE);

	if(lr == RIGHTPART){
		solids.back()->SetName("soRightRearLeg0");
		joints.back()->SetName("joRightHip");
	} else{
		solids.back()->SetName("soLeftRearLeg0");
		joints.back()->SetName("joLeftHip");
	}

	return solids.back();
}
void CRFourLegsTinyAnimalBody::InitRearLeg1(LREnum lr, PHSolidIf* sLeg0){
	CDRoundConeDesc		rcDesc;
	PHSolidDesc			solidDesc;
	PHBallJointDesc		ballDesc;

	// Solid
	solidDesc.mass   = massRF;
	solids.push_back(phScene->CreateSolid(solidDesc));
	rcDesc.radius = lowerSizes;
	rcDesc.length = lowerLength;
	solids.back()->AddShape(phSdk->CreateShape(rcDesc));

	//��r-�㑫�Ԃ̊֐�
	if(jointType == HINGE_MODE){
		PHHingeJointDesc hingeDesc;
		hingeDesc.poseSocket.Pos()	= Vec3d(0, 0, -upperLength/2.0);
		hingeDesc.poseSocket.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.posePlug.Pos()	= Vec3d(0, 0, lowerLength / 2.0);
		hingeDesc.posePlug.Ori()	= Quaterniond::Rot(Rad(-90), 'x');
		hingeDesc.origin			= 0;
		hingeDesc.spring			= springFront;
		hingeDesc.damper			= damperFront;
		hingeDesc.upper				= rangeLowerRear.upper;
		hingeDesc.lower				= rangeLowerRear.lower;
		joints.push_back(CreateJoint(sLeg0, solids.back(), hingeDesc));
	} else if(jointType == BALL_MODE){
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
	joints.push_back(CreateJoint(sLeg0, solids.back(), ballDesc));
	phScene->SetContactMode(sLeg0, solids.back(), PHSceneDesc::MODE_NONE);
	if(lr == RIGHTPART){
		solids.back()->SetName("soRightRearLeg1");
		joints.back()->SetName("joRightRearElbow");
	} else{
		solids.back()->SetName("soLeftRearLeg1");
		joints.back()->SetName("joLeftRearElbow");
	}
}

void CRFourLegsTinyAnimalBody::CreateFrontLegs(LREnum lr, PHSolidIf* sBody){
	PHSolidIf* leg0 = InitFrontLeg0(lr, sBody);
	InitFrontLeg1(lr, leg0);
}

void CRFourLegsTinyAnimalBody::CreateRearLegs(LREnum lr, PHSolidIf* sBody){
	PHSolidIf* leg0 = InitRearLeg0(lr, sBody);
	InitRearLeg1(lr, leg0);
}

// --- --- ---
void CRFourLegsTinyAnimalBody::InitLegs(PHSolidIf* sBody){
	if(rfLeg) CreateFrontLegs(RIGHTPART, sBody);
	if(lfLeg) CreateFrontLegs(LEFTPART,  sBody);
	if(rrLeg) CreateRearLegs (RIGHTPART, sBody);
	if(lrLeg) CreateRearLegs (LEFTPART,  sBody);
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
