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
#include "Physics.h"
#include <Physics/PHBallJoint.h>
//#include "../../../Project\personal\Matunaga\HapticJointTest\BoxStack.h"
#include <Math.h>
#pragma hdrstop

using namespace PTM;
using namespace std;

namespace Spr{;
//----------------------------------------------------------------------------
// PH3ElementBallJointDesc

PH3ElementBallJointDesc::PH3ElementBallJointDesc(){
	
	PHBallJointDesc();
	secondDamper		  = 0.0;
	yieldStress			=0.0;			// �~������
	hardnessRate		=1.0;		// �~�����͈ȉ��̏ꍇ�ɓ�ڂ̃_���p�W���Ɋ|����䗦

}

//----------------------------------------------------------------------------
// PH3ElementBallJoint
// �R���X�g���N�^
PH3ElementBallJoint::PH3ElementBallJoint(const PH3ElementBallJointDesc& desc){
	PHBallJoint::PHBallJoint();
}

// �I�[�o�[���C�h����Ă���̂�PH3ElementBallJoint��ǂ񂾎��ɂ�
// PHBallJoint::CompBias()�ł͂Ȃ��Ă��̊֐��������������Ă΂��B
void PH3ElementBallJoint::CompBias(){
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	
	db.v() = Xjrel.r * dtinv;		//	���i�덷�̉����̂��߁A���x�Ɍ덷/dt�����Z, Xjrel.r: �\�P�b�g�ɑ΂���v���O�̈ʒu
	db.v() *= engine->velCorrectionRate;

	Quaterniond propQ = goal * Xjrel.q.Inv();	
	Vec3d propV = propQ.RotationHalf();

	// ���搧�����������Ă���ꍇ��prop�̍��W��ϊ����čl���Ȃ��Ƃ����Ȃ��B
	if (anyLimit){
		propV = Jcinv * propV;
	}

	// �o�l�_���p�������Ă�����\�z����
	if (spring != 0.0 || damper != 0.0 || secondDamper!=0.0){
		//3�v�f���f��
		double dtinv = 1.0 / GetScene()->GetTimeStep(), tmp;
		double D1 = damper;
		double D2 = secondDamper;
		double K = spring;
		double h = GetScene()->GetTimeStep();
		
		//�~�����͈ȉ��ł̓W���C���g���ł�����
		if(f.w().norm()>=yieldStress ){
			K = spring;
			D1 = damper;
			D2=secondDamper;
		}else{
			K = spring;
			D1 = damper;
			D2= secondDamper*hardnessRate;
		}
		//3�v�f���f���̌v�Z
		ws=vjrel;	//�o�l�ƃ_���p�̕��񕔂̑���
		tmp = D1+D2+K*h;

		xs[1] = ((D1+D2)/tmp)*xs[0] + (D2*h/tmp)*ws;	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V
		
		for(int i=0;i<3;i++){
		dA.w()[i]= tmp/(D2*(K*h+D1)) * dtinv /I[i];
		}
		db.w() = K/(K*h+D1)*(xs[0].w()) ;
	
		xs[0]=xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
	}else{
			//dA.w().clear();
			db.w().clear();
	}
	
	Vec3d vJc = Jc * vjrel.w();
	// ����t���O�̎w��onLimit[0]: swing, onLimit[1]: twist
	// nowTheta[0]: swing, nowTheta[1]: twist
	// ���搧�����z���Ă�����AdA:�֐߂��_�炩�����鐬����0�ɂ���Adb:�N�����Ă������������ɖ߂�	
	// x�������ɍS����������ꍇ	
	if(onLimit[0].onLower){
		dA.w()[0] = 0;
		db.w()[0] = (nowTheta[0] - limitSwing[0]) * dtinv * engine->velCorrectionRate;
	}
	
	else if(onLimit[0].onUpper){
		dA.w()[0] = 0;
		db.w()[0] = (nowTheta[0] - limitSwing[1]) * dtinv * engine->velCorrectionRate;
	}

	//z�������ɍS����������ꍇ
	if(onLimit[1].onLower && (vJc.z < 0)){
		dA.w()[2] = 0;
		db.w()[2] = (nowTheta[1] - limitTwist[0]) * dtinv * engine->velCorrectionRate;
	}
	else if(onLimit[1].onUpper && (vJc.z > 0)){
		dA.w()[2] = 0;
		db.w()[2] = (nowTheta[1] - limitTwist[1]) * dtinv * engine->velCorrectionRate;
	}
}

//----------------------------------------------------------------------------
// PH3ElementBallJointNode
PH3ElementBallJointNode::PH3ElementBallJointNode(const PH3ElementBallJointNodeDesc& desc):PHBallJointNode(desc){
}

void PH3ElementBallJointNode::CompJointJacobian(){
	PH3ElementBallJoint* j = GetJoint();

	Quaterniond q = j->Xjrel.q;
	for(int i = 0; i < 3; i++)
		J.col(i).sub_vector(PTM::TSubVectorDim<0,3>()).clear();
	J.col(0).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(1.0, 0.0, 0.0);
	J.col(1).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 1.0, 0.0);
	J.col(2).sub_vector(PTM::TSubVectorDim<3, 3>()) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNodeND<3>::CompJointJacobian();
}

void PH3ElementBallJointNode::CompJointCoriolisAccel(){
	//PHBallJoint* j = GetJoint();
	//cj.v().clear();
	//((SwingTwist&)(j->position)).Coriolis(cj.w(), j->velocity);
	//cj.w.clear();
	cj.clear();		//�֐ߍ��W��quaternion�ɂƂ�ꍇ�R���I������0
}

void PH3ElementBallJointNode::UpdateJointPosition(double dt){
	PH3ElementBallJoint* j = GetJoint();
	j->Xjrel.q += j->Xjrel.q.Derivative(j->vjrel.w()) * dt;
	j->Xjrel.q.unitize();
}

void PH3ElementBallJointNode::CompRelativePosition(){
	PH3ElementBallJoint* j = GetJoint();
	j->Xjrel.r.clear();
	//j->Xjrel.q��UpdateJointPosition�ōX�V�ς�
}
void PH3ElementBallJointNode::CompRelativeVelocity(){
	PH3ElementBallJoint* j = GetJoint();
	j->vjrel.v().clear();
	j->vjrel.w() = j->velocity;
}

void PH3ElementBallJointNode::ModifyJacobian(){
	PH3ElementBallJoint* j = GetJoint();
	Jq = j->GetJcinv();
}

void PH3ElementBallJointNode::CompBias(){

	PH3ElementBallJoint* j = GetJoint();
	j->dA.clear();
	j->db.clear();
	j->CompBias();
	dA = j->dA.w();
	db = j->db.w();
}

void PH3ElementBallJointNode::Projection(double& f, int k){
	PH3ElementBallJoint* j = GetJoint();
	OnLimit* limit=j->GetOnLimit();
	for(int i=0; k<2; k++){
		if(limit[i].onLower)
			f = max(0.0, f);
		if(limit[i].onUpper)
			f = min(0.0, f);
	}
}
}
