/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#pragma hdrstop

using namespace PTM;
using namespace std;

namespace Spr{;

//----------------------------------------------------------------------------
// PHHingeJoint
PHHingeJoint::PHHingeJoint(const PHHingeJointDesc& desc){
	SetDesc(&desc);
	movableAxes[0] = 5;
}

void PHHingeJoint::UpdateJointState(){
	//�������̍S���͍��v���Ă�����̂Ɖ��肵�Ċp�x������
	position[0] = Xjrel.q.Theta();
	if(Xjrel.q.Axis().Z() < 0.0)
		position = -position;
	velocity[0] = vjrel.w().z;
}

double PHHingeJoint::GetDeviation(){
	// �s�A���ȃg���N�ω�������邽�� (�[���}�C�̂悤�ɂ�����ł�������悤�ɍ폜)�B 07/07/26
	//// ���ނ��낱�̃R�[�h�����邱�Ƃŕs�A���ȃg���N�ω����������Ă���̂ł́H�Ǝv�������D 08/10/07 mitake
	//// ��������Ȃ��Ɣ��U����̂Ŗ߂��D�����V�~�����[�^�͈�ӂɒl���w��o���Ȃ��ƌv�Z��������Ȃ��́H 09/06/01 toki
	// while(diff >  M_PI) diff -= 2 * M_PI;
	// while(diff < -M_PI) diff += 2 * M_PI;
	// ���������̂ق����͂₢�̂ŕύX  09/07/06 mitake
	double diff = PHJoint1D::GetDeviation();
	diff = ( (diff / (2*M_PI)) - floor(diff / (2*M_PI)) ) * (2*M_PI);
	if (diff > M_PI) { diff -= 2 * M_PI; }
	return diff;
}

void PHHingeJoint::CompBias(){
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	
	// �S���덷�␳�̂��߂̃o�C�A�X
	if (engine->numIterCorrection==0){	//	Correction �𑬓xLCP�ōs���ꍇ
		//	���̃X�e�b�v�ł̈ʒu�̌덷�̗\���l��0�ɂȂ�悤�ȑ��x��ݒ�
		//	dv * dt = x + v*dt
		db.v() = Xjrel.r * dtinv + vjrel.v();
		//	�p�x�̌덷��0�ɂ���悤�ȉ�]�p�x�����߂�B
		Quaterniond qarc;
		qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // ������v������悤�ȉ�]
		db.w() = -(qarc.Theta() * dtinv) * qarc.Axis() + vjrel.w();
		db *= engine->velCorrectionRate;
	}

}

void PHHingeJoint::CompError(){
	B.v() = Xjrel.r;

	//	B.w() = Xjrel.q.V();
	//	B.w().z = 0.0;
	Quaterniond qarc;
	qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // ������v������悤�ȉ�]
	B.w() = -qarc.Theta() * qarc.Axis();
}



//-----------------------------------------------------------------------------
void PHHingeJointNode::CompJointJacobian(){
	J.col(0).sub_vector(PTM::TSubVectorDim<0, 3>()).clear();
	J.col(0).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNode1D::CompJointJacobian();
}

void PHHingeJointNode::CompJointCoriolisAccel(){
	cj.clear();
}

void PHHingeJointNode::CompRelativeVelocity(){
	PHJoint1D* j = GetJoint();
	j->vjrel.v().clear();
	j->vjrel.w() = Vec3d(0.0, 0.0, j->velocity[0]);
}

void PHHingeJointNode::CompRelativePosition(){
	PHJoint1D* j = GetJoint();
	j->Xjrel.q = Quaterniond::Rot(j->position[0], 'z');
	j->Xjrel.r.clear();
}

}
