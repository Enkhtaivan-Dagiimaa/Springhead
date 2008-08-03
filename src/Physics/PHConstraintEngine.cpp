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
#include <float.h>
#include <Collision/CDDetectorImp.h>
#include <Collision/CDQuickHull2D.h>
#include <Collision/CDQuickHull2DImp.h>
#include <Collision/CDCutRing.h>

using namespace PTM;
using namespace std;
namespace Spr{;

void PHSolidPairForLCP::OnContDetect(PHShapePairForLCP* sp, PHConstraintEngine* engine, unsigned ct, double dt){
	//	��������2�̓ʌ`���ڐG�ʂŐ؂������̐؂���̌`�����߂�
	//int start = engine->points.size();
	sp->EnumVertex(engine, ct, solid[0], solid[1]);
	//int end = engine->points.size();

	//	HASE_REPORT
/*	DSTR << "st:" << sp->state << " depth:" << sp->depth;
	DSTR << " n:" << sp->normal;
	DSTR << " p:" << sp->center;
	DSTR << " r:" << end-start;
	DSTR << std::endl;
	DSTR << "  ring " << end-start << ":";
	for(start; start!=end; ++start){
		PHContactPoint* p = ((PHContactPoint*)&*engine->points[start]);
		DSTR << p->pos << " ";
	}
	DSTR << sp->center;
	DSTR << std::endl;
*/		
}

// �ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D	
void PHShapePairForLCP::EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1){
	//	center �� normal�����ʂƌ�������ʂ����߂Ȃ��Ƃ����Ȃ��D
	//	�ʂ̒��_���ʂ̑��ɂ���ʂ������ΏہD
	//	quick hull �� n log r ������C���`���Ԃŏo���邱�Ƃ͂���Ă����ׂ��D

	//	�e3�p�`�ɂ��āC���_���ǂ��瑤�ɂ��邩���肵�C�����ɂ�����̂�ΏۂƂ���D
	//	������C�@���{���l�̌`�ŕ\������D
	//	���̏����͓ʌ`�󂪎����Ă��ėǂ��D
	//	������̕\���`���Ƃ��āC2���Ȑ��������D���̏ꍇ�C�����͕Ԃ��Ȃ�
	//	��2���Ȑ���Mullar��Preparata�ɂ͓���Ȃ��ŕʂɂ��Ă����D

	//	���Α��x���݂�2D�̍��W�n�����߂�B
	FPCK_FINITE(solid0->pose);
	FPCK_FINITE(solid1->pose);
	Vec3d v0 = solid0->GetPointVelocity(center);
	Vec3d v1 = solid1->GetPointVelocity(center);
	Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
	local.Ex() = normal;
	local.Ey() = v1-v0;
	local.Ey() -= local.Ey() * normal * normal;
	if (local.Ey().square() > 1e-6){
		local.Ey().unitize(); 
	}else{
		if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
		else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
	}
	local.Ez() =  local.Ex() ^ local.Ey();
	if (local.det() < 0.9) {
		DSTR << "Error: local coordinate error." << std::endl;
		DSTR << local;
		Vec3d a = Vec3d(local[0][0], local[1][0], local[2][0]);
		Vec3d b = Vec3d(local[0][1], local[1][1], local[2][1]);
		Vec3d c = Vec3d(local[0][2], local[1][2], local[2][2]);
		DSTR << "dot(local[0], local[1]) :" << dot(a, b) << std::endl;
		DSTR << "dot(local[1], local[2]) :" << dot(b, c) << std::endl;
		DSTR << "dot(local[2], local[0]) :" << dot(c, a) << std::endl;
		DSTR << "norm a :" << a.norm() << std::endl;
		DSTR << "norm b :" << b.norm() << std::endl;
		DSTR << "norm c :" << c.norm() << std::endl;
		DSTR << "normal:" << normal << std::endl;
		DSTR << "v1-v0:" << v1-v0 << std::endl;
		assert(0);
	}

	//	�ʂƖʂ��G���ꍇ������̂ŁA�ڐG���ʑ��p�`��ʌ`��ɂȂ邱�Ƃ�����B
	//	�؂�������߂�B�܂��A���ꂼ��̌`��̐؂�����
	CDCutRing cutRing(commonPoint, local);	//	commonPoint�Ȃ�΁A������܂ޖʂŐ؂�΁A�K���؂���̒��ɂȂ�B
	int nPoint = engine->points.size();
	//	�����ɐ؂��������ꍇ�D(���ȂǂȂ����̂�����)
	bool found = shape[0]->FindCutRing(cutRing, shapePoseW[0]);
	int nLine0 = cutRing.lines.size();
	if (found) found = shape[1]->FindCutRing(cutRing, shapePoseW[1]);
	int nLine1 = cutRing.lines.size() - nLine0;
	if (found){
		//	2�̐؂���̃A���h���Ƃ��āA2���̂̐ڐG�ʂ̌`������߂�B
		cutRing.MakeRing();		
		section.clear();
		if (cutRing.vtxs.begin != cutRing.vtxs.end && !(cutRing.vtxs.end-1)->deleted){
			CDQHLine<CDCutLine>* vtx = cutRing.vtxs.end-1;
			do{
				assert(finite(vtx->dist));
#ifdef _DEBUG
				if (vtx->dist < 1e-200){
					DSTR << "Error:  PHShapePairForLCP::EnumVertex() :  distance too small." << std::endl;
					DSTR << vtx->dist << vtx->normal << std::endl;
					DSTR << cutRing.local << std::endl;
					
					DSTR << "Lines:(" << nLine0 << "+" << nLine1 << ")" << std::endl;
					for(unsigned i=0; i<cutRing.lines.size(); ++i){
						DSTR << cutRing.lines[i].dist << "\t" << cutRing.lines[i].normal << "\t";
						Vec2d pos = cutRing.lines[i].dist * cutRing.lines[i].normal;
						DSTR << pos.X() << "\t" << pos.Y() << std::endl;
					}

					DSTR << "Vertices in dual space:" << std::endl;
					for(CDQHLine<CDCutLine>* vtx = cutRing.vtxs.begin; vtx!=cutRing.vtxs.end; ++vtx){
						if (vtx->deleted) continue;
						DSTR << vtx->dist << "\t" << vtx->normal << "\t";
						double d = vtx->dist;
						if (d==0) d=1e-100;
						Vec2d pos = vtx->normal * d;
						DSTR << pos.X() << "\t" << pos.Y() << std::endl;
					}
					cutRing.lines.clear();
					shape[0]->FindCutRing(cutRing, shapePoseW[0]);
					shape[1]->FindCutRing(cutRing, shapePoseW[1]);
					continue;
				}
#endif
				Vec3d pos;
				pos.sub_vector(1, Vec2d()) = vtx->normal / vtx->dist;
				pos = cutRing.local * pos;
				section.push_back(pos);
				PHContactPoint *point = DBG_NEW PHContactPoint(local, this, pos, solid0, solid1);
				point->SetScene(engine->GetScene());
				point->engine = engine;

				if(engine->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
				if(engine->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);
				engine->points.push_back(point);
				vtx = vtx->neighbor[0];
			} while (vtx!=cutRing.vtxs.end-1);
		}
	}
	if (nPoint == (int)engine->points.size()){	//	�ЂƂ��ǉ����Ă��Ȃ����؂�����Ȃ����� or �����Ă�ConvexHull�����Ȃ������D
		//	������1�_�ŐڐG���Ă���D
		PHContactPoint *point = DBG_NEW PHContactPoint(local, this, center, solid0, solid1);
		point->SetScene(engine->GetScene());
		point->engine = engine;

		if(engine->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
		if(engine->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);

		engine->points.push_back(point);
	}
}

void PHSolidPairForLCP::OnDetect(PHShapePairForLCP* sp, PHConstraintEngine* engine, unsigned ct, double dt){
	//	�@�������߂�
	sp->CalcNormal();
	//	��������2�̓ʌ`���ڐG�ʂŐ؂������̐؂���̌`�����߂�
	sp->EnumVertex(engine, ct, solid[0], solid[1]);
}			

//----------------------------------------------------------------------------
// PHConstraintEngine
PHConstraintEngine::PHConstraintEngine(){
	numIter				 = 15;
	numIterCorrection	 = 0;
	numIterContactCorrection = 0;
	velCorrectionRate	 = 0.3;		//< 0.5���Ƒ傫�����Ĕn�����U���Ă��܂�
	posCorrectionRate	 = 0.3;		//< 0.5���Ƒ傫�����Ĕn�����U���Ă��܂�(07/12/30 toki)
	shrinkRate			 = 0.7;
	shrinkRateCorrection = 0.7;
	freezeThreshold		 = 0.0;
	contactCorrectionRate = 0.1;
	bGearNodeReady = false;
	bSaveConstraints = false;
}

PHConstraintEngine::~PHConstraintEngine(){
	
}

void PHConstraintEngine::Clear(){
	points.clear();
	joints.clear();
}

PHJoint* PHConstraintEngine::CreateJoint(const IfInfo* ii, const PHJointDesc& desc, PHSolid* lhs, PHSolid* rhs){
	UTRef<PHSolid> *plhs, *prhs;
	plhs = solids.Find(lhs);
	prhs = solids.Find(rhs);
	if(!plhs || !prhs)
		return NULL;
	
	PHJoint* joint = NULL;
	if(ii == PHHingeJointIf::GetIfInfoStatic())
		joint = DBG_NEW PHHingeJoint();
	else if(ii == PHSliderJointIf::GetIfInfoStatic())
		joint = DBG_NEW PHSliderJoint();
	else if(ii == PHBallJointIf::GetIfInfoStatic())
		joint = DBG_NEW PHBallJoint();
	else if(ii == PH3ElementBallJointIf::GetIfInfoStatic())
		joint = DBG_NEW PH3ElementBallJoint();
	else if(ii == PHPathJointIf::GetIfInfoStatic())
		joint = DBG_NEW PHPathJoint();
	else if(ii == PHSpringIf::GetIfInfoStatic())
		joint = DBG_NEW PHSpring();
	else assert(false);
	joint->SetScene(GetScene());
	joint->SetDesc(&desc);
	joint->solid[0] = lhs;
	joint->solid[1] = rhs;
	AddChildObject(joint->Cast());
	return joint;
}

PHRootNode* PHConstraintEngine::CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid){
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		if((*it)->FindBySolid(solid))
			return NULL;
	PHSolids::iterator it = find(solids.begin(), solids.end(), solid);
	if(it == solids.end())
		return NULL;
	
	PHRootNode* root = DBG_NEW PHRootNode();
	AddChildObject(root->Cast());
	root->AddChildObject(solid->Cast());
	return root;
}
PHTreeNode* PHConstraintEngine::CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent, PHSolid* solid){
	PHTreeNode* node;
	
	//�����̃c���[�Ɋ܂܂�Ă��Ȃ����`�F�b�N
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		if((*it)->FindBySolid(solid))
			return NULL;
	//parent�������̃c���[�̃m�[�h���`�F�b�N�i�O�̂��߁j
	bool found = false;
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
		if((*it)->Includes(parent)){
			found = true;
			break;
		}
	}
	if(!found)return NULL;

	//parent�ɑΉ����鍄�̂�solid�Ŏw�肳�ꂽ���̂Ƃ��Ȃ��S�����擾
	PHJoint* joint = DCAST(PHJoint, joints.FindBySolidPair(parent->GetSolid(), solid));
	if(!joint)return NULL;
	//�S���̎�ނɑΉ�����m�[�h���쐬
	node = joint->CreateTreeNode();
	node->AddChildObject(joint->Cast());
	parent->AddChildObject(node->Cast());

	return node;
}

