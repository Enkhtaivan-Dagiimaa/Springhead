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
// PHBallJointDesc

PHBallJointDesc::PHBallJointDesc(){

	spring			= 0.0;
	damper			= 0.0;
	limitSwing[0]	= FLT_MAX;
	limitSwing[1]	= FLT_MAX;
	limitTwist[0]	= FLT_MAX;
	limitTwist[1]	= FLT_MAX;	
	limitDir		= Vec3d(0.0, 0.0, 1.0);
	goal			= Quaterniond(1, 0, 0, 0);
	fMax			= FLT_MAX;
	fMin			= -FLT_MAX;
	desiredVelocity = Quaterniond(1, 0, 0, 0);
}

//----------------------------------------------------------------------------
// PHBallJoint
PHBallJoint::PHBallJoint(const PHBallJointDesc& desc){
	SetDesc(&desc);
	axisIndex[0] = 3;
	axisIndex[1] = 4;
	axisIndex[2] = 5;
	
	// limit��ɋ��邩�ǂ����̃t���O�̏�����
	for(int i=0; i<3; ++i){
		onLimit[i].onLower = false;
		onLimit[i].onUpper = false;
	}
	anyLimit = false;
	

}

void PHBallJoint::AfterSetDesc(){
	if (GetScene()){
		fMinDt = fMin * GetScene()->GetTimeStep();
		fMaxDt = fMax * GetScene()->GetTimeStep();
	}
	PHJointND<3>::AfterSetDesc();
}

void PHBallJoint::UpdateJointState(){
	// Jc.Ez() : Socket�ɑ΂���Plug�̌����Ă������(��currentVector)
	Jc.Ez() = Xjrel.q * Vec3f(0.0, 0.0, 1.0);
	if((onLimit[0].onLower || onLimit[0].onUpper) && (Jc.Ez() != limitDir)){
		Jc.Ex() = cross(Jc.Ez(),(Jc.Ez() - limitDir)).unit();
		Jc.Ey() = cross(Jc.Ez(), Jc.Ex());
		Jcinv   = Jc.trans();
	}
}

void PHBallJoint::SetConstrainedIndex(bool* con){

	// -----bool* con�̈Ӗ�----------------------------------
	// �S�����鎲�ɂ��Ẵt���O
	// con[0]-con[2]:���i�^���ix,y,z�j
	// con[3]-con[5]:��]�^���ix���܂��,y���܂��,z���܂��j
	// true:�S������Afalse:�S�����Ȃ�
	//-------------------------------------------------------

	con[0] = con[1] = con[2] = true;				
	con[3] = con[4] = con[5] = false;
	
	// ���݂�Socket��Plug�Ƃ̊Ԃ̊p�x���v�Z

	nowTheta[0]	= acos(dot(limitDir, Jc.Ez()));			///< Swing�p�̌v�Z	
	
	Quaterniond qSwing;
	Vec3d half =  0.5 * (Vec3d(0.0, 0.0, 1.0) + Jc.Ez());	
	double l = half.norm();
	if (l>1e-20){
		half /= l;
		qSwing.V() = cross(half, Vec3d(0,0,1));
		qSwing.w = sqrt(1-qSwing.V().square());
	}
	else{
		qSwing.V() = Vec3d(1,0,0);
		qSwing.w = 0;
	}

	Quaterniond twistQ = qSwing * Xjrel.q;
	nowTheta[1] = twistQ.Theta();						///< Twist�p�̌v�Z���s���Ă���	
	if (twistQ.z < 0)
		nowTheta[1]  *= -1;								///< Twist��]�������΂��������Ƃ�����̂ł��̑΍�
	
	if(nowTheta[1] < Rad(-180)) nowTheta[1] += Rad(360);
	if(nowTheta[1] > Rad( 180)) nowTheta[1] -= Rad(360);
	
	// ���搧���ɂ������Ă��邩�̔���
	// swing�p�̉��搧���̊m�F
	if (limitSwing[0]!=FLT_MAX && nowTheta[0] < limitSwing[0])
		onLimit[0].onLower = true;
	else if(limitSwing[1]!=FLT_MAX && nowTheta[0] > limitSwing[1])
		onLimit[0].onUpper = true;
	else{
		onLimit[0].onLower = false;
		onLimit[0].onUpper = false;
	}	

	// twist�p�̉��搧���̊m�F
	if(limitTwist[0]!=FLT_MAX && nowTheta[1] < limitTwist[0])
		onLimit[1].onLower = true;
	else if(limitTwist[1]!=FLT_MAX && nowTheta[1] > limitTwist[1])
		onLimit[1].onUpper = true;
	else{
		onLimit[1].onLower = false;
		onLimit[1].onUpper = false;
	}

	// �ǂ��������搧���ɂ������Ă����true
	if((onLimit[0].onUpper || onLimit[0].onLower) ||
	   (onLimit[1].onUpper || onLimit[1].onLower))  
	   anyLimit = true;
	else anyLimit = false;

	// ��̌v�Z�𓥂܂��Ė���A��]���̍S�������̍X�V������
	con[3] = onLimit[0].onUpper || onLimit[0].onLower || spring != 0.0 || damper != 0.0;
	con[4] = spring != 0.0	    || damper != 0.0;
	con[5] = onLimit[1].onUpper || onLimit[1].onLower || spring != 0.0 || damper != 0.0;
}

