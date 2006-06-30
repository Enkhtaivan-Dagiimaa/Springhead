/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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

namespace Spr{;

///���̂Ɋւ���t�����
class PHSolidInfo : public Object{
public:
	PHSolid*	solid;		///< PHSolid�ւ̎Q��
	PHSolidInfo(PHSolid* s):solid(s){}
};

template<class TSolidInfo>
class PHSolidInfos : public std::vector< UTRef<TSolidInfo> >{
public:
	typedef std::vector< UTRef<TSolidInfo> > base;
	typedef typename base::iterator iterator;
	iterator Find(PHSolid* s){
		iterator is;
		for(is = base::begin(); is != base::end(); is++)
			if((*is)->solid == s)
				break;
		return is;
	};
};


/// ���̂̑g�̏��
struct PHSolidPairState{
	bool bEnabled;
};

/// ���̂̑g
template<class TSolidInfo, class TShapePair, class TEngine>
class PHSolidPair : public PHSolidPairState, public UTRefCount{
public:
	typedef TShapePair shapepair_type;
	typedef TEngine engine_type;

	TSolidInfo* solid[2];
	UTCombination< UTRef<TShapePair> > shapePairs;

	virtual void Init(TSolidInfo* s0, TSolidInfo* s1){
		solid[0] = s0;
		solid[1] = s1;
		int ns0 = s0->solid->NShape(), ns1 = s1->solid->NShape();
		shapePairs.resize(ns0, ns1);
		bEnabled = true;
	}

	virtual void OnDetect(shapepair_type* sp, engine_type* engine, unsigned ct, double dt){}	///< ���������m���ꂽ�Ƃ��̏���
	
	bool Detect(TEngine* engine, unsigned int ct, double dt){
		if(!bEnabled)return false;

		// �����ꂩ��Solid�Ɍ`�󂪊��蓖�Ă��Ă��Ȃ��ꍇ�͐ڐG�Ȃ�
		PHSolid *s0 = solid[0]->solid, *s1 = solid[1]->solid;
		if(s0->NShape() == 0 || s1->NShape() == 0) return false;

		// �S�Ă�shape pair�ɂ��Č����𒲂ׂ�
		bool found = false;
		TShapePair* sp;
		for(int i = 0; i < s0->NShape(); i++)for(int j = 0; j < s1->NShape(); j++){
			sp = shapePairs.item(i, j);
			//sp.UpdateShapePose(solid[0]->solid->GetPose(), solid[1]->solid->GetPose());
			//����shape pair�̌�������/�@���ƐڐG�̈ʒu�����߂�D
			if(sp->Detect(
				ct,
				DCAST(CDConvex, s0->GetShape(i)), DCAST(CDConvex, s1->GetShape(j)),
				s0->GetPose() * s0->GetShapePose(i), s1->GetPose() * s1->GetShapePose(j)))
			{
				found = true;
				OnDetect(sp, engine, ct, dt);
			}
		}
		return found;
	}

	void SetState(const PHSolidPairState& s){
		*((PHSolidPairState*)this) = s;
	}
};

///	PHContactDetector�̏��
struct PHContactDetectorState{
	size_t nSolidPair;	///<	SolidPair�̐�
	size_t nShapePair;	///<	ShapePair�̐�
	PHSolidPairState* SolidStates(){
		char* ptr = ((char*)this) + sizeof(*this);
		return (PHSolidPairState*)ptr;
	}
	CDShapePairState* ShapeStates(){
		char* ptr = ((char*)this) + sizeof(*this) + nSolidPair*sizeof(PHSolidPairState);
		return (CDShapePairState*)ptr;
	}
	size_t GetSize(){
		return sizeof(*this) + nSolidPair*sizeof(PHSolidPairState)
			+ nShapePair*sizeof(CDShapePairState);
	}
	PHContactDetectorState(int n, int m):nSolidPair(n), nShapePair(m){}

};

template<class TSolidInfo, class TShapePair, class TSolidPair, class TEngine>
class PHContactDetector : public PHEngine{

	// AABB�Ń\�[�g���邽�߂̍\����
	struct Edge{
		float edge;				///<	�[�̈ʒu(�O���[�o���n)
		int	index;				///<	���� solid�̈ʒu
		bool bMin;				///<	���[�Ȃ�true
		bool operator < (const Edge& s) const { return edge < s.edge; }
	};
	typedef std::vector<Edge> Edges;

public:
	typedef TShapePair shapepair_type;
	typedef TSolidPair solidpair_type;
	typedef TEngine engine_type;
	typedef UTCombination< UTRef<TSolidPair> > PHSolidPairs;

	bool						bContactEnabled;///< �ڐG���L�������ꂽ���̂̑g��1�ł����݂����true
	PHSolidInfos<TSolidInfo>	solids;			///< ���̂̔z��
	PHSolidPairs				solidPairs;		///< ���̂̑g�̔z��

	virtual void Clear(){
		solidPairs.clear();
		solids.clear();
	}
	
	///< ���̂̒ǉ�
	virtual bool AddChildObject(ObjectIf* o){
		PHSolid* s = DCAST(PHSolid, o);
		if(s && solids.Find(s) == solids.end()){
			solids.push_back(DBG_NEW TSolidInfo(s));
			int N = (int)solids.size();
			assert(solidPairs.height() == N-1 && solidPairs.width() == N-1);
			solidPairs.resize(N, N);
			for(int i = 0; i < N-1; i++){
				solidPairs.item(i, N-1) = DBG_NEW TSolidPair();
				solidPairs.item(i, N-1)->Init(solids[i], solids[N-1]);
			}
			return true;
		}
		return false;
	}

