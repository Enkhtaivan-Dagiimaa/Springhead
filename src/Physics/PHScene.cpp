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
#include <sstream>

namespace Spr{;

//----------------------------------------------------------------------------
//	PHScene
PHScene::PHScene(const PHSceneDesc& desc):PHSceneDesc(desc){
	Init();
}
void PHScene::Init(){
	engines.scene = this;
	Scene::Clear();

	solids = DBG_NEW PHSolidContainer;
	engines.Add(solids);
	PHSolidInitializer* si = DBG_NEW PHSolidInitializer;
	si->container = solids;
	engines.Add(si);
	
	gravityEngine = DBG_NEW PHGravityEngine;
	engines.Add(gravityEngine);
	gravityEngine->accel = gravity;

	penaltyEngine = DBG_NEW PHPenaltyEngine;
	engines.Add(penaltyEngine);
	
	constraintEngine = DBG_NEW PHConstraintEngine;
	constraintEngine->numIter = numIteration;
	engines.Add(constraintEngine);

	ikEngine = DBG_NEW PHIKEngine;
	ikEngine->numIter = 5;
	engines.Add(ikEngine);

	count = 0;
}
void PHScene::BeforeGetDesc() const{
	if (gravityEngine)
		((PHScene*)this)->gravity = gravityEngine->accel;
}
PHSdkIf* PHScene::GetSdk(){
	NameManagerIf* nm = GetNameManager();
	PHSdkIf* sdk = DCAST(PHSdkIf, nm);
	return sdk;
}

PHSolidIf* PHScene::CreateSolid(const PHSolidDesc& desc){
	UTRef<PHSolid> s = DBG_NEW PHSolid(desc, Cast());
	AddChildObject(s->Cast());
	return s->Cast();
}
int PHScene::NSolids()const{
	return solids->solids.size();
}
PHSolidIf** PHScene::GetSolids(){
	return solids->solids.empty() ? NULL : (PHSolidIf**)&*solids->solids.begin();
}

CDShapeIf* PHScene::CreateShape(const IfInfo* ii, const CDShapeDesc& desc){
	return GetSdk()->CreateShape(ii, desc);
}

PHJointIf* PHScene::CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const IfInfo* ii, const PHJointDesc& desc){
	PHJoint* joint = constraintEngine->CreateJoint(ii, desc, lhs->Cast(), rhs->Cast());
	AddChildObject(joint->Cast());
	return joint->Cast();
}
int PHScene::NJoints()const{
	return constraintEngine->joints.size();
}
PHConstraintIf* PHScene::GetJoint(int i){
	return DCAST(PHConstraintIf, constraintEngine->joints[i]);
}
int PHScene::NContacts()const{
	return constraintEngine->points.size();
}
PHContactPointIf* PHScene::GetContact(int i){
	return DCAST(PHContactPointIf, constraintEngine->points[i]);
}

int	PHScene::NSolidPairs()const{
	return (int)constraintEngine->solidPairs.size();
}

PHSolidPairForLCPIf* PHScene::GetSolidPair(int i, int j){
	return constraintEngine->solidPairs.item(i, j)->Cast();
}

PHRootNodeIf* PHScene::CreateRootNode(PHSolidIf* root, const PHRootNodeDesc& desc){
	PHRootNode* node = constraintEngine->CreateRootNode(desc, root->Cast());
	AddChildObject(node->Cast());
	return node->Cast();
}
int PHScene::NRootNodes()const{
	return constraintEngine->trees.size();
}
PHRootNodeIf* PHScene::GetRootNode(int i){
	return constraintEngine->trees[i]->Cast();
}

PHTreeNodeIf* PHScene::CreateTreeNode(PHTreeNodeIf* parent, PHSolidIf* child, const PHTreeNodeDesc& desc){
	PHTreeNode* node = constraintEngine->CreateTreeNode(desc, parent->Cast(), child->Cast());
	if(!node)
		return NULL;
	node->SetScene(Cast());
	return node->Cast();
}

PHGearIf* PHScene::CreateGear(PHJoint1DIf* lhs, PHJoint1DIf* rhs, const PHGearDesc& desc){
	PHGear* gear = constraintEngine->CreateGear(desc, lhs->Cast(), rhs->Cast());
	AddChildObject(gear->Cast());
	return gear->Cast();
}
int PHScene::NGears()const{
	return constraintEngine->gears.size();
}
PHGearIf* PHScene::GetGear(int i){
	return constraintEngine->gears[i]->Cast();
}
PHPathIf* PHScene::CreatePath(const PHPathDesc& desc){
	PHPath* path = constraintEngine->CreatePath(desc)->Cast();
	AddChildObject(path->Cast());
	return path->Cast();
}
int PHScene::NPaths()const{
	return constraintEngine->paths.size();
}
PHPathIf* PHScene::GetPath(int i){
	return constraintEngine->paths[i]->Cast();
}
PHRayIf* PHScene::CreateRay(const PHRayDesc& desc){
	PHRay* ray = DBG_NEW PHRay();
	ray->SetDesc(&desc);
	AddChildObject(ray->Cast());
	return ray->Cast();
}
int PHScene::NRays()const{
	return rays.size();
}
PHRayIf* PHScene::GetRay(int i){
	return rays[i]->Cast();
}

