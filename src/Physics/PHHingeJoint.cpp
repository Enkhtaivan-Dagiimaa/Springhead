/*
 *  Copyright (c) 2003-2010, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Physics/PHHingeJoint.h>
#include <Physics/PHConstraintEngine.h>

using namespace PTM;
using namespace std;

namespace Spr{;

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHHingeJointNode

void PHHingeJointNode::CompJointJacobian(){
	J.col(0).sub_vector(PTM::TSubVectorDim<0, 3>()).clear();
	J.col(0).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNode1D::CompJointJacobian();
}

void PHHingeJointNode::CompJointCoriolisAccel(){
	cj.clear();
}

void PHHingeJointNode::CompRelativeVelocity(){
	PH1DJoint* j = GetJoint();
	j->vjrel.v().clear();
	j->vjrel.w() = Vec3d(0.0, 0.0, j->velocity[0]);
}

void PHHingeJointNode::CompRelativePosition(){
	PH1DJoint* j = GetJoint();
	j->Xjrel.q = Quaterniond::Rot(j->position[0], 'z');
	j->Xjrel.r.clear();
}

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHHingeJoint

PHHingeJoint::PHHingeJoint(const PHHingeJointDesc& desc) {
	SetDesc(&desc);
	
	// �����E�S�����̐ݒ�
	nMovableAxes   = 1;
	movableAxes[0] = 5;
	InitTargetAxes();
}

// ----- �G���W������Ăяo�����֐�

void PHHingeJoint::UpdateJointState(){
	//�������̍S���͍��v���Ă�����̂Ɖ��肵�Ċp�x������
	position[0] = Xjrel.q.Theta();
	if (Xjrel.q.Axis().Z() < 0.0) { position = -position; }
	velocity[0] = vjrel.w().z;
}

// ----- PHConstraint�̔h���N���X�Ŏ��������@�\

void PHHingeJoint::CompBias(){
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	
	// �S���덷�␳�̂��߂̃o�C�A�X
	if (engine->numIterCorrection==0){ // Correction�𑬓xLCP�ōs���ꍇ
		//	���̃X�e�b�v�ł̈ʒu�̌덷�̗\���l��0�ɂȂ�悤�ȑ��x��ݒ�
		//	dv * dt = x + v*dt
		db.v() = Xjrel.r * dtinv + vjrel.v();

		//	�p�x�̌덷��0�ɂ���悤�ȉ�]�p�x�����߂�B
		Quaterniond qarc;
		qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // ������v������悤�ȉ�]
		db.w() = -(qarc.Theta() * dtinv) * qarc.Axis() + vjrel.w();
		db *= engine->velCorrectionRate;
	}

	// �e�N���X��CompBias�Dmotor,limit��CompBias���Ă΂��̂ōŌ�ɌĂ�
	PH1DJoint::CompBias();
}

void PHHingeJoint::CompError(){
	B.v() = Xjrel.r;

	Quaterniond qarc;
	qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // ������v������悤�ȉ�]
	B.w() = -qarc.Theta() * qarc.Axis();
}

// ----- �C���^�t�F�[�X�̎���

double PHHingeJoint::GetDeviation(){
	double diff = PH1DJoint::GetDeviation();
	diff = ( (diff / (2*M_PI)) - floor(diff / (2*M_PI)) ) * (2*M_PI);
	if (diff > M_PI) { diff -= 2 * M_PI; }
	return diff;
}

}
