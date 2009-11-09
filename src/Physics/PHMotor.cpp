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

PHMotor1D::PHMotor1D(){}
	
void PHMotor1D::SetupLCP(){
	fMinDt = joint->fMin * joint->GetScene()->GetTimeStep();
	fMaxDt = joint->fMax * joint->GetScene()->GetTimeStep();

	// �I�t�Z�b�g�͂̂ݗL���̏ꍇ�͍S���͏����l�ɐݒ肷�邾���ł悢
	if(joint->spring == 0.0 && joint->damper == 0.0){
		dA = db = 0.0;
		joint->motorf.z = joint->offsetForce;
	}
	else{
		A = joint->A[joint->axisIndex[0]];
		b = joint->b[joint->axisIndex[0]];

		if(joint->secondDamper == 0.0){
			double tmp = 1.0 / (joint->damper + joint->spring * joint->GetScene()->GetTimeStep());
			dA = tmp * joint->GetScene()->GetTimeStepInv();
			db = tmp * (joint->spring * joint->GetDeviation()
				- joint->damper * joint->targetVelocity - joint->offsetForce);
		}
		else{
			//3�v�f���f���̐ݒ�
			double dtinv = joint->GetScene()->GetTimeStepInv(), tmp, tmpA, tmpB;
			double D1 = joint->damper;
			double D2 = joint->secondDamper;
			double K = joint->spring;
			double h = joint->GetScene()->GetTimeStep();
			
			ws = joint->vjrel;	//�o�l�̃_���p�̕��񕔂̑���
			tmp = D2-D1+K*h;
			joint->xs[1] = ((D2-D1)/tmp)*joint->xs[0] + (D2*h/(D2-D1))*ws;	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V	
			tmpA = (D2-D1)*(D2-D1)/(D1*D2*tmp) ;
			tmpB = K*(D2-D1)*(D2-D1)/(D2*tmp*tmp)*(joint->xs[0].w().z);
			dA = tmpA * dtinv;
			db = tmpB * (K * joint->GetDeviation() - D1 * joint->targetVelocity - joint->offsetForce);
			joint->xs[0] = joint->xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
		}
		Ainv = 1.0 / (A + dA);
		joint->motorf.z *= joint->engine->shrinkRate;
	}
			
	// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
	joint->CompResponse(joint->motorf.z, 0);
}

void PHMotor1D::IterateLCP(){
	if(joint->spring == 0.0 && joint->damper == 0.0)
		return;

	int j = joint->axisIndex[0];
	double fnew = joint->motorf.z - joint->engine->accelSOR * Ainv * (dA * joint->motorf.z + b + db
			 + joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);

	// �g���N����
	fnew = min(max(fMinDt, fnew), fMaxDt);
	joint->CompResponse(fnew - joint->motorf.z, 0);
	joint->motorf.z = fnew;
}

////////////////////////////////////////////////////////////////////////////////////////

PHBallJointMotor::PHBallJointMotor(){
	yieldFlag	=  false;
	fMaxDt		=  FLT_MAX;
	fMinDt		= -FLT_MAX;
}

void PHBallJointMotor::ElasticDeformation(){
	double tmp = 1.0 / (joint->damper + joint->spring * dt);
	Vec3d I = joint->Inertia, v0 = joint->targetVelocity, f0 = joint->offsetForce;
	for(int i=0;i<3;i++){
		dA[i] = tmp * dtinv * I[i];		
		db[i] = tmp * (- K * I[i] * propV[i] - D * I[i] * v0[i] - f0[i]);
	}
}

