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

using namespace std;
using namespace PTM;
namespace Spr{;

//-----------------------------------------------------------------------------
IF_OBJECT_IMP_ABST(PHTreeNode, SceneObject);

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
		AddChild(n);
		n->scene = scene;
		n->engine = engine;
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

/*int PHTreeNode::GetTotalDof(){
	int dof = GetDof();
	for(int i = 0; i < Children().size(); i++)
		dof += Children()[i]->GetTotalDof();
	return dof;
}*/

/*int PHTreeNode::AssignID(int _id, std::vector<PHTreeNode*>& table){
	id = _id++;
	table[id] = this;
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		_id = AssignID(_id, table);
	return _id;
}*/

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
	InitArticulatedInertia();
	CompSpatialTransform();
	CompJointJacobian();

	CompCoriolisAccel();
	
	//�q�m�[�h��Ia���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Ia��ςݏグ��
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
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
	InitArticulatedBiasForce();

	//�q�m�[�h��Za���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Za��ςݏグ��
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompArticulatedBiasForce();

	ZplusIc = Z + Ic;

	//�e�m�[�h��Ia��ς�
	AccumulateBiasForce();
	
}

void PHTreeNode::CompResponse(const SpatialVector& dF, bool bUpdate){
	dZ = -1.0 * dF;
	CompBiasForceDiff(bUpdate);
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
IF_OBJECT_IMP(PHRootNode, PHTreeNode);

bool PHRootNode::AddChildObject(ObjectIf* o){
	if(PHTreeNode::AddChildObject(o))
		return true;
	PHSolidIf* s = DCAST(PHSolidIf, o);
	if(s){
		solid = s->Cast();
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
	CompArticulatedInertia();
	CompArticulatedBiasForce();
	CompAccel();
}

void PHRootNode::CompArticulatedInertia(){
	InitArticulatedInertia();

	//�q�m�[�h��Ia���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Ia��ςݏグ��
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompArticulatedInertia();

	//�t�s����v�Z
	(Matrix6d&)Iinv = I.inv();
}

void PHRootNode::CompArticulatedBiasForce(){
	InitArticulatedBiasForce();

	//�q�m�[�h��Za���v�Z������D
	//�q�m�[�h�B�͐e�m�[�h�i�܂肱�̃m�[�h�j�Ɏ�����Za��ςݏグ��
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
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

void PHRootNode::CompBiasForceDiff(bool bUpdate){
	if(bUpdate)
		Z += dZ;
	if(GetSolid()->IsDynamical()){
		da = -1 * (Iinv * dZ);
		if(bUpdate)
			solid->dv += da;
	}
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccelDiff(bUpdate);
}
void PHRootNode::UpdateVelocity(double dt){
	solid->UpdateVelocity(dt);
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdateVelocity(dt);
}
void PHRootNode::UpdatePosition(double dt){
	solid->UpdatePosition(dt);
	solid->SetUpdated(true);
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->UpdatePosition(dt);
}

template<int NDOF>
PHTreeNodeND<NDOF>::PHTreeNodeND(){
	dtau.clear();
	f.clear();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::AccumulateInertia(){
	Xtr_Mat_X(XIX, Xcp, I);

	for(int i = 0; i < NDOF; i++)
		IJ[i] = I * J[i];
	for(int i = 0; i < NDOF; i++)
		XtrIJ[i] = Xcp.trans() * IJ[i];
	for(int i = 0; i < NDOF; i++)for(int j = 0; j < NDOF; j++)
        JIJ[i][j] = J[i] * IJ[j];
	JIJinv = JIJ.inv();

	// J_JIJinv
	for(int i = 0; i < NDOF; i++){
		J_JIJinv[i].clear();
		for(int j = 0; j < NDOF; j++){
			J_JIJinv[i] += J[j] * JIJinv[j][i];
		}
	}
	// IJ_JIJinv
	for(int i = 0; i < NDOF; i++){
		IJ_JIJinv[i].clear();
		for(int j = 0; j < NDOF; j++){
			IJ_JIJinv[i] += IJ[j] * JIJinv[j][i];
		}
	}
	// XtrIJ_JIJinv
	for(int i = 0; i < NDOF; i++){
		XtrIJ_JIJinv[i].clear();
		for(int j = 0; j < NDOF; j++){
			XtrIJ_JIJinv[i] += XtrIJ[j] * JIJinv[j][i];
		}
	}
	// XtrIJ_JIJinv_Jtr
	XtrIJ_JIJinv_Jtr.clear();
	for(int i = 0; i < NDOF; i++)
		XtrIJ_JIJinv_Jtr += VVtr(XtrIJ_JIJinv[i], J[i]);
	// XtrIJ_JIJinv_JtrIX
	XtrIJ_JIJinv_JtrIX.clear();
	for(int i = 0; i < NDOF; i++)
		XtrIJ_JIJinv_JtrIX += VVtr(XtrIJ_JIJinv[i], XtrIJ[i]);
	
	GetParent()->I += (XIX - XtrIJ_JIJinv_JtrIX);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::AccumulateBiasForce(){
	for(int i = 0; i < NDOF; i++)
		JtrZplusIc[i] = J[i] * ZplusIc;

	TVector<NDOF, double> tmp = GetJoint()->torque * scene->GetTimeStep() - JtrZplusIc;
	SpatialVector tmp2;
	for(int i = 0; i < NDOF; i++)
		tmp2 += IJ_JIJinv[i] * tmp[i];
	GetParent()->Z += Xcp.trans() * (ZplusIc + tmp2);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompJointJacobian(){
	// �h���N���X���v�Z�������R�r�A���ɍ��W�ϊ���������
	for(int i = 0; i < NDOF; i++)
		J[i] = Xcj * J[i];
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompAccel(){
	ap = Xcp * GetParent()->a;
	//�����x���v�Z
	TVector<NDOF, double> IJ_ap;
	for(int i = 0; i < NDOF; i++)
		IJ_ap[i] = IJ[i] * ap;
	accel = JIJinv * (GetJoint()->torque * scene->GetTimeStep() - IJ_ap - JtrZplusIc);

	//�d�S����̉����x(�q�m�[�h�̐ϕ��Ŏg�p����)
	SpatialVector J_accel;
	for(int i = 0; i < NDOF; i++)
		J_accel += J[i] * accel[i];
	a = ap + c + J_accel;
	GetSolid()->dv = a;

	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccel();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompBiasForceDiff(bool bUpdate){
	if(bUpdate)
		Z += dZ;
	GetParent()->dZ = Xcp.trans() * dZ - XtrIJ_JIJinv_Jtr * dZ;
	GetParent()->CompBiasForceDiff(bUpdate);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompAccelDiff(bool bUpdate){
	SpatialVector dap = Xcp * GetParent()->da;
	daccel = JIJinv * dtau;
	for(int i = 0; i < NDOF; i++)
		daccel[i] -= IJ_JIJinv[i] * dap + J_JIJinv[i] * dZ;
	dZ.clear(); // ����̌Ăяo���̂��߂ɂ����ŃN���A
	dtau.clear();
    da = dap;
	for(int i = 0; i < NDOF; i++)
		da += J[i] * daccel[i];
	if(bUpdate){
		accel += daccel;
		a += da;
		GetSolid()->dv = a;
	}
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccelDiff(bUpdate);
}
template<int NDOF>
void PHTreeNodeND<NDOF>::UpdateJointVelocity(double dt){
	GetJoint()->velocity += accel;
}
template<int NDOF>
void PHTreeNodeND<NDOF>::UpdateJointPosition(double dt){
	GetJoint()->position += GetJoint()->velocity * dt;
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompResponse(const TVector<NDOF, double>& _dtau, bool bUpdate){
	dtau = _dtau;
	SpatialVector tmp;
	for(int i = 0; i < NDOF; i++)
		tmp += IJ_JIJinv[i] * dtau[i];
	GetParent()->dZ = Xcp.trans() * tmp;
	GetParent()->CompBiasForceDiff(bUpdate);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::CompResponseMatrix(){
	const double eps = 1.0e-6;
	for(int i = 0; i < NDOF; i++){
		CompResponse(Jq.row(i), false);
		A[i] = max(eps, Jq.row(i) * daccel);
	}
}

template<int NDOF>
void PHTreeNodeND<NDOF>::ModifyJacobian(){
	//�֐ߑ��x�̐������ɍS������ꍇ�͒P�ʍs��
	init_unitize(Jq);
}

template<int NDOF>
void PHTreeNodeND<NDOF>::SetupLCP(){
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
		for(int i = 0; i < NDOF; i++)
			Ainv[i] = 1.0 / (A[i] + dA[i]);

		// �S���͏����l�ɂ�鑬�x�ω��ʂ��v�Z
		CompResponse(f);
	}
	PHTreeNode::SetupLCP();
}

template<int NDOF>
void PHTreeNodeND<NDOF>::IterateLCP(){
	TVector<NDOF, double> fnew;
	double df;
	for(int i = 0; i < NDOF; i++){
		if(!constr[i])continue;
		fnew[i] = f[i] - Ainv[i] * (dA[i] * f[i] + b[i] + Jq.row(i) * accel);
		Projection(fnew[i], i);
		df = fnew[i] - f[i];
		CompResponse(Jq.row(i) * df);
		f[i] = fnew[i];
	}
	PHTreeNode::IterateLCP();
}

//-----------------------------------------------------------------------------
OBJECT_IMP_ABST(PHTreeNode1D, PHTreeNode);

PHTreeNode1D::PHTreeNode1D(){
	gear = NULL;
	gearNode = NULL;
	parent1D = NULL;
	sumJIJ = sumJIJinv = 0.0;
	sumJtrZplusIc = sumJtrdZ = 0.0;
	sumtorque = 0.0;
}

void PHTreeNode1D::AddGear(PHGear* gear, PHTreeNode1D* child){
	//���̃m�[�h���X�ɏ�̃m�[�h�ƘA�����Ă��Ȃ��ꍇ�͂��̃m�[�h���g�b�v�ɂȂ�
	if(!gearNode)
		gearNode = this;
	child->gearNode = gearNode;
	child->gear = gear;
	child->parent1D = this;
	gear->bArticulated = true;
}

void PHTreeNode1D::InitArticulatedInertia(){
	PHTreeNode::InitArticulatedInertia();
	if(gearNode == this){
		sumXIX.clear();
		sumXtrIJ.clear();
		sumXtrIJ_sumJIJinv.clear();
		sumXtrZplusIc.clear();
		sumJIJ = sumJIJinv = 0.0;
		sumJtrZplusIc = 0.0;
		sumtorque = 0.0;
	}
}

void PHTreeNode1D::CompCoriolisAccel(){
	PHTreeNode::CompCoriolisAccel();
	//�M�A�A�����Ă���ꍇ�͏�i�̃R���I�����𑫂�
	if(gearNode && gearNode != this)
		c += Xcp * GetParent()->c;
}

void PHTreeNode1D::AccumulateInertia(){
	Xtr_Mat_X(XIX, Xcg, I);
	IJ[0]				= I * J[0];
	XtrIJ[0]			= Xcg.trans() * IJ[0];
	JIJ[0][0]			= J[0] * IJ[0];
	JIJinv[0][0]		= 1.0 / JIJ[0][0];
	J_JIJinv[0]			= J[0] * JIJinv[0][0];
	IJ_JIJinv[0]		= IJ[0] * JIJinv[0][0];
	XtrIJ_JIJinv[0]		= XtrIJ[0] * JIJinv[0][0];
	XtrIJ_JIJinv_Jtr	= VVtr(XtrIJ_JIJinv[0], J[0]);
	XtrIJ_JIJinv_JtrIX	= VVtr(XtrIJ_JIJinv[0], XtrIJ[0]);
	
	if(gearNode){
		gearNode->sumXIX   += XIX;
		gearNode->sumXtrIJ += XtrIJ[0];
		gearNode->sumJIJ   += JIJ[0][0];
	}
	if(gearNode == this){
		sumJIJinv = 1.0 / sumJIJ;
		sumXtrIJ_sumJIJinv = sumXtrIJ * sumJIJinv;
		GetParent()->I += (sumXIX - VVtr(sumXtrIJ, sumXtrIJ_sumJIJinv));
	}
	if(!gearNode)
		GetParent()->I += (XIX - XtrIJ_JIJinv_JtrIX);
}

void PHTreeNode1D::AccumulateBiasForce(){
	JtrZplusIc[0] = J[0] * ZplusIc;
	XtrZplusIc    = Xcp.trans() * ZplusIc;
	
  	if(gearNode){
		gearNode->sumXtrZplusIc += XtrZplusIc;
		gearNode->sumtorque     += GetJoint()->torque[0];
		gearNode->sumJtrZplusIc += JtrZplusIc[0];
	}

	if(gearNode == this)
		GetParent()->Z +=
			sumXtrZplusIc + sumXtrIJ_sumJIJinv * (sumtorque * scene->GetTimeStep() - sumJtrZplusIc);
	if(!gearNode)
		GetParent()->Z +=
			XtrZplusIc + XtrIJ_JIJinv[0] * (GetJoint()->torque[0] * scene->GetTimeStep() - JtrZplusIc[0]);
}

void PHTreeNode1D::CompSpatialTransform(){
	PHTreeNode::CompSpatialTransform();
	if(gearNode && gearNode != this)
		 Xcg = Xcp * GetParent()->Xcp;
	else Xcg = Xcp;
}

void PHTreeNode1D::CompJointJacobian(){
	PHTreeNodeND<1>::CompJointJacobian();
	// �M�A�A�����Ă���ꍇ��(��i�m�[�h�̃��R�r�A���{�M�A��*�����̃��R�r�A��)
	if(gearNode && gearNode != this)
		J[0] = Xcp * parent1D->J[0] + gear->ratio * J[0];
}

void PHTreeNode1D::CompAccel(){
	//�֐߉����x
	if(gearNode){
		if(gearNode == this){
			accel[0] = sumJIJinv * (GetJoint()->torque[0] * scene->GetTimeStep() - sumXtrIJ * GetParent()->a - sumJtrZplusIc);
			a = ap + c + J[0] * accel[0];
		}
		else{
			ap = Xcg * gearNode->GetParent()->a;
			accel[0] = gear->ratio * DCAST(PHTreeNode1D, GetParent())->accel[0];
			a = ap + c + J[0] * gearNode->accel[0];
		}
	}
	else{
		ap = Xcp * GetParent()->a;
		accel[0] = JIJinv[0][0] * (GetJoint()->torque[0] * scene->GetTimeStep() - IJ[0] * ap - JtrZplusIc[0]);
		a = ap + c + J[0] * accel[0];
	}
	//DSTR << accel[0] << endl;
	//DSTR << GetJoint()->velocity[0] << endl;
	
	GetSolid()->dv = a;
	
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccel();
}

void PHTreeNode1D::CompBiasForceDiff(bool bUpdate){
	if(bUpdate)
		Z += dZ;
	if(gearNode){
		gearNode->sumXtrdZ += Xcg.trans() * dZ;
		gearNode->sumJtrdZ += J[0] * dZ;
	}	
	if(gearNode == this){
		GetParent()->dZ = sumXtrdZ - sumXtrIJ_sumJIJinv * sumJtrdZ;
		sumXtrdZ.clear();
		sumJtrdZ = 0.0;
	}
	if(!gearNode)
        GetParent()->dZ = Xcp.trans() * dZ - XtrIJ_JIJinv_Jtr * dZ;
	GetParent()->CompBiasForceDiff(bUpdate);
}

void PHTreeNode1D::CompAccelDiff(bool bUpdate){
	SpatialVector dap;
	if(gearNode){
		if(gearNode == this){
			daccel[0] = sumJIJinv * (dtau[0] - sumXtrIJ * GetParent()->da - sumJtrdZ);
			da = dap + J[0] * daccel[0];
		}
		else{
			dap = Xcg * gearNode->GetParent()->da;
			daccel[0] = gear->ratio * parent1D->daccel[0];
			da = dap + J[0] * gearNode->daccel[0];
		}
	}
	else{
		dap = Xcp * GetParent()->da;
		daccel[0] = JIJinv[0][0] * dtau[0] - IJ_JIJinv[0] * dap - J_JIJinv[0] * dZ;
		da = dap + J[0] * daccel[0];
	}
	dZ.clear(); // ����̌Ăяo���̂��߂ɂ����ŃN���A
	dtau.clear();
	if(bUpdate){
		accel[0] += daccel[0];
		a += da;
		GetSolid()->dv = a;
	}
	for(container_t::iterator it = Children().begin(); it != Children().end(); it++)
		(*it)->CompAccelDiff(bUpdate);
}

/*void PHTreeNode1D::CompResponse(double _dtau, bool bUpdate){
	dtau = _dtau;
	GetParent()->dZ = Xcp.trans() * (IJ_JIJinv * dtau);
	GetParent()->CompBiasForceDiff(bUpdate);
}*/

void PHTreeNode1D::Projection(double& _f, int k){
	PHJoint1D* j = GetJoint();
	if(j->onLower)
		_f = max(0.0, _f);
	if(j->onUpper)
		_f = min(0.0, _f);
}

void PHTreeNode1D::UpdateJointVelocity(double dt){
	if(gearNode && gearNode != this)
		 GetJoint()->velocity = gear->ratio * parent1D->GetJoint()->velocity;
	else PHTreeNodeND<1>::UpdateJointVelocity(dt);
}

void PHTreeNode1D::UpdateJointPosition(double dt){
	if(gearNode && gearNode != this)
		 GetJoint()->position = gear->ratio * parent1D->GetJoint()->position;
	else PHTreeNodeND<1>::UpdateJointPosition(dt);
}

//-----------------------------------------------------------------------------
// explicit instantiation
template class PHTreeNodeND<1>;
template class PHTreeNodeND<2>;
template class PHTreeNodeND<3>;
//template class PHTreeNodeND<4>;
	
}
