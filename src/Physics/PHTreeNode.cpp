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

using namespace std;
using namespace PTM;
namespace Spr{;

//-----------------------------------------------------------------------------
PHTreeNode::PHTreeNode():joint(0){
	dZ.clear();
}

bool PHTreeNode::AddChildObject(ObjectIf* o){
	PHJointIf* j = DCAST(PHJointIf, o);
	if(j){
		joint = j->Cast();
		joint->bArticulated = true;
		joint->solid[1]->treeNode = this;
		return true;
	}
	PHTreeNode* n = DCAST(PHTreeNode, o);
	if(n){
		char name[256];
		if(strcmp(n->GetName(), "") == 0)
			sprintf(name, "%s%d", GetName(), Children().size());
		n->SetScene(GetScene());
		n->SetName(name);
		n->scene = scene;
		n->engine = engine;
		engine->bGearNodeReady = false;
		AddChild(n);
		return true;
	}
	return false;
}
size_t PHTreeNode::NChildObject(){
	return (joint ? 1 : 0) + Children().size();
}
ObjectIf* PHTreeNode::GetChildObject(size_t i){
	if(joint){
		if(i == 0)
			return joint->Cast();
		i--;
	}
	return Children()[i]->Cast();
}

void PHTreeNode::Enable(bool on){
	bEnabled = on;
	if(joint)
		joint->bArticulated = on;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->Enable(on);
}

bool PHTreeNode::Includes(PHTreeNode* node){
	if(node == this)return true;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		if((*it)->Includes(node))
			return true;
	return false;
}

PHTreeNode*	PHTreeNode::FindBySolid(PHSolid* solid){
	if(GetSolid() == solid)return this;
	PHTreeNode* node;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++){
		node = (*it)->FindBySolid(solid);
		if(node)return node;
	}
	return NULL;
}

PHTreeNode*	PHTreeNode::FindByJoint(PHJoint* j){
	if(joint == j)return this;
	PHTreeNode* node;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++){
		node = (*it)->FindByJoint(j);
		if(node)return node;
	}
	return NULL;
}

void PHTreeNode::Prepare(PHScene* s, PHConstraintEngine* e){
	scene = s;
	engine = e;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->Prepare(s, e);
}

void PHTreeNode::ResetGearNode(){
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->ResetGearNode();
}

void PHTreeNode::CompSpatialTransform(){
	Xcp = joint->Xj[1].inv() * joint->Xjrel * joint->Xj[0];
	Xcj = joint->Xj[1].inv() * SpatialTransform(Vec3d(), joint->Xjrel.q);
}

void PHTreeNode::CompCoriolisAccel(){
	PHSolid*	sp = GetParent()->GetSolid();
	PHSolid*	s = GetSolid();
	SpatialVector		&vp = sp->v, &v = s->v, &vjrel = joint->vjrel;
	SpatialTransform	&Xj0 = joint->Xj[0], &Xj1 = joint->Xj[1], &Xjrel = joint->Xjrel;
	SpatialVector	vj = joint->Xj[0] * vp;

	SpatialVector cpj, cjj, ccj;
	cpj.v() = (vp.w() * Xj0.r) * vp.w() - vp.w().square() * Xj0.r;
	cpj.w().clear();

	cjj.v() = (vj.w() * Xjrel.r) * vj.w() - vj.w().square() * Xjrel.r + 2 * vj.w() % vjrel.v();
	cjj.w() = vj.w() % vjrel.w();

	ccj.v() = (v.w() * Xj1.r) * v.w() - v.w().square() * Xj1.r;
	ccj.w().clear();

	CompJointCoriolisAccel();
	c = Xj1.inv() * (
		  Xjrel * SpatialTransform(Vec3d(), Xj0.q) * cpj
		+ SpatialTransform(Vec3d(), Xjrel.q) * cjj
		- SpatialTransform(Vec3d(), Xj1.q) * ccj);
	c += Xcj * cj;
	c *= scene->GetTimeStep();
}

void PHTreeNode::InitArticulatedInertia(){
	double m = GetSolid()->GetMass();
	I.vv() = Matrix3d::Diag(m, m, m);
	I.vw().clear();
	I.wv().clear();
	I.ww() = GetSolid()->GetInertia();
}

