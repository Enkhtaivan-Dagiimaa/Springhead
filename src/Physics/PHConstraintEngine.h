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
	int		numIteration;			///< ���x�X�VLCP�̔�����
	double	correctionRate;			///< �덷�C���䗦
	double	shrinkRate;				///< LCP�����l��O��̉��ɑ΂��ďk��������䗦
	//int max_iter_correction;		///< �덷�C��LCP�̔�����
	//double max_error;

	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* CreateJoint(const PHJointDesc& desc, PHSolid* lhs = NULL, PHSolid* rhs = NULL);	///< �֐߂̒ǉ�����
	//bool AddJoint(PHSolidIf* lhs, PHSolidIf* rhs, PHJointIf* j);
	PHRootNode* CreateRootNode(const PHRootNodeDesc& desc, PHSolid* solid = NULL);	///< �c���[�\���̃��[�g�m�[�h���쐬
	PHTreeNode* CreateTreeNode(const PHTreeNodeDesc& desc, PHTreeNode* parent = NULL, PHSolid* solid = NULL);	///< �c���[�\���̒��ԃm�[�h���쐬
	PHGear*		CreateGear(const PHGearDesc& desc, PHJoint1D* lhs = NULL, PHJoint1D* rhs = NULL);	///< �M�A���쐬

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
	
	void SetupLCP();	///< ���x�X�VLCP�̏���
	void IterateLCP();			///< ���x�X�VLCP�̈�x�̔���
	
	//void SetupCorrection(double dt);///< �덷�C��LCP�̏���
	//void IterateCorrection();		///< �덷�C��LCP�̈�x�̔���

	virtual bool AddChildObject(ObjectIf* o);
};

}	//	namespace Spr
#endif
