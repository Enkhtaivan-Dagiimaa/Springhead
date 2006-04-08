#ifndef PHJOINT_H
#define PHJOINT_H

#include <SprPhysics.h>
#include "PHConstraint.h"

namespace Spr{;

class PHJoint : public InheritConstraint<PHJointIf, PHConstraint>{
public:
};

/// PHJoint1D����h������N���X�̂��߂̃��[�^
template<class inf, class base>
class InheritJoint1D : public InheritConstraint<inf, base>{
public:
	void	SetRange(double lower, double upper){base::SetRange(lower, upper);}
	void	GetRange(double& lower, double& upper){base::GetRange(lower, upper);}
	void	SetMotorTorque(double t){base::SetMotorTorque(t);}
	double	GetMotorTorque(){return base::GetMotorTorque();}
	// void	SetDesiredPosition(double p, double t) = 0;	/// �ڕW�ψʂ�ݒ肷��
	// double	GetDesiredPosition() = 0;				/// �ڕW�ψʂ��擾����
	void	SetDesiredVelocity(double v){base::SetDesiredVelocity(v);}
	double	GetDesiredVelocity(){return base::GetDesiredVelocity();}
	void	SetSpring(double K){base::SetSpring(K);}
	double	GetSpring(){return base::GetSpring();}
	void	SetSpringOrigin(double org){base::SetSpringOrigin(org);}
	double	GetSpringOrigin(){return base::GetSpringOrigin();}
	void	SetDamper(double D){base::SetDamper(D);}
	double	GetDamper(){return base::GetDamper();}
};

class PHJoint1D : public InheritConstraint<PHJoint1DIf, PHJoint>{
protected:
	void	CompDof();

public:
	//OBJECTDEF(PHJoint1D);
	bool on_lower, on_upper;	/// ���͈͂̉����A����ɒB���Ă���ꍇ��true
	double lower, upper;		/// ���͈͂̉����A���
	double torque;
	double pos_d, vel_d;		/// �ڕW�ψʁA�ڕW���x
	double spring, origin, damper;	/// �o�l�W���A�o�l���_�A�_���p�W��
	
	virtual void	SetRange(double l, double u){lower = l, upper = u;}
	virtual void	GetRange(double& l, double& u){l = lower, u = upper;}
	virtual void	SetMotorTorque(double t){mode = MODE_TORQUE; torque = t;}
	virtual double	GetMotorTorque(){return torque;}
	//virtual void SetDesiredPosition(double p){mode = MODE_POSITION; pos_d = p;}
	//virtual double GetDesiredPosition(){return pos_d;}
	virtual void	SetDesiredVelocity(double v){mode = MODE_VELOCITY; vel_d = v;}
	virtual double	GetDesiredVelocity(){return vel_d;}
	virtual void	SetSpring(double K){spring = K;}
	virtual double	GetSpring(){return spring;}
	virtual void	SetSpringOrigin(double org){origin = org;}
	virtual double	GetSpringOrigin(){return origin;}
	virtual void	SetDamper(double D){damper = D;}
	virtual double	GetDamper(){return damper;}
	
	virtual void	SetDesc(const PHJointDesc& desc);
};

}

#endif
