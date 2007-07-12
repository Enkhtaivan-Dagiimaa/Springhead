/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CREyeController.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{

IF_OBJECT_IMP(CREyeController, SceneObject);

void CREyeController::LookAt(Vec3f pos, Vec3f vel){
	nextLookatPos = pos;
	nextLookatVel = vel;
}

void CREyeController::Step(){
	/*/ �������͂Ƃ肠���������R�[�h
	currLookatPos = nextLookatPos;
	
	Vec3f targetDirL  = (currLookatPos - soLEye->GetPose().Pos()).unit();
	Vec3f targetDirR  = (currLookatPos - soREye->GetPose().Pos()).unit();

	ControlEyeToTargetDir(soLEye, targetDirL);
	ControlEyeToTargetDir(soREye, targetDirR);
	/*/
	controlState = GetNextState(controlState);

	Vec3f targetDirL, targetDirR;
	switch(controlState){
	case CS_SACCADE:
		SaccadeControl();
		break;
	case CS_PURSUIT:
		PursuitControl();
		break;
	default:
		break;
	}
	/**/
}

CREyeControllerState::ControlState CREyeController::GetNextState(ControlState currentCS){
	switch(controlState){
	case CS_SACCADE:
		return CS_SACCADE;
		break;
	case CS_PURSUIT:
		return CS_SACCADE;
		break;
	default:
		break;
	}
}

void CREyeController::SaccadeControl(){
	currLookatPos = nextLookatPos;
	currLookatVel = nextLookatVel;

	Vec3f targetDirL  = (currLookatPos - soLEye->GetPose().Pos()).unit();
	Vec3f targetDirR  = (currLookatPos - soREye->GetPose().Pos()).unit();

	ControlEyeToTargetDir(soLEye, targetDirL);
	ControlEyeToTargetDir(soREye, targetDirR);
}

void CREyeController::PursuitControl(){
}

void CREyeController::ControlEyeToTargetDir(PHSolidIf* soEye, Vec3f target){
	Vec3f currentDir = (soEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, 1.0f)).unit();
	Vec3f errorYawPitch = PTM::cross(currentDir, target);
	Vec3f derror = soEye->GetAngularVelocity();
	Vec3f torque = (Kp * (errorYawPitch)) - (Kd * derror);

 	soEye->AddTorque(torque);
}

}
