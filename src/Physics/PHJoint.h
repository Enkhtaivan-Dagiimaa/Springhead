/*
 *  Copyright (c) 2003-2010, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINT_H
#define PHJOINT_H

#include <Physics/SprPHJoint.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHJointLimit.h>
#include <Physics/PHJointMotor.h>
#include <Physics/PhysicsDecl.hpp>

namespace Spr{;

class PHJoint : public PHConstraint {
public:
	SPR_OBJECTDEF_ABST(PHJoint);
	SPR_DECLMEMBEROF_PHJointDesc;

	/// �R���X�g���N�^
	PHJoint() {}

	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){ return NULL; }

	// ----- �C���^�t�F�[�X�̎���

	void	SetMaxForce(double max){ fMax = fabs(max); }
	double	GetMaxForce(){ return fMax; }
};

template<int NDOF> class PHTreeNodeND;

template<int NDOF>
class PHNDJoint : public PHJoint {
protected:
	friend class PHTreeNodeND<NDOF>;

public:
	/// �֐߂̈ʒu�E���x
	PTM::TVector<NDOF,double> position, velocity;

	/// �R���X�g���N�^
	PHNDJoint(){
		position.clear();
		velocity.clear();
		nMovableAxes = NDOF;
	}
};

class PH1DJoint : public PHNDJoint<1> {
protected:
	friend class PHTreeNode1D;
	friend class PH1DJointLimit;
	friend class PH1DJointMotor;

	PH1DJointLimit* limit;			///< ���͈͍S��
	PH1DJointMotor  motor;			///< ���[�^

public:
	SPR_OBJECTDEF_ABST1(PH1DJoint, PHJoint);
	SPR_DECLMEMBEROF_PH1DJointDesc;

	/// �R���X�g���N�^
	PH1DJoint() {
		motor.joint = this;
		limit = NULL;
	}

	// ----- PHConstraint�̔h���N���X�Ŏ�������@�\

	/// �ǂ̎��R�x�𑬓x�S�����邩��ݒ�
	virtual void SetupAxisIndex() {
		PHJoint::SetupAxisIndex();
		motor.SetupAxisIndex();
		if (limit) { limit->SetupAxisIndex(); }
	}
	
	/// LCP�̕␳�l�̌v�Z�D�덷�C���p
	virtual void CompBias() {
		PHJoint::CompBias();
		motor.CompBias();
		if (limit) { limit->CompBias(); }
	}

	// ----- ���̃N���X�ƁC���̃N���X����h������N���X�̋@�\

	/// �o�l���_�i�ڕW�p�x�j����̕΍���Ԃ��D��]�֐߂��I�[�o���C�h����
	virtual double	GetDeviation(){
		return GetPosition() - GetTargetPosition();
	}

	// ----- �C���^�t�F�[�X�̎���

	/// ChildObject�D�����ǉ��ł���
	virtual bool AddChildObject(ObjectIf* o) {
		if (!limit) { limit = o->Cast(); if(limit){ limit->joint=this;return true; }}
		return PHConstraint::AddChildObject(o);
	}
	virtual size_t NChildObject() const {
		return((limit?1:0)+PHConstraint::NChildObject());
	}
	virtual ObjectIf* GetChildObject(size_t i) {
		if (i==0 && limit) { return limit->Cast(); }
		return PHConstraint::GetChildObject(i - (limit ? 1 : 0));
	}

	PH1DJointLimitIf* CreateLimit(const PH1DJointLimitDesc& desc) {
		PH1DJointLimitIf* limit = GetScene()->CreateObject(PH1DJointLimitIf::GetIfInfoStatic(), &desc)->Cast();
		if (limit) { AddChildObject(limit); }
		return limit;
	}

	double	GetPosition() { UpdateState(); return position[0]; }
	double	GetVelocity() { UpdateState(); return velocity[0]; }

	PH1DJointLimitIf* GetLimit() { return limit->Cast(); }

	virtual void SetSpring(const double& spring) { this->spring = spring; }
	virtual double GetSpring() { return spring; }
	virtual void SetDamper(const double& damper) { this->damper = damper; }
	virtual double GetDamper() { return damper; }
	virtual void SetSecondDamper(const double& secondDamper) { this->secondDamper = secondDamper; }
	virtual double GetSecondDamper() { return secondDamper; }
	virtual void SetTargetPosition(const double& targetPosition) { this->targetPosition = targetPosition; }
	virtual double GetTargetPosition() { return targetPosition; }
	virtual void SetTargetVelocity(const double& targetVelocity) { this->targetVelocity = targetVelocity; }
	virtual double GetTargetVelocity() { return targetVelocity; }
	virtual void SetOffsetForce(const double& offsetForce) { this->offsetForce = offsetForce; }
	virtual double GetOffsetForce() { return offsetForce; }
	virtual void SetYieldStress(const double& yieldStress) { this->yieldStress = yieldStress; }
	virtual double GetYieldStress() { return yieldStress; }
	virtual void SetHardnessRate(const double& hardnessRate) { this->hardnessRate = hardnessRate; }
	virtual double GetHardnessRate() { return hardnessRate; }
	virtual void SetSecondMoment(double sM) { secondMoment = sM; }
	virtual double GetSecondMoment() { return secondMoment; }
	virtual double GetMotorForce() { if (limit) { if (limit->IsOnLimit()) return 0; } return(f[movableAxes[0]] / GetScene()->GetTimeStep()); }
};

}

#endif
