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
// PH3ElementHingeJointDesc

PH3ElementHingeJointDesc::PH3ElementHingeJointDesc(){
	
	PHHingeJointDesc();
	secondDamper		  = 0.0;

}

//----------------------------------------------------------------------------
// PH3ElementHingeJoint
// �R���X�g���N�^
PH3ElementHingeJoint::PH3ElementHingeJoint(const PH3ElementHingeJointDesc& desc){
	PHHingeJoint::PHHingeJoint();
}

void PH3ElementHingeJoint::CompBias(){
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

	if(mode == PHJointDesc::MODE_VELOCITY){
		db.w().z = -desiredVelocity;
	}else if(spring != 0.0 || damper != 0.0){
		if (onUpper&& GetVelocity()>0){
		}else if (onLower && GetVelocity()<0){
		}else{
			double diff;
			diff = GetPosition() - origin;
			double springLim = 0;
			double damper_ = damper;
			double diffLim = 0;
			if (onUpper){
				diffLim = GetPosition() - upper;
				springLim = 10000;
				damper_   = 100;
			} else if (onLower){
				diffLim = GetPosition() - lower;
				springLim = 10000;
				damper_   = 100;
			}

			// �s�A���ȃg���N�ω�������邽�� (�[���}�C�̂悤�ɂ�����ł�������悤�ɍ폜)�B 07/07/26
			//// ���ނ��낱�̃R�[�h�����邱�Ƃŕs�A���ȃg���N�ω����������Ă���̂ł́H�Ǝv�������D 08/10/07 mitake
			while(diff >  M_PI) diff -= 2 * M_PI;
			while(diff < -M_PI) diff += 2 * M_PI;
			
			//3�v�f���f���̐ݒ�
			double dtinv = 1.0 / GetScene()->GetTimeStep(), tmp, tmpA, tmpB;
			double D1 = damper_;
			double D2 = secondDamper;
			double K = spring;
			double h = GetScene()->GetTimeStep();
			
			ws=vjrel;	//�o�l�̃_���p�̕��񕔂̑���
			tmp = D2-D1+K*h;
			xs[1] = ((D2-D1)/tmp)*xs[0] + (D2*h/(D2-D1))*ws;	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V	
			tmpA =(D2-D1)*(D2-D1)/(D1*D2*tmp) ;
			tmpB =K*(D2-D1)*(D2-D1)/(D2*tmp*tmp)*(xs[0].w().z);
			dA.w().z= tmpA* dtinv;
			db.w().z = tmpB * ((spring * diff + springLim*diffLim)- (damper_ * desiredVelocity) - offsetForce );	
			xs[0]=xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
			}
	}
}
}