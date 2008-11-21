/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <Physics/PHContactDetector.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHGear.h>
#include <Physics/PHPathJoint.h>
#include <Collision/CDDetectorImp.h>

namespace Spr{;

class PHSolid;
class PHJoint;
class PHRootNode;
class PHConstraintEngine;

///	�`��̑g
class PHShapePairForLCP: public CDShapePair{
public:
	SPR_OBJECTDEF(PHShapePairForLCP);
	std::vector<Vec3d>	section;	///< �����f�ʂ̒��_�D�X��PHContactPoint�ƂȂ�D�����̂��߂ɕێ�
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1);
	int NSectionVertexes(){return section.size();}		//(section�̐���Ԃ��j
	Vec3d GetSectionVertex(int i){return section[i];}	//(i�Ԗڂ�section��Ԃ��j
//	double GetContactDimension(int NSection){
//		for(i = 0; i < Nsection; i++){
};

/// Solid�̑g
class PHConstraintEngine;

class PHSolidPairForLCP : public PHSolidPair<PHShapePairForLCP, PHConstraintEngine>, public Object{
public:
	SPR_OBJECTDEF(PHSolidPairForLCP);
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	virtual void OnContDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	int	GetContactState(int i, int j){return shapePairs.item(i, j)->state;}
	unsigned GetLastContactCount(int i, int j){return shapePairs.item(i, j)->lastContactCount;}
	Vec3d GetCommonPoint(int i, int j){return shapePairs.item(i, j)->commonPoint;}
	double GetContactDepth(int i, int j){return shapePairs.item(i, j)->depth;}
	PHShapePairForLCPIf* GetShapePair(int i, int j){return shapePairs.item(i, j)->Cast();}
};

struct PHConstraintsSt{
	std::vector<PHConstraintStatePrivate> points;
	std::vector<PHConstraintStatePrivate> joints;
	std::vector<PHConstraintStatePrivate> gears;
};

class PHConstraintEngine : public PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	friend class PHConstraint;
	friend class PHShapePairForLCP;
	SPR_OBJECTDEF1(PHConstraintEngine, PHEngine);
public:
	typedef PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> Detector;
	int		numIter;					///< ���x�X�VLCP�̔�����
	int		numIterCorrection;			///< �덷�C��LCP�̔�����
	int		numIterContactCorrection;	///< �ڐG�_�̌덷�C��LCP�̔�����
	double	velCorrectionRate;			///< ���x��LCP�Ŋ֐ߍS���̌덷���C������ꍇ�̌덷�C���䗦
	double	posCorrectionRate;			///< �ʒu��LCP�ŁC�֐ߍS���̌덷���C������ꍇ�̌덷�C���䗦
	double  contactCorrectionRate;		///< �ڐG�̐N�������̂��߂́C���x��LCP�ł̕␳�䗦�D
	double	shrinkRate;					///< LCP�����l��O��̉��ɑ΂��ďk��������䗦
	double	shrinkRateCorrection;
	double	freezeThreshold;			///< ���̂��t���[�Y����臒l
	bool	bGearNodeReady;				///< �M�A�m�[�h�����܂��\������Ă��邩�̃t���O�D�m�[�h��M�A��ǉ��E�폜���邽�т�false�ɂȂ�
	bool	bSaveConstraints;			///< SaveState, LoadState�ɁC constraints ���܂߂邩�ǂ����D�{���s�v�����Cf, F���ω�����D
	
	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* CreateJoint(const IfInfo* ii, const PHJointDesc& desc, PHSolid* lhs = NULL, PHSolid* rhs = NULL);	///< �֐߂̒ǉ�����
	PHRootNode* CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid = NULL);	///< �c���[�\���̃��[�g�m�[�h���쐬
	PHTreeNode* CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent = NULL, PHSolid* solid = NULL);	///< �c���[�\���̒��ԃm�[�h���쐬
	PHGear*		CreateGear(const PHGearDesc& desc, PHJoint1D* lhs = NULL, PHJoint1D* rhs = NULL);	///< �M�A���쐬
	PHPath*		CreatePath(const PHPathDesc& desc);
	void		UpdateGearNode();
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	virtual void StepPart1();		///< 
	virtual void StepPart2();		///< 
	//virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids();			///< ���ʂ�Solid�ɔ��f����
	void Clear();

	PHConstraints	points;			///< �ڐG�_�̔z��
	PHConstraints	joints;			///< �֐߂̔z��
	typedef std::vector< UTRef<PHRootNode> > PHRootNodes;
	PHRootNodes		trees;			///< Articulated Body System�̔z��
	PHGears			gears;
	typedef std::vector< UTRef<PHPath> > PHPaths;
	PHPaths			paths;
	
	void SetupLCP();				///< ���x�X�VLCP�̏���
	void IterateLCP();				///< ���x�X�VLCP�̈�x�̔���
	void SetupCorrectionLCP();		///< �덷�C��LCP�̏���
	void IterateCorrectionLCP();	///< �덷�C��LCP�̈�x�̔���

	virtual PHConstraintsIf* GetContactPoints();
	virtual void SetVelCorrectionRate(double value){velCorrectionRate = value;}
	virtual void SetPosCorrectionRate(double value){posCorrectionRate = value;}
	virtual void SetContactCorrectionRate(double value){contactCorrectionRate = value;}

	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);

	virtual size_t GetStateSize() const;
	virtual void ConstructState(void* m) const;
	virtual void DestructState(void* m) const ;
	virtual bool GetState(void* s) const ;
	virtual void SetState(const void* s);
};

}	//	namespace Spr
#endif
