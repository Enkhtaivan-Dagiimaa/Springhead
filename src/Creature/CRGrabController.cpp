/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CRGrabController.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// 
void CRGrabController::Init(){
	CRController::Init(); 

	/*
	// ����ɗp����Body�Ƃ��̍��̂̎擾
	CRHingeHumanBodyIf* hiBody=NULL;
	CRTrunkFootHumanBodyIf* tfBody=NULL;
	for (int i=0; i<creature->NBodies(); ++i) {
		if (!hiBody) {
			hiBody = DCAST(CRHingeHumanBodyIf, creature->GetBody(i));
		}
		if (!tfBody) {
			tfBody = DCAST(CRTrunkFootHumanBodyIf, creature->GetBody(i));
		}
	}
	body = hiBody;

	soWaist  = hiBody->GetSolid(CRHingeHumanBodyDesc::SO_WAIST);
	soChest  = hiBody->GetSolid(CRHingeHumanBodyDesc::SO_CHEST);

	soWaistT = tfBody->GetSolid(CRTrunkFootHumanBodyDesc::SO_WAIST);
	soChestT = tfBody->GetSolid(CRTrunkFootHumanBodyDesc::SO_CHEST);

	// ���ʂ̃R���g���[���iReaching�j�̎擾
	reachLeft  = NULL; reachRight = NULL; reachChest = NULL;
	for (int i=0; i<creature->NControllers(); i++) {
		if (!reachLeft) {
			CRReachingControllerIf* r = DCAST(CRReachingControllerIf, creature->GetController(i));
			if (r && r->GetSolid()==hiBody->GetSolid(CRHingeHumanBodyDesc::SO_LEFT_HAND)) {
				reachLeft = r;
			}
		}
		if (!reachRight) {
			CRReachingControllerIf* r = DCAST(CRReachingControllerIf, creature->GetController(i));
			if (r && r->GetSolid()==hiBody->GetSolid(CRHingeHumanBodyDesc::SO_RIGHT_HAND)) {
				reachRight = r;
			}
		}
		if (!reachChest) {
			CRReachingControllerIf* r = DCAST(CRReachingControllerIf, creature->GetController(i));
			if (r && r->GetSolid()==hiBody->GetSolid(CRHingeHumanBodyDesc::SO_CHEST)) {
				reachChest = r;
			}
		}
	}

	// ���B�\�����f���邽�߂̐��l���Ɍv�Z���Ă���
	CRHingeHumanBodyDesc descHIBody;
	hiBody->GetDesc(&descHIBody);
	reachableDistance = (float)((descHIBody.upperArmLength + descHIBody.lowerArmLength + (descHIBody.handLength / 2.0f) + descHIBody.chestHeight + descHIBody.abdomenHeight + (descHIBody.waistHeight / 2.0f)));
	shoulderHeightFromWaist = (float)(descHIBody.waistHeight/2.0 + descHIBody.abdomenHeight + descHIBody.chestHeight);

	// ������Ԃ̐ݒ�
	controlState = CRGrabControllerIf::CRGC_STANDBY;
	*/
}

