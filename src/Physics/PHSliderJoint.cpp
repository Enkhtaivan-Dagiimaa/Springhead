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
// PHSliderJoint
IF_OBJECT_IMP(PHSliderJoint, PHJoint1D)

void PHSliderJoint::UpdateJointState(){
	position = Xjrel.r.z;
	velocity = vjrel.v.z;
}

void PHSliderJoint::CompBias(){
	double dtinv = 1.0 / scene->GetTimeStep();
	db.v = Xjrel.r * dtinv + vjrel.v;
	db.v.z = 0.0;
	db.w = qjrel.AngularVelocity((qjrel - Quaterniond()) * dtinv) + vjrel.w;
	db *= engine->correctionRate;
	if(mode == MODE_VELOCITY){
		db.v.z = -vel_d;
	}
	else if(spring != 0.0 || damper != 0.0){
		double diff = GetPosition() - origin;
		double tmp = 1.0 / (damper + spring * scene->GetTimeStep());
		dA.v.z = tmp / scene->GetTimeStep();
		db.v.z = spring * (diff) * tmp;
	}
}

/*void PHSliderJoint::CompError(double dt){
	B[0] = rjrel.x;
	B[1] = rjrel.y;
	B[2] = qjrel.x;
	B[3] = qjrel.y;
	B[4] = qjrel.z;
	if(on_upper)
		B[5] = rjrel.z - upper;
	if(on_lower)
		B[5] = rjrel.z - lower;
}*/

void PHSliderJoint::Projection(double& f, int k){
	if(k == 2){
		if(on_lower)
			f = max(0.0, f);
		if(on_upper)
			f = min(0.0, f);
	}
}

/*void PHSliderJoint::ProjectionCorrection(double& F, int k){
	if(k == 5){
		if(on_lower)
			F = max(0.0, F);
		if(on_upper)
			F = min(0.0, F);
	}
}*/

//-----------------------------------------------------------------------------
//OBJECT_IMP(PHSliderJointNode, PHTreeNode1D);

void PHSliderJointNode::CompJointJacobian(){
	J.v = Vec3d(0.0, 0.0, 1.0);
	J.w.clear();
	PHTreeNode1D::CompJointJacobian();
}
void PHSliderJointNode::CompJointCoriolisAccel(){
	cj.clear();
}
void PHSliderJointNode::CompRelativeVelocity(){
	PHJoint1D* j = GetJoint();
	j->vjrel.v = Vec3d(0.0, 0.0, j->velocity);
	j->vjrel.w.clear();
}
void PHSliderJointNode::CompRelativePosition(){
	PHJoint1D* j = GetJoint();
	j->Xjrel.R = Matrix3d::Unit();
	j->Xjrel.r = Vec3d(0.0, 0.0, j->position);
}

}