void PHTreeNode::CompArticulatedInertia(){
	CompSpatialTransform();
	CompJointJacobian();

	CompCoriolisAccel();
	
	//�q�m�[�h��Ia���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Ia��ςݏグ��
	//���M�A�̕���A���ɂ����ăM�A�g���C���̐擪�m�[�h�i�ł�begin���ɂ���j���Ō�ɌĂԕK�v�����邽��
	//  reverse_iterator��p����
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->InitArticulatedInertia();
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->CompArticulatedInertia();

	Ic = I * c;

	//�e�m�[�h��Ia��ς�
	AccumulateInertia();
}

void PHTreeNode::InitArticulatedBiasForce(){
	// PHSolidInfo��dv�������悤�Ȏ��Ȃ̂ŋ��L�������ق�������
	PHSolid* s = GetSolid();
	if(s->IsDynamical()){
		Z.v() = -1.0 * s->f.v();
		Z.w() = -1.0 * s->f.w() + s->v.w() % (s->GetInertia() * s->v.w());
		Z *= scene->GetTimeStep();
	}
	else Z.clear();
}

void PHTreeNode::CompArticulatedBiasForce(){
	//�q�m�[�h��Za���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Za��ςݏグ��
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->InitArticulatedBiasForce();
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->CompArticulatedBiasForce();

	ZplusIc = Z + Ic;

	//�e�m�[�h��Ia��ς�
	AccumulateBiasForce();
	
}

void PHTreeNode::ResetCorrectionVarsRecursive(){
	ResetCorrectionVars();
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->ResetCorrectionVarsRecursive();
}
void PHTreeNode::ResetCorrectionVars(){
	Z.clear();
}

void PHTreeNode::CompResponse(const SpatialVector& df, bool bUpdate, bool bImpulse){
	dZ = -1.0 * df;
	CompBiasForceDiff(bUpdate, bImpulse);
}

void PHTreeNode::SetupLCP(){
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->SetupLCP();
}
void PHTreeNode::IterateLCP(){
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->IterateLCP();
}

void PHTreeNode::UpdateVelocity(double dt){
	PHSolid* sp = GetParent()->GetSolid();
	PHSolid* s  = GetSolid();
	UpdateJointVelocity(dt);
	CompRelativeVelocity();
	(Vec6d&)(s->v) = Xcp * sp->v + Xcj * joint->vjrel;
	//s->oldVel	 = s->GetVelocity();
	//s->oldAngVel = s->GetAngularVelocity();
	s->SetVelocity       (s->GetOrientation() * s->v.v());
	s->SetAngularVelocity(s->GetOrientation() * s->v.w());
	
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdateVelocity(dt);
}
void PHTreeNode::UpdatePosition(double dt){
	PHSolid* sp = GetParent()->GetSolid();
	PHSolid* s  = GetSolid();
	UpdateJointPosition(dt);
	CompRelativePosition();
	CompSpatialTransform();
	SpatialTransform Xp(sp->GetCenterPosition(), sp->GetOrientation());
	SpatialTransform Xc = Xcp * Xp;
	Xc.q.unitize();
	s->SetCenterPosition(Xc.r);
	s->SetOrientation(Xc.q);
	s->SetUpdated(true);
	
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdatePosition(dt);
}

//-----------------------------------------------------------------------------
bool PHRootNode::AddChildObject(ObjectIf* o){
	if(PHTreeNode::AddChildObject(o))
		return true;
	PHSolidIf* s = DCAST(PHSolidIf, o);
	if(s){
		solid = s->Cast();
		solid->treeNode = this;
		return true;
	}
	return false;
}
size_t PHRootNode::NChildObject(){
	return (solid ? 1 : 0) + Children().size();
}
ObjectIf* PHRootNode::GetChildObject(size_t i){
	if(solid){
		if(i == 0)
			return solid->Cast();
		i--;
	}
	return Children()[i]->Cast();
}
void PHRootNode::SetupABA(){
	if(!bEnabled)return;
	InitArticulatedInertia();
	CompArticulatedInertia();
	InitArticulatedBiasForce();
	CompArticulatedBiasForce();
	CompAccel();
}