void CRGrabController::Step(){
	/*
	int RSSR = CRReachingControllerIf::RS_SOLID_REACHED;
	if (controlState==CRGrabControllerIf::CRGC_REACH) {
		// ��̂Ђ�̌������X�V��������
		reachLeft->SetTargetOri(soChest->GetPose().Ori()*Quaterniond::Rot(Rad(90),'y'), Vec3d(0,0,0));
		reachRight->SetTargetOri(soChest->GetPose().Ori()*Quaterniond::Rot(Rad(-90),'y'), Vec3d(0,0,0));

		if (reachLeft->GetReachState()==RSSR && reachRight->GetReachState()==RSSR) {
			// �ێ��p�΂˂�L����
			grabSpring.first->Enable(true);
			grabSpring.second->Enable(true);

			controlState = CRGrabControllerIf::CRGC_REACH_COMPLETE;
		}

	} else if (controlState==CRGrabControllerIf::CRGC_UPHOLD) {
		if (reachLeft->GetReachState()==RSSR && reachRight->GetReachState()==RSSR) {
			// �̂���������
			// DCAST(CRHingeHumanBodyIf,body)->KeepUpperBodyPose();
			// DCAST(CRHingeHumanBodyIf,body)->SetUpperBodyStiffness(5.0);
			// ����I��
			// reachLeft->Reset();
			// reachRight->Reset();
			reachChest->Reset();

			controlState = CRGrabControllerIf::CRGC_UPHOLD_COMPLETE;
		}

	} else if (controlState==CRGrabControllerIf::CRGC_UPHOLD_COMPLETE) {
		reachLeft->SetTargetPos(soWaistT->GetPose()*Vec3f(-targetRadius, targetRadius*2.5, -targetRadius*1.2), Vec3f(0,0,0));
		reachLeft->SetTargetOri(soChestT->GetPose().Ori()*Quaterniond::Rot(Rad(90),'y'), Vec3d(0,0,0));
		reachRight->SetTargetPos(soWaistT->GetPose()*Vec3f( targetRadius, targetRadius*2.5, -targetRadius*1.2), Vec3f(0,0,0));
		reachRight->SetTargetOri(soChestT->GetPose().Ori()*Quaterniond::Rot(Rad(-90),'y'), Vec3d(0,0,0));

	} else if (controlState==CRGrabControllerIf::CRGC_PLACE) {
		if (reachLeft->GetReachState()==RSSR && reachRight->GetReachState()==RSSR) {
			// �S������I��
			AbortAll();

			controlState = CRGrabControllerIf::CRGC_PLACE_COMPLETE;
		}
	}
	*/
}

bool CRGrabController::Reach(PHSolidIf* solid, float radius){
	return false;
	/*
	if (!IsReachable(solid, (float)0.9)) {
		// �Ƃǂ��Ȃ�
		return false;
	}
	
	targetSolid  = solid;
	targetRadius = radius;

	// �c���n�_�̌v�Z
	Vec3f reachPointDirL;
	Vec3f w2t = (targetSolid->GetPose().Pos() - soWaist->GetPose().Pos());
	reachPointDirL = Vec3f(w2t.Z(), 0, -w2t.X()).unit();
	if (PTM::cross(w2t, reachPointDirL).Y() < 0) {
		reachPointDirL = -reachPointDirL;
	}

	// �c���p�̃o�l�������C������΍쐬
	GrabSpringMap::iterator it = grabSpringMap.find(targetSolid);
	if (it==grabSpringMap.end()) {
		PHSpringDesc descSpring;
		descSpring.bEnabled = false;
		descSpring.spring   = Vec3d(1,1,1) * 500;
		descSpring.damper   = Vec3d(1,1,1) *   5;
		descSpring.poseSocket.Pos() = Vec3f(0,0,0);

		descSpring.posePlug.Pos()   = targetSolid->GetPose().Ori().Inv() *  reachPointDirL*targetRadius*0.95 - Vec3f(0,0.25,0);
		PHJointIf* sprL = phScene->CreateJoint(reachLeft->GetSolid(), targetSolid, descSpring);

		descSpring.posePlug.Pos()   = targetSolid->GetPose().Ori().Inv() * -reachPointDirL*targetRadius*0.95 - Vec3f(0,0.25,0);
		PHJointIf* sprR = phScene->CreateJoint(reachRight->GetSolid(), targetSolid, descSpring);

		SpringPair sp;
		sp.first  = DCAST(PHSpringIf,sprL);
		sp.second = DCAST(PHSpringIf,sprR);
		grabSpringMap[targetSolid] = sp;

		grabSpring = sp;
	} else {
		grabSpring = it->second;
	}

	// �����₷���悤�㔼�g�����炩������
	// DCAST(CRHingeHumanBodyIf,body)->SetUpperBodyStiffness(0.5);

	// ���B�^���̊J�n���w��
	reachLeft->SetTargetPos(targetSolid->GetPose().Pos() + reachPointDirL*targetRadius*1.0, solid->GetVelocity());
	reachLeft->SetTargetOri(soChest->GetPose().Ori()*Quaterniond::Rot(Rad(90),'y'), Vec3d(0,0,0));
	reachLeft->SetTargetTime(1.5);

	reachRight->SetTargetPos(targetSolid->GetPose().Pos() - reachPointDirL*targetRadius*1.0, solid->GetVelocity());
	reachRight->SetTargetOri(soChest->GetPose().Ori()*Quaterniond::Rot(Rad(-90),'y'), Vec3d(0,0,0));
	reachRight->SetTargetTime(1.5);

	reachChest->SetTargetPos(targetSolid->GetPose().Pos(), Vec3f(0,0,0));
	reachChest->SetTargetTime(1.5);

	reachLeft->Start(CRReachingControllerIf::CM_P3R2, -1);
	reachRight->Start(CRReachingControllerIf::CM_P3R2, -1);
	//reachChest->Start(CRReachingControllerIf::CM_P3R0, -1);

	controlState = CRGrabControllerIf::CRGC_REACH;
	return true;
	*/
}

