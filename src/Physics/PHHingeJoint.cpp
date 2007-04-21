/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
IF_OBJECT_IMP(PHHingeJoint, PHJoint1D)

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
	double dtinv = 1.0 / scene->GetTimeStep();
	//	hase	�������ꂽ�ق����A�኱���艻����B
	//	�{�����x�����̓s�x�C�����Ă���̂ŁAPD����ł͂Ȃ��̂����ǁA
	//	shrinkRatio �������ꍇ�A�S���͂��c�邽�߁AD������ƈ��艻����B
	//	tazz �ǋL�D�v����Ɍ��݂̌덷�ł͂Ȃ��������̗\���덷��0�ɂ���悤�ɂ���
	db.v() = (Xjrel.r * dtinv + vjrel.v());
	db.w() = (Xjrel.q.AngularVelocity((Xjrel.q - Quaterniond()) * dtinv) + vjrel.w());
	db.w().z = 0.0;
	db *= engine->velCorrectionRate;

	double diff;
	if(mode == MODE_VELOCITY){
		db.w().z = -vel_d;
	}
	else if(onLower || onUpper){
	}
	else if(spring != 0.0 || damper != 0.0){
		diff = GetPosition() - origin;
		while(diff >  M_PI) diff -= 2 * M_PI;
		while(diff < -M_PI) diff += 2 * M_PI;
		double tmp = 1.0 / (damper + spring * scene->GetTimeStep());
		dA.w().z = tmp * dtinv;
		db.w().z = spring * (diff) * tmp;
	}
}

void PHHingeJoint::CompError(){
	//B.v() = Xjrel.r;
	//B.w() = Xjrel.q.V();
	//B.w().z = 0.0;
}


//-----------------------------------------------------------------------------
IF_OBJECT_IMP(PHHingeJointNode, PHTreeNode1D);

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
	DSTR << j->Xjrel.q << endl;
	j->Xjrel.r.clear();
}

}
