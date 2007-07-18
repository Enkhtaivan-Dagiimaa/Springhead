/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "CREyeController.h"

#define CREYECTRL_DEBUG true

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRPursuitController
void CRPursuitController::StepHoriz(double destLH, double destRH, double angleHeadH, double dt){
	double IL = destLH - angleLH;
	double IR = destRH - angleRH;
	double IVL = (IL - lastIL)/dt;
	double IVR = (IR - lastIR)/dt;
	lastIL = IL;
	lastIR = IR;

	intL += ((R1*S + R2*K)*IL + (R1*N + R2*H)*IVL + (R1*K + R2*S)*IR + (R1*H + R2*N)*IVR) * dt;
	intR += ((R1*S + R2*K)*IR + (R1*N + R2*H)*IVR + (R1*K + R2*S)*IL + (R1*H + R2*N)*IVL) * dt;
	angleLH += intL - A1*(R1+R2)*angleHeadH;
	angleRH += intR - A1*(R1+R2)*angleHeadH;
}

void CRPursuitController::StepVert(double destV, double angleHeadV, double dt){
	angleV = destV;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CREyeController
IF_OBJECT_IMP(CREyeController, SceneObject);

void CREyeController::LookAt(Vec3f pos, Vec3f vel){
	lookatPos = pos;
	lookatVel = vel;
}

void CREyeController::Step(){
	double dt = DCAST(PHSceneIf, GetScene())->GetTimeStep() / 10.0;

	// ���o����ɗp�����ԗʂ̌v�Z
	CalcEyeStatusValue();

	// �����Ԃ̑J��
	controlState = GetNextState(controlState);

	// ���ۂ̊ዅ�^������̎��s
	Vec3f targetL=Vec3d(0,0,0), targetR=Vec3d(0,0,0);
	double angleLH=0, angleLV=0, angleRH=0, angleRV=0;
	switch(controlState){
	case CS_SACCADE:
		if(CREYECTRL_DEBUG){std::cout << "Saccade" << std::endl;}
		pursuitCtrl.Reset();
		pursuitCtrl.angleLH = Vec3ToAngH(lookatPos-soLEye->GetPose().Pos());
		pursuitCtrl.angleV = Vec3ToAngV(lookatPos-soLEye->GetPose().Pos());
		pursuitCtrl.angleRH = Vec3ToAngH(lookatPos-soREye->GetPose().Pos());
		ControlEyeToTargetDir(soLEye, pursuitCtrl.angleLH, pursuitCtrl.angleV);
		ControlEyeToTargetDir(soREye, pursuitCtrl.angleRH, pursuitCtrl.angleV);
		break;
	case CS_PURSUIT:
		if(CREYECTRL_DEBUG){std::cout << "Pursuit" << std::endl;}
		pursuitCtrl.StepHoriz(locLLookatH, locRLookatH, 0, dt);
		pursuitCtrl.StepVert(locLLookatV, 0, dt);
		ControlEyeToTargetDir(soLEye, pursuitCtrl.angleLH, pursuitCtrl.angleV);
		ControlEyeToTargetDir(soLEye, pursuitCtrl.angleRH, pursuitCtrl.angleV);
		break;
	default:
		break;
	}
}

void CREyeController::CalcEyeStatusValue(){
	// �ዅ�̍��W�n�Ƃ��ē��̑O������Z���A�������Y���Ƃ���E��n���[�J�����W�ɕϊ�
	qToLoc = soHead->GetPose().Ori().Inv(); // �O���[�o�����烍�[�J�����W�ɕϊ�
	qToGlo = soHead->GetPose().Ori();       // ���[�J������O���[�o�����W�ɕϊ�

	// -- �����i�p�x�\���j
	locLEyeAxisH = Vec3ToAngH(qToLoc * soLEye->GetPose().Ori() * Vec3d(0,0,1));
	locLEyeAxisV = Vec3ToAngV(qToLoc * soLEye->GetPose().Ori() * Vec3d(0,0,1));
	locREyeAxisH = Vec3ToAngH(qToLoc * soREye->GetPose().Ori() * Vec3d(0,0,1));
	locREyeAxisV = Vec3ToAngV(qToLoc * soREye->GetPose().Ori() * Vec3d(0,0,1));
	
	// -- �e�ዅ���猩�����W�̕����i�p�x�\���j
	Vec3d locLLookat = qToLoc * (lookatPos - soLEye->GetPose().Pos());
	Vec3d locRLookat = qToLoc * (lookatPos - soREye->GetPose().Pos());
	double locLLookatH = Vec3ToAngH(locLLookat), locLLookatV = Vec3ToAngV(locLLookat);
	double locRLookatH = Vec3ToAngH(locRLookat), locRLookatV = Vec3ToAngV(locRLookat);

	if (CREYECTRL_DEBUG) {
		std::cout << "lookatPos : " << lookatPos << std::endl;

		std::cout << "locLEyeAxisH : " << Deg(locLEyeAxisH) << std::endl;
		std::cout << "locLEyeAxisV : " << Deg(locLEyeAxisV) << std::endl;
		std::cout << "locREyeAxisH : " << Deg(locREyeAxisH) << std::endl;
		std::cout << "locREyeAxisV : " << Deg(locREyeAxisV) << std::endl;

		std::cout << "locLLookatH : " << Deg(locLLookatH) << std::endl;
		std::cout << "locLLookatV : " << Deg(locLLookatV) << std::endl;
		std::cout << "locRLookatH : " << Deg(locRLookatH) << std::endl;
		std::cout << "locRLookatV : " << Deg(locRLookatV) << std::endl;
	}
}

CREyeControllerState::ControlState CREyeController::GetNextState(ControlState currentCS){
	double locErrLH = locLLookatH - locLEyeAxisH;
	double locErrLV = locLLookatV - locLEyeAxisV;
	double locErrRH = locRLookatH - locREyeAxisH;
	double locErrRV = locRLookatV - locREyeAxisV;

	// ��������Ǝ��̐����Ԃ̌���
	switch(controlState){
	case CS_SACCADE:
		if ((abs(locErrLH) < Rad(10.0f)) && (abs(locErrRH)  < Rad(10.0f)) && (abs(locErrLV) < Rad(10.0f)) && (abs(locErrRV) < Rad(10.0f))) {
			// Saccade�I��, Pursuit�ֈڍs
			return CS_PURSUIT;
		}else{
			// Saccade�p��
			return CS_SACCADE;
		}
		break;
	case CS_PURSUIT:
		if ((abs(locErrLH) > Rad(15.0f)) || (abs(locErrRH)  > Rad(15.0f)) || (abs(locErrLV) > Rad(15.0f)) || (abs(locErrRV) > Rad(15.0f))) {
			// Saccade�ֈڍs
			return CS_SACCADE;
		}else{
			// Pursuit�p��
			return CS_PURSUIT;
		}
		break;
	default:
		assert(false && "���B���Ȃ��͂��̕�");
		break;
	}

	// �����ɂ͂��Ȃ��͂�������
	return CS_SACCADE;
}

void CREyeController::ControlEyeToTargetDir(PHSolidIf* soEye, Vec3d target){
	/**/ //���ʂ�PD����
	Vec3f currentDir = (soEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, 1.0f)).unit();
	Vec3f errorYawPitch = PTM::cross(currentDir, target);
	Vec3f derror = soEye->GetAngularVelocity();
	Vec3f torque = (Kp * (errorYawPitch)) - (Kd * derror);
 	soEye->AddTorque(torque);
	/**/

	/*/ //���䂵�Ȃ��Œ��ڎp���ݒ�i���z�I�Ȃ����Ƃ��d������j
	double angH = Vec3ToAngH(target);
	double angV = Vec3ToAngV(target);
	std::cout << "(angH, angV) = (" << Deg(angH) << ", " << Deg(angV) << ")" << std::endl;
	Quaterniond ori = Quaterniond::Rot(-angV,'x')*Quaterniond::Rot(angH,'y');
	Posed pose = soEye->GetPose();
	pose.Ori() = ori;
	soEye->SetPose(pose);
	/**/
}

