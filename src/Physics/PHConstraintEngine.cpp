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
	int start = engine->points.size();
	sp->EnumVertex(engine, ct, solid[0], solid[1]);
	int end = engine->points.size();

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
	if (local.det() < 0.99) {
		DSTR << "Error: local coordinate error." << std::endl;
		DSTR << local;
		DSTR << "normal:" << normal << std::endl;
		DSTR << "v1-v0:" << v1-v0 << std::endl;
		assert(0);
	}

	//	�ʂƖʂ��G���ꍇ������̂ŁA�ڐG���ʑ��p�`��ʌ`��ɂȂ邱�Ƃ�����B
	//	�؂�������߂�B�܂��A���ꂼ��̌`��̐؂�����
	CDCutRing cutRing(center, local);
	int nPoint = engine->points.size();
	//	�����ɐ؂��������ꍇ�D(���ȂǂȂ����̂�����)
	bool found = shape[0]->FindCutRing(cutRing, shapePoseW[0]);
	int nLine0 = cutRing.lines.size();
	if(found) found = shape[1]->FindCutRing(cutRing, shapePoseW[1]);
	int nLine1 = cutRing.lines.size() - nLine0;
	if (found){
		//	2�̐؂���̃A���h���Ƃ��āA2���̂̐ڐG�ʂ̌`������߂�B
		cutRing.MakeRing();
//		cutRing.Print(DSTR);
//		DSTR << "contact center:" << center << " normal:" << normal << "  vtxs:" << std::endl;
		for(CDQHLine<CDCutLine>* vtx = cutRing.vtxs.begin; vtx!=cutRing.vtxs.end; ++vtx){
			if (vtx->deleted) continue;
			assert(finite(vtx->dist));
			if (vtx->dist < 1e-200){
				DSTR << "Error:  PHShapePairForLCP::EnumVertex() :  distance too small." << std::endl;
				DSTR << vtx->dist << vtx->normal << std::endl;
				DSTR << cutRing.local << std::endl;
				
				DSTR << "Lines:(" << nLine0 << "+" << nLine1 << ")" << std::endl;
				for(unsigned i=0; i<cutRing.lines.size(); ++i){
					DSTR << cutRing.lines[i].dist << "\t" << cutRing.lines[i].normal << "\t";
					Vec3d pos = cutRing.lines[i].dist * cutRing.lines[i].normal;
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

			Vec3d pos;
			pos.sub_vector(1, Vec2d()) = vtx->normal / vtx->dist;
			pos = cutRing.local * pos;

			PHContactPoint *point = DBG_NEW PHContactPoint(local, this, pos, solid0, solid1);
			point->scene = DCAST(PHScene, engine->GetScene());
			point->engine = engine;

			if(engine->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
			if(engine->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);

			engine->points.push_back(point);
		}
	}
	if (nPoint == engine->points.size()){	//	�ЂƂ��ǉ����Ă��Ȃ����؂�����Ȃ����� or �����Ă�ConvexHull�����Ȃ������D
		//	������1�_�ŐڐG���Ă���D

		PHContactPoint *point = DBG_NEW PHContactPoint(local, this, center, solid0, solid1);
		point->scene = DCAST(PHScene, engine->GetScene());
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
OBJECT_IMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	numIteration	= 10;
	correctionRate	= 0.2;
	shrinkRate		= 0.9;
}

PHConstraintEngine::~PHConstraintEngine(){

}

void PHConstraintEngine::Clear(){
	points.clear();
	joints.clear();
}

PHJoint* PHConstraintEngine::CreateJoint(const PHJointDesc& desc){
	PHJoint* joint;
	switch(desc.type){
	case PHConstraintDesc::HINGEJOINT:
		joint = DBG_NEW PHHingeJoint();
		break;
	case PHConstraintDesc::SLIDERJOINT:
		joint = DBG_NEW PHSliderJoint();
		break;
	case PHConstraintDesc::BALLJOINT:
		joint = DBG_NEW PHBallJoint();
		break;
/*	case PHConstraintDesc::PATHJOINT:
		joint = DBG_NEW PHPathJoint();
		break;*/
	case PHConstraintDesc::SPRING:
		joint = DBG_NEW PHSpring();
		break;
	default: assert(false);
	}
	joint->scene = DCAST(PHScene, GetScene());
	joint->engine = this;
	joint->SetDesc(desc);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(const PHJointDesc& desc){
	PHJoint* joint = CreateJoint(desc);
	joints.push_back(joint);
	return joint;
}

PHJoint* PHConstraintEngine::AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc){
	UTRef<PHSolid> *plhs, *prhs;
	plhs = solids.Find(lhs);
	prhs = solids.Find(rhs);
	if(!plhs || !prhs)
		return NULL;
	
	PHJoint* joint = CreateJoint(desc);
	joint->solid[0] = *plhs;
	joint->solid[1] = *prhs;
	joints.push_back(joint);

	//�֐߂łȂ���ꂽ���̊Ԃ̐ڐG�͖�����
	//EnableContact(lhs, rhs, false);

	return joint;
}
bool PHConstraintEngine::AddJoint(PHSolidIf* lhs, PHSolidIf* rhs, PHJointIf* j){
	PHSolids::iterator islhs, isrhs;
	islhs = (PHSolids::iterator) solids.Find(lhs->Cast());
	isrhs = (PHSolids::iterator) solids.Find(rhs->Cast());
	if(islhs == solids.end() || isrhs == solids.end()) return false;
	
	PHJoint* joint = DCAST(PHJoint, j);
	joint->solid[0] = *islhs;
	joint->solid[1] = *isrhs;
	joints.push_back(joint);
	return true;
}

PHRootNode* PHConstraintEngine::AddRootNode(PHSolid* solid){
	//�����̃c���[�Ɋ܂܂�Ă��Ȃ����`�F�b�N
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		if((*it)->FindBySolid(solid))
			return NULL;
	//�V�������[�g�m�[�h���쐬
	PHSolids::iterator it = (PHSolids::iterator) solids.Find(solid);
	if(it == solids.end())
		return NULL;
	PHRootNode* root = DBG_NEW PHRootNode();
	root->solid = *it;
	root->SetScene(GetScene());
	(*it)->treeNode = root;
	trees.push_back(root);
	return root;
}
PHTreeNode* PHConstraintEngine::AddNode(PHTreeNode* parent, PHSolid* solid){
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
	PHConstraint* con = joints.FindBySolidPair(parent->GetSolid(), solid);
	if(!con)return NULL;
	PHJoint* joint = DCAST(PHJoint, con);
	if(!joint)return NULL;
	//�m�[�h���쐬
	PHTreeNode* node = joint->CreateTreeNode();
	//�I�u�W�F�N�g�Ԃ̃����N
	node->joint = joint;
	node->scene = DCAST(PHScene, GetScene());
	node->engine = this;
	parent->AddChild(node);
	joint->bArticulated = true;
	joint->solid[1]->treeNode = node;

	//�M�A�m�[�h�̍X�V

	return node;
}

PHGear* PHConstraintEngine::AddGear(PHJoint1D* lhs, PHJoint1D* rhs, const PHGearDesc& desc){
	PHGear* gear = DBG_NEW PHGear();
	gear->joint[0] = lhs;
	gear->joint[1] = rhs;
	gear->SetDesc(desc);
	gear->scene = DCAST(PHScene, GetScene());
	gear->engine = this;
	gears.push_back(gear);
	//lhs��rhs�������ABA�c���[�Őe�q�֌W�ɂ���ꍇ�A�M�A�m�[�h���X�V����
	PHTreeNode1D *nodeL, *nodeR;
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
		nodeL = DCAST(PHTreeNode1D, (*it)->FindByJoint(lhs));
		nodeR = DCAST(PHTreeNode1D, (*it)->FindByJoint(rhs));
		if(!nodeL || !nodeR)continue;
		if(nodeL->GetParent() == nodeR)
			nodeR->AddGear(gear, nodeL);
		else if(nodeR->GetParent() == nodeL)
			nodeL->AddGear(gear, nodeR);
	}
	return gear;
}

void PHConstraintEngine::SetupLCP(){
	PHScene* scene = DCAST(PHScene, GetScene());
	double dt = scene->GetTimeStep();

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
/*void PHConstraintEngine::SetupCorrection(double dt){
	PHSolidInfos<PHSolidInfoForLCP>::iterator it;
	for(it = solids.begin(); it != solids.end(); it++)
		(*it)->SetupCorrection();
	points.SetupCorrection(dt, max_error);
	joints.SetupCorrection(dt, max_error);
}*/
void PHConstraintEngine::IterateLCP(){
	int count = 0;
	while(true){
		if(count == numIteration)
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
/*void PHConstraintEngine::IterateCorrection(){
	int count = 0;
	while(true){
		if(count == max_iter_correction){
			//DSTR << "max count." << " iteration count: " << count << " dFsum: " << dFsum << endl;
			break;
		}
		points.IterateCorrection();
		joints.IterateCorrection();

		count++;
	}
}*/

void PHConstraintEngine::UpdateSolids(){
	PHScene* scene = DCAST(PHScene, GetScene());
	double dt = scene->GetTimeStep();

	// �c���[�ɑ����Ȃ����̂̍X�V
	for(PHSolids::iterator is = solids.begin(); is != solids.end(); is++){
		if(!(*is)->treeNode){
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

#include <windows.h>

void PHConstraintEngine::Step(){
	PHScene* scene = DCAST(PHScene, GetScene());
	unsigned int ct = scene->GetCount();
	double dt = scene->GetTimeStep();
	LARGE_INTEGER freq, val[2];
	QueryPerformanceFrequency(&freq);

	//���������m
	QueryPerformanceCounter(&val[0]);
	points.clear();
	if(bContactEnabled)
		//Detect(ct, dt);
		ContDetect(ct, dt);
	QueryPerformanceCounter(&val[1]);
	//DSTR << "cd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//�O���Step�ȍ~�ɕʂ̗v���ɂ���č��̂̈ʒu�E���x���ω������ꍇ
	//���R�r�A�����̍Čv�Z
	//�e���̂̑O����
	for(PHSolids::iterator it = solids.begin(); it != solids.end(); it++)
		(*it)->UpdateCacheLCP(dt);
	for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
		(*it)->UpdateState();
	for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
		(*it)->UpdateState();
	
	QueryPerformanceCounter(&val[0]);
	SetupLCP();
	QueryPerformanceCounter(&val[1]);
	//DSTR << "sd " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	QueryPerformanceCounter(&val[0]);
	IterateLCP();
	QueryPerformanceCounter(&val[1]);
	//DSTR << "id " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	//Correction(dt, ct);

	//�ʒu�E���x�̍X�V
	UpdateSolids();	
}

PHConstraints PHConstraintEngine::GetContactPoints(){
	return points;
}

/*void PHConstraintEngine::Correction(double dt, int ct){
	LARGE_INTEGER freq, val[2];
	QueryPerformanceFrequency(&freq);

	QueryPerformanceCounter(&val[0]);
	SetupCorrection(dt);
	QueryPerformanceCounter(&val[1]);
	//DSTR << "sc " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;

	QueryPerformanceCounter(&val[0]);
	IterateCorrection();
	QueryPerformanceCounter(&val[1]);
	//DSTR << "ic " << (double)(val[1].QuadPart - val[0].QuadPart)/(double)(freq.QuadPart) << endl;
}*/


}

