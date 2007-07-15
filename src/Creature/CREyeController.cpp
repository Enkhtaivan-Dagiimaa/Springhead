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

IF_OBJECT_IMP(CREyeController, SceneObject);

void CREyeController::LookAt(Vec3f pos, Vec3f vel){
	nextLookatPos = pos;
	nextLookatVel = vel;
}

void CREyeController::Step(){
	// ���o����ɗp�����ԗʂ̌v�Z
	CalcEyeStatusValue();

	ControlEyeToTargetDir(soLEye, nextLookatPos - soLEye->GetPose().Pos());
	//ControlEyeToTargetDir(soREye, nextLookatPos - soREye->GetPose().Pos());
	return;

	// �����Ԃ̑J��
	controlState = GetNextState(controlState);

	// ���ۂ̊ዅ�^������̎��s
	Vec3f targetDirL, targetDirR;
	switch(controlState){
	case CS_SACCADE:
		if(CREYECTRL_DEBUG){std::cout << "Saccade" << std::endl;}
		SaccadeControl();
		break;
	case CS_PURSUIT:
		if(CREYECTRL_DEBUG){std::cout << "Pursuit" << std::endl;}
		PursuitControl();
		break;
	default:
		break;
	}

	//ControlEyeToTargetDir(soLEye, nextLookatPos - soLEye->GetPose().Pos());
	//ControlEyeToTargetDir(soREye, nextLookatPos - soREye->GetPose().Pos());
}

