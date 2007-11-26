/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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

namespace Spr{;

class PHSolid;
class PHJoint;
class PHRootNode;
class PHConstraintEngine;

///	�`��̑g
class PHShapePairForLCP : public CDShapePair{
public:
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolid* solid0, PHSolid* solid1);
};

/// Solid�̑g
class PHConstraintEngine;
class PHSolidPairForLCP : public PHSolidPair<PHShapePairForLCP, PHConstraintEngine>{
public:
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
	virtual void OnContDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
};

class PHConstraintEngine : public PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	friend class PHConstraint;
	friend class PHShapePairForLCP;
	OBJECTDEF_NOIF(PHConstraintEngine, PHEngine);
public:
	typedef PHContactDetector<PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> Detector;
	int		numIter;					///< ���x�X�VLCP�̔�����
	int		numIterCorrection;			///< �덷�C��LCP�̔�����
	int		numIterContactCorrection;	///< �ڐG�_�̌덷�C��LCP�̔�����
	double	velCorrectionRate;			///< �덷�C���䗦
	double	posCorrectionRate;
	double	shrinkRate;				///< LCP�����l��O��̉��ɑ΂��ďk��������䗦
	double	shrinkRateCorrection;
	double	freezeThreshold;		///< ���̂��t���[�Y����臒l
	bool	bGearNodeReady;			///< �M�A�m�[�h�����܂��\������Ă��邩�̃t���O�D�m�[�h��M�A��ǉ��E�폜���邽�т�false�ɂȂ�
	
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
	//virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids();			///< ���ʂ�Solid�ɔ��f����
	PHConstraints GetContactPoints();
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

	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
};

}	//	namespace Spr
#endif
