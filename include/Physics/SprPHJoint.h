/**
 *	@file SprPHJoint.h
 *	@brief �ڐG��֐߂Ȃ�
*/
#ifndef SPR_PHJOINTIf_H
#define SPR_PHJOINTIf_H
#include <Foundation/SprObject.h>

namespace Spr{;

/// �֐߂̃f�B�X�N���v�^
struct PHJointDesc{
	/// �֐߂̎��
	enum JointType{
		JOINT_CONTACT,
		JOINT_HINGE,
		JOINT_SLIDER,
		JOINT_BALL,
	} type;
	/// �L��/�����t���O
	bool bEnabled;
	/// ���̂��猩���֐߂̈ʒu�ƌX��
	Posed poseJoint[2];
	PHJointDesc():bEnabled(true){}
};

/// 1���֐߂̃f�B�X�N���v�^
struct PHJoint1DDesc : public PHJointDesc{
	double	lower, upper;	/// ���͈�. lower < upper�̂Ƃ��ɗL���ƂȂ�
	double	spring;		/// �o�l�W��
	double  origin;		/// �o�l���_
	double  damper;		/// �_���p�W��
	double	torque;		/// ���[�^�g���N
	PHJoint1DDesc():lower(0.0), upper(0.0), spring(0.0), origin(0.0), damper(0.0), torque(0.0){}
};

/// �q���W�̃f�B�X�N���v�^
struct PHHingeJointDesc : public PHJoint1DDesc{
	PHHingeJointDesc(){
		type = JOINT_HINGE;
	}
};

/// �X���C�_�̃f�B�X�N���v�^
struct PHSliderJointDesc : public PHJoint1DDesc{
	PHSliderJointDesc(){
		type = JOINT_SLIDER;
	}
};

/// �{�[���W���C���g�̃f�B�X�N���v�^
struct PHBallJointDesc : public PHJointDesc{
	double	max_angle;		/// �~����̉��͈�
	PHBallJointDesc(){
		type = JOINT_BALL;
		max_angle = 0.0;
	}
};

/// �S���̃C���^�t�F�[�X
struct PHConstraintIf : public SceneObjectIf{
	IF_DEF(PHConstraint);

	/** @brief �S����L��/����������
		@param bEnable true�Ȃ�ΗL�����Cfalse�Ȃ�Ζ���������
	 */
	virtual void Enable(bool bEnable = true) = 0;

	/** @brief �L�������������擾����
		@return �L���Ȃ��true, �����Ȃ��false
	 */
	virtual bool IsEnabled() = 0;

	/** @brief �S���̎�ނ��擾����
		@return �S���̎��
	 */
	virtual PHJointDesc::JointType GetJointType() = 0;
};

/// �ڐG�_�S���̃C���^�t�F�[�X
struct PHContactPointIf : public PHConstraintIf{
	IF_DEF(PHContactPoint);
};

/// �֐߂̃C���^�t�F�[�X
struct PHJointIf : public PHConstraintIf{
	IF_DEF(PHJoint);
};

/// 1���֐߂̃C���^�t�F�[�X
struct PHJoint1DIf : public PHJointIf{
	IF_DEF(PHJoint1D);

	/** @brief ���͈͂�ݒ肷��
		@param lower ���͈͂̉���
		@param upper ���͈͂̏��
	 */
	virtual void	SetRange(double lower, double upper) = 0;
	
	/** @brief ���͈͂��擾����
		@param lower ���͈͂̉������擾����ϐ��ւ̎Q��
		@param upper ���͈͂̏�����擾����ϐ��ւ̎Q��
	 */
	virtual void	GetRange(double& lower, double& upper) = 0;

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	virtual void	SetMotorTorque(double t) = 0;

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	virtual double	GetMotorTorque() = 0;

	//virtual void	SetDesiredPosition(double p, double t) = 0;	/// �ڕW�ψʂ�ݒ肷��
	//virtual double	GetDesiredPosition() = 0;				/// �ڕW�ψʂ��擾����

	/** @brief �ڕW���x��ݒ肷��
		@param vel �ڕW���x
	 */
	virtual void	SetDesiredVelocity(double v) = 0;

	/** @brief �ڕW���x���擾����
		@return �ڕW���x
	 */
	virtual double	GetDesiredVelocity() = 0;

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	virtual void	SetSpring(double spring) = 0;

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	virtual double	GetSpring() = 0;

	/** @brief �o�l�̌��_��ݒ肷��
		@param origin �o�l���_

		�o�l���_�Ƃ̓o�l�̗͂�0�ƂȂ�֐ߕψʂ̂��ƁD
	 */
	virtual void	SetSpringOrigin(double origin) = 0;

	/** @brief �o�l�̌��_���擾����
		@return �o�l���_
	 */
	virtual double	GetSpringOrigin() = 0;

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	virtual void	SetDamper(double damper) = 0;

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	virtual double	GetDamper() = 0;

	/** @brief �֐ߕψʂ��擾����
		@return �֐ߕψ�
	 */
	virtual double	GetPosition() = 0;

	/** @brief �֐ߑ��x���擾����
		@return �֐ߑ��x
	 */
	virtual double	GetVelocity() = 0;
};

/// �q���W�̃C���^�t�F�[�X
struct PHHingeJointIf : public PHJoint1DIf{
	IF_DEF(PHHingeJoint);
};

/// �X���C�_�̃C���^�t�F�[�X
struct PHSliderJointIf : public PHJoint1DIf{
	IF_DEF(PHSliderJoint);
};

/// �{�[���W���C���g�̃C���^�t�F�[�X
struct PHBallJointIf : public PHConstraintIf{
	IF_DEF(PHBallJoint);
};

}

#endif
