#ifndef PHJOINT_H
#define PHJOINT_H

#include <SprPhysics.h>
#include "PHConstraint.h"

namespace Spr{;

class PHJoint : public InheritConstraint<PHJointIf, PHConstraint>{
public:

};
class PHJoint1D : public InheritConstraint<PHJoint1DIf, PHJoint>{
public:
	//OBJECTDEF(PHJoint1D);
	bool on_lower, on_upper;	/// ���͈͂̉����A����ɒB���Ă���ꍇ��true
	double lower, upper;		/// ���͈͂̉����A���
	double torque;
	double pos_d, vel_d;		/// �ڕW�ψʁA�ڕW���x
	double spring, origin, damper;	/// �o�l�W���A�o�l���_�A�_���p�W��
	
	virtual void Init(PHSolidAux* lhs, PHSolidAux* rhs, const PHJointDesc& desc);
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
	
};

}

#endif