	///< ���̂̍폜
	virtual bool DelChildObject(ObjectIf* o){
		PHSolid* s = DCAST(PHSolid, o);
	 	if(!s)return false;
		typename PHSolidInfos<TSolidInfo>::iterator is = solids.Find(s);
		if(is != solids.end()){
			int idx = is - solids.begin();
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
		PHContactDetectorState s(NSolidPairs(), NShapePairs());
		return s.GetSize();
	}
	virtual void ConstructState(void* m) const {
		new (m) PHContactDetectorState(NSolidPairs(), NShapePairs());
	}
	virtual void DestructState(void* m) const {
		((PHContactDetectorState*)m)->~PHContactDetectorState();
	}
	virtual bool GetState(void* s) const {
		PHContactDetectorState* es = ((PHContactDetectorState*)s);
		PHSolidPairState* solidStates = es->SolidStates();
		CDShapePairState* shapeStates = es->ShapeStates();
		//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
		int solidPos=0;
		int shapePos=0;
		TSolidPair* sp;
		for(int j=0; j<solidPairs.width(); ++j){
			for(int i=0; i<j; ++i){
				sp = solidPairs.item(i, j);
				solidStates[solidPos] = *(PHSolidPairState*)sp;
				++solidPos;
				for(int r = 0; r < sp->shapePairs.height(); ++r){
					for(int c = 0; c < sp->shapePairs.width(); ++c){
						shapeStates[shapePos] = *(CDShapePairState*)(sp->shapePairs.item(r, c));
						++shapePos;
					}
				}
			}
		}
		return true;
	}
	virtual void SetState(const void* s){
		PHContactDetectorState* es = (PHContactDetectorState*)s;
		PHSolidPairState* solidStates = es->SolidStates();
		CDShapePairState* shapeStates = es->ShapeStates();
		//	solidPairs.item(i,j)�@�� i<j�������g���Ă���̂ł��������ۑ�
		int solidPos=0;
		int shapePos=0;
		TSolidPair* sp;
		for(int j=0; j<solidPairs.width(); ++j){
			for(int i=0; i<j; ++i){
				sp = solidPairs.item(i, j);
				sp->SetState(solidStates[solidPos]);
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
		int isolid = (solids.Find(solid) - solids.begin());
		int i, j;
		TSolidPair* sp;
		PHSolid *slhs, *srhs;
		for(i = 0; i < isolid; i++){
			sp = solidPairs.item(i, isolid);
			slhs = sp->solid[0]->solid;
			sp->shapePairs.resize(slhs->NShape(), solid->NShape());
			for(j = 0; j < slhs->NShape(); j++)
				sp->shapePairs.item(j, solid->NShape()-1) = DBG_NEW TShapePair();
		}
		for(i = isolid+1; i < (int)solids.size(); i++){
			sp = solidPairs.item(isolid, i);
			srhs = sp->solid[1]->solid;
			sp->shapePairs.resize(solid->NShape(), srhs->NShape());
			for(j = 0; j < srhs->NShape(); j++)
				sp->shapePairs.item(solid->NShape()-1, j) = DBG_NEW TShapePair();
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
		typename PHSolidInfos<TSolidInfo>::iterator ilhs = solids.Find((PHSolid*)lhs), irhs = solids.Find((PHSolid*)rhs);
		if(ilhs == solids.end() || irhs == solids.end())
			return;
		int i = ilhs - solids.begin(), j = irhs - solids.begin();
		if(i > j)std::swap(i, j);
		assert(i < solidPairs.height() && j < solidPairs.width());
		solidPairs.item(i, j)->bEnabled = bEnable;
		bContactEnabled = ActiveSolidPairExists();
	}

	virtual void EnableContact(PHSolidIf** group, size_t length, bool bEnable){
		std::vector<int> idx;
		typename PHSolidInfos<TSolidInfo>::iterator it;
		for(int i = 0; i < (int)length; i++){
			it = solids.Find((PHSolid*)group[i]);
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
		typename PHSolidInfos<TSolidInfo>::iterator it = solids.Find((PHSolid*)solid);
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
		int N = solids.size();

		//1. BBox���x���̏Փ˔���
		Vec3f dir(0,0,1);
		Edges edges;
		edges.resize(2 * N);
		typename Edges::iterator eit = edges.begin();
		for(int i = 0; i < N; ++i){
			solids[i]->solid->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
			eit[0].index = i; eit[0].bMin = true;
			eit[1].index = i; eit[1].bMin = false;
			eit += 2;
		}
		std::sort(edges.begin(), edges.end());
		//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
		typedef std::set<int> SolidSet;
		SolidSet cur;							//	���݂�Solid�̃Z�b�g
		bool found = false;
		for(typename Edges::iterator it = edges.begin(); it != edges.end(); ++it){
			if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					int f1 = it->index;
					int f2 = *itf;
					if (f1 > f2) std::swap(f1, f2);
					//2. Solid��Solid�̏Փ˔���
					found |= solidPairs.item(f1, f2)->Detect((TEngine*)this, ct, dt);
				}
				cur.insert(it->index);
			}else{
				cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
			}
		}
		return found;
	}

};

}

#endif
