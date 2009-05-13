/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONTACTDETECTOR_H
#define PHCONTACTDETECTOR_H

//#include <SprPhysics.h>
#include <vector>
#include <Base/Combination.h>
#include <Collision/CDDetectorImp.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Physics/PHSolid.h>
#include <Foundation/UTPreciseTimer.h>

namespace Spr{;

//#define REPORT_TIME 1
#ifdef REPORT_TIME
extern Spr::UTPreciseTimer ptimerForCd;
#endif

/// ���̂̑g�̏��
struct PHSolidPairSt{
	bool bEnabled;
	PHSolidPairSt():bEnabled(false){}
};

/// ���̂̑g
template<class TShapePair, class TEngine>
class PHSolidPair : public PHSolidPairSt{
public:
	typedef TShapePair shapepair_type;
	typedef TEngine engine_type;

	PHSolid* solid[2];
	UTCombination< UTRef<TShapePair> > shapePairs;

	virtual ~PHSolidPair(){}
	virtual void Init(PHSolid* s0, PHSolid* s1){
		solid[0] = s0;
		solid[1] = s1;
		int ns0 = s0->NShape(), ns1 = s1->NShape();
		shapePairs.resize(ns0, ns1);
		for(int i = 0; i < ns0; i++)for(int j = 0; j < ns1; j++){
			TShapePair* sp = shapePairs.item(i, j);
			if (!sp){
				sp = DBG_NEW TShapePair();
				shapePairs.item(i, j) = sp;
			}
			sp->shape[0] = solid[0]->GetShape(i)->Cast();
			sp->shape[1] = solid[1]->GetShape(j)->Cast();
		}
		bEnabled = true;
	}

	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt){}	///< ���������m���ꂽ�Ƃ��̏���
	virtual void OnContDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt){}	///< ���������m���ꂽ�Ƃ��̏���
	
	bool Detect(TEngine* engine, unsigned int ct, double dt){
		if(!bEnabled)return false;

		// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�͐ڐG�Ȃ�
		if(solid[0]->NShape() == 0 || solid[1]->NShape() == 0) return false;

		// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
		bool found = false;
		TShapePair* sp;
		for(int i = 0; i < solid[0]->NShape(); i++)for(int j = 0; j < solid[1]->NShape(); j++){
			sp = shapePairs.item(i, j);
			//sp.UpdateShapePose(solid[0]->solid->GetPose(), solid[1]->solid->GetPose());
			//����shape pair�̌�������/�@���ƐڐG�̈ʒu�����߂�D
			if(sp->Detect(
				ct,
				solid[0]->GetPose() * solid[0]->GetShapePose(i), solid[1]->GetPose() * solid[1]->GetShapePose(j)))
			{
				found = true;
				OnDetect(sp, engine, ct, dt);
			}
		}
		return found;
	}

	bool ContDetect(TEngine* engine, unsigned int ct, double dt){
		if(!bEnabled)return false;
		// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�͐ڐG�Ȃ�
		if(solid[0]->NShape() == 0 || solid[1]->NShape() == 0) return false;
		// �����Ƃ��t���[�Y��Ԃ̏ꍇ�͐ڐG�Ȃ�
		if(solid[0]->IsFrozen() && solid[1]->IsFrozen())
			return false;
		
		std::vector<Vec3d> deltaPos[2];
		std::vector<Posed> shapePose[2];
		for(int i = 0; i < 2; i++){
			deltaPos[i].resize(solid[i]->NShape());
			shapePose[i].resize(solid[i]->NShape());
			for(int j = 0; j < solid[i]->NShape(); j++){
				CDConvex* convex = DCAST(CDConvex, solid[i]->GetShape(j));
				Posed lp = solid[i]->GetShapePose(j);
				Vec3d c = lp * convex->CalcCenterOfMass();
				deltaPos[i][j] = solid[i]->GetDeltaPosition(c);
				shapePose[i][j] = solid[i]->GetPose() * lp;
				shapePose[i][j].Pos() -= deltaPos[i][j];
			}
		}
		// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
		bool found = false;
		TShapePair* sp;
		for(int i = 0; i < solid[0]->NShape(); i++)for(int j = 0; j < solid[1]->NShape(); j++){
			Vec3d d0 = deltaPos[0][i];
			Vec3d d1 = deltaPos[1][j];
			sp = shapePairs.item(i, j);
			//����shape pair�̌�������/�@���ƐڐG�̈ʒu�����߂�D
			if(sp->DetectContinuously(ct, 
				shapePose[0][i], deltaPos[0][i],
				shapePose[1][j], deltaPos[1][j]))
			{
				assert(0.9 < sp->normal.norm() && sp->normal.norm() < 1.1);
				found = true;
				OnContDetect(sp, engine, ct, dt);
			}
		}
		// �t���[�Y�̉���
		if(found){
			if(solid[0]->IsDynamical() && !solid[1]->IsFrozen()){
				solid[0]->SetFrozen(false);
			}
			else if(solid[1]->IsDynamical() && !solid[0]->IsFrozen()){
				solid[1]->SetFrozen(false);
			}
		}

		return found;
	}

	void SetSt(const PHSolidPairSt& s){
		*((PHSolidPairSt*)this) = s;
	}
	void GetSt(PHSolidPairSt& s){
		s = *this;
	}

	///�w�肵��solid��Ԃ�
	PHSolidIf* GetSolid(int i){return solid[i]->Cast();}

};

