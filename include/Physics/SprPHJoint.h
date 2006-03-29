#ifndef SPR_PHJOINTIf_H
#define SPR_PHJOINTIf_H
#include <Foundation/SprObject.h>

namespace Spr{;

struct PHJointDesc{
	enum JointType{
		JOINT_CONTACT,
		JOINT_HINGE,
		JOINT_SLIDER,
		JOINT_BALL,
	} type;					/// �֐߂̎��
	bool bEnabled;
	Posed poseJoint[2];		/// ���̂��猩���֐߂̈ʒu�ƌX��
	PHJointDesc():bEnabled(true){}
};
struct PHJoint1DDesc : public PHJointDesc{
	double	lower, upper;	/// ���͈�. lower < upper�̂Ƃ��ɗL���ƂȂ�
	PHJoint1DDesc():lower(0.0), upper(0.0){}
};
struct PHHingeJointDesc : public PHJoint1DDesc{
	PHHingeJointDesc(){
		type = JOINT_HINGE;
	}
};
struct PHSliderJointDesc : public PHJoint1DDesc{
	PHSliderJointDesc(){
		type = JOINT_SLIDER;
	}
};
struct PHBallJointDesc : public PHJointDesc{
	double	max_angle;		/// �~����̉��͈�
	PHBallJointDesc(){
		type = JOINT_BALL;
		max_angle = 0.0;
	}
};

struct PHConstraintIf : public SceneObjectIf{
	IF_DEF(PHConstraint);
	virtual void Enable(bool bEnable = true) = 0;		/// �֐߂�L��/����������
	virtual bool IsEnabled() = 0;
	virtual PHJointDesc::JointType GetJointType() = 0;	/// �֐߂̎��
};
struct PHContactPointIf : public PHConstraintIf{
	IF_DEF(PHContactPoint);
};
struct PHJointIf : public PHConstraintIf{
	IF_DEF(PHJoint);
};
struct PHJoint1DIf : public PHJointIf{
	IF_DEF(PHJoint1D);
	virtual void	SetRange(double lower, double upper) = 0;	/// ���͈͂�ݒ肷��
	virtual void	SetTorque(double t) = 0;					/// �֐߃g���N��ݒ肷��
	virtual double	GetTorque() = 0;							/// �֐߃g���N���擾����
	//virtual void	SetDesiredPosition(double p, double t) = 0;	/// �ڕW�ψʂ�ݒ肷��
	//virtual double	GetDesiredPosition() = 0;				/// �ڕW�ψʂ��擾����
	virtual void	SetDesiredVelocity(double v) = 0;			/// �ڕW���x��ݒ肷��
	virtual double	GetDesiredVelocity() = 0;					/// �ڕW���x���擾����
	virtual double	GetPosition() = 0;							/// �ψʂ��擾����
	virtual double	GetVelocity() = 0;							/// ���x���擾����
};
struct PHHingeJointIf : public PHJoint1DIf{
	IF_DEF(PHHingeJoint);
};
struct PHSliderJointIf : public PHJoint1DIf{
	IF_DEF(PHSliderJoint);
};
struct PHBallJointIf : public PHConstraintIf{
	IF_DEF(PHBallJoint);
};

}

#endif
