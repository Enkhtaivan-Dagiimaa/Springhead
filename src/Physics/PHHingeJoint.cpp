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
IF_OBJECT_IMP(PHHingeJoint, PHJoint1D)

PHHingeJoint::PHHingeJoint(const PHHingeJointDesc& desc){
	SetDesc(&desc);
	axisIndex[0] = 5;
}

bool PHHingeJoint::GetDesc(void* desc){
	PHJoint1D::GetDesc(desc);
	return true;
}

void PHHingeJoint::SetDesc(const void* desc){
	PHJoint1D::SetDesc(desc);
	
	const PHHingeJointDesc& descHinge = *(const PHHingeJointDesc*)desc;

	SetMotorTorque(descHinge.torque);
}

void PHHingeJoint::UpdateJointState(){
	//軸方向の拘束は合致しているものと仮定して角度を見る
	position[0] = Xjrel.q.Theta();
	if(Xjrel.q.Axis().Z() < 0.0)
		position = -position;
	velocity[0] = vjrel.w().z;
}

void PHHingeJoint::CompBias(){
//	DSTR << "spring " << spring << " goal " << origin*180/M_PI << endl;
	double dtinv = 1.0 / scene->GetTimeStep();
	if (engine->numIterCorrection==0){	//	Correction を速度LCPで行う場合
		//	次のステップでの位置の誤差の予測値が0になるような速度を設定
		//	dv * dt = x + v*dt
		db.v() = Xjrel.r * dtinv + vjrel.v();
		//	角度の誤差を0にするような回転角度を求める。
		Quaterniond qarc;
		qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // 軸を一致させるような回転
		db.w() = -(qarc.Theta() * dtinv) * qarc.Axis() + vjrel.w();
		db *= engine->velCorrectionRate;
	}

	if(mode == MODE_VELOCITY){
		db.w().z = -vel_d;
	}
	else if(onLower || onUpper){
		dA.w()[2] = 0;
		db.w()[2] = (position[0] - origin) * dtinv * engine->velCorrectionRate;
	}
	else if(spring != 0.0 || damper != 0.0){
		double diff;
		diff = GetPosition() - origin;
		// 不連続なトルク変化を避けるため (ゼンマイのようにいくらでも巻けるように削除)。 07/07/26
		// while(diff >  M_PI) diff -= 2 * M_PI;
		// while(diff < -M_PI) diff += 2 * M_PI;
		double tmp = 1.0 / (damper + spring * scene->GetTimeStep());
		dA.w().z = tmp * dtinv;
		db.w().z = spring * (diff) * tmp;
	}

}

void PHHingeJoint::CompError(){
	B.v() = Xjrel.r;

	//	B.w() = Xjrel.q.V();
	//	B.w().z = 0.0;
	Quaterniond qarc;
	qarc.RotationArc(Xjrel.q * Vec3d(0,0,1), Vec3d(0,0,1)); // 軸を一致させるような回転
	B.w() = -qarc.Theta() * qarc.Axis();
}

void PHHingeJoint::Projection(double& f, int k){
	//拘束条件が1→0に戻る時にLCPのλ(トルク)を無理やり0にしてw（速度・角速度）を求められるようにする関数
	//k:con[k]のkの部分、fは力λのこと
	if(k == 5){
		if(onLower)
			f = max(0.0, f);
		else if(onUpper)
			f = min(0.0, f);
		if(fMax < f)
			f = max(0.0, fMax);
		if(f < fMin)
			f = min(0.0, fMin);
	}
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
	j->Xjrel.r.clear();
}

}