void CREyeController::CalcEyeStatusValue(){
	// �Վ�
	currLookatPos = nextLookatPos;
	currLookatVel = nextLookatVel;

	// �ዅ�̍��W�n�Ƃ��ē��̑O������Z���A�������Y���Ƃ���E��n���[�J�����W�ɕϊ�
	Quaterniond qToLoc = soHead->GetPose().Ori().Inv(); // �O���[�o�����烍�[�J�����W�ɕϊ�

	// ��{��
	// -- �ڂ̉�]��
	Quaterniond locLEyeOri, locREyeOri;
	locLEyeOri = qToLoc * soLEye->GetPose().Ori();
	locREyeOri = qToLoc * soREye->GetPose().Ori();
	// -- �ڂ̉�]�p���x
	Vec3d locLEyeAngvel, locREyeAngvel;
	locLEyeAngvel = qToLoc * soLEye->GetAngularVelocity();
	locREyeAngvel = qToLoc * soREye->GetAngularVelocity();
	// -- ���̉�]�p���x
	Vec3d locHeadAngvel;
	locHeadAngvel = qToLoc * soHead->GetAngularVelocity();

	// ���ԗ�
	// -- �����i�p�x�\���j
	double locLEyeAxisH = Vec3ToAngH(locLEyeOri * Vec3d(0,0,1));
	double locLEyeAxisV = Vec3ToAngV(locLEyeOri * Vec3d(0,0,1));
	double locREyeAxisH = Vec3ToAngH(locREyeOri * Vec3d(0,0,1));
	double locREyeAxisV = Vec3ToAngV(locREyeOri * Vec3d(0,0,1));
	// -- �e�ዅ���猩�����W�̕����i�p�x�\���j
	Vec3d locLLookat = qToLoc * (currLookatPos - soLEye->GetPose().Pos());
	Vec3d locRLookat = qToLoc * (currLookatPos - soREye->GetPose().Pos());
	double locLLookatH = Vec3ToAngH(locLLookat), locLLookatV = Vec3ToAngV(locLLookat);
	double locRLookatH = Vec3ToAngH(locRLookat), locRLookatV = Vec3ToAngV(locRLookat);

	// ����Ɏg����
	double dt = DCAST(PHSceneIf, GetScene())->GetTimeStep();
	// -- ���W�����덷
	locErrLH = locLLookatH - locLEyeAxisH;
	locErrLV = locLLookatV - locLEyeAxisV;
	locErrRH = locRLookatH - locREyeAxisH;
	locErrRV = locRLookatV - locREyeAxisV;
	// -- ���W�����덷�ω���
	locDErrLH = (locErrLH - locLastErrLH) / dt; locLastErrLH = locErrLH;
	locDErrLV = (locErrLV - locLastErrLV) / dt; locLastErrLV = locErrLV;
	locDErrRH = (locErrRH - locLastErrRH) / dt; locLastErrRH = locErrRH;
	locDErrRV = (locErrRV - locLastErrRV) / dt; locLastErrRV = locErrRV;
	// -- ������]�p���x
	locHeadAngvelH = Vec3ToAngH(locHeadAngvel);
	locHeadAngvelV = Vec3ToAngV(locHeadAngvel);

	if (CREYECTRL_DEBUG) {
		std::cout << "locLEyeAxisH : " << Deg(locLEyeAxisH) << std::endl;
		std::cout << "locLEyeAxisV : " << Deg(locLEyeAxisV) << std::endl;
		std::cout << "locREyeAxisH : " << Deg(locREyeAxisH) << std::endl;
		std::cout << "locREyeAxisV : " << Deg(locREyeAxisV) << std::endl;

		std::cout << "locLLookatH : " << Deg(locLLookatH) << std::endl;
		std::cout << "locLLookatV : " << Deg(locLLookatV) << std::endl;
		std::cout << "locRLookatH : " << Deg(locRLookatH) << std::endl;
		std::cout << "locRLookatV : " << Deg(locRLookatV) << std::endl;

		std::cout << "locErrLH : " << Deg(locErrLH) << std::endl;
		std::cout << "locErrLV : " << Deg(locErrLV) << std::endl;
		std::cout << "locErrRH : " << Deg(locErrRH) << std::endl;
		std::cout << "locErrRV : " << Deg(locErrRV) << std::endl;
	}

	/*
	Vec3f apHoriz, apVert;
	apHoriz = apVert = soHead->GetPose().Ori().Inv() * currLookatPos;

	apHoriz[1] = 0.0f;
	apHoriz = soHead->GetPose().Ori() * apHoriz;
	apVert[0]  = 0.0f;
	apVert  = soHead->GetPose().Ori() * apVert;

	Vec3f eyeDirRelL = soHead->GetPose().Ori().Inv() * soLEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, -1.0f);
	Vec3f eyeDirRelR = soHead->GetPose().Ori().Inv() * soREye->GetPose().Ori() * Vec3f(0.0f, 0.0f, -1.0f);
	t1 =  atan2(eyeDirRelL.Z(), -eyeDirRelL.X());
	t2 =  atan2(eyeDirRelR.Z(), -eyeDirRelR.X());
	t3 = -atan2(eyeDirRelL.Y(),  eyeDirRelL.Z());
	t4 = -atan2(eyeDirRelR.Y(),  eyeDirRelR.Z());

	float dt = (float)(DCAST(PHSceneIf, GetScene())->GetTimeStep());

	Vec3f vecL = apHoriz - soLEye->GetCenterPosition();
	vecL = soHead->GetPose().Ori().Inv() * vecL;
	float t1_a = atan2(-vecL.Z(),vecL.X());
	eL   = t1_a - t1;
	vL   = (t1_a - last_t1_a) / dt;
	last_t1_a = t1_a;

	Vec3f vecR = apHoriz - soREye->GetCenterPosition();
	vecR = soHead->GetPose().Ori().Inv() * vecR;
	float t2_a = atan2(-vecR.Z(),vecR.X());
	eR   = t2_a - t2;
	vR   = (t2_a - last_t2_a) / dt;
	last_t2_a = t2_a;

	Vec3f vecLV = apVert - soLEye->GetCenterPosition();
	vecLV = soHead->GetPose().Ori().Inv() * vecLV;
	float t3_a = atan2(vecLV.Y(),-vecLV.Z());
	eLV  = t3_a - t3;
	vLV  = (t3_a - last_t3_a) / dt;
	last_t3_a = t3_a;

	Vec3f vecRV = apVert - soREye->GetCenterPosition();
	vecRV = soHead->GetPose().Ori().Inv() * vecRV;
	float t4_a = atan2(vecRV.Y(),-vecRV.Z());
	eRV  = t4_a - t4;
	vRV  = (t4_a - last_t4_a) / dt;
	last_t4_a = t4_a;
	*/
}