///	PHContactDetector�̏��
struct PHContactDetectorSt{
	size_t nSolidPair;	///<	SolidPair�̐�
	size_t nShapePair;	///<	ShapePair�̐�
	PHSolidPairSt* SolidStates(){
		char* ptr = ((char*)this) + sizeof(*this);
		return (PHSolidPairSt*)ptr;
	}
	CDShapePairSt* ShapeStates(){
		char* ptr = ((char*)this) + sizeof(*this) + nSolidPair*sizeof(PHSolidPairSt);
		return (CDShapePairSt*)ptr;
	}
	size_t GetSize(){
		return sizeof(*this) + nSolidPair*sizeof(PHSolidPairSt)
			+ nShapePair*sizeof(CDShapePairSt);
	}
	PHContactDetectorSt(int n=0, int m=0):nSolidPair(n), nShapePair(m){}
};

// AABB�Ń\�[�g���邽�߂̍\����
struct Edge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int	index;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef std::vector<Edge> Edges;

template<class TShapePair, class TSolidPair, class TEngine>
class PHContactDetector : public PHEngine{

public:
#ifdef _DEBUG
	int nMaxOverlapObject;
#endif
	typedef TShapePair shapepair_type;
	typedef TSolidPair solidpair_type;
	typedef TEngine engine_type;
	typedef UTCombination< UTRef<TSolidPair> > PHSolidPairs;

	bool						bContactEnabled;///< �ڐG���L�������ꂽ���̂̑g��1�ł����݂����true
	PHSolids					solids;			///< ���̂̔z��
	PHSolidPairs				solidPairs;		///< ���̂̑g�̔z��
	std::vector<PHSolidIf*>		inactiveSolids; ///< ��͖@��K�p���Ȃ����̂̏W��

	PHContactDetector(){
	}

	virtual void Clear(){
		solidPairs.clear();
		solids.clear();
	}
	
	///< ��͖@��K�p���Ȃ����̂̒ǉ�
	virtual void AddInactiveSolid(PHSolidIf* solid){
		inactiveSolids.push_back(solid);
	}

	///< ��͖@��K�p���Ȃ����̂̌���
	virtual bool IsInactiveSolid(PHSolidIf* solid){
		for(std::vector<PHSolidIf*>::iterator it = inactiveSolids.begin(); it != inactiveSolids.end(); it++){
			if((*it) == solid)return true;
		}
		return false;
	}

	///< ���̂̒ǉ�
	virtual bool AddChildObject(ObjectIf* o){
		PHSolid* s = DCAST(PHSolid, o);
		if(s && !solids.Find(s)){
			solids.push_back(s);
			int N = (int)solids.size();
			assert(solidPairs.height() == N-1 && solidPairs.width() == N-1);
			solidPairs.resize(N, N);
			for(int i = 0; i < N-1; i++){
				solidPairs.item(i, N-1) = DBG_NEW TSolidPair();
				solidPairs.item(i, N-1)->Init(solids[i], solids[N-1]);
			}
			if(s->NShape())
				UpdateShapePairs(s);
			return true;
		}
		return false;
	}