PHIKNodeIf* PHScene::CreateIKNode(const IfInfo* ii, const PHIKNodeDesc& desc){
	PHIKNode* node = ikEngine->CreateIKNode(ii, desc)->Cast();
	AddChildObject(node->Cast());
	return node->Cast();
}
int PHScene::NIKNodes(){
	return ikEngine->nodes.size();
}
PHIKNodeIf* PHScene::GetIKNode(int i){
	return ikEngine->nodes[i];
}
PHIKControlPointIf* PHScene::CreateIKControlPoint(const IfInfo* ii, const PHIKControlPointDesc& desc){
	PHIKControlPoint* controlpoint = ikEngine->CreateIKControlPoint(ii, desc)->Cast();
	AddChildObject(controlpoint->Cast());
	return controlpoint->Cast();
}
int PHScene::NIKControlPoints(){
	return ikEngine->controlpoints.size();
}
PHIKControlPointIf* PHScene::GetIKControlPoint(int i){
	return ikEngine->controlpoints[i];
}

void PHScene::Clear(){
	engines.Clear();
	Init();
}

void PHScene::SetTimeStep(double dt){
	timeStep = dt;
}

void PHScene::Step(){
	ClearForce();
	GenerateForce();
	Integrate();
}
void PHScene::ClearForce(){
	engines.ClearForce();
}
	
#if defined _MSC_VER	
#define FP_ERROR_MASK	(_EM_INEXACT|_EM_UNDERFLOW)
void PHScene::GenerateForce(){
	_clearfp();
	_controlfp(FP_ERROR_MASK, _MCW_EM);
	engines.GenerateForce();
	_controlfp(_MCW_EM, _MCW_EM);
}
void PHScene::Integrate(){
	_clearfp();
	_controlfp(FP_ERROR_MASK, _MCW_EM);
	engines.Integrate();
	_controlfp(_MCW_EM, _MCW_EM);
	count++;
}
#else
void PHScene::GenerateForce(){
	engines.GenerateForce();
}
void PHScene::Integrate(){
	engines.Integrate();
	count++;
}	
#endif
void PHScene::IntegratePart1(){
	for(PHEngines::iterator it = engines.dynamicalSystem; *it!=constraintEngine && it!=engines.end2(); ++it){
		(*it)->Step();		
	}
	constraintEngine->StepPart1();
}
void PHScene::IntegratePart2(){
	constraintEngine->StepPart2();
	PHEngines::iterator it;
	for(it = engines.dynamicalSystem; it!=engines.end2(); ++it){
		if (*it==constraintEngine){
			++it;
			break;
		}
	}
	for(; it!=engines.end2(); ++it){
		(*it)->Step();
	}
}
	
void PHScene::SetContactMode(PHSolidIf* lhs, PHSolidIf* rhs, PHSceneDesc::ContactMode mode){
	penaltyEngine->EnableContact(lhs, rhs, mode == PHSceneDesc::MODE_PENALTY);
	constraintEngine->EnableContact(lhs, rhs, mode == PHSceneDesc::MODE_LCP);
}

void PHScene::SetContactMode(PHSolidIf** group, size_t length, PHSceneDesc::ContactMode mode){
	penaltyEngine->EnableContact(group, length, mode == PHSceneDesc::MODE_PENALTY);
	constraintEngine->EnableContact(group, length, mode == PHSceneDesc::MODE_LCP);
}

void PHScene::SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode mode){
	penaltyEngine->EnableContact(solid, mode == PHSceneDesc::MODE_PENALTY);
	constraintEngine->EnableContact(solid, mode == PHSceneDesc::MODE_LCP);
}

void PHScene::SetContactMode(PHSceneDesc::ContactMode mode){
	penaltyEngine->EnableContact(mode == PHSceneDesc::MODE_PENALTY);
	constraintEngine->EnableContact(mode == PHSceneDesc::MODE_LCP);
}
int PHScene::GetNumIteration(){
	return constraintEngine->numIter;
}
void PHScene::SetNumIteration(int n){
	constraintEngine->numIter = n;
}

