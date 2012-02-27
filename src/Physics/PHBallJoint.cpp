/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 * 
 *�@SwingDir�̐����͓����Ă��܂���B�K�v�Ȑl���撣���Ď������Ă�������(by toki 2007.12.05)
 *
 */
#include <Physics/PHBallJoint.h>
#include <Physics/PHConstraintEngine.h>

using namespace PTM;
using namespace std;

namespace Spr{;
// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHBallJointNode

void PHBallJointNode::CompJointJacobian(){
	PHBallJoint* j = GetJoint();
	Quaterniond q = j->Xjrel.q;
	for(int i = 0; i < 3; i++)
		J.col(i).sub_vector(PTM::TSubVectorDim<0,3>()).clear();
	J.col(0).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(1.0, 0.0, 0.0);
	J.col(1).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 1.0, 0.0);
	J.col(2).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNodeND<3>::CompJointJacobian();
}

void PHBallJointNode::CompJointCoriolisAccel(){
	cj.clear();		//�֐ߍ��W��quaternion�ɂƂ�ꍇ�R���I������0
}

void PHBallJointNode::UpdateJointPosition(double dt){
	PHBallJoint* j = GetJoint();
	j->Xjrel.q += j->Xjrel.q.Derivative(j->vjrel.w()) * dt;
	j->Xjrel.q.unitize();
}

void PHBallJointNode::CompRelativePosition(){
	PHBallJoint* j = GetJoint();
	j->Xjrel.r.clear();
}

void PHBallJointNode::CompRelativeVelocity(){
	PHBallJoint* j = GetJoint();
	j->vjrel.v().clear();
	j->vjrel.w() = j->velocity;
}

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 
// PHBallJoint

PHBallJoint::PHBallJoint(const PHBallJointDesc& desc){
	SetDesc(&desc);
	nMovableAxes   = 3;
	movableAxes[0] = 3;
	movableAxes[1] = 4;
	movableAxes[2] = 5;
	InitTargetAxes();

	limit = NULL;
	motor.joint = this;
}

void PHBallJoint::SetupLCP(){
	PHJoint::SetupLCP();
	if (limit) { limit->SetupLCP(); }
}

void PHBallJoint::IterateLCP(){
	PHJoint::IterateLCP();
	if (limit) { limit->IterateLCP(); }
}

void PHBallJoint::CompBias(){
	//	���i�덷�̉����̂��߁A���x�Ɍ덷/dt�����Z, Xjrel.r: �\�P�b�g�ɑ΂���v���O�̈ʒu�̃Y��
	db.v_range(0,3) = Xjrel.r * GetScene()->GetTimeStepInv();
	db.v_range(0,3) *= engine->velCorrectionRate;

	motor.CompBias();
}

void PHBallJoint::UpdateJointState(){
	// position�̍X�V�FBallJoint�� position ��SwingTwist���W�n�̊p�x�l�Ƃ���

	// Swing�p�̌v�Z
	Vec3d  lD = Vec3d(); if(limit){ lD = limit->GetLimitDir(); }
	Vec3d  ez = Xjrel.q * Vec3d(0.0, 0.0, 1.0);
	double  c = dot(lD, ez); c = max(-1.0, min(c, 1.0));
	position[0] = acos(c);

	// Swing���ʊp�̌v�Z
	if (ez.x == 0) {
		position[1] = (ez.y >= 0) ? M_PI/2.0 : 3*M_PI/2.0;
	} else {
		position[1] = atan(ez.y / ez.x);
		if (ez.x < 0) {
			position[1] +=   M_PI;
		} else if (ez.x > 0 && ez.y < 0) {
			position[1] += 2*M_PI;
		}
	}

	// Twist�p�̌v�Z
	Quaterniond qSwing;
	Vec3d  halfEz = 0.5*(Vec3d(0,0,1) + ez);
	double l = halfEz.norm();
	if (l > 1e-20) {
		qSwing.V() = cross(halfEz/l, Vec3d(0,0,1));
		qSwing.W() = sqrt(1 - qSwing.V().square());
	} else {
		qSwing.V() = Vec3d(1,0,0);
		qSwing.W() = 0;
	}
	Quaterniond qTwist = qSwing * Xjrel.q;
	position[2] = qTwist.Theta();
	if (qTwist.z < 0) { position[2] *= -1; } ///< Twist��]�������΂��������Ƃ�����̂ł��̑΍�
	if (position[2] < -M_PI) { position[2] += 2*M_PI; }
	if (position[2] >  M_PI) { position[2] -= 2*M_PI; }
}

void PHBallJoint::CompError(){
	B.v_range(0,3) = Xjrel.r;
}

}