	///< ���̂̍폜
	virtual bool DelChildObject(ObjectIf* o){
		PHSolid* s = DCAST(PHSolid, o);
	 	if(!s)return false;
		typename PHSolids::iterator is = find(solids.begin(), solids.end(), s);
		if(is != solids.end()){
			int idx = (int)(is - solids.begin());
			solids.erase(is);
			solidPairs.erase_row(idx);
			solidPairs.erase_col(idx);
			return true;
		}
		return false;
	}

	int NSolidPairs() const{
		int n = (int)solids.size();
		return n * (n - 1) / 2;
	}
	int NShapePairs() const{
		int n = (int)solids.size(), N = 0;
		for(int i = 0; i < n; i++)for(int j = i+1; j < n; j++)
			N += (int)(solidPairs.item(i,j)->shapePairs.size());
		return N;
	}

	virtual size_t GetStateSize() const {
		PHContactDetectorSt s(NSolidPairs(), NShapePairs());
		return s.GetSize();
	}
	virtual void ConstructState(void* m) const {
		new (m) PHContactDetectorSt(NSolidPairs(), NShapePairs());
	}
	virtual void DestructState(void* m) const {
		((PHContactDetectorSt*)m)->~PHContactDetectorSt();
	}
	virtual bool GetState(void* s) const {
		PHContactDetectorSt* es = ((PHContactDetectorSt*)s);
		es->nSolidPair = (size_t)NSolidPairs();
		es->nShapePair = (size_t)NShapePairs();
		PHSolidPairSt* solidStates = es->SolidStates();
		CDShapePairSt* shapeStates = es->ShapeStates();
		//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
		int solidPos=0;
		int shapePos=0;
		TSolidPair* sp;
		for(int j=1; j<solidPairs.width(); ++j){
			for(int i=0; i<j; ++i){
				sp = solidPairs.item(i, j);
				sp->GetSt(solidStates[solidPos]);				
				++solidPos;
				for(int r = 0; r < sp->shapePairs.height(); ++r){
					for(int c = 0; c < sp->shapePairs.width(); ++c){
						shapeStates[shapePos] = *(CDShapePairSt*)(sp->shapePairs.item(r, c));
						++shapePos;
					}
				}
			}
		}
		return true;
	}
	virtual void SetState(const void* s){
		PHContactDetectorSt* es = (PHContactDetectorSt*)s;
		assert(es->nSolidPair == (size_t)NSolidPairs());
		assert(es->nShapePair == (size_t)NShapePairs());
		PHSolidPairSt* solidStates = es->SolidStates();
		CDShapePairSt* shapeStates = es->ShapeStates();
		//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
		int solidPos=0;
		int shapePos=0;
		TSolidPair* sp;
		for(int j=1; j<solidPairs.width(); ++j){
			for(int i=0; i<j; ++i){
				sp = solidPairs.item(i, j);
				sp->SetSt(solidStates[solidPos]);
				++solidPos;
				for(int r = 0; r < sp->shapePairs.height(); ++r){
					for(int c = 0; c < sp->shapePairs.width(); ++c){
						sp->shapePairs.item(r, c)->SetState(shapeStates[shapePos]);
						++shapePos;
					}
				}
			}
		}
	}

	///< Solid��Shape���ǉ����ꂽ�Ƃ���Solid����Ă΂��
	void UpdateShapePairs(PHSolid* solid){
		int isolid = (solids.Find(solid) - &solids[0]);
		int i, j;
		TSolidPair* sp;
		PHSolid *slhs, *srhs;
		for(i = 0; i < isolid; i++){
			sp = solidPairs.item(i, isolid);
			slhs = sp->solid[0];
			srhs = solid;
			sp->shapePairs.resize(slhs->NShape(), srhs->NShape());
			for(j = 0; j < slhs->NShape(); j++){
				TShapePair* n = DBG_NEW TShapePair();
				n->shape[0] = slhs->GetShape(j)->Cast();
				n->shape[1] = srhs->GetShape(srhs->NShape()-1)->Cast();
				sp->shapePairs.item(j, srhs->NShape()-1) = n;
			}
		}
		for(i = isolid+1; i < (int)solids.size(); i++){
			sp = solidPairs.item(isolid, i);
			slhs = solid;
			srhs = sp->solid[1];
			sp->shapePairs.resize(solid->NShape(), srhs->NShape());
			for(j = 0; j < srhs->NShape(); j++){
				TShapePair* n = DBG_NEW TShapePair();
				n->shape[0] = slhs->GetShape(slhs->NShape()-1)->Cast();
				n->shape[1] = srhs->GetShape(j)->Cast();
				sp->shapePairs.item(slhs->NShape()-1, j) = n;
			}
		}
	}