void PHScene::SetGravity(const Vec3d& accel){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	ge->accel = accel;
}
Vec3f PHScene::GetGravity(){
	PHGravityEngine* ge;
	engines.Find(ge);
	assert(ge);
	return ge->accel;
}
ObjectIf* PHScene::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = Scene::CreateObject(info, desc);
	if (!rv){
		if (info->Inherit(CDShapeIf::GetIfInfoStatic())){
			rv = CreateShape(info, *(const CDShapeDesc*)desc);
		}
	}
	return rv;
}
size_t PHScene::NChildObject() const{
	//return engines.size();
	return NSolids() + NJoints() + NRootNodes() + NGears() + NPaths()
		+ NContacts() + NRays();
}
ObjectIf* PHScene::GetChildObject(size_t pos){
	//return engines[pos]->Cast();
	if(pos < (size_t)NSolids()) return GetSolids()[pos];
	pos -= NSolids();
	if(pos < (size_t)NJoints()) return GetJoint(pos);
	pos -= NJoints();
	if(pos < (size_t)NRootNodes()) return GetRootNode(pos);
	pos -= NRootNodes();
	if(pos < (size_t)NGears()) return GetGear(pos);
	pos -= NGears();
	if(pos < (size_t)NPaths()) return GetPath(pos);
	pos -= NPaths();
	if(pos < (size_t)NContacts()) return GetContact(pos);
	pos -= NContacts();
	if(pos < (size_t)NRays()) return GetRay(pos);
	return NULL;
}
bool PHScene::AddChildObject(ObjectIf* o){
	bool ok = false;
	PHSolid* solid = DCAST(PHSolid, o);
	if(solid){
		if(	solids->AddChildObject(o) &&
			gravityEngine->AddChildObject(o) &&
			penaltyEngine->AddChildObject(o) &&
			constraintEngine->AddChildObject(o))
		{
            SetContactMode(solid->Cast(), PHSceneDesc::MODE_LCP);	//�f�t�H���g��LCP
			solid->scene = this;
			solid->engine = constraintEngine;
			ok = true;
		}
	}
	PHJointIf* con = DCAST(PHJointIf, o);
	if(con && constraintEngine->AddChildObject(con))
		ok = true;
	PHTreeNodeIf* node = DCAST(PHTreeNodeIf, o);
	if(node && constraintEngine->AddChildObject(o))
		ok = true;
	PHGearIf* gear = DCAST(PHGearIf, o);
	if(gear && constraintEngine->AddChildObject(o))
		ok = true;
	PHPathIf* path = DCAST(PHPathIf, o);
	if(path && constraintEngine->AddChildObject(o))
		ok = true;
	PHRayIf* ray = DCAST(PHRayIf, o);
	if(ray){
		rays.push_back(ray->Cast());
		ok = true;
	}
	PHIKNodeIf* ikNode = DCAST(PHIKNodeIf, o);
	if(ikNode && ikEngine->AddChildObject(o))
		ok = true;
	PHIKControlPointIf* ikPoint = DCAST(PHIKControlPointIf, o);
	if(ikPoint && ikEngine->AddChildObject(o))
		ok = true;

	// MergeScene�Ȃǂő���Scene����ړ����Ă���ꍇ������̂ŏ��L�����X�V����
	if(ok){
		SceneObject* so = DCAST(SceneObject, o);
		so->SetScene(Cast());
		// ���O���t���Ă��Ȃ��ꍇ�Ƀf�t�H���g����^����
		if(strcmp(so->GetName(), "") == 0){
			char name[256];
			if(solid)
				sprintf(name, "solid%d", NSolids()-1);
			else if(con)
				sprintf(name, "joint%d", NJoints()-1);
			else if(node)
				sprintf(name, "node%d", NRootNodes()-1);
			else if(gear)
				sprintf(name, "gear%d", NGears()-1);
			else if(path)
				sprintf(name, "path%d", NPaths()-1);
			else if(ray)
				sprintf(name, "ray%d", NRays() - 1);
			else if(ikNode)
				sprintf(name, "iknode%d", NIKNodes()-1);
			else if(ikPoint)
				sprintf(name, "ikpoint%d", NIKControlPoints()-1);
			so->SetName(name);
		}
	}
	return ok;
}
bool PHScene::DelChildObject(ObjectIf* o){
	PHSolidIf* solid = DCAST(PHSolidIf, o);
	if(solid){
		solids->DelChildObject(o);
		penaltyEngine->DelChildObject(o);
		constraintEngine->DelChildObject(o);
		gravityEngine->DelChildObject(o);
		return true;
	}
	PHJointIf* con = DCAST(PHJointIf, o);
	if(con)
		return constraintEngine->DelChildObject(o);
	PHTreeNodeIf* node = DCAST(PHTreeNodeIf, o);
	if(node)
		return constraintEngine->DelChildObject(o);
	PHGearIf* gear = DCAST(PHGearIf, o);
	if(gear)
		return constraintEngine->DelChildObject(o);
	PHPathIf* path = DCAST(PHPathIf, o);
	if(path)
		return constraintEngine->DelChildObject(o);
	PHRayIf* ray = DCAST(PHRayIf, o);
	if(ray){
		PHRays::iterator it = find(rays.begin(), rays.end(), (PHRay*)ray->Cast());
		if(it != rays.end()){
			rays.erase(it);
			return true;
		}
		return false;
	}

	return false;
}

