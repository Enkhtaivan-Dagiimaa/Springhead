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
	//	交差する2つの凸形状を接触面で切った時の切り口の形を求める
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

// 接触解析．接触部分の切り口を求めて，切り口を構成する凸多角形の頂点をengineに拘束として追加する．	
void PHShapePairForLCP::EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1){
	//	center と normalが作る面と交差する面を求めないといけない．
	//	面の頂点が別の側にある面だけが対象．
	//	quick hull は n log r だから，線形時間で出来ることはやっておくべき．

	//	各3角形について，頂点がどちら側にあるか判定し，両側にあるものを対象とする．
	//	交線を，法線＋数値の形で表現する．
	//	この処理は凸形状が持っていて良い．
	//	＃交線の表現形式として，2次曲線も許す．その場合，直線は返さない
	//	＃2次曲線はMullar＆Preparataには入れないで別にしておく．

	//	相対速度をみて2Dの座標系を決める。
	FPCK_FINITE(solid0->pose);
	FPCK_FINITE(solid1->pose);
	Vec3d v0 = solid0->GetPointVelocity(center);
	Vec3d v1 = solid1->GetPointVelocity(center);
	Matrix3d local;	//	contact coodinate system 接触の座標系
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

	//	面と面が触れる場合があるので、接触が凸多角形や凸形状になることがある。
	//	切り口を求める。まず、それぞれの形状の切り口を列挙
	CDCutRing cutRing(commonPoint, local);	//	commonPointならば、それを含む面で切れば、必ず切り口の中になる。
	int nPoint = engine->points.size();
	//	両方に切り口がある場合．(球などないものもある)
	bool found = shape[0]->FindCutRing(cutRing, shapePoseW[0]);
	int nLine0 = cutRing.lines.size();
	if (found) found = shape[1]->FindCutRing(cutRing, shapePoseW[1]);
	int nLine1 = cutRing.lines.size() - nLine0;
	if (found){
		//	2つの切り口のアンドをとって、2物体の接触面の形状を求める。
		cutRing.MakeRing();
		section.clear();
		if (cutRing.vtxs.begin != cutRing.vtxs.end && !(cutRing.vtxs.end-1)->deleted){
			CDQHLine<CDCutLine>* vtx = cutRing.vtxs.end-1;
			for(; vtx->neighbor[0] != cutRing.vtxs.end-1; vtx = vtx->neighbor[0]){
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
				section.push_back(pos);
				PHContactPoint *point = DBG_NEW PHContactPoint(local, this, pos, solid0, solid1);
				point->scene = DCAST(PHScene, engine->GetScene());
				point->engine = engine;

				if(engine->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
				if(engine->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);

				engine->points.push_back(point);
			}
		}
	}
	if (nPoint == (int)engine->points.size()){	//	ひとつも追加していない＝切り口がなかった or あってもConvexHullが作れなかった．
		//	きっと1点で接触している．
		PHContactPoint *point = DBG_NEW PHContactPoint(local, this, center, solid0, solid1);
		point->scene = DCAST(PHScene, engine->GetScene());
		point->engine = engine;

		if(engine->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
		if(engine->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);

		engine->points.push_back(point);
	}
}

void PHSolidPairForLCP::OnDetect(PHShapePairForLCP* sp, PHConstraintEngine* engine, unsigned ct, double dt){
	//	法線を求める
	sp->CalcNormal();
	//	交差する2つの凸形状を接触面で切った時の切り口の形を求める
	sp->EnumVertex(engine, ct, solid[0], solid[1]);
}			

//----------------------------------------------------------------------------
// PHConstraintEngine
IF_OBJECT_IMP(PHConstraintEngine, PHEngine);

PHConstraintEngine::PHConstraintEngine(){
	numIter				 = 15;
	numIterCorrection	 = 0;
	numIterContactCorrection = 0;
	velCorrectionRate	 = 0.3;		//< 0.5だと大きすぎて馬が発振してしまう
	posCorrectionRate	 = 0.3;		//< 0.5だと大きすぎて馬が発振してしまう(07/12/30 toki)
	shrinkRate			 = 0.7;
	shrinkRateCorrection = 0.7;
	freezeThreshold		 = 0.0;
	bGearNodeReady = false;
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
	else if(ii == PHPathJointIf::GetIfInfoStatic())
		joint = DBG_NEW PHPathJoint();
	else if(ii == PHSpringIf::GetIfInfoStatic())
		joint = DBG_NEW PHSpring();
	else assert(false);
	
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
	
	//既存のツリーに含まれていないかチェック
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		if((*it)->FindBySolid(solid))
			return NULL;
	//parentが既存のツリーのノードかチェック（念のため）
	bool found = false;
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
		if((*it)->Includes(parent)){
			found = true;
			break;
		}
	}
	if(!found)return NULL;

	//parentに対応する剛体とsolidで指定された剛体とをつなぐ拘束を取得
	PHJoint* joint = DCAST(PHJoint, joints.FindBySolidPair(parent->GetSolid(), solid));
	if(!joint)return NULL;
	//拘束の種類に対応するノードを作成
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
		con->scene = DCAST(PHScene, GetScene());
		con->engine = this;
		joints.push_back(con);
		return true;
	}
	PHRootNode* root = DCAST(PHRootNode, o);
	if(root){
		root->Prepare(DCAST(PHScene, GetScene()), this);
		trees.push_back(root);
		bGearNodeReady = false;
		return true;
	}
	PHGear* gear = DCAST(PHGear, o);
	if(gear){
		gear->scene = DCAST(PHScene, GetScene());
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
	
	// ＊相互依存するオブジェクトの削除が必要だが未実装
	
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
	/* 相互に依存関係があるので呼び出し順番には注意する */
	
	//ツリー構造の前処理(ABA関係)
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++)
		(*it)->SetupABA();

	//接触拘束の前処理
	for(PHConstraints::iterator it = points.begin(); it != points.end(); it++)
		(*it)->SetupLCP();
	//関節拘束の前処理
	for(PHConstraints::iterator it = joints.begin(); it != joints.end(); it++)
		(*it)->SetupLCP();
	//ギア拘束の前処理
	for(PHGears::iterator it = gears.begin(); it != gears.end(); it++)
		(*it)->SetupLCP();
	//ツリー構造の前処理
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
	PHScene* scene = DCAST(PHScene, GetScene());
	double dt = scene->GetTimeStep();

	// ツリーに属さない剛体の更新
	for(PHSolids::iterator is = solids.begin(); is != solids.end(); is++){
		if(!(*is)->treeNode && !(*is)->IsUpdated()){
			(*is)->UpdateVelocity(dt);
			(*is)->UpdatePosition(dt);
			(*is)->SetUpdated(true);
		}
	}

	// ツリーに属する剛体の更新
	for(PHRootNodes::iterator it = trees.begin(); it != trees.end(); it++){
		(*it)->UpdateVelocity(dt);
		(*it)->UpdatePosition(dt);
	}
}

//#define REPORT_TIME 1
#ifdef REPORT_TIME
}
#include <Foundation/UTPreciseTimer.h>
Spr::UTPreciseTimer ptimer;
namespace Spr{
#endif

void PHConstraintEngine::Step(){
//	DSTR << "nContact:" <<  points.size() << std::endl;

	PHScene* scene = DCAST(PHScene, GetScene());
	unsigned int ct = scene->GetCount();
	double dt = scene->GetTimeStep();

	// 必要ならばギアノードの更新
	if(!bGearNodeReady){
		UpdateGearNode();
		bGearNodeReady = true;
	}
	//交差を検知
	points.clear();
#ifdef REPORT_TIME
	ptimer.CountUS();
#endif
	if(bContactEnabled)
		//Detect(ct, dt);
		ContDetect(ct, dt);
#ifdef REPORT_TIME
	DSTR << " col:" << ptimer.CountUS();
#endif
	//前回のStep以降に別の要因によって剛体の位置・速度が変化した場合
	//ヤコビアン等の再計算
	//各剛体の前処理
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
	//位置・速度の更新
	UpdateSolids();	
}

PHConstraintsIf* PHConstraintEngine::GetContactPoints(){
	return DCAST(PHConstraintsIf, &points);
}

}