	/// bEnabled�t���O��true��SolidPair�����݂��邩
	bool ActiveSolidPairExists(){
		bool yes = false;
		int n = (int)solids.size();
		for(int i = 0; i < n; i++)for(int j = i+1; j < n; j++)
			yes |= solidPairs.item(i, j)->bEnabled;
		return yes;
	}

	virtual void EnableContact(PHSolidIf* lhs, PHSolidIf* rhs, bool bEnable){
		typename PHSolids::iterator ilhs, irhs;
		ilhs = find(solids.begin(), solids.end(), (PHSolid*)(lhs->Cast()));
		irhs = find(solids.begin(), solids.end(), (PHSolid*)(rhs->Cast()));
		if(ilhs == solids.end() || irhs == solids.end())
			return;
		int i = (int)(ilhs - solids.begin()), j = (int)(irhs - solids.begin());
		if(i > j)std::swap(i, j);
		assert(i < solidPairs.height() && j < solidPairs.width());
		solidPairs.item(i, j)->bEnabled = bEnable;
		bContactEnabled = ActiveSolidPairExists();
	}

	virtual void EnableContact(PHSolidIf** group, size_t length, bool bEnable){
		std::vector<int> idx;
		typename PHSolids::iterator it;
		for(int i = 0; i < (int)length; i++){
			it = find(solids.begin(), solids.end(), (PHSolid*)(group[i]->Cast()));
			if(it != solids.end())
				idx.push_back(it - solids.begin());
		}
		sort(idx.begin(), idx.end());
		for(int i = 0; i < (int)idx.size(); i++){
			for(int j = i+1; j < (int)idx.size(); j++){
				solidPairs.item(idx[i], idx[j])->bEnabled = bEnable;
			}
		}
		bContactEnabled = ActiveSolidPairExists();
	}

	virtual void EnableContact(PHSolidIf* solid, bool bEnable){
		typename PHSolids::iterator it = find(solids.begin(), solids.end(), (PHSolid*)(solid->Cast()));
		if(it == solids.end())
			return;
		int idx = it - solids.begin();
		for(int i = 0; i < idx; i++)
			solidPairs.item(i, idx)->bEnabled = bEnable;
		for(int i = idx+1; i < (int)solids.size(); i++)
			solidPairs.item(idx, i)->bEnabled = bEnable;
		bContactEnabled = ActiveSolidPairExists();
	}

	virtual void EnableContact(bool bEnable){
		int n = (int)solids.size();
		for(int i = 0; i < n; i++)for(int j = i+1; j < n; j++)
			solidPairs.item(i, j)->bEnabled = bEnable;
		bContactEnabled = bEnable;
	}

