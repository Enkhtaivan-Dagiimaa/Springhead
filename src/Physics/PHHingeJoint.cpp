/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;

namespace Spr{;

//----------------------------------------------------------------------------
// PHHingeJoint
PHHingeJoint::PHHingeJoint(const PHHingeJointDesc& desc){
	SetDesc(&desc);
	axisIndex[0] = 5;
}

void PHHingeJoint::UpdateJointState(){
	//�������̍S���͍��v���Ă�����̂Ɖ��肵�Ċp�x������
	position[0] = Xjrel.q.Theta();
	if(Xjrel.q.Axis().Z() < 0.0)
		position = -position;
	velocity[0] = vjrel.w().z;
}

void PHHingeJoint::CompBias(){
//	DSTR << "spring " << spring << " goal " << origin*180/M_PI << endl;
	double dtinv = 1.0 / GetScene()->GetTimeStep();
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

	if(mode == MODE_VELOCITY || mode == MODE_TRAJECTORY_TRACKING){

		db.w().z = -desiredVelocity;
	}
	else if(onLower || onUpper){
		dA.w()[2] = 0;
		db.w()[2] = (position[0] - origin) * dtinv * engine->velCorrectionRate;
	}
	else if(spring != 0.0 || damper != 0.0){
		double diff;
		diff = GetPosition() - origin;
		// �s�A���ȃg���N�ω�������邽�� (�[���}�C�̂悤�ɂ�����ł�������悤�ɍ폜)�B 07/07/26
		// while(diff >  M_PI) diff -= 2 * M_PI;
		// while(diff < -M_PI) diff += 2 * M_PI;
		double tmp = 1.0 / (damper + spring * GetScene()->GetTimeStep());
		dA.w().z = tmp * dtinv;
		db.w().z = spring * (diff) * tmp;
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
	J.col(0).SUBVEC(0, 3).clear();
	J.col(0).SUBVEC(3, 3) = Vec3d(0.0, 0.0, 1.0);
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