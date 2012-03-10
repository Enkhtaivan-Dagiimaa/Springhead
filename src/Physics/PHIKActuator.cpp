/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHIKActuator.h>
#include <Physics/PHHingeJoint.h>
#include <Physics/PHBallJoint.h>

using namespace std;
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// IKActuator

bool PHIKActuator::AddChildObject(ObjectIf* o){
	PHIKEndEffector* endeffector = o->Cast();
	if (endeffector) {
		this->eef = endeffector;
		return true;
	}

	PHIKActuator* actuator = o->Cast();
	if (actuator) {
		// �����̎q�ɂ���
		children.push_back(actuator);
		this->bActuatorAdded = true;
		for (int j=0; j<actuator->descendant.size(); ++j) {
			// �V���Ȏq�̎q�����Љ�Ă��炤
			descendant.push_back(actuator->descendant[j]);
			// �V���Ȏq�̎q���Ɏ������c��ł��鎖��������
			actuator->descendant[j]->ascendant.push_back(this);
			actuator->descendant[j]->bActuatorAdded = true;
			for (int i=0; i<ascendant.size(); ++i) {
				// �����̑c��ɐV���Ȏq�̎���`����
				ascendant[i]->descendant.push_back(actuator->descendant[j]);
				ascendant[i]->bActuatorAdded = true;
				// �V���Ȏq�Ɏ����̑c��̎���`����
				actuator->descendant[j]->ascendant.push_back(ascendant[i]);
			}
		}
		return true;
	}

	return false;
}

ObjectIf* PHIKActuator::GetChildObject(size_t pos){
	if (pos==0) {
		if (eef!=NULL) {
			return eef->Cast();
		} else {
			if (children.size() > 0) {
				return children[pos]->Cast();
			} else {
				return NULL;
			}
		}
	} else {
		if (eef!=NULL) { pos--; }
		if (children.size() > pos) {
			return children[pos]->Cast();
		} else {
			return NULL;
		}
	}
}

size_t PHIKActuator::NChildObject() const{
	return( children.size() + (eef==NULL ? 0 : 1) );
}