void PHBallJointMotor::PlasticDeformation(){

	//3�v�f���f��
	/*		K
		�\VVVV�\   D2
	�\|			]�\�� �\
		�\ �� �\
			D1
	*/
	//�Y���ό`(3�v�f���f��)
	D  *= joint->hardnessRate;
	D2 *= joint->hardnessRate;
	K  *= joint->hardnessRate;
	double tmp = D+D2+K*dt;
	ws = joint->vjrel;	//�o�l�ƃ_���p�̕��񕔂̑���

	joint->xs[1] = ((D+D2)/tmp)*joint->xs[0] + (D2*dt/tmp)*ws;	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V
	for(int i=0;i<3;i++){
		dA[i]= tmp/(D2*(K*dt+D)) * dtinv /I[i];
	}
	db = K/(K*dt+D)*(joint->xs[0].w()) ;
	
	//ELASTIC_PLASTIC���[�h�̏ꍇ,PLASTIC��Ԃ̏I�����Ɏc���ψʂ�ۑ�����ʒu��TargetPosition��ύX
	if(joint->type==PHBallJointDesc::ELASTIC_PLASTIC){
		if(ws.w().norm()<0.01){
			yieldFlag = false;
			joint->SetTargetPosition(joint->Xjrel.q);
		}
	}
	joint->xs[0]=joint->xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
}

