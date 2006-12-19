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
	OBJECTDEF_NOIF(PHConstraintEngine, PHEngine);

	PHJoint* CreateJoint(const PHJointDesc& desc);
	
public:
	int		numIteration;			///< ���x�X�VLCP�̔�����
	double	correctionRate;			///< �덷�C���䗦
	double	shrinkRate;				///< LCP�����l��O��̉��ɑ΂��ďk��������䗦
	//int max_iter_correction;		///< �덷�C��LCP�̔�����
	//double max_error;

	PHConstraintEngine();
	~PHConstraintEngine();
	
	PHJoint* AddJoint(const PHJointDesc& desc);	///< �֐߂�ǉ�����i�t�@�C�����[�_�p�j
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	///< �֐߂̒ǉ�����
	bool AddJoint(PHSolidIf* lhs, PHSolidIf* rhs, PHJointIf* j);
	
	PHRootNode* AddRootNode(PHSolid* solid);	///< �c���[�\���̃��[�g�m�[�h���쐬
	PHTreeNode* AddNode(PHTreeNode* parent, PHSolid* solid);	///< �c���[�\���̒��ԃm�[�h���쐬
	PHGear*		AddGear(PHJoint1D* lhs, PHJoint1D* rhs, const PHGearDesc& desc);	///< �M�A���쐬

	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	//virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids();			///< ���ʂ�Solid�ɔ��f����
	PHConstraints GetContactPoints();
	void Clear();

//protected:
	PHConstraints	points;			///< �ڐG�_�̔z��
	PHConstraints	joints;			///< �֐߂̔z��
	typedef std::vector< UTRef<PHRootNode> > PHRootNodes;
	PHRootNodes		trees;			///< Articulated Body System�̔z��
	PHGears			gears;
	
	void SetupLCP();	///< ���x�X�VLCP�̏���
	void IterateLCP();			///< ���x�X�VLCP�̈�x�̔���
	
	//void SetupCorrection(double dt);///< �덷�C��LCP�̏���
	//void IterateCorrection();		///< �덷�C��LCP�̈�x�̔���
	
	friend class PHShapePairForLCP;
};

}	//	namespace Spr
#endif