PHPath* PHConstraintEngine::CreatePath(const PHPathDesc& desc){
	PHPath* path = DBG_NEW PHPath(desc);
	AddChildObject(path->Cast());
	return path;
}

PHGear* PHConstraintEngine::CreateGear(const PHGearDesc& desc, PHJoint1D* lhs, PHJoint1D* rhs){
	PHGear* gear = DBG_NEW PHGear();
	gear->joint[0] = lhs;
	gear->joint[1] = rhs;
	gear->SetDesc(&desc);
	AddChildObject(gear->Cast());
	return gear;
}

bool PHConstraintEngine::AddChildObject(ObjectIf* o){
	if(Detector::AddChildObject(o))
		return true;
	
	PHConstraint* con = DCAST(PHConstraint, o);
	if(con){
		con->engine = this;
		joints.push_back(con);
		return true;
	}
	PHRootNode* root = DCAST(PHRootNode, o);
	if(root){
		root->Prepare(GetScene()->Cast(), this);
		trees.push_back(root);
		bGearNodeReady = false;
		return true;
	}
	PHGear* gear = DCAST(PHGear, o);
	if(gear){
		gear->engine = this;
		gears.push_back(gear);
		bGearNodeReady = false;
		return true;
	}
	PHPath* path = DCAST(PHPath, o);
	if(path){
		paths.push_back(path);
		return true;
	}
	return false;
}