void PHRootNode::SetupCorrectionABA(){
	if(!bEnabled)return;
	ResetCorrectionVarsRecursive();
}

void PHRootNode::CompArticulatedInertia(){
	//�q�m�[�h��Ia���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Ia��ςݏグ��
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->InitArticulatedInertia();
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->CompArticulatedInertia();

	//�t�s����v�Z
	(Matrix6d&)Iinv = I.inv();
}

void PHRootNode::CompArticulatedBiasForce(){
	//�q�m�[�h��Za���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Za��ςݏグ��
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->InitArticulatedBiasForce();
	for(container_t::reverse_iterator it = Children().rbegin(); it != Children().rend(); it++)
		(*it)->CompArticulatedBiasForce();
}

void PHRootNode::CompAccel(){
	if(GetSolid()->IsDynamical()){
		a = -1 * (Iinv * Z);
	}else{
		a.clear();
	}
	solid->dv = a;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccel();
}

void PHRootNode::CompBiasForceDiff(bool bUpdate, bool bImpulse){
	if(bUpdate)
		Z += dZ;
	if(GetSolid()->IsDynamical()){
		da = -1 * (Iinv * dZ);
		if(bUpdate){
			if(bImpulse)
				 solid->dV += da;
			else{
				solid->dv += da;
			}
		}
	}
	else da.clear();
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccelDiff(bUpdate, bImpulse);
}

void PHRootNode::UpdateVelocity(double dt){
	if(!bEnabled)return;
	solid->UpdateVelocity(dt);
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdateVelocity(dt);
}
void PHRootNode::UpdatePosition(double dt){
	if(!bEnabled)return;
	solid->UpdatePosition(dt);
	solid->SetUpdated(true);
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdatePosition(dt);
}

//-----------------------------------------------------------------------------

template<int NDOF>
PHTreeNodeND<NDOF>::PHTreeNodeND(){
	dtau.clear();
	dZ.clear();
	JtrdZ.clear();
	f.clear();
	accel.clear();
	vel.clear();
	gear = NULL;
	gearNode = NULL;
	parentND = NULL;
}

template<int NDOF>
void PHTreeNodeND<NDOF>::AddGear(PHGear* gear, PHTreeNodeND<NDOF>* child){
	//child���q�m�[�h�̏ꍇ�ƁC�Z��m�[�h�i�e�������j�̏ꍇ������
	//���̃m�[�h���X�ɏ�̃m�[�h�ƘA�����Ă��Ȃ��ꍇ�͂��̃m�[�h���g�b�v�ɂȂ�
	if(!gearNode)
		gearNode = this;
	child->gearNode = gearNode;
	child->gear = gear;
	if(child->GetParent() == this)
		 child->parentND = this;
	if(gearNode == this){
		gearChildren.push_back(child);
		gearChildren.insert(gearChildren.end(), child->gearChildren.begin(), child->gearChildren.end());
		child->gearChildren.clear();
	}
	gear->bArticulated = true;
}

