/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Physics.h"
#include <iostream>
#include <iomanip>
#pragma hdrstop

using namespace PTM;
using namespace std;
namespace Spr{;

PHMotor1D::PHMotor1D(){
	yieldFlag = false;
	A = Ainv = dA = b = db = 0.0;
	fMaxDt = DBL_MAX;
}

void PHMotor1D::ElasticDeformation(){
	double tmp = 1.0 / (D + K * dt);
	dA = tmp * dtinv;
	double pos = joint->GetPosition();
	double tar = joint->GetTargetPosition();
	// offsetForce��dA�Ɠ��������ɂ��Ȃ���΂Ȃ�Ȃ��Dby toki 2009.11.12
	db = tmp * (K * (pos - tar)	- D * joint->targetVelocity - joint->offsetForce * dtinv);
}

void PHMotor1D::PlasticDeformation(){

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
	dA= tmp/(D2*(K*dt+D)) * dtinv;
	db = K/(K*dt+D)*(joint->xs[0].w().z) ;
	
	//ELASTIC_PLASTIC���[�h�̏ꍇ,PLASTIC��Ԃ̏I�����Ɏc���ψʂ�ۑ�����ʒu��TargetPosition��ύX
	if(joint->type==PHJointDesc::ELASTIC_PLASTIC){
		if(ws.w().norm()<0.01){
			yieldFlag = false;
			Vec3d ori = joint->Xjrel.q.Rotation();
			joint->SetTargetPosition(ori.z);
			//Vec2d range;
			//joint->GetRange(range[0],range[1]);
			//range +=Vec2d(Deg(ori.z),Deg(ori.z));
			//if(range[0]>360) range -=Vec2d(360,360);
			//if(range[1]<-360) range +=Vec2d(360,360);
			//DSTR<<range[0]<<","<<range[1]<<","<<Deg(ori.z)<<std::endl;
			//joint->SetRange(range[0], range[1]);
		}
	}
	joint->xs[0]=joint->xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
}
void PHMotor1D::SetupLCP(){
	dt		= joint->GetScene()->GetTimeStep();
	dtinv	= 1.0/dt;
	fMaxDt  = joint->fMax * dt;
	K		= joint->spring;
	D		= joint->damper;
	D2		= joint->secondDamper;

	// �I�t�Z�b�g�͂̂ݗL���̏ꍇ�͍S���͏����l�ɐݒ肷�邾���ł悢
	if(joint->spring == 0.0 && joint->damper == 0.0){
		dA = db = 0.0;
		if(joint->offsetForce >= joint->fMax) 
			joint->motorf.z = joint->fMax;
		else 
			joint->motorf.z = joint->offsetForce;
	}
	else{
		A = joint->A[joint->axisIndex[0]];
		b = joint->b[joint->axisIndex[0]];

		switch(joint->type){
		case PHJointDesc::ELASTIC:	//PHDeformationType::Elastic 0�@�����l
			ElasticDeformation();
			break;
		case PHJointDesc::PLASTIC:	//PHDeformationType::Plastic 1
			PlasticDeformation();
			break;
		case PHJointDesc::ELASTIC_PLASTIC: //PHDeformationType::ELASTIC_PLASTIC 2	
			if(IsYield()){
				PlasticDeformation();	//�Y���ό`
			}else {
				ElasticDeformation();	//�e���ό`
			}
			break;
		default:
			ElasticDeformation();
			break;
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
	double fold = joint->motorf.z;
	double fnew = fold - joint->engine->accelSOR * Ainv * (dA * fold + b + db
			 + joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);

	// �g���N����
	fnew = (fnew > 0) ? min(fnew, fMaxDt) : max(fnew, -fMaxDt);
	joint->CompResponse(fnew - fold, 0);
	joint->motorf.z = fnew;
#if 0
	static int count = 0;
	int nu = joint->GetScene()->GetNumIteration();
	if(count == nu){
		DSTR << "f:" << setw(4) << setfill(' ') << fnew/dt << ", fMax:" << setw(4) << setfill(' ')  << fMaxDt/dt << ", f_0:" << setw(4) << setfill(' ') << joint->GetOffsetForce() << endl;
		count = 0;
	}else count++;
#endif
}

bool PHMotor1D::IsYield(){
	//f�̕��ϒl���v�Z
	double dt		= joint->GetScene()->GetTimeStep();
	double fNorm = 0;
	for(int i=0; i<5 ;i++){
		if(i==4){
			joint->fs[4] = joint->motorf / dt;
		}else{ 
			joint->fs[i] = joint->fs[i+1];
		}
		
		fNorm+=joint->fs[i].norm()/5;
	}
	if(fNorm > joint->yieldStress){
		yieldFlag = true;
	}
	return yieldFlag;
}
////////////////////////////////////////////////////////////////////////////////////////

PHBallJointMotor::PHBallJointMotor(){
	yieldFlag	=  false;
	fMaxDt		=  FLT_MAX;
}

void PHBallJointMotor::ElasticDeformation(){
	Vec3d v0 = joint->targetVelocity, f0 = joint->offsetForce;
	for(int i=0;i<3;i++){
		double tmp = 1.0 / (D[i] + K[i] * dt);
		dA[i] = tmp * dtinv;		
		db[i] = tmp * (- K[i] * propV[i] - D[i] * v0[i] - f0[i] * dtinv);
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
	
	ws = joint->vjrel;	//�o�l�ƃ_���p�̕��񕔂̑���

	
	for(int i=0;i<3;i++){
		double tmp = D[i]+D2[i]+K[i]*dt;
		joint->xs[1].w()[i] = ((D[i]+D2[i])/tmp)*joint->xs[0].w()[i] + (D2[i]*dt/tmp) * ws.w()[i];	//�o�l�ƃ_���p�̕��񕔂̋����̍X�V
		dA[i]= tmp/(D2[i]*(K[i]*dt+D[i])) * dtinv;
		db[i] = K[i]/(K[i]*dt+D[i])*(joint->xs[0].w()[i]) ;
	}


	//Quaterniond qAll = joint->targetPosition.Inv() * joint->Xjrel.q;;//3�v�f�S�̂̕ψʍ��̃N�H�[�^�j�I��
	//Quaterniond qForct;
	//Vec3d fAngle = joint->xs[1].w();
	//qForct.FromEuler(Vec3d(fAngle.y,fAngle.z,fAngle.x));//�o�l�_���p���̕ψʍ��̃N�H�[�^�j�I��
	//CSVOUT<<Deg(joint->Xjrel.q.Theta())<<","<<Deg(qForct.Theta())<<std::endl;
	
	//ELASTIC_PLASTIC���[�h�̏ꍇ,PLASTIC��Ԃ̏I�����Ɏc���ψʂ�ۑ�����ʒu��TargetPosition��ύX
	//if(joint->type==PHBallJointDesc::ELASTIC_PLASTIC){
	//	//if(ws.w().norm()<0.1){
	//	if(ws.w().norm()<0.001){
	//		yieldFlag = false;
	//		Quaterniond qAll = joint->Xjrel.q;//3�v�f�S�̂̕ψʍ��̃N�H�[�^�j�I��
	//		Quaterniond qForct;
	//		Vec3d fAngle = joint->xs[1].w();
	//		qForct.FromEuler(Vec3d(fAngle.y,fAngle.z,fAngle.x));//�o�l�_���p���̕ψʍ��̃N�H�[�^�j�I��
	//		//DSTR<<"All: "<<Deg(qAll.Theta())<<"Forct: "<<Deg(qForct.Theta())<<std::endl;
	//		//DSTR<<"All: "<<qAll.Axis()<<"Forct: "<<qForct.Axis()<<std::endl;
	//		Quaterniond difQ = qForct.Inv() * qAll;
	//		joint->SetTargetPosition(difQ);
	//	}
	//}
	joint->xs[0]=joint->xs[1];	//�o�l�ƃ_���p�̕��񕔂̋����̃X�e�b�v��i�߂�
}

void PHBallJointMotor::SetupLCP(){
	fMaxDt = joint->fMax * joint->GetScene()->GetTimeStep();
	dt		= joint->GetScene()->GetTimeStep();
	dtinv	= joint->GetScene()->GetTimeStepInv();
	D  = joint->damper *joint->Inertia;
	D2 = joint->secondDamper *joint->Inertia;
	K  = joint->spring *joint->Inertia;

	// �I�t�Z�b�g�͂̂ݗL���̏ꍇ�͍S���͏����l�ɐݒ肷�邾���ł悢
	if(joint->spring == 0.0 && joint->damper == 0.0){
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

		////���̂̌`����l�������o�l�_���p��ݒ肷��ꍇ
		//if(I[0]!=1&&I[1]!=1&&I[2]!=1){
		//	//���̂̕ό`�Ɏg�p����ꍇ
		//	/*x���Cy�����̕ό`(�Ȃ��j
		//		I(�f��2�����[�����g),E(�����O��),T(�g���N),l(���̊Ԃ̋���)�Ƃ����Ƃ�
		//			T=EI��/l
		//	  z�����̕ό`�i�˂���j
		//		G(����f�e���W��),v(�|���\����)
		//		G=E/2(1+v)
		//		T=GI��/l 
		//	*/
		//	double v=0.3;		//�|���\�����0.3���炢������

		//	//�l�p�`�̏ꍇ
		//	if(I[0]>I[1]){
		//		I[2]=I[1]*4/(2*(1+v));
		//	}else{
		//		I[2]=I[0]*4/(2*(1+v));
		//	}
		//}
		switch(joint->type){
			case PHJointDesc::ELASTIC:	//PHDeformationType::Elastic 0�@�����l
				ElasticDeformation();
				break;
			case PHJointDesc::PLASTIC:	//PHDeformationType::Plastic 1
				PlasticDeformation();
				break;
			case PHJointDesc::ELASTIC_PLASTIC: //PHDeformationType::ELASTIC_PLASTIC 2	
				if(IsYield()){
					PlasticDeformation();	//�Y���ό`
				}else {
					ElasticDeformation();	//�e���ό`
				}
				break;
			default:
				ElasticDeformation();
				break;
		}
		for(int i = 0; i < 3; i++)
			Ainv[i] = 1.0 / (A[i] + dA[i]);
		joint->motorf *= joint->engine->shrinkRate;
	}
}

void PHBallJointMotor::IterateLCP(){
	if(joint->spring == 0.0 && joint->damper == 0.0)
		return;

	Vec3d fnew;
	for(int i = 0; i < 3; i++){
		int j = joint->axisIndex[i];
		fnew[i] = joint->motorf[i] - joint->engine->accelSOR * Ainv[i] * (dA[i] * joint->motorf[i] + b[i] + db[i]
				+ joint->J[0].row(j) * joint->solid[0]->dv + joint->J[1].row(j) * joint->solid[1]->dv);	

		//if (!FPCK_FINITE(fnew[j])) fnew[j] = joint->motorf[i]; //naga 


		
		if(fMaxDt < fnew[i]){
			DSTR<<joint->GetName()<<" : fnew["<<i<<"]="<<fnew[i]<<" fMax Over"<<std::endl;
			fnew[i] = fMaxDt;
		}
		else if(fnew[i] < -fMaxDt){
			DSTR<<joint->GetName()<<" : fnew["<<i<<"]="<<fnew[i]<<" fMax Over"<<std::endl;
			fnew[i] = -fMaxDt;
		}
		
		joint->CompResponse(fnew[i] - joint->motorf[i], i);
		joint->motorf[i] = fnew[i];
	}
	
	//DSTR<<joint->GetName()<<" : fnew"<<fnew<<std::endl;
	//DSTR<<joint->GetName()<<" : dv1 "<<joint->solid[0]->dv<<" : dv2 "<<joint->solid[1]->dv<<std::endl;//naga
	
}
bool PHBallJointMotor::IsYield(){
	//f�̕��ϒl���v�Z
	double dt		= joint->GetScene()->GetTimeStep();
	fNorm = 0;
	for(int i=0; i<50 ;i++){
		if(i==49){
			joint->fs[49] = joint->motorf / dt;
		}else{ 
			joint->fs[i] = joint->fs[i+1];
		}
		fNorm+=joint->fs[i].norm()/50;
	}

	double yieldStressRate = 1.0;
	if((fNorm < joint->yieldStress* yieldStressRate) && yieldFlag){
		//Quaterniond qAll =joint->Xjrel.q * joint->targetPosition.Inv();//3�v�f�S�̂̕ψʍ��̃N�H�[�^�j�I��
		Quaterniond qAll = joint->Xjrel.q;//3�v�f�S�̂̕ψʍ��̃N�H�[�^�j�I��
		Quaterniond qForct;
		Vec3d fAngle = joint->xs[1].w();
		qForct.FromEuler(Vec3d(fAngle.y,fAngle.z,fAngle.x));//�o�l�_���p���̕ψʍ��̃N�H�[�^�j�I��
		//DSTR<<"All: "<<Deg(qAll.Theta())<<"Forct: "<<Deg(qForct.Theta())<<std::endl;
		Quaterniond difQ = qForct.Inv() * qAll;
		joint->SetTargetPosition(difQ);
		yieldFlag = false;
		//DSTR<<"3Element END"<<std::endl;
	}

	if(fNorm > joint->yieldStress){
		yieldFlag = true;
		//DSTR<<"3Element START"<<std::endl;
	}

	return yieldFlag;
}
}