void PHConstraintEngine::UpdateGearNode(){
	for(int i = 0; i < (int)trees.size(); i++)
		trees[i]->ResetGearNode();

	for(int i = 0; i < (int)gears.size(); i++){
		PHGear* gear = gears[i];
		gear->bArticulated = false;
		PHTreeNode1D *nodeL, *nodeR;
		for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
			nodeL = DCAST(PHTreeNode1D, (*it)->FindByJoint(gear->joint[0]));
			nodeR = DCAST(PHTreeNode1D, (*it)->FindByJoint(gear->joint[1]));
			if(!nodeL || !nodeR)continue;
			if(nodeL->GetParent() == nodeR)
				nodeR->AddGear(gear, nodeL);
			else if(nodeR->GetParent() == nodeL)
				nodeL->AddGear(gear, nodeR);
			else if(nodeL->GetParent() == nodeR->GetParent())
				nodeL->AddGear(gear, nodeR);
		}
	}
}

bool PHConstraintEngine::DelChildObject(ObjectIf* o){
	
	// �����݈ˑ�����I�u�W�F�N�g�̍폜���K�v����������
	
	PHJoint* joint = DCAST(PHJoint, o);
	if(joint){
		PHConstraints::iterator it = find(joints.begin(), joints.end(), joint);
		if(it != joints.end()){
			joints.erase(it);
			return true;
		}
		return false;
	}
	PHRootNode* root = DCAST(PHRootNode, o);
	if(root){
		PHRootNodes::iterator it = find(trees.begin(), trees.end(), root);
		if(it != trees.end()){
			trees.erase(it);
			bGearNodeReady = false;
			return true;
		}
		return false;
	}
	PHGear* gear = DCAST(PHGear, o);
	if(gear){
		PHGears::iterator it = find(gears.begin(), gears.end(), gear);
		if(it != gears.end()){
			gears.erase(it);
			bGearNodeReady = false;
			return true;
		}
		return false;
	}
	PHPath* path = DCAST(PHPath, o);
	if(path){
		PHPaths::iterator it = find(paths.begin(), paths.end(), path);
		if(it != paths.end()){
			paths.erase(it);
			return true;
		}
		return false;
	}
	return false;
}