bool CRGrabController::IsReachable(PHSolidIf* solid){
	return false;
	// return IsReachable(solid, 1.0f);
}

bool CRGrabController::IsReachable(PHSolidIf* solid, float safety){
	return false;
	/*
	float distance = (solid->GetPose().Pos() - soWaist->GetPose().Pos()).norm();
	return(distance < (reachableDistance*safety));
	*/
}

bool CRGrabController::IsReachComplete(){
	return false;
	// return(controlState==CRGrabControllerIf::CRGC_REACH_COMPLETE);
}

bool CRGrabController::Uphold(){
	return false;
	/*
	if (!IsUpholdable()) {
		// �܂��͂�łȂ�
		return false;
	}

	reachLeft->SetTargetPos(soWaistT->GetPose()*Vec3f(-targetRadius, targetRadius*2.5, -targetRadius*1.2), Vec3f(0,0,0));
	reachLeft->SetTargetOri(soChestT->GetPose().Ori()*Quaterniond::Rot(Rad(90),'y'), Vec3d(0,0,0));
	reachLeft->SetTargetTime(2.0);

	reachRight->SetTargetPos(soWaistT->GetPose()*Vec3f( targetRadius, targetRadius*2.5, -targetRadius*1.2), Vec3f(0,0,0));
	reachRight->SetTargetOri(soChestT->GetPose().Ori()*Quaterniond::Rot(Rad(-90),'y'), Vec3d(0,0,0));
	reachRight->SetTargetTime(2.0);

	reachChest->SetTargetPos(soWaistT->GetPose().Pos()+Vec3f(0,shoulderHeightFromWaist,0), Vec3f(0,0,0));
	reachChest->SetTargetTime(1.0);

	reachLeft->Start(CRReachingControllerIf::CM_P3R2, -1);
	reachRight->Start(CRReachingControllerIf::CM_P3R2, -1);
	// reachChest->Start(CRReachingControllerIf::CM_P3R0, -1);

	controlState = CRGrabControllerIf::CRGC_UPHOLD;
	return true;
	*/
}

bool CRGrabController::IsUpholdable(){
	return false;
	// return(controlState==CRGrabControllerIf::CRGC_REACH_COMPLETE);
}

bool CRGrabController::IsUpholdComplete(){
	return false;
	// return(controlState==CRGrabControllerIf::CRGC_UPHOLD_COMPLETE);
}