// --- --- --- --- ---
void PHIKActuator::SetupMatrix(){
	if (this->bEnabled) {
		// --- --- --- --- ---
		// �ϐ��̏�����

		// ���A��
		if (this->bNDOFChanged) {
			alpha.resize(this->ndof);
			beta.resize(this->ndof);
		}
		alpha.clear();
		beta.clear();

		// ��
		for (int i=0; i<nLinks(); ++i) {
			if (link(i)==this) { continue; } // <!!> this,this�Ɠ�������Ȃ炱��͂���Ȃ�
			if (this->bNDOFChanged || (link(i)->bNDOFChanged && link(i)->bEnabled) || this->bActuatorAdded) {
				gamma[link(i)->number].resize(this->ndof, link(i)->ndof);
			}
			gamma[link(i)->number].clear();
		}

		// ���̓���P�[�X�i����ƂȂ�A�N�`���G�[�^���������g�ł��郡�j
		if (this->bNDOFChanged) {
			gamma[this->number].resize(this->ndof,this->ndof);
		}
		gamma[this->number].clear();

		// �i�i���R�r�A���j
		for (int i=0; i<descendant.size(); ++i) {
			PHIKEndEffector* child_eef = descendant[i]->eef; if (child_eef==NULL) { continue; }
			if (this->bNDOFChanged || (child_eef->bNDOFChanged && child_eef->bEnabled) || this->bActuatorAdded) {
				Mj[child_eef->number].resize(child_eef->ndof, this->ndof);
			}
			Mj[child_eef->number].clear();
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
	for (int i=0; i<descendant.size(); ++i) {
		PHIKEndEffector* child_eef = descendant[i]->eef;
		if (child_eef==NULL || !(child_eef->bEnabled)) { continue; }
		CalcJacobian(child_eef);
	}
}

void PHIKActuator::PrepareSolve(){
	if (!bEnabled) { return; }

	PHIKEngineIf* engine = DCAST(PHSceneIf,GetScene())->GetIKEngine();

	for (int i=0; i< ndof; ++i) {
		for (int j=0; j<descendant.size(); ++j) {
			PHIKEndEffector* child_eef = descendant[j]->eef;
			if (child_eef==NULL || !(child_eef->bEnabled)) { continue; }

			int eef_n = child_eef->number;

			PTM::VVector<double> eef_v;
			child_eef->GetTempTarget(eef_v);

			for (int k=0; k < child_eef->ndof; ++k) {

				// ���A��
				alpha[i] += ( (Mj[eef_n][k][i]/bias) * (Mj[eef_n][k][i]) );
				beta[i]  += ( (Mj[eef_n][k][i]/bias) * (eef_v[k])  );

				// ��[act_y, this]  <!!> link(m)��this���܂ނ悤�ɂȂ����̂� this<->this �������ł��邩��
				for (int m=0; m < nLinks(); ++m) {
					if (link(m)==this || !(link(m)->bEnabled)) { continue; }
					int act_n = link(m)->number;
					for (int j=0; j<link(m)->ndof; ++j) {
						if (link(m)->Mj.find(eef_n) != link(m)->Mj.end()) {
							gamma[act_n][i][j] += ( (Mj[eef_n][k][i]/bias) * (link(m)->Mj[eef_n][k][j] / link(m)->bias) );
						}
					}
				}

				// ��[this, this]
				for (int j=0; j<ndof; ++j) {
					if (i!=j) {
						gamma[number][i][j] += ( (Mj[eef_n][k][i]/bias) * (Mj[eef_n][k][j]/bias) );
					}
				}

			}
		}
	}

	omega.clear();
	omega_prev.clear();
}

void PHIKActuator::ProceedSolve(){
	omega_prev  = omega;

	for (int i=0; i<ndof; ++i) {
		double delta_epsilon = 0;

		// ��
		for (int j=0; j < nLinks(); ++j) {
			if (link(j)==this || !(link(j)->IsEnabled())) { continue; }
			int act_n = link(j)->number;
			if (gamma.find(act_n) != gamma.end()) {
				for (int k=0; k<link(j)->ndof; ++k) {
					delta_epsilon   += ( (gamma[act_n][i][k]) * (link(j)->omega[k]) );
				}
			}
		}

		// ��
		for (int k=0; k<ndof; ++k) {
			if (k!=i) {
				if (gamma.find(number) != gamma.end()) {
					delta_epsilon   += ( (gamma[number][i][k]) * (omega[k])  );
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
		omega[i]  = invAlpha * (beta[i]  - delta_epsilon);
	}

	// �㏈��
	AfterProceedSolve();
}

// --- --- --- --- ---
bool PHIKBallActuator::AddChildObject(ObjectIf* o){
	PHBallJointIf* jo = o->Cast();
	if (jo) {
		this->joint = jo;
		return true;
	}
	return PHIKActuator::AddChildObject(o);
}

ObjectIf* PHIKBallActuator::GetChildObject(size_t pos){
	if (pos == 0 && this->joint != NULL) { return this->joint; }
	if (this->joint != NULL) {
		return PHIKActuator::GetChildObject(pos - 1);
	} else {
		return PHIKActuator::GetChildObject(pos);
	}
	return NULL;
}

size_t PHIKBallActuator::NChildObject() const{
	if (this->joint != NULL) { return 1 + PHIKActuator::NChildObject(); }
	return PHIKActuator::NChildObject();
}

void PHIKBallActuator::BeforeSetupMatrix(){
	// �p�����������G���h�G�t�F�N�^��������Ύ��R�x���Q�ɉ�����i�璷������̂��߁j

	bool bFound = false;;
	for (int i=0; i<descendant.size(); ++i) {
		if (descendant[i]->eef && descendant[i]->eef->bEnabled && descendant[i]->eef->bOrientation) {
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

	for (int i=0; i<descendant.size(); ++i) {
		PHIKEndEffector* eef = descendant[i]->eef; if (eef==NULL) { continue; }
		if (eef->bEnabled && !eef->bOrientation) {
			// �֐߂̉�]���S
			PHBallJoint* j = DCAST(PHBallJoint,joint);
			PHBallJointDesc d; j->GetDesc(&d);
			Vec3d Pj = j->solid[0]->GetPose() * d.poseSocket * Vec3d(0,0,0);

			// �G���h�G�t�F�N�^�ʒu
			Vec3d Pe = eef->solid->GetPose() * eef->targetLocalPosition;

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
}

void PHIKBallActuator::Move(){
	if (!bEnabled) { return; }

	// ��]���x�N�g���ɂ���
	Vec3d  w = Vec3d();
	for (int i=0; i<ndof; ++i) { w += ( omega[i]/bias) * e[i]; }

	// Axis-Angle�\���ɂ���
	double  angle =  w.norm();
	Vec3d axis = w;
	if (angle != 0) {
		axis = axis / angle;
	} else {
		axis = Vec3d(1,0,0);
	}

	// Quaternion�ɂ���
	Quaterniond diffQ = Quaterniond::Rot( angle,  axis);

	// �֐߃��[�J���n�ɒ���
	Posed poseSocket; joint->GetSocketPose(poseSocket);
	Posed poseSolid = joint->GetSocketSolid()->GetPose();
	
	Vec3d       Pj    = (poseSolid * poseSocket) * Vec3d(0,0,0);
	Quaterniond Qj    = (poseSolid * poseSocket).Ori();
	Vec3d       diff  = (Qj.Inv() * diffQ * Qj).RotationHalf();

	joint->SetTargetVelocity(velocityGain * diff);

	return;
}

// --- --- --- --- ---
bool PHIKHingeActuator::AddChildObject(ObjectIf* o){
	PHHingeJointIf* jo = o->Cast();
	if (jo) {
		this->joint = jo;
		return true;
	}
	return PHIKActuator::AddChildObject(o);
}

ObjectIf* PHIKHingeActuator::GetChildObject(size_t pos){
	if (pos == 0 && this->joint != NULL) { return this->joint; }
	if (this->joint != NULL) {
		return PHIKActuator::GetChildObject(pos - 1);
	} else {
		return PHIKActuator::GetChildObject(pos);
	}
	return NULL;
}

size_t PHIKHingeActuator::NChildObject() const{
	if (this->joint != NULL) { return 1 + PHIKActuator::NChildObject(); }
	return PHIKActuator::NChildObject();
}

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
}

void PHIKHingeActuator::Move(){
	if (!bEnabled) { return; }

	double  diff = omega[0]/bias;
	joint->SetTargetVelocity(velocityGain * diff);

	return;
}

}