void PHConstraintEngine::SetupLCP(){
	/* ���݂Ɉˑ��֌W������̂ŌĂяo�����Ԃɂ͒��ӂ��� */
	
	//�c���[�\���̑O����(ABA�֌W)
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		(*it)->SetupABA();

	//�ڐG�S���̑O����
	for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
		(*it)->SetupLCP();
	//�֐ߍS���̑O����
	for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
		(*it)->SetupLCP();
	//�M�A�S���̑O����
	for(PHGears::iterator it = gears.begin(); it != gears.end(); it++)
		(*it)->SetupLCP();
	//�c���[�\���̑O����
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		(*it)->SetupLCP();

}
void PHConstraintEngine::SetupCorrectionLCP(){
	if(numIterCorrection)
		for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
			(*it)->SetupCorrectionABA();

	if(numIterContactCorrection)
		for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
			(*it)->SetupCorrectionLCP();

	if(numIterCorrection)
		for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
			(*it)->SetupCorrectionLCP();
}
void PHConstraintEngine::IterateLCP(){
	int count = 0;
	while(true){
		if(count == numIter)
			break;
		for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
			(*it)->IterateLCP();
		for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
			(*it)->IterateLCP();
		for(PHGears::iterator it = gears.begin(); it != gears.end(); it++)
			(*it)->IterateLCP();
		for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
			(*it)->IterateLCP();
		count++;
	}
}
void PHConstraintEngine::IterateCorrectionLCP(){
	int end = max(numIterCorrection, numIterContactCorrection);
	for(int i=0; i!=end; ++i){
		if (i<numIterContactCorrection)
			for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
				(*it)->IterateCorrectionLCP();
		if (i<numIterCorrection)
			for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
				(*it)->IterateCorrectionLCP();
	}
}

void PHConstraintEngine::UpdateSolids(){
	double dt = GetScene()->GetTimeStep();

	// �c���[�ɑ����Ȃ����̂̍X�V
	for(PHSolids::iterator is = solids.begin(); is != solids.end(); is++){
		if(!(*is)->treeNode && !(*is)->IsUpdated()){
			(*is)->UpdateVelocity(dt);
			(*is)->UpdatePosition(dt);
			(*is)->SetUpdated(true);
		}
	}

	// �c���[�ɑ����鍄�̂̍X�V
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
		(*it)->UpdateVelocity(dt);
		(*it)->UpdatePosition(dt);
	}
}

