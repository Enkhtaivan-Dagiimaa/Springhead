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

namespace Spr{;

class PHSolid;
class PHJoint;
class PHConstraintEngine;

///	�`��̑g
class PHShapePairForLCP : public CDShapePair{
public:
	void CalcNormal(PHSolid* solid0, PHSolid* solid1);
	///	�ڐG��́D�ڐG�����̐؂�������߂āC�؂�����\������ʑ��p�`�̒��_��engine�ɍS���Ƃ��Ēǉ�����D
	void EnumVertex(PHConstraintEngine* engine, unsigned ct, PHSolidInfoForLCP* solid0, PHSolidInfoForLCP* solid1);
};

/// Solid�̑g
class PHConstraintEngine;
class PHSolidPairForLCP : public PHSolidPair<PHSolidInfoForLCP, PHShapePairForLCP, PHConstraintEngine>{
public:
	virtual void OnDetect(PHShapePairForLCP* cp, PHConstraintEngine* engine, unsigned ct, double dt);
};

class PHConstraintEngine : public PHContactDetector<PHSolidInfoForLCP, PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine>{
	typedef PHContactDetector<PHSolidInfoForLCP, PHShapePairForLCP, PHSolidPairForLCP, PHConstraintEngine> base_type;
	friend class PHConstraint;

	OBJECT_DEF_NOIF(PHConstraintEngine);

	PHJoint* CreateJoint(const PHJointDesc& desc);
	
public:
	PHConstraintEngine();
	~PHConstraintEngine();
	PHJoint* AddJoint(const PHJointDesc& desc);	///< �֐߂�ǉ�����i�t�@�C�����[�_�p�j
	PHJoint* AddJoint(PHSolid* lhs, PHSolid* rhs, const PHJointDesc& desc);	///< �֐߂̒ǉ�����
	
	virtual int GetPriority() const {return SGBP_CONSTRAINTENGINE;}
	virtual void Step();			///< 
	virtual void Dynamics(double dt, int ct);		///< 
	//virtual void Correction(double dt, int ct);		///< 
	void UpdateSolids(double dt);	///< ���ʂ�Solid�ɔ��f����
	virtual void Clear();

protected:
	PHConstraints	points;			///< �ڐG�_�̔z��
	PHConstraints	joints;			///< �֐߂̔z��
	int max_iter_dynamics;			///< ���x�X�VLCP�̔�����
	//int max_iter_correction;		///< �덷�C��LCP�̔�����
	double correction_rate;			///< �덷�C���䗦
	//double max_error;

	void SetupDynamics(double dt);	///< ���x�X�VLCP�̏���
	void IterateDynamics();			///< ���x�X�VLCP�̈�x�̔���
	
	//void SetupCorrection(double dt);///< �덷�C��LCP�̏���
	//void IterateCorrection();		///< �덷�C��LCP�̈�x�̔���
	
	friend class PHShapePairForLCP;
};

}	//	namespace Spr
#endif
