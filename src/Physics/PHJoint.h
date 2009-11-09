/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHJOINT_H
#define PHJOINT_H

#include <SprPhysics.h>
#include <Physics/PHConstraint.h>
#include <Physics/PHJointLimit.h>
#include <Physics/PHMotor.h>
#include <Physics/PhysicsDecl.hpp>


namespace Spr{;

class PHJoint : public PHConstraint{
public:
	SPR_OBJECTDEF_ABST(PHJoint);
	SPR_DECLMEMBEROF_PHJointDesc;
	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){return NULL;}
	//ControlMode���擾,�ݒ肷��
	//virtual PHJointDesc::PHControlMode	GetMode() {return mode;}
	//virtual void	SetMode(PHJointDesc::PHControlMode mode) {this->mode = mode;}
	/// �R���X�g���N�^
	PHJoint();
};

template<int NDOF> class PHTreeNodeND;


template<int NDOF>
class PHJointND : public PHJoint{
public:
	typedef	PTM::TVector<NDOF, double> coord_t;

	int		axisIndex[NDOF];		//< ���Ԗڂ̎����֐ߎ��ƂȂ邩
	coord_t position, velocity;
	
	void	CompResponse(double df, int k){
		PHConstraint::CompResponse(df, axisIndex[k]);
	}

	virtual void SetConstrainedIndex(bool* con){
		std::fill(con, con+6, true);
		for(int i = 0; i < NDOF; i++)
			con[axisIndex[i]] = false;
	}

	PHJointND(){
		position.clear();
		velocity.clear();
	}
protected:
	//virtual coord_t GetTorqueND() = 0;
	friend class PHTreeNodeND<NDOF>;
};

class PHJoint1D : public PHJointND<1>{
protected:
	friend class PHTreeNode1D;
	friend class PHJointLimit1D;
	friend class PHMotor1D;

	PHJointLimit1D	limit;			///< ���͈͍S��
	PHMotor1D		motor;			///< ���[�^

	//virtual coord_t GetTorqueND(){ return (coord_t&)torque; }
public:
	SPR_OBJECTDEF_ABST1(PHJoint1D, PHJoint);
	SPR_DECLMEMBEROF_PHJoint1DDesc;

	/// �C���^�t�F�[�X�̎���
	double	GetPosition() const {return position[0];}
	double	GetVelocity() const {return velocity[0];}
	void	SetMotorTorque(double t){ offsetForce = t; }
	double	GetMotorTorque() const { return offsetForce; }
	void	SetRange(double l, double u){lower = l, upper = u;}
	void	GetRange(double& l, double& u) const {l = lower, u = upper;}
	void	SetTargetVelocity(double v){targetVelocity = v;}
	double	GetTargetVelocity() const {return targetVelocity;}
	void	SetTrajectoryVelocity(double v){targetVelocity = v;}
	double  GetTrajectoryVelocity(){return targetVelocity;}
	void	SetSpring(double K){spring = K;}
	double	GetSpring() const {return spring;}
	void	SetTargetPosition(double org){ targetPosition = org;}
	double	GetTargetPosition() const {return targetPosition;}
	void	SetDamper(double D){damper = D;}
	double	GetDamper() const {return damper;}
	void	SetSecondDamper(double D2)	{secondDamper = D2;}
	double  GetSecondDamper()			{return secondDamper;}
	void	SetOffsetForce(double dat){ offsetForce = dat; }
	double	GetOffsetForce(){ return offsetForce;}
	bool	IsLimit(){ return (limit.onLower || limit.onUpper); }
	void	SetTorqueMax(double max){fMax = fabs(max); }
	double	GetTorqueMax(){return fMax;}

	/// �I�[�o���C�h
	virtual void	SetupLCP();
	virtual	void	IterateLCP();
	virtual void	SetupCorrectionLCP();
	virtual void	IterateCorrectionLCP();
	//virtual void	AddMotorTorque(){f[axisIndex[0]] = torque * GetScene()->GetTimeStep();}
	//virtual void	SetConstrainedIndexCorrection(bool* con);
	//virtual void	CompJointBias();
	//virtual void	Projection(double& f, int k);
	//virtual void	ProjectionCorrection(double& F, int k);

	/// �o�l���_�i�ڕW�p�x�j����̕΍���Ԃ��D��]�֐߂��I�[�o���C�h����
	virtual double	GetDeviation(){ return GetPosition() - targetPosition; }

	PHJoint1D();
};

}

#endif
