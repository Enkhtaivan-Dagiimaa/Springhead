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
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace PTM;
using namespace std;

namespace Spr{;
//----------------------------------------------------------------------------
// PH3ElementBallJointDesc

PH3ElementBallJointDesc::PH3ElementBallJointDesc(){
	
	PHBallJointDesc();
	secondDamper		  = 0.0;

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
	if (spring != 0.0 || damper != 0.0){
		double D1 = damper;
		double D2 = secondDamper;
		double K = spring;
		double h = GetScene()->GetTimeStep();
		SpatialVector wt;
		SpatialVector ft;

		//double tmp = 1.0 / (damper + spring * GetScene()->GetTimeStep()) ;
		double tmp =-(D1+D2+K)/(D2*(D1+K*h));
		dA.w()[0] = tmp * dtinv;
		dA.w()[1] = tmp * dtinv;
		dA.w()[2] = tmp * dtinv;

		//db.w() = - spring * propV * tmp;
		db.w()[0] = (D1*wt.w()[0]-(1+D1/D2)*ft.w()[0])/(D1+K*h);
		db.w()[1] = (D1*wt.w()[1]-(1+D1/D2)*ft.w()[1])/(D1+K*h);
		db.w()[2] = (D1*wt.w()[2]-(1+D1/D2)*ft.w()[2])/(D1+K*h);
		//�P�X�e�b�v�O��w��f��p����̂ŁAdb�̌v�Z��Ɏ��̃X�e�b�v�ɗp���錻�݂�w,f���X�V
		wt=vjrel;
		ft=f;
	}
	else{
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

}