void CREyeController::ControlEyeToTargetDir(PHSolidIf* soEye, double horiz, double vert){
	/**/ //���ʂ�PD����
	Vec3d target = Quaterniond::Rot(-vert,'x')*Quaterniond::Rot(horiz,'y')*Vec3d(0,0,1);
	Vec3f currentDir = (soEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, 1.0f)).unit();
	Vec3f errorYawPitch = PTM::cross(currentDir, target);
	Vec3f derror = soEye->GetAngularVelocity();
	Vec3f torque = (Kp * (errorYawPitch)) - (Kd * derror);
 	soEye->AddTorque(torque);
	/**/

	/*/ //���䂵�Ȃ��Œ��ڎp���ݒ�i���z�I�Ȃ����Ƃ��d������j
	double angH = horiz;
	double angV = vert;
	std::cout << "(angH, angV) = (" << Deg(angH) << ", " << Deg(angV) << ")" << std::endl;
	Quaterniond ori = Quaterniond::Rot(-angV,'x')*Quaterniond::Rot(angH,'y');
	Posed pose = soEye->GetPose();
	pose.Ori() = ori;
	soEye->SetPose(pose);
	/**/
}

double CREyeController::Vec3ToAngH(Vec3d v){
	double D = sqrt(v.X()*v.X()+v.Z()*v.Z());
	return(atan2(v.X()/D, v.Z()/D));
}

double CREyeController::Vec3ToAngV(Vec3d v){
	double D = sqrt(v.Y()*v.Y()+v.Z()*v.Z());
	return(atan2(v.Y()/D, v.Z()/D));
}
}