PHEngines* PHScene::GetEngines(){
	return &engines;
}

int PHScene::NEngines(){
	
	return engines.size();
}

PHEngineIf* PHScene::GetEngine(int i){
	return XCAST(engines[i]);
}

PHConstraintEngineIf* PHScene::GetConstraintEngine(){
	return XCAST(constraintEngine);
}

PHGravityEngineIf* PHScene::GetGravityEngine(){
	return XCAST(gravityEngine);
}

PHPenaltyEngineIf* PHScene::GetPenaltyEngine(){
	return XCAST(penaltyEngine);
}

PHIKEngineIf* PHScene::GetIKEngine(){
	return XCAST(ikEngine);
}

size_t PHScene::GetStateSize() const{
	return sizeof(PHSceneState) + 
		(constraintEngine ? constraintEngine->GetStateSize() : 0);
}
void PHScene::ConstructState(void* m) const{
	new (m) PHSceneState();
	char* p = (char*)m;
	p += sizeof(PHSceneState);
	if (constraintEngine){
		constraintEngine->ConstructState(p);
	}
}
void PHScene::DestructState(void* m) const{
	char* p = (char*)m;
	((PHSceneState*)p)->~PHSceneState();
	p += sizeof(PHSceneState);
	if (constraintEngine){
		constraintEngine->DestructState(p);
	}
}
bool PHScene::GetState(void* s) const{
	char* p = (char*) s;
	*(PHSceneState*)p = *this;
	p += sizeof(PHSceneState);
	if (constraintEngine){
		constraintEngine->GetState(p);
	}
	return true;
}
void PHScene::SetState(const void* s){
	const char* p = (char*) s;
	*(PHSceneState*)this = *(const PHSceneState*)p;
	p += sizeof(PHSceneState);
	if (constraintEngine){
		constraintEngine->SetState(p);
	}	
}
bool PHScene::WriteState(std::ofstream& fout){
	if(!fout) return false;
	PHSceneState p = *this;
	fout.write((char*)&p, sizeof(PHSceneState));
	if(constraintEngine){
		constraintEngine->WriteState(fout);
	}
	return true;
}
void PHScene::ReadState(std::ifstream& fin){
	if(!fin) return;
	PHSceneState p;
	fin.read((char*)&p, sizeof(PHSceneState));
	*(PHSceneState*) this = p;
	if(constraintEngine){
		constraintEngine->ReadState(fin);
	}
}

void PHScene::SetStateMode(bool bConstraints){
	constraintEngine->bSaveConstraints = bConstraints;
}

//----------------------------------------------------------------------------
//	PHRay
void PHRay::Apply(){
	Vec3f ori, dir;
	Vec3f point[2];
	float offset[2];
	Posed p, pinv;

	hits.clear();

	PHScene* phScene = DCAST(PHScene, GetScene());
	for(int i = 0; i < phScene->NSolids(); i++){
		PHSolid* solid = DCAST(PHSolid, phScene->GetSolids()[i]);

		for(int j = 0; j < (int)solid->NShape(); j++){
			p = (solid->GetPose() * solid->GetShapePose(j));
			pinv = p.Inv();
			ori = pinv * origin;
			dir = pinv.Ori() * direction;
			CDShape* shape = solid->GetShape(j)->Cast();
			int num = shape->LineIntersect(ori, dir, point, offset);
			for(int k = 0; k < num; k++){
				hits.push_back(PHRaycastHit());
				hits.back().solid = solid->Cast();
				hits.back().shape = shape->Cast();
				hits.back().point = p * point[k];
				hits.back().distance = offset[k];
			}
		}
	}

}

PHRaycastHit* PHRay::GetNearest(){
	PHRaycastHit* nearest = NULL;
	float distMin = FLT_MAX;
	for(int i = 0; i < (int)hits.size(); i++){
		if(hits[i].distance < distMin){
			nearest = &hits[i];
			distMin = hits[i].distance;
		}
	}
	return nearest;
}

}