template<int NDOF>
void PHTreeNodeND<NDOF>::ResetGearNode(){
	gear = NULL;
	gearNode = NULL;
	parentND = NULL;
	PHTreeNode::ResetGearNode();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::InitArticulatedInertia(){
	PHTreeNode::InitArticulatedInertia();
	if(gearNode == this){
		sumXIX.clear();
		sumXtrIJ.clear();
		sumXtrIJ_sumJIJinv.clear();
		sumXtrZplusIc.clear();
		sumJIJ.clear();
		sumJIJinv.clear();
		sumJtrZplusIc.clear();
		sumtorque.clear();
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompCoriolisAccel(){
	PHTreeNode::CompCoriolisAccel();
	//����M�A�A�����Ă���ꍇ�͏�i�̃R���I�����𑫂�
	if(gearNode && gearNode != this && GetParent() != gearNode->GetParent())
		c += Xcp * GetParent()->c;
}

template<int NDOF>
void PHTreeNodeND<NDOF>::AccumulateInertia(){
	Xtr_Mat_X(XIX, Xcg, I);

	IJ = I * J;
	for(int i = 0; i < NDOF; i++)
		(SpatialVector&)XtrIJ.col(i) = Xcg.trans() * (const SpatialVector&)IJ.col(i);
	JIJ = J.trans() * IJ;
	JIJinv = JIJ.inv();
	J_JIJinv = J * JIJinv;
	IJ_JIJinv = I * J_JIJinv;
	XtrIJ_JIJinv = XtrIJ * JIJinv;
	(Matrix6d&)XtrIJ_JIJinv_Jtr = XtrIJ_JIJinv * J.trans();
	(Matrix6d&)XtrIJ_JIJinv_JtrIX = XtrIJ_JIJinv * XtrIJ.trans();
	
	if(gearNode){
		gearNode->sumXIX   += XIX;
		gearNode->sumXtrIJ += XtrIJ;
		gearNode->sumJIJ   += JIJ;
	}
	if(gearNode == this){
		sumJIJinv = sumJIJ.inv();
		sumXtrIJ_sumJIJinv = sumXtrIJ * sumJIJinv;
		GetParent()->I += (sumXIX - sumXtrIJ * sumXtrIJ_sumJIJinv.trans());
	}
	if(!gearNode)
		GetParent()->I += (XIX - XtrIJ_JIJinv_JtrIX);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::AccumulateBiasForce(){
	JtrZplusIc = J.trans() * ZplusIc;
	XtrZplusIc = Xcp.trans() * ZplusIc;

  	if(gearNode){
		gearNode->sumXtrZplusIc += XtrZplusIc;
		gearNode->sumtorque     += GetJoint()->GetTorqueND();
		gearNode->sumJtrZplusIc += JtrZplusIc;
	}
	if(gearNode == this)
		GetParent()->Z +=
			sumXtrZplusIc + sumXtrIJ_sumJIJinv * (sumtorque * scene->GetTimeStep() - sumJtrZplusIc);
	if(!gearNode)
		GetParent()->Z +=
			XtrZplusIc + XtrIJ_JIJinv * (GetJoint()->GetTorqueND() * scene->GetTimeStep() - JtrZplusIc);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompSpatialTransform(){
	PHTreeNode::CompSpatialTransform();
	// ����M�A�A���̏ꍇ
	if(gearNode && gearNode != this && GetParent() != gearNode->GetParent())
		 Xcg = Xcp * parentND->Xcg;
	else Xcg = Xcp;
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompJointJacobian(){
	// �h���N���X���v�Z�������R�r�A���ɍ��W�ϊ���������
	for(int i = 0; i < NDOF; i++)
		(SpatialVector&)J.col(i) = Xcj * (const SpatialVector&)J.col(i);

	// �M�A�A�����Ă���ꍇ��(��i�m�[�h�̃��R�r�A���{�M�A��*�����̃��R�r�A��)
	if(gearNode && gearNode != this){
		// ����M�A�A��
		if(GetParent() != gearNode->GetParent()){
			for(int i = 0; i < NDOF; i++)
				J.col(i) = Xcp * (const SpatialVector&)parentND->J.col(i) + gear->GetRatio() * J.col(i);
		}
		// ����M�A�A��
		else J = gear->GetRatio() * J;
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompAccel(){
	if(gearNode){
		if(gearNode == this)
			 accel = sumJIJinv * (GetJoint()->GetTorqueND() * scene->GetTimeStep() - sumXtrIJ.trans() * GetParent()->a - sumJtrZplusIc);
		else if(GetParent() != gearNode->GetParent())
			 accel = gear->GetRatio() * parentND->accel;
		else accel = gear->GetRatio() * gearNode->accel;
		(Vec6d&)a = Xcg * gearNode->GetParent()->a + c + J * gearNode->accel;
	}
	else{
		accel = JIJinv * (GetJoint()->GetTorqueND() * scene->GetTimeStep() - XtrIJ.trans() * GetParent()->a - JtrZplusIc);
		(Vec6d&)a = Xcp * GetParent()->a + c + J * accel;
	}

	GetSolid()->dv = a;

	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccel();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompAccelDiff(bool bUpdate, bool bImpulse){
	if(gearNode){
		if(gearNode == this){
			 daccel = sumJIJinv * (dtau - sumXtrIJ.trans() * GetParent()->da - JtrdZ);
		}
		else if(GetParent() != gearNode->GetParent()){
			 daccel = gear->GetRatio() * parentND->daccel;
		}
		else{
			daccel = gear->GetRatio() * gearNode->daccel;
		}
		(Vec6d&)da = Xcg * gearNode->GetParent()->da + J * gearNode->daccel;
	}
	else{
		daccel = JIJinv * (dtau - XtrIJ.trans() * GetParent()->da - JtrdZ);
		(Vec6d&)da = Xcp * GetParent()->da + J * daccel;
	}

	dZ.clear(); // ����̌Ăяo���̂��߂ɂ����ŃN���A
	dtau.clear();
	JtrdZ.clear();
	
	if(bUpdate){
		if(bImpulse){
			vel += daccel;
			(Vec6d&)GetSolid()->dV = Xcp * GetParent()->GetSolid()->dV + J * vel;
		}
		else{
			accel += daccel;
			(Vec6d&)GetSolid()->dv = Xcp * GetParent()->GetSolid()->dv + J * accel;
		}
	}
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccelDiff(bUpdate, bImpulse);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::ResetCorrectionVars(){
	vel.clear();
	PHTreeNode::ResetCorrectionVars();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompBiasForceDiff(bool bUpdate, bool bImpulse){
	if(bUpdate)
		Z += dZ;
	if(gearNode){
		gearNode->JtrdZ = J.trans() * dZ;
		(Vec6d&)gearNode->GetParent()->dZ = Xcg.trans() * dZ - gearNode->sumXtrIJ_sumJIJinv * gearNode->JtrdZ;
		gearNode->GetParent()->CompBiasForceDiff(bUpdate, bImpulse);
	}
	else{
		JtrdZ = J.trans() * dZ;
		(Vec6d&)GetParent()->dZ = Xcp.trans() * dZ - XtrIJ_JIJinv * JtrdZ;
		GetParent()->CompBiasForceDiff(bUpdate, bImpulse);
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::UpdateJointVelocity(double dt){
	if(gearNode && gearNode != this){
		if(GetParent() != gearNode->GetParent())
			 GetJoint()->velocity = gear->GetRatio() * parentND->GetJoint()->velocity;
		else GetJoint()->velocity = gear->GetRatio() * gearNode->GetJoint()->velocity;
	}
	else GetJoint()->velocity += accel;
}
template<int NDOF>
void PHTreeNodeND<NDOF>::UpdateJointPosition(double dt){
	if(gearNode && gearNode != this){
		if(GetParent() != gearNode->GetParent())
			 GetJoint()->position = gear->GetRatio() * parentND->GetJoint()->position;
		else GetJoint()->position = gear->GetRatio() * gearNode->GetJoint()->position;
	}
	else{
		GetJoint()->position += GetJoint()->velocity * dt + vel;		
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompResponse(const PTM::TVector<NDOF, double>& _dtau, bool bUpdate){
	if(gearNode){
		if(gearNode == this)
			 dtau = _dtau;
		else gearNode->dtau = gear->GetRatio() * _dtau;
		(Vec6d&)gearNode->GetParent()->dZ = gearNode->sumXtrIJ_sumJIJinv * gearNode->dtau;
		gearNode->GetParent()->CompBiasForceDiff(bUpdate, false);
	}
	else{
		dtau = _dtau;
		(Vec6d&)GetParent()->dZ = XtrIJ_JIJinv * dtau;
		GetParent()->CompBiasForceDiff(bUpdate, false);
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompResponseMatrix(){
	const double eps = 1.0e-6;
	VecNd Jrow;
	for(int i = 0; i < NDOF; i++){
		Jrow = Jq.row(i);
		CompResponse(Jrow, false);
		A[i] = max(eps, Jrow * daccel);
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::ModifyJacobian(){
	//�֐ߑ��x�̐������ɍS������ꍇ�͒P�ʍs��
	init_unitize(Jq);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::SetupLCP(){
	if(!bEnabled)
		return;
	// �M�A�A�����Ă���ꍇ�C�o�l�_���p�̌��ʂ̓M�A�g���C���擪�m�[�h�ɏW�񂳂���
	if(gearNode && gearNode != this)
		return;

	PHJointND<NDOF>* j = GetJoint();
	bool con, constrAtAll = false;
	for(int i = 0; i < NDOF; i++){
		con = j->constr[j->axisIndex[i]];
		if(con && constr[i])			// �p�����čS�������ꍇ
			 f[i] *= engine->shrinkRate;
		else f[i] = 0.0;					// �V�K�ɍS������� or �S������Ȃ�
		constr[i] = con;
		constrAtAll |= con;
	}
	ModifyJacobian();
	if(constrAtAll){
		// LCP��b�x�N�g��
		b = Jq * GetJoint()->velocity;
		dA.clear();
		db.clear();
		CompBias();	// �ڕW���C�o�l�_���p�ɂ��␳�����v�Z
		b += db;
		
		// LCP��A�s��̑Ίp�������v�Z
		CompResponseMatrix();
		//DSTR << "A: " << A << endl;
		for(int i = 0; i < NDOF; i++)
			Ainv[i] = 1.0 / (A[i] + dA[i]);

		// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
		CompResponse(f, true);
	}
	PHTreeNode::SetupLCP();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::IterateLCP(){
	if(!bEnabled)
		return;
	if(gearNode && gearNode != this)
		return;

	VecNd fnew, Jrow;
	double df;
	for(int i = 0; i < NDOF; i++){
		if(!constr[i])continue;
		fnew[i] = f[i] - Ainv[i] * (dA[i] * f[i] + b[i] + Jq.row(i) * accel);
		Projection(fnew[i], i);
		df = fnew[i] - f[i];
		Jrow = Jq.row(i);
		CompResponse(Jrow * df, true);
		f[i] = fnew[i];
	}
	PHTreeNode::IterateLCP();
}

//-----------------------------------------------------------------------------
PHTreeNode1D::PHTreeNode1D(){
}

void PHTreeNode1D::CompBias(){
	PHJoint1D* j = GetJoint();
	
	double diff;
	double dt = scene->GetTimeStep(), dtinv = 1.0 / dt;
	if(j->mode == PHJointDesc::MODE_VELOCITY){
		db[0] = -j->GetDesiredVelocity();
		return;
	}

	if(j->onLower || j->onUpper){
		// ���Ή�
		//return;
	}

	double D = j->damper, K = j->spring, origin = j->origin;
	PHJoint1D* jchild;
	// �M�A�g���C���̐擪�̏ꍇ�C�A�����Ă���֐߂̃o�l�_���p�W���𑫂����킹��
	if(gearNode){
		double Di, Ki, ratio;
		origin = K * j->origin;
		for(int i = 0; i < (int)gearChildren.size(); i++){
			jchild = DCAST(PHJoint1D, gearChildren[i]->GetJoint());
			if(!jchild)continue;	// ���R�x�̈قȂ�֐߂Ƃ̘A���F�N���蓾�Ȃ��͂�
			ratio = gearChildren[i]->gear->GetRatio();
			Di = ratio * ratio * jchild->damper;
			Ki = ratio * ratio * jchild->spring;
			D += Di;	// �o�l�_���p�W���̓M�A��̎���{
			K += Ki;
			origin += Ki * jchild->origin;		// �o�l���_�͌W���ɂ��d�S
		}
		if(K > 0.0)
			origin /= K;
	}
	if(K != 0.0 || D != 0.0){
		diff = j->GetPosition() - origin;
		// ���܂ɕs���萫�ɂ�薳����ɒ���t�����Ƃ�����A����while�Ŗ������[�v���Ă��܂�
		if(abs(diff) > 1.0e3)
			diff = 0.0;
		//while(diff >  M_PI) diff -= 2 * M_PI;
		//while(diff < -M_PI) diff += 2 * M_PI;
		double tmp = 1.0 / (D + K * dt);
		dA[0] = tmp * dtinv;
		db[0] = K * (diff) * tmp;
	}
}

void PHTreeNode1D::Projection(double& _f, int k){
	PHJoint1D* j = GetJoint();
	if(j->onLower)
		_f = max(0.0, _f);
	if(j->onUpper)
		_f = min(0.0, _f);
}

//-----------------------------------------------------------------------------
// explicit instantiation
template class PHTreeNodeND<1>;
template class PHTreeNodeND<2>;
template class PHTreeNodeND<3>;
//template class PHTreeNodeND<4>;
	
}
