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
		double dtinv = 1.0 / GetScene()->GetTimeStep(), tmp;
		double D1 = damper;
		double D2 = secondDamper;
		double K = spring;
		double h = GetScene()->GetTimeStep();
		
		ws=vjrel;	//�o�l�̃_���p�̕��񕔂̑���
		tmp = D2-D1+K*h;
		xs[1] = ((D2-D1)/tmp)*xs[0] + (D2*h/(D2-D1))*ws;	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V		
		dA.w()[0]= (D2-D1)*(D2-D1)/(D1*D2*tmp) * dtinv;
		dA.w()[1]= (D2-D1)*(D2-D1)/(D1*D2*tmp) * dtinv;
		dA.w()[2]= (D2-D1)*(D2-D1)/(D1*D2*tmp) * dtinv;

		db.w() = K*(D2-D1)*(D2-D1)/(D2*tmp*tmp)*(xs[0].w()) ;
		
		xs[0]=xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�

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
