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
#include <Physics/PhysicsDecl.hpp>


namespace Spr{;

class PHJoint : public PHConstraint{
public:
	SPR_OBJECTDEF_ABST(PHJoint);
	
	/// ABA�őΉ�����PHTreeNode�̔h���N���X�𐶐����ĕԂ�
	virtual PHTreeNode* CreateTreeNode(){return NULL;}
	
	/// �R���X�g���N�^
	PHJoint();
};

template<int NDOF> class PHTreeNodeND;


template<int NDOF>
class PHJointND : public PHJoint{
public:
	typedef	PTM::TVector<NDOF, double> coord_t;

	int		axisIndex[NDOF];
	coord_t position, velocity;
	
	PHJointND(){
		position.clear();
		velocity.clear();
	}
protected:
	virtual coord_t GetTorqueND() = 0;
	friend class PHTreeNodeND<NDOF>;
};

class PHJoint1D : public PHJointND<1>{
protected:
	double  fMaxDt, fMinDt;				///< �֐߂̏o�����*dt�̍ő�l�A�ŏ��l
	bool	onLower, onUpper;			///< ���͈͂̉����A����ɒB���Ă���ꍇ��true
	virtual void AfterSetDesc();
	virtual coord_t GetTorqueND(){ return (coord_t&)torque; }
	friend class PHTreeNode1D;
public:
	SPR_OBJECTDEF_ABST1(PHJoint1D, PHJoint);
	SPR_DECLMEMBEROF_PHJoint1DDesc;

	/// �C���^�t�F�[�X�̎���
	virtual double	GetPosition() const {return position[0];}
	virtual double	GetVelocity() const {return velocity[0];}
	virtual void	SetMotorTorque(double t){mode = MODE_TORQUE; torque = t;}
	virtual double	GetMotorTorque() const {return torque;}
	virtual void	SetRange(double l, double u){lower = l, upper = u;}
	virtual void	GetRange(double& l, double& u) const {l = lower, u = upper;}
	virtual void	SetDesiredVelocity(double v){mode = MODE_VELOCITY; desiredVelocity = v;}
	virtual double	GetDesiredVelocity() const {return desiredVelocity;}
	virtual void	SetSpring(double K){spring = K;}
	virtual double	GetSpring() const {return spring;}
	virtual void	SetSpringOrigin(double org){origin = org;}
	virtual double	GetSpringOrigin() const {return origin;}
	virtual void	SetDamper(double D){damper = D;}
	virtual double	GetDamper() const {return damper;}
	virtual void	SetTorqueMax(double max){fMax = max; fMaxDt = fMax * GetScene()->GetTimeStep(); }
	virtual double	GetTorqueMax(){return fMax;}
	virtual void	SetTorqueMin(double min){fMin = min; fMinDt = fMin * GetScene()->GetTimeStep(); }
	virtual double	GetTorqueMin(){return fMin;}

	/// �I�[�o���C�h
	virtual void	AddMotorTorque(){f[axisIndex[0]] = torque * GetScene()->GetTimeStep();}
	virtual void	SetConstrainedIndex(bool* con);
	virtual void	SetConstrainedIndexCorrection(bool* con);
	virtual void	Projection(double& f, int k);
	virtual void	ProjectionCorrection(double& F, int k);
	PHJoint1D();
};

}

#endif
