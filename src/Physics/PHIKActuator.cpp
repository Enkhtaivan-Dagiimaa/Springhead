/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#pragma hdrstop
#include "PHIKActuator.h"
#include "Physics/PHJoint.h"

using namespace std;
namespace Spr{;

//static std::ofstream *dlog;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// IKActuatorDesc

PHIKActuatorDesc::PHIKActuatorDesc() {
	bEnabled = true;

	bias = 1.0;

	spring = 0.0;
	damper = 0.0;
}

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// IKActuator

// --- --- --- --- ---
void PHIKActuator::SetupMatrix(){
	if (this->bEnabled) {
		// --- --- --- --- ---
		// �ϐ��̏�����

		// ���A��
		if (this->bNDOFChanged) {
			alpha.resize(this->ndof);
			beta.resize(this->ndof);
			// std::cout << "��[" << number << "] : " << this->ndof << std::endl;
		}
		alpha.clear();
		beta.clear();

		// ��
		for(ASetIter act=linkedActuators.begin(); act!=linkedActuators.end(); ++act) {
			if (this->bNDOFChanged || ((*act)->bNDOFChanged && (*act)->bEnabled) || this->bActuatorAdded) {
				gamma[(*act)->number].resize(this->ndof, (*act)->ndof);
				// std::cout << "��[" << number << ", " << (*act)->number << "] : (" << this->ndof << "," << (*act)->ndof << ")" << std::endl;
			}
			gamma[(*act)->number].clear();
		}

		// ���̓���P�[�X�i����ƂȂ�A�N�`���G�[�^���������g�ł��郡�j
		if (this->bNDOFChanged) {
			gamma[this->number].resize(this->ndof,this->ndof);
		}
		gamma[this->number].clear();

		// �i�i���R�r�A���j
		for (ESetIter eef=linkedEndEffectors.begin(); eef!=linkedEndEffectors.end(); ++eef) {
			if (this->bNDOFChanged || ((*eef)->bNDOFChanged && (*eef)->bEnabled) || this->bEndEffectorAdded) {
				Mj[(*eef)->number].resize((*eef)->ndof, this->ndof);
				// std::cout << "�i[" << (*eef)->number << ", " << this->number << "] : (" << (*eef)->ndof << "," << this->ndof << ")" << std::endl;
			}
			Mj[(*eef)->number].clear();
		}

		// �ցA��
		if (this->bNDOFChanged) {
			omega.resize(this->ndof);
			omega_prev.resize(ndof);
			tau.resize(ndof);
		}
		omega.clear();
		omega_prev.clear();
		tau.clear();
	}
}

void PHIKActuator::CalcAllJacobian(){
	for(ESetIter eef=linkedEndEffectors.begin(); eef!=linkedEndEffectors.end(); ++eef){
		if (! DCAST(PHIKEndEffector,*eef)->bEnabled) { continue; }

		int n = DCAST(PHIKEndEffector,*eef)->number;
		CalcJacobian(*eef);

		//(*dlog) << "--- J[nd:" << number << "][cp:" << n << "] ---" << std::endl;
		//(*dlog) << Mj[n] << std::endl;
	}
}

void PHIKActuator::PrepareSolve(){
	if (!bEnabled) { return; }

	for (int i=0; i< ndof; ++i) {
		for(ESetIter eef=linkedEndEffectors.begin(); eef!=linkedEndEffectors.end(); ++eef){
			if (! (*eef)->bEnabled) { continue; }
			int eef_n = (*eef)->number;
			PTM::VVector<double> eef_v = (*eef)->GetTempTarget();

			//(*dlog) << "--- v[cp:" << eef_n << "] ---" << std::endl;
			//(*dlog) << eef_v << std::endl;

			for (int k=0; k < (*eef)->ndof; ++k) {

				// ���A��
				alpha[i] += ( (Mj[eef_n][k][i]/bias) * (Mj[eef_n][k][i]) );
				beta[i]  += ( (Mj[eef_n][k][i]/bias) * (eef_v[k]) );

				// ��[act_y, this]
				for(ASetIter act=linkedActuators.begin(); act!=linkedActuators.end(); ++act){
					if (!((*act)->bEnabled)) { continue; }
					int act_n = (*act)->number;
					for (int j=0; j<(*act)->ndof; ++j) {
						if ((*act)->Mj.find(eef_n) != (*act)->Mj.end()) {
							gamma[act_n][i][j] += ( (Mj[eef_n][k][i]/bias) * ((*act)->Mj[eef_n][k][j] / (*act)->bias) );
						}
					}
				}

				// ��[this, this]
				for (int j=0; j<ndof; ++j) {
					if (i!=j) {
						//(*dlog) << number << ":" << i << ":" << j << ": += " << (Mj[eef_n][k][i]/bias) << ", " << (Mj[eef_n][k][j]/bias) << std::endl;
						gamma[number][i][j] += ( (Mj[eef_n][k][i]/bias) * (Mj[eef_n][k][j]/bias) );
					}
				}

			}
		}
	}


	//(*dlog) << "--- alpha[" << number << "] ---" << std::endl;
	//(*dlog) << alpha << std::endl;

	//(*dlog) << "--- beta[" << number << "] ---" << std::endl;
	//(*dlog) << beta << std::endl;

	for(ASetIter act=linkedActuators.begin(); act!=linkedActuators.end(); ++act){
		if (!((*act)->IsEnabled())) { continue; }
		int act_n = (*act)->number;
		//(*dlog) << "--- gamma[" << number << "][" << n_y_n << "] ---" << std::endl;
		//(*dlog) << gamma[n_y_n] << std::endl;
	}
	//(*dlog) << "--- gamma[" << number << "][" << number << "] ---" << std::endl;
	//(*dlog) << gamma[number] << std::endl;

	omega.clear();
	omega_prev.clear();
}

void PHIKActuator::ProceedSolve(){
	omega_prev = omega;

	for (int i=0; i<ndof; ++i) {
		double delta_epsilon = 0;

		// ��
		for(ASetIter act=linkedActuators.begin(); act!=linkedActuators.end(); ++act){
			if (!((*act)->IsEnabled())) { continue; }
			int act_n = (*act)->number;
			if (gamma.find(act_n) != gamma.end()) {
				for (int k=0; k<(*act)->ndof; ++k) {
					delta_epsilon += ( (gamma[act_n][i][k]) * ((*act)->omega[k]) );
				}
			}
		}

		// ��
		for (int k=0; k<ndof; ++k) {
			if (k!=i) {
				if (gamma.find(number) != gamma.end()) {
					delta_epsilon += ( (gamma[number][i][k]) * (omega[k]) );
				}
			}
		}

		// �ւ̍X�V
		double invAlpha = 0;
		if (alpha[i]!=0) {
			invAlpha = (1.0 / alpha[i]);
		} else {
			invAlpha = 1e+20;
		}
		omega[i] = invAlpha * (beta[i] - delta_epsilon);
	}

	// �㏈��
	AfterProceedSolve();
}

void PHIKActuator::RegisterEndEffector(PHIKEndEffectorIf* endeffector){
	ASet* la = &(DCAST(PHIKEndEffector,endeffector)->linkedActuators);
	for(ASetIter act=la->begin(); act!=la->end(); ++act){
		linkedActuators.insert(*act); bActuatorAdded = true;
		PHIKActuator* self = this;
		if ((*act)->linkedActuators.find(self) == (*act)->linkedActuators.end()) {
			(*act)->linkedActuators.insert(self);
			(*act)->bActuatorAdded = true;
		}
	}

	DCAST(PHIKEndEffector,endeffector)->linkedActuators.insert(this);
	linkedEndEffectors.insert(endeffector->Cast()); bEndEffectorAdded = true;
}

// --- --- --- --- ---
void PHIKBallActuator::BeforeSetupMatrix(){
	// �p�����������G���h�G�t�F�N�^��������Ύ��R�x���Q�ɉ�����i�璷������̂��߁j

	bool bFound = false;;
	for(ESetIter eef=linkedEndEffectors.begin(); eef!=linkedEndEffectors.end(); ++eef){
		if ((*eef)->bEnabled && (*eef)->bOrientation) {
			bFound = true;
		}
	}

	if (bFound) {
		if (ndof != 3) {
			ndof = 3;
			bNDOFChanged = true;
		}
	} else {
		if (ndof != 2) {
			ndof = 2;
			bNDOFChanged = true;
		}
	}
}

void PHIKBallActuator::BeforeCalcAllJacobian(){
	CalcAxis();
}

void PHIKBallActuator::CalcAxis(){
	e[0] = Vec3d(1,0,0);
	e[1] = Vec3d(0,1,0);
	e[2] = Vec3d(0,0,1);

	for(ESetIter eef=linkedEndEffectors.begin(); eef!=linkedEndEffectors.end(); ++eef){
		if ((*eef)->bEnabled && !(*eef)->bOrientation) {
			// �֐߂̉�]���S
			PHBallJoint* j = DCAST(PHBallJoint,joint);
			PHBallJointDesc d; j->GetDesc(&d);
			Vec3d Pj = j->solid[0]->GetPose() * d.poseSocket * Vec3d(0,0,0);

			// �G���h�G�t�F�N�^�ʒu
			Vec3d Pe = (*eef)->solid->GetPose() * (*eef)->targetLocalPosition;

			// �֐߉�]���S<->�G���h�G�t�F�N�^ ��
			Vec3d e0 = (Pe - Pj);
			if (e0.norm() != 0){
				e0 = e0 / e0.norm();
			} else {
				e0 = Vec3d(1,0,0);
			}

			// ��]�������߂�
			if (abs(PTM::dot(e0,Vec3d(1,0,0))) > Rad(10)) {
				e[0] = Vec3d(0,1,0);
			} else {
				e[0] = Vec3d(1,0,0);
			}
			e[0] = e[0] - (PTM::dot(e[0],e0) * e0);
			e[0] = e[0] / e[0].norm();
			e[1] = PTM::cross(e0,e[0]);
			e[2] = e0;

			return;
		}
	}
}

void PHIKBallActuator::CalcJacobian(PHIKEndEffector* endeffector){
	int n = endeffector->number;

	// �A�N�`���G�[�^��] <=> �G���h�G�t�F�N�^�ʒu
	if (endeffector->bPosition){
		// �֐߂̉�]���S
		PHBallJoint* j = DCAST(PHBallJoint,joint);
		PHBallJointDesc d; j->GetDesc(&d);
		Vec3d Pj = j->solid[0]->GetPose() * d.poseSocket * Vec3d(0,0,0);

		// �G���h�G�t�F�N�^�ʒu
		Vec3d Pe = endeffector->solid->GetPose() * endeffector->targetLocalPosition;

		// �O�σx�N�g�����烄�R�r�A�������߂�
		for (int i=0; i<ndof; ++i) {
			Vec3d v = PTM::cross(e[i],(Pe-Pj));
			Mj[n][0][i] = v[0];  Mj[n][1][i] = v[1];  Mj[n][2][i] = v[2];
		}
	}

	// �A�N�`���G�[�^��] <=> �G���h�G�t�F�N�^��]
	if (endeffector->bOrientation){
		int stride = (endeffector->bPosition ? 3 : 0);

		// �P�ʍs��
		for (int i=0; i<3; ++i) {
			for (int j=0; j<ndof; ++j) {
				Mj[n][i+stride][j] = ((i==j) ? 1 : 0);
			}
		}
	}

	// std::cout << "Jb_" << number << " : " << std::endl << Mj[n] << std::endl;
}

void PHIKBallActuator::Move(){
	if (!bEnabled) { return; }

	// std::cout << "��_" << number << " : " << omega << std::endl;
	// std::cout << "   with e : " << e[0] << e[1] << e[2] << std::endl;

	if (true/*omega.norm() < Rad(360)*/) {

		// ��]���x�N�g���ɂ���
		Vec3d w = Vec3d();
		for (int i=0; i<ndof; ++i) {
			w += (omega[i]/bias) * e[i];
		}

		// Axis-Angle�\���ɂ���
		double angle = w.norm();
		Vec3d axis = w;
		if (angle != 0) {
			axis = axis / angle;
		} else {
			axis = Vec3d(1,0,0);
		}

		// Quaternion�ɂ���
		Quaterniond dQ = Quaterniond::Rot(angle, axis);

		// �֐߂̃��[�J�����W�����Ƃ߂�
		PHBallJoint* j = DCAST(PHBallJoint,joint);
		PHBallJointDesc d; j->GetDesc(&d);
		Vec3d Pj = j->solid[0]->GetPose() * d.poseSocket * Vec3d(0,0,0);
		Quaterniond Qj = (j->solid[0]->GetPose() * d.poseSocket).Ori();

		// �֐߂̃��[�J�����W�n�ɂ���
		Quaterniond pos = Qj.Inv() * dQ * Qj * joint->GetPosition();

		Vec3d targetPosition = pos.RotationHalf();
		Vec3d orig = jGoal.RotationHalf();

		Vec3d newGoal;
		if (jSpring + spring != 0) {
			newGoal = (jSpring*orig + spring*targetPosition) * (1/(jSpring + spring));
		} else {
			newGoal = Vec3d();
		}

		pos = Quaterniond::Rot(newGoal.norm(), (newGoal.norm()!=0)?(newGoal.unit()):(Vec3d(1,0,0)));

		// �֐߂𓮂���
		joint->SetSpring(jSpring + spring);
		joint->SetDamper(jDamper + damper);
		joint->SetTargetPosition(pos);

		/*
		Vec3d vel = dT / DCAST(PHSceneIf,GetScene())->GetTimeStep();
		if (vel.norm() > Rad(360)) {
			vel = vel.unit() * Rad(360);
		}
		joint->SetMode(PHBallJointDesc::MODE_VELOCITY);
		joint->SetTargetVelocity(vel);
		*/

	} else {
		std::cout << "Divergence! : IKBall" << std::endl;
	}

	return;
}

void PHIKBallActuator::MoveStatic() {
	Move();
	PHSolidIf* soParent = joint->GetSocketSolid();
	PHSolidIf* soChild  = joint->GetPlugSolid();

	PHBallJointDesc d;
	joint->GetDesc(&d);
	Posed poseSocket = d.poseSocket;
	Posed posePlug   = d.posePlug;

	Posed q=Posed(), p;
	q.Ori() = joint->GetTargetPosition();

	p = soParent->GetPose() * poseSocket * q * posePlug.Inv();
	soChild->SetPose(p);

	DCAST(PHConstraint,joint)->UpdateState();
}

void PHIKBallActuator::MoveToNaturalPosition(){
	joint->SetSpring(jSpring);
	joint->SetDamper(jDamper);
	joint->SetTargetPosition(jGoal);
}

// --- --- --- --- ---
void PHIKHingeActuator::CalcJacobian(PHIKEndEffector* endeffector){
	int n = endeffector->number;
	PHHingeJoint* j = DCAST(PHHingeJoint,joint);

	// �A�N�`���G�[�^��] <=> �G���h�G�t�F�N�^�ʒu
	if (endeffector->bPosition){
		Vec3d Pm = j->solid[0]->GetPose() * j->Xj[0].r;
		Vec3d Rm = j->solid[0]->GetPose().Ori() * j->Xj[0].q * Vec3d(0,0,1);

		Vec3d Pe = endeffector->solid->GetPose() * endeffector->targetLocalPosition;

		Vec3d M3 = PTM::cross((Pm-Pe), Rm);
		for (int i=0; i<3; ++i) {
			Mj[n][i][0] = M3[i];
		}
	}

	// �A�N�`���G�[�^��] <=> �G���h�G�t�F�N�^��]
	if (endeffector->bOrientation){
		int stride = (endeffector->bPosition ? 3 : 0);

		// �q���W�̉�]���x�N�g��
		Vec3d Rm = j->solid[0]->GetPose().Ori() * j->Xj[0].q * Vec3d(0,0,1);
		for (int i=0; i<3; ++i) {
			Mj[n][i+stride][0] = Rm[i];
		}
	}

	// std::cout << "Jh_" << number << " : " << std::endl << Mj[n] << std::endl;
}

void PHIKHingeActuator::Move(){
	if (!bEnabled) { return; }

	static const double Pi = 3.141592653589;

	// �V������]�p�x
	double angle  = joint->GetPosition() + (omega[0]/bias);

	double newGoal;
	if (jSpring + spring != 0) {
		newGoal = (jSpring*jGoal + spring*angle) * (1/(jSpring + spring));
	} else {
		newGoal = 0;
	}

	// �g���N���������邽�߂̃I�t�Z�b�g�̒ǉ�
	/*
	double torque = tau[0];
	newGoal += torque * Rad(16) / joint->GetSpring();
	*/

	// �֐߂𓮂���
	if (abs(newGoal) < Rad(360)) {
		joint->SetSpring(jSpring + spring);
		joint->SetDamper(jDamper + damper);
		joint->SetTargetPosition(newGoal);
	} else {
		std::cout << "Divergence! : IKHinge" << std::endl;
	}
}

void PHIKHingeActuator::MoveStatic() {
	Move();
	PHSolidIf* soParent = joint->GetSocketSolid();
	PHSolidIf* soChild  = joint->GetPlugSolid();

	PHHingeJointDesc d;
	joint->GetDesc(&d);
	Posed poseSocket = d.poseSocket;
	Posed posePlug   = d.posePlug;

	Posed q=Posed(), p;
	q.Ori() = Quaterniond::Rot(joint->GetTargetPosition(), 'z');

	p = soParent->GetPose() * poseSocket * q * posePlug.Inv();
	soChild->SetPose(p);

	DCAST(PHConstraint,joint)->UpdateState();
}

void PHIKHingeActuator::MoveToNaturalPosition(){
	joint->SetSpring(jSpring);
	joint->SetDamper(jDamper);
	joint->SetTargetPosition(jGoal);
}

void PHIKHingeActuator::AfterProceedSolve(){
	// ���搧���i�e�X�g�j
	/*
	if (joint->GetPosition() > Rad(30) && omega[0] > 0) {
		omega[0] = 0;
	} else if (joint->GetPosition() < Rad(-30) && omega[0] < 0) {
		omega[0] = 0;
	}
	*/
}

}