bool CRGrabController::Place(Vec3d pos){
	return false;
	/*
	if (!IsPlaceable(pos)) {
		return false;
	}

	Vec3f reachPointDirL;
	Vec3f w2t = (pos - soWaist->GetPose().Pos());
	reachPointDirL = Vec3f(w2t.Z(), 0, -w2t.X()).unit();
	if (PTM::cross(w2t, reachPointDirL).Y() < 0) {
		reachPointDirL = -reachPointDirL;
	}

	reachLeft->SetTargetPos(pos + reachPointDirL*targetRadius*1.2, Vec3f(0,0,0));
	reachLeft->SetTargetTime(1.5);

	reachRight->SetTargetPos(pos - reachPointDirL*targetRadius*1.2, Vec3f(0,0,0));
	reachRight->SetTargetTime(1.5);

	reachChest->SetTargetPos(pos, Vec3f(0,0,0));
	reachChest->SetTargetTime(1.5);

	reachLeft->Start(CRReachingControllerIf::CM_P3R0, -1);
	reachRight->Start(CRReachingControllerIf::CM_P3R0, -1);
	// reachChest->Start(CRReachingControllerIf::CM_P3R0, -1);

	controlState = CRGrabControllerIf::CRGC_PLACE;

	return true;
	*/
}

bool CRGrabController::IsPlaceable(Vec3d pos){
	return false;
	// return IsPlaceable(pos, 1.0f);
}

bool CRGrabController::IsPlaceable(Vec3d pos, float safety){
	return false;
	/*
	if (controlState!=CRGrabControllerIf::CRGC_UPHOLD_COMPLETE && controlState!=CRGrabControllerIf::CRGC_PLACE) {
		return false;
	}
	float distance = (pos - soWaist->GetPose().Pos()).norm();
	return(distance < (reachableDistance*safety));
	*/
}

bool CRGrabController::IsPlaceComplete(){
	return false;
	/*
	return(controlState==CRGrabControllerIf::CRGC_PLACE_COMPLETE
		|| controlState==CRGrabControllerIf::CRGC_STANDBY);
	*/
}

void CRGrabController::Abort(){
	/*
	int RSSR = CRReachingControllerIf::RS_SOLID_REACHED;
	if (controlState==CRGrabControllerIf::CRGC_REACH) {
		AbortAll();
	} else if (controlState==CRGrabControllerIf::CRGC_UPHOLD) {
		reachChest->Reset();
		controlState = CRGrabControllerIf::CRGC_REACH_COMPLETE;
	} else if (controlState==CRGrabControllerIf::CRGC_PLACE) {
		reachLeft->Reset();
		reachRight->Reset();
		reachChest->Reset();
		if (reachLeft->GetReachState()==RSSR && reachRight->GetReachState()==RSSR) {
			targetSolid = NULL;
			targetRadius = 0.0f;
			placePos = Vec3f(0,0,0);
			grabSpring.first = NULL;
			grabSpring.second = NULL;
			controlState = CRGrabControllerIf::CRGC_PLACE_COMPLETE;
		}
	}
	// DCAST(CRHingeHumanBodyIf,body)->ResetUpperBodyPose();
	// DCAST(CRHingeHumanBodyIf,body)->SetUpperBodyStiffness(1.0);
	*/
}

void CRGrabController::AbortAll(){
	/*
	if (grabSpring.first && grabSpring.second) {
		grabSpring.first->Enable(false);
		grabSpring.second->Enable(false);
	}
	reachLeft->Reset();
	reachRight->Reset();
	reachChest->Reset();
	targetSolid = NULL;
	targetRadius = 0.0f;
	placePos = Vec3f(0,0,0);
	grabSpring.first = NULL;
	grabSpring.second = NULL;
	// DCAST(CRHingeHumanBodyIf,body)->ResetUpperBodyPose();
	// DCAST(CRHingeHumanBodyIf,body)->SetUpperBodyStiffness(1.0);
	controlState = CRGrabControllerIf::CRGC_STANDBY;
	*/
}

CRGrabControllerIf::CRGCControlState CRGrabController::GetControlState(){
	// return controlState;
	return((CRGrabControllerIf::CRGCControlState)0);
}
}