	///< �S�̂̌����̌��m
	bool Detect(unsigned int ct, double dt){
		/* �ȉ��̗���Ō��������߂�
			1. Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
			2. �eSolid�̑g�ɂ���
				2a. Shape��BBox���x���ł̌������� (������)
				2b. �eShape�̑g�ɂ���GJK�Ō����`��𓾂�
				2c. �����`�󂩂�@�������߁A�@���Ɋւ��Č`����ˉe���C���̒��_��ڐG�_�Ƃ���
				2d. ����ꂽ�ڐG�_����PHContactPoints�ɋl�߂Ă���
		*/		
#ifdef REPORT_TIME
		ptimerForCd.CountUS();
#endif
		int N = solids.size();

		//1. BBox���x���̏Փ˔���
		Vec3f dir(0,0,1);
		Edges edges;
		edges.resize(2 * N);
		typename Edges::iterator eit = edges.begin();
		for(int i = 0; i < N; ++i){
			solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
			eit[0].index = i; eit[0].bMin = true;
			eit[1].index = i; eit[1].bMin = false;
			eit += 2;
		}
#ifdef REPORT_TIME
		DSTR << "  spt:" << ptimerForCd.CountUS();
#endif
		std::sort(edges.begin(), edges.end());
#ifdef REPORT_TIME
		DSTR << "  sort:" << ptimerForCd.CountUS();
#endif
		//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
		typedef std::set<int> SolidSet;
		SolidSet cur;							//	���݂�Solid�̃Z�b�g
		bool found = false;
#ifdef _DEBUG
		nMaxOverlapObject = 0;
#endif
		for(typename Edges::iterator it = edges.begin(); it != edges.end(); ++it){
			if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					int f1 = it->index;
					int f2 = *itf;
					if (f1 > f2) std::swap(f1, f2);
					//2. Solid��Solid�̏Փ˔���
#ifdef REPORT_TIME
					ptimerForCd.Stop();
#endif
					found |= solidPairs.item(f1, f2)->Detect((TEngine*)this, ct, dt);
#ifdef REPORT_TIME
					ptimerForCd.Start();
#endif
				}
				cur.insert(it->index);
#ifdef _DEBUG
				if (nMaxOverlapObject < (int)cur.size()) nMaxOverlapObject = cur.size();
#endif
			}else{
				cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
			}
		}
#ifdef REPORT_TIME
		DSTR << "  narrow:" << ptimerForCd.CountUS();
#endif
		return found;
	}

	///< �S�̂̌����̌��m�C�A���ڐG���m(Continuous Collision Detect)��
	bool ContDetect(unsigned int ct, double dt){
		/* �ȉ��̗���Ō��������߂�
			1. Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
			2. �eSolid�̑g�ɂ���
				2a. Shape��BBox���x���ł̌������� (������)
				2b. �eShape�̑g�ɂ���GJK�Ō����`��𓾂�
				2c. �����`�󂩂�@�������߁A�@���Ɋւ��Č`����ˉe���C���̒��_��ڐG�_�Ƃ���
				2d. ����ꂽ�ڐG�_����PHContactPoints�ɋl�߂Ă���
		*/		
#ifdef REPORT_TIME
		ptimerForCd.CountUS();
#endif
		int N = solids.size();

		//1. BBox���x���̏Փ˔���
		Vec3f dir(0,0,1);
		Edges edges;
		edges.resize(2 * N);
		typename Edges::iterator eit = edges.begin();
		for(int i = 0; i < N; ++i){
			solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
			Vec3d dPos = solids[i]->GetDeltaPosition();
			float dLen = (float) (dPos * dir);
			if (dLen < 0){
				eit[0].edge += dLen;
			}else{
				eit[1].edge += dLen;
			}
			eit[0].index = i; eit[0].bMin = true;
			eit[1].index = i; eit[1].bMin = false;
			eit += 2;
		}
#ifdef REPORT_TIME
		DSTR << "  spt:" << ptimerForCd.CountUS();
#endif
		std::sort(edges.begin(), edges.end());
#ifdef REPORT_TIME
		DSTR << "  sort:" << ptimerForCd.CountUS();
#endif
		//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
		typedef std::set<int> SolidSet;
		SolidSet cur;							//	���݂�Solid�̃Z�b�g
		bool found = false;
#ifdef _DEBUG
		nMaxOverlapObject = 0;
#endif
		for(typename Edges::iterator it = edges.begin(); it != edges.end(); ++it){
			if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					int f1 = it->index;
					int f2 = *itf;
					if (f1 > f2) std::swap(f1, f2);
					//2. Solid��Solid�̏Փ˔���
#ifdef REPORT_TIME
					ptimerForCd.Stop();
#endif
					found |= solidPairs.item(f1, f2)->ContDetect((TEngine*)this, ct, dt); 
#ifdef REPORT_TIME
					ptimerForCd.Start();
#endif
				}
				cur.insert(it->index);
#ifdef _DEBUG
				if (nMaxOverlapObject < (int)cur.size()) nMaxOverlapObject = cur.size();
#endif
			}else{
				cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
			}
		}
#ifdef REPORT_TIME
		DSTR << "  narrow:" << ptimerForCd.CountUS();
#endif
		return found;
	}
};

}

#endif