// ���R�r�A���̊p���x���������W�ϊ�����SwingTwist�p�̎��ԕω����ւ̃��R�r�A���ɂ���
void PHBallJoint::ModifyJacobian(){
	//J[0].wv()����0�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[0].ww()����0�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[1].wv()����1�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	//J[1].ww()����1�̑��x���瑊�Ίp���x�֕ϊ����郄�R�r�A��
	if(anyLimit){
		J[0].wv() = Jcinv * J[0].wv();
		J[0].ww() = Jcinv * J[0].ww();
		J[1].wv() = Jcinv * J[1].wv();
		J[1].ww() = Jcinv * J[1].ww();
	}
}

void PHBallJoint::CompBias(){
	
	double dtinv = 1.0 / GetScene()->GetTimeStep();
	
	db.v() = Xjrel.r * dtinv;		//	���i�덷�̉����̂��߁A���x�Ɍ덷/dt�����Z, Xjrel.r: �\�P�b�g�ɑ΂���v���O�̈ʒu�̃Y��
	db.v() *= engine->velCorrectionRate;
	// �ʒu����̎��̌v�Z
	Quaterniond propQ = goal * Xjrel.q.Inv();	// Xjrel.q�̖ڕWgoal��Xjrel.q�̎��ۂ̊p�x�̍���Quaternion�Ŏ擾
	/*******************************************************************************************************
	����Ȃ��p�x�̍�����]���x�N�g���ɕϊ��DpropV(�c�肳��̘_���ł���q[t])�ɑ΂���db.w()���v�Z���Ă���.
	���R����0[rad]�ŁCpropV[rad]�L�т����ɑ΂��Ă̊p�x�o�l���\�����Ă���ƍl����΂����D
	********************************************************************************************************/
	Vec3d propV = propQ.RotationHalf();			

	// ���搧�����������Ă���ꍇ��prop�̍��W��ϊ����čl���Ȃ��Ƃ����Ȃ��B
	if (anyLimit){
		propV = Jcinv * propV;
	}
	if(mode == MODE_VELOCITY){
		// ���̕ӂ̖ڕW�O���֐��̔����Ƃ��̌v�Z���Ă���ł����񂾂낤������H
		db.w()		= -Jcinv * desiredVelocity.RotationHalf();
	}
	else if(mode == MODE_TRAJECTORY_TRACKING){
		preQd		= qd;
		qd			= goal.RotationHalf();
		preQdDot	= desiredVelocity;
		qdDot		= (qd - preQd) / GetScene()->GetTimeStep();
		qdWDot		= (qdDot - preQdDot) / GetScene()->GetTimeStep();
	}
	// �o�l�_���p�������Ă�����\�z����
	if (spring != 0.0 || damper != 0.0){
		double tmp = 1.0 / (damper + spring * GetScene()->GetTimeStep());
		/*****************************************************************************
		w[t+1] = (A+dA) * ��[t+1] + (b+db)
		��A�̓��C�o�l�_���p���\������̂́CBallJoint�̏ꍇ�͉�]�̎O���ɂ��ĂȂ̂ŁC
		SpatialVector dA.w()��tmp = (D + K��t)^{-1}�����Ă���D
		dtinv������ɂ�����̂�,ppt�ȂǕ\�L�Ƃ͈Ⴂ�CA=JM^{-1}J^T ��t�̂����C
		�v���O�������Ōv�Z���Ă���̂́CA = JM^{-1}J^T�����ŁC���Ƃ���
		(A + dA)��t�����Ă��邽��
		******************************************************************************/
		dA.w() = tmp * dtinv * Vec3d(1.0, 1.0, 1.0);

		/****
		�O���Ǐ]�����db�C�����s��̈����͑��Ή����x�ɑ΂��āC�e���̂ւ̔���p�����Ǝq���̂ւ̍�p�����ɂ�����
		�S���͉�]�����Ȃ̂ŁC�����s��̂����̊������[�����g�������������Ă���
		****/
		if(mode == MODE_TRAJECTORY_TRACKING){
			db.w() = tmp * ( (spring * -(qd - Xjrel.q.Inv().RotationHalf()))
						  + (solid[0]->GetInertia() * qdWDot)
						  - (solid[1]->GetInertia() * qdWDot)
						  + (damper * -qdDot) );
		}
		/**/
		// �ʒu�����b�̒ǉ������C�����Ɠ������ǃ}�C�i�X���t���̂�b���������Ȃ�����ɓ�������������
		else db.w() = -tmp * spring * propV;
	}
	else{
		//dA.w().clear();
		db.w().clear();
	}
	
	// vJc : Jc�ɂ���Ďʑ������S�����W�n���猩��Plug�̊p���x
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

void PHBallJoint::CompError(){
	B.v() = Xjrel.r;
	
	B.w()[0] = (onLimit[0].onLower ? (nowTheta[0] - limitSwing[0]) :
	onLimit[0].onUpper ? (nowTheta[0] - limitSwing[1]) : 0.0);

}

void PHBallJoint::Projection(double& f, int k){
	//���̗͐ς̏�Ԃɑ΂��ĉ��炩�̑����^����D
	//k:con[k]��k�̕���(0�`2:���i�C3�`5:��])�Af�͂���ɑΉ�����̓ɂ̂���
	//�̓� = 0 �ɂ��邱�ƂŊ֐߂̍S�������������D
	//�S��������1��0�ɖ߂鎞��LCP�̃�(�g���N)�𖳗����0�ɂ���w�i���x�E�p���x�j�����߂���悤�ɂ���D
	//< fMaxDt, fMinDt�̏����ł͊֐߂��S���������킯�ł͂Ȃ��̂ŁC�P�Ȃ�㏑�����s���D

	if (k==3){
		if(onLimit[0].onLower)
			f = max(0.0, f);
		else if(onLimit[0].onUpper)
			f = min(0.0, f);
		else if(fMaxDt < f)
			f = fMaxDt;
		else if(f < fMinDt)
			f = fMinDt;
	}

	if (k==5){
		if(onLimit[1].onLower)
			f = max(0.0, f);
		else if(onLimit[1].onUpper)
			f = min(0.0, f);
		else if(fMaxDt < f)
			f = fMaxDt;
		else if(f < fMinDt)
			f = fMinDt;
	}

}

//----------------------------------------------------------------------------
// PHBallJointNode
void PHBallJointNode::CompJointJacobian(){
	PHBallJoint* j = GetJoint();
	//SwingTwist& angle = (SwingTwist&)(j->position);
	//angle.Jacobian(Jst);
	//Matrix3d test = Jst * Jcinv;
	Quaterniond q = j->Xjrel.q;
	for(int i = 0; i < 3; i++)
		J.col(i).SUBVEC(0, 3).clear();
	/*J[0].w() = 2.0 * Vec3d(-q.x, -q.y, -q.z);
	J[1].w() = 2.0 * Vec3d( q.w,  q.z, -q.y);
    J[2].w() = 2.0 * Vec3d(-q.z,  q.w,  q.x);
    J[3].w() = 2.0 * Vec3d( q.y, -q.x,  q.w);*/
	J.col(0).SUBVEC(3, 3) = Vec3d(1.0, 0.0, 0.0);
	J.col(1).SUBVEC(3, 3) = Vec3d(0.0, 1.0, 0.0);
	J.col(2).SUBVEC(3, 3) = Vec3d(0.0, 0.0, 1.0);
	PHTreeNodeND<3>::CompJointJacobian();
}

void PHBallJointNode::CompJointCoriolisAccel(){
	//PHBallJoint* j = GetJoint();
	//cj.v().clear();
	//((SwingTwist&)(j->position)).Coriolis(cj.w(), j->velocity);
	//cj.w.clear();
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
	//j->Xjrel.q��UpdateJointPosition�ōX�V�ς�
}
void PHBallJointNode::CompRelativeVelocity(){
	PHBallJoint* j = GetJoint();
	j->vjrel.v().clear();
	//j->vjrel.w() = ((Quaterniond&)j->position).AngularVelocity((Quaterniond&)j->velocity);
	j->vjrel.w() = j->velocity;
}

void PHBallJointNode::ModifyJacobian(){
	PHBallJoint* j = GetJoint();
	Jq = j->Jcinv;
}

void PHBallJointNode::CompBias(){
	dA.clear();
	db.clear();
}

void PHBallJointNode::Projection(double& f, int k){
	PHBallJoint* j = GetJoint();
	
	for(int i=0; k<2; k++){
		if(j->onLimit[i].onLower)
			f = max(0.0, f);
		if(j->onLimit[i].onUpper)
			f = min(0.0, f);
	}
}

}