void PHBallJointMotor::SetupLCP(){
	fMinDt = joint->fMin * joint->GetScene()->GetTimeStep();
	fMaxDt = joint->fMax * joint->GetScene()->GetTimeStep();
	dt		= joint->GetScene()->GetTimeStep();
	dtinv	= joint->GetScene()->GetTimeStepInv();
	D  = joint->damper;
	D2 = joint->secondDamper;
	K  = joint->spring;
	I  = joint->Inertia;
	
	// �I�t�Z�b�g�͂̂ݗL���̏ꍇ�͍S���͏����l�ɐݒ肷�邾���ł悢
	if(K == 0.0 && D == 0.0){
		dA.clear();
		db.clear();
		joint->motorf = joint->offsetForce;
	}
	else{
		// �ʒu����̌v�Z
		/*******************************************************************************************************
		����Ȃ��p�x�̍�����]���x�N�g���ɕϊ��DpropV(�c�肳��̘_���ł���q[t])�ɑ΂���db.w()���v�Z���Ă���.
		���R����0[rad]�ŁCpropV[rad]�L�т����ɑ΂��Ă̊p�x�o�l���\�����Ă���ƍl����΂����D
		********************************************************************************************************/
		propQ = joint->targetPosition * joint->Xjrel.q.Inv();	// Xjrel.q�̖ڕWtargetPosition��Xjrel.q�̎��ۂ̊p�x�̍���Quaternion�Ŏ擾
		propV = propQ.RotationHalf();

		A = joint->A.w();
		b = joint->b.w();
		
		if(joint->secondDamper == 0.0){
			/*****************************************************************************
			w[t+1] = (A+dA) * ��[t+1] + (b+db)
			��A�̓��C�o�l�_���p���\������̂́CBallJoint�̏ꍇ�͉�]�̎O���ɂ��ĂȂ̂ŁC
			SpatialVector dA.w()��tmp = (D + K��t)^{-1}�����Ă���D
			dtinv������ɂ�����̂�,ppt�ȂǕ\�L�Ƃ͈Ⴂ�CA=JM^{-1}J^T ��t�̂����C
			�v���O�������Ōv�Z���Ă���̂́CA = JM^{-1}J^T�����D
			���Ƃ���(A + dA)��t�����Ă��邽��
			******************************************************************************/
			double tmp = 1.0 / (D + K * dt);
			dA = tmp * dtinv * Vec3d(1.0, 1.0, 1.0);

			// ���搧�����������Ă���ꍇ��prop�̍��W��ϊ����Ȃ��Ƃ����Ȃ��B
			//if (joint->IsLimit())
			//	propV = Jcinv * propV; //< �S�����R�r�A�����Ăǂ��łƂ��񂾂��������

			/****
			�c�肳��̘_���̎�25��db�ɑ�������D
			�ʒu����݂̂ł���΁C�ȉ��̎���1�s�ڂ̂݁D
			�O���Ǐ]����ł͎c���2�s���ӂ��ށDoffset�ɂ͊O�Ōv�Z���Ă������������e���\����������
			���x����ł�D�𖳌���ɔ�΂��D�ʒu���䓙�֖߂�����D���߂��̂��Y�ꂸ�ɁD
			****/
			db = tmp * (- K * propV - D * joint->targetVelocity - joint->offsetForce);

			/* [comment]:�̂�PHControlMode�̖��c�D
			if(mode == PHJointDesc::MODE_VELOCITY){
				if(anyLimit)
					db.w() = -Jcinv * targetVelocity;
				else
					db.w() = - targetVelocity;
			}else if(mode == PHJointDesc::MODE_TRAJ){
				if(anyLimit)
					db.w() = -Jcinv * (targetVelocity + spring * propV);
				else
					db.w() = - (targetVelocity + spring * propV);
			}else if (mode == PHJointDesc::MODE_POSITION){
				// �o�l�_���p�������Ă�����\�z����
				if (spring != 0.0 || damper != 0.0){
					double tmp = 1.0 / (damper + spring * GetScene()->GetTimeStep());
				}
			}*/
		}
		else{
			//Save�Ɋւ��镔����fs���ۑ�����Ă��Ȃ��̂ňꎞ�I�ɃR�����g�A�E�g
			//f�̕��ϒl���v�Z
			fNorm = 0;
			for(int i=0; i<5 ;i++){
				if(i==4){
					joint->fs[4] = joint->motorf;
				}else{ 
					joint->fs[i] = joint->fs[i+1];
				}
				
				fNorm+=joint->fs[i].norm()/5;
			}
			//fNorm = joint->motorf.norm();

			//���̂̌`����l�������o�l�_���p��ݒ肷��ꍇ
			if(I[0]!=1&&I[1]!=1&&I[2]!=1){
				//���̂̕ό`�Ɏg�p����ꍇ
				/*x���Cy�����̕ό`(�Ȃ��j
					I(�f��2�����[�����g),E(�����O��),T(�g���N),l(���̊Ԃ̋���)�Ƃ����Ƃ�
	�@				T=EI��/l
				  z�����̕ό`�i�˂���j
					G(����f�e���W��),v(�|���\����)
					G=E/2(1+v)
					T=GI��/l 
				*/
				double v=0.3;		//�|���\�����0.3���炢������

				//�l�p�`�̏ꍇ
				if(I[0]>I[1]){
					I[2]=I[1]*4/(2*(1+v));
				}else{
					I[2]=I[0]*4/(2*(1+v));
				}
			}

			if(fNorm > joint->yieldStress){
				yieldFlag = true;
			}
			switch(joint->type){
			case PHBallJointDesc::ELASTIC:	//PHDeformationType::Elastic 0�@�����l
				ElasticDeformation();
				break;
			case PHBallJointDesc::PLASTIC:	//PHDeformationType::Plastic 1
				PlasticDeformation();
				break;
			case PHBallJointDesc::ELASTIC_PLASTIC: //PHDeformationType::ELASTIC_PLASTIC 2	
				if(yieldFlag){
					PlasticDeformation();	//�Y���ό`
				}else {
					ElasticDeformation();	//�e���ό`
				}
				break;
			default:
				ElasticDeformation();
				break;
			}
		}
		for(int i = 0; i < 3; i++)
			Ainv[i] = 1.0 / (A[i] + dA[i]);
		joint->motorf *= joint->engine->shrinkRate;
	}
}

void PHBallJointMotor::IterateLCP(){
	if(K == 0.0 && D == 0.0)
		return;

	Vec3d fnew;
	for(int i = 0; i < 3; i++){
		int j = joint->axisIndex[i];
		fnew[i] = joint->motorf[i] - joint->engine->accelSOR * Ainv[i] * (dA[i] * joint->motorf[i] + b[i] + db[i]
				+ joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);	

		if(fMaxDt < fnew[i])
			fnew[i] = fMaxDt;
		else if(joint->motorf[i] < fMinDt)
			joint->motorf[i] = fMinDt;
		
		joint->CompResponse(fnew[i] - joint->motorf[i], i);
		joint->motorf[i] = fnew[i];
	}
	
}

}