#ifdef REPORT_TIME
}
#include <Foundation/UTPreciseTimer.h>
namespace Spr{
UTPreciseTimer ptimer;
UTPreciseTimer ptimerForCd;
#endif


void PHConstraintEngine::StepPart1(){
	unsigned int ct = GetScene()->GetCount();
	double dt = GetScene()->GetTimeStep();
	// �K�v�Ȃ�΃M�A�m�[�h�̍X�V
	if(!bGearNodeReady){
		UpdateGearNode();
		bGearNodeReady = true;
	}
	//���������m
	points.clear();
	if(bContactEnabled){
		ContDetect(ct, dt);
	}
}
void PHConstraintEngine::StepPart2(){
	double dt = GetScene()->GetTimeStep();
	unsigned int ct = GetScene()->GetCount();

	for(PHSolids::iterator it = solids.begin(); it != solids.end(); it++)
		(*it)->UpdateCacheLCP(dt);
	for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
		(*it)->UpdateState();
	for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
		(*it)->UpdateState();
	
	SetupLCP();
	IterateLCP();

	SetupCorrectionLCP();
	IterateCorrectionLCP();

	//�ʒu�E���x�̍X�V
	UpdateSolids();	
}
	
void PHConstraintEngine::Step(){
//	DSTR << "nContact:" <<  points.size() << std::endl;
	unsigned int ct = GetScene()->GetCount();
	double dt = GetScene()->GetTimeStep();

	// �K�v�Ȃ�΃M�A�m�[�h�̍X�V
	if(!bGearNodeReady){
		UpdateGearNode();
		bGearNodeReady = true;
	}
	//���������m
	points.clear();
#ifdef REPORT_TIME
	ptimer.CountUS();
#endif
	if(bContactEnabled){
		//Detect(ct, dt);
		ContDetect(ct, dt);
#ifdef _DEBUG
//		DSTR << "nMaxOverlapObject = " << nMaxOverlapObject << std::endl;
#endif
	}
#ifdef REPORT_TIME
	DSTR << " col:" << ptimer.CountUS();
#endif
	//�O���Step�ȍ~�ɕʂ̗v���ɂ���č��̂̈ʒu�E���x���ω������ꍇ
	//���R�r�A�����̍Čv�Z
	//�e���̂̑O����
#ifdef REPORT_TIME
	ptimer.CountUS();
#endif
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); it++)
		(*it)->UpdateCacheLCP(dt);
	for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
		(*it)->UpdateState();
	for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
		(*it)->UpdateState();
	
	SetupLCP();
#ifdef REPORT_TIME
	DSTR << " sup:" << ptimer.CountUS();
#endif
	IterateLCP();

#ifdef REPORT_TIME
	DSTR << " ite:" << ptimer.CountUS() << std::endl;
#endif
	SetupCorrectionLCP();
	IterateCorrectionLCP();
	//�ʒu�E���x�̍X�V
	UpdateSolids();	
}

PHConstraintsIf* PHConstraintEngine::GetContactPoints(){
	return DCAST(PHConstraintsIf, &points);
}


//	state�֌W�̎���
size_t PHConstraintEngine::GetStateSize() const{
	return Detector::GetStateSize() + 
		(bSaveConstraints ? sizeof(PHConstraintsSt) : 0);
}
void PHConstraintEngine::ConstructState(void* m) const{
	char* p = (char*)m;
	if (bSaveConstraints){
		p += Detector::GetStateSize();
		new (p) PHConstraintsSt;
	}
}
void PHConstraintEngine::DestructState(void* m) const {
	Detector::DestructState(m);
	char* p = (char*)m;
	if (bSaveConstraints){
		p += Detector::GetStateSize();
		((PHConstraintsSt*)p)->~PHConstraintsSt();
	}
}
bool PHConstraintEngine::GetState(void* s) const {
	Detector::GetState(s);
	char* p = (char*)s;
	if (bSaveConstraints){
		PHConstraintsSt* st = (PHConstraintsSt*)(p + Detector::GetStateSize());
		st->points.resize(points.size());
		for(size_t i=0; i<points.size(); ++i){
			points[i]->GetState(&st->points[i]);
		}		
		st->joints.resize(joints.size());
		for(size_t i=0; i<joints.size(); ++i){
			joints[i]->GetState(&st->gears[i]);
		}
		st->gears.resize(gears.size());
		for(size_t i=0; i<gears.size(); ++i){
			gears[i]->GetState(&st->gears[i]);
		}
	}
	return true;
}
void PHConstraintEngine::SetState(const void* s){
	Detector::SetState(s);
	char* p = (char*)s;
	if (bSaveConstraints){
		PHConstraintsSt* st = (PHConstraintsSt*)(p + Detector::GetStateSize());
		st->points.resize(points.size());
		for(size_t i=0; i<points.size(); ++i){
			points[i]->SetState(&st->points[i]);
		}		
		st->joints.resize(joints.size());
		for(size_t i=0; i<joints.size(); ++i){
			joints[i]->SetState(&st->gears[i]);
		}
		st->gears.resize(gears.size());
		for(size_t i=0; i<gears.size(); ++i){
			gears[i]->SetState(&st->gears[i]);
		}
	}
}
}