CREyeControllerState::ControlState CREyeController::GetNextState(ControlState currentCS){
	// ��������Ǝ��̐����Ԃ̌���
	switch(controlState){
	case CS_SACCADE:
		std::cout << Deg(eL) << ", " << Deg(eR) << ", " << Deg(eLV) << ", " << Deg(eRV) << std::endl;
		if ((abs(eL) < Rad(0.1f)) && (abs(eR)  < Rad(0.1f)) && (abs(eLV) < Rad(0.1f)) && (abs(eRV) < Rad(0.1f))) {
			// Saccade�I��, Pursuit�ֈڍs
			integrator_L = integrator_R = integrator_Lv = integrator_Rv = 0.0f;
			soLEye->SetVelocity(Vec3f(0,0,0));
			soREye->SetVelocity(Vec3f(0,0,0));
			return CS_PURSUIT;
		}else{
			// Saccade�p��
			return CS_SACCADE;
		}
		break;
	case CS_PURSUIT:
		//if ((abs(eL) > Rad(1.0f)) || (abs(eR)  > Rad(1.0f)) || (abs(eLV) > Rad(1.0f)) || (abs(eRV) > Rad(1.0f))) {
		if ((abs(eL) > Rad(5.0f)) || (abs(eR)  > Rad(5.0f)) || (abs(eLV) > Rad(5.0f)) || (abs(eRV) > Rad(5.0f))) {
		//if ((abs(eL) > Rad(50.0f)) || (abs(eR)  > Rad(50.0f)) || (abs(eLV) > Rad(50.0f)) || (abs(eRV) > Rad(50.0f))) {
			// Saccade�ֈڍs
			currLookatPos = nextLookatPos;
			currLookatVel = nextLookatVel;
			saccadeTimer = 0.0f;
			saccadeFromR = soLEye->GetPose().Ori() * Vec3f(0.0f,0.0f,1.0f);
			saccadeFromL = soREye->GetPose().Ori() * Vec3f(0.0f,0.0f,1.0f);
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

void CREyeController::SaccadeControl(){
	Vec3f saccadeToL  = (currLookatPos - soLEye->GetPose().Pos()).unit();
	Vec3f saccadeToR  = (currLookatPos - soREye->GetPose().Pos()).unit();

	float aL = acos(PTM::dot(saccadeToL/saccadeToL.norm(), saccadeFromL/saccadeFromL.norm()));
	float aR = acos(PTM::dot(saccadeToR/saccadeToR.norm(), saccadeFromR/saccadeFromR.norm()));
	float t = (saccadeTimer += (float)((DCAST(PHSceneIf, GetScene()))->GetTimeStep()));
	float L = (float)Rad(500.0f);
	float T = 0.05f;
	float lengthL = 1.0f;
	float lengthR = 1.0f;

	#define theta(x)   (6*pow(((x)/T),5) - 15*pow(((x)/T),4) + 10*pow(((x)/T),3))
	#define dtheta(x)  (30*pow((x),4)/pow(T,5) - 60*pow((x),4)/pow(T,4) + 30*pow((x),2)/pow(T,3))

	if (aL*dtheta(T/2.0f) >= L) {
		float a_ = L/dtheta(T/2.0f);
		if (0 <= t && t < T/2.0f) {
			lengthL = (a_*theta(t))/aL;
		} else if (t < ((aL-a_)/L + T/2.0f)) {
			lengthL = (L*t + a_/2.0f -L*T/2.0f)/aL;
		} else if (t < ((aL-a_)/L + T     )) {
			lengthL = (a_*theta(t-(aL-a_)/L) + (aL-a_))/aL;
		}
	} else {
		if (0 <=t && t < T) {
			lengthL = (aL*theta(t))/aL;
		}
	}
	if (aR*dtheta(T/2.0f) >= L) {
		float a_ = L/dtheta(T/2.0f);
		if (0 <= t && t < T/2.0f) {
			lengthR = (a_*theta(t))/aR;
		} else if (t < ((aR-a_)/L + T/2.0f)) {
			lengthR = (L*t + a_/2.0f -L*T/2.0f)/aR;
		} else if (t < ((aR-a_)/L + T     )) {
			lengthR = (a_*theta(t-(aR-a_)/L) + (aR-a_))/aR;
		}
	} else {
		if (0 <=t && t < T) {
			lengthR = (aR*theta(t))/aR;
		}
	}

	Quaternionf headori     = soHead->GetPose().Ori();
	Quaternionf headori_inv = headori.Inv();
	Vec3f dirL = headori*((headori_inv*saccadeToL)*lengthL) + saccadeFromL*(1-lengthL);
	Vec3f dirR = headori*((headori_inv*saccadeToR)*lengthR) + saccadeFromR*(1-lengthR);

	ControlEyeToTargetDir(soLEye, dirL);
	ControlEyeToTargetDir(soREye, dirR);
}

void CREyeController::PursuitControl(){
	const float alpha1 = 0.5f;
	const float rho1 = 1.50f;
	const float rho2 = 0.50f;
	// Error
	const float sigma = 100.0f;
	const float kappa = 50.0f;
	// Velocity
	const float nu =  0.5f;
	const float eta = 0.2f;

	currLookatPos = nextLookatPos;
	currLookatVel = nextLookatVel;

	float dw  = (float)(soHead->GetAngularVelocity().Y());
	float dwv = (float)(soHead->GetAngularVelocity().X());

	float dt  = (float)(DCAST(PHSceneIf, GetScene())->GetTimeStep());

	//// Horizontal
	float node_L_1 = -(sigma*eL + nu*vL) - (kappa*eR + eta*vR) + (dw*alpha1);
	float node_R_1 =  (sigma*eR + nu*vR) + (kappa*eL + eta*vL) - (dw*alpha1);
	
	integrator_L += node_L_1 * dt;
	integrator_R += node_R_1 * dt;
	
	float out_t1 = -(integrator_L * rho1) + (integrator_R * rho2) + t1;
	float out_t2 = -(integrator_L * rho2) + (integrator_R * rho1) + t2;

	//// Vertical
	float node_L_2 = (sigma*eLV + nu*vLV);// + (dwv*alpha1);
	float node_R_2 = (sigma*eRV + nu*vRV);// + (dwv*alpha1);

	integrator_Lv += node_L_2 * dt;
	integrator_Rv += node_R_2 * dt;

	float out_t3 = (integrator_Lv * rho1) + t3;
	float out_t4 = (integrator_Rv * rho1) + t4;

	Vec3f dirL, dirR;

	if (cos(out_t3)!=0){
		dirL = Vec3f(-cos(out_t1), -sin(out_t1)*tan(out_t3), sin(out_t1));
	}else{
		dirL = Vec3f(-cos(out_t1), 0.0f, 0.0f);
	}

	if (cos(out_t4)!=0){
		dirR = Vec3f(-cos(out_t2), -sin(out_t2)*tan(out_t4), sin(out_t2));
	}else{
		dirR = Vec3f(-cos(out_t2), 0.0f, 0.0f);
	}

	dirL = -(soHead->GetPose().Ori() * dirL);
	dirR = -(soHead->GetPose().Ori() * dirR);
	
	ControlEyeToTargetDir(soLEye, dirL);
	ControlEyeToTargetDir(soREye, dirR);
}

void CREyeController::ControlEyeToTargetDir(PHSolidIf* soEye, Vec3f target){
	/**/
	Vec3f currentDir = (soEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, 1.0f)).unit();
	Vec3f errorYawPitch = PTM::cross(currentDir, target);
	Vec3f derror = soEye->GetAngularVelocity();
	Vec3f torque = (Kp * (errorYawPitch)) - (Kd * derror);
 	soEye->AddTorque(torque);
	/**/

	/*/
	double angH = Vec3ToAngH(target);
	double angV = Vec3ToAngV(target);
	std::cout << "(angH, angV) = (" << Deg(angH) << ", " << Deg(angV) << ")" << std::endl;
	Quaterniond ori = Quaterniond::Rot(-angV,'x')*Quaterniond::Rot(angH,'y');
	Posed pose = soEye->GetPose();
	pose.Ori() = ori;
	soEye->SetPose(pose);
	/**/

	/*/
	double angH = Vec3ToAngH(target);
	double angV = Vec3ToAngV(target);
	std::cout << "(angH, angV) = (" << Deg(angH) << ", " << Deg(angV) << ")" << std::endl;

	Vec3f curr = (soEye->GetPose().Ori() * Vec3f(0.0f, 0.0f, 1.0f)).unit();
	double angHc = Vec3ToAngH(curr);
	double angVc = Vec3ToAngV(curr);

	Vec3f tqD = soEye->GetAngularVelocity()*Kd;
	Vec3f tqH = Vec3d(0,1,0)*angHc*Kp, tqV = Vec3d(-1,0,0)*angVc*Kp;
	soEye->AddTorque(tqH+tqV-tqD);
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
