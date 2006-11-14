/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHJoint.h
 *	@brief �ڐG��֐߂Ȃ�
*/
#ifndef SPR_PHJOINTIf_H
#define SPR_PHJOINTIf_H
#include <Foundation/SprObject.h>

namespace Spr{;

/** \addtogroup gpPhysics */
//@{
/** \defgroup gpJoint �W���C���g*/
//@{

/// �S���̃f�B�X�N���v�^
struct PHConstraintDesc{
	/// �S���̎��
	enum ConstraintType{
		CONTACT,
		HINGEJOINT,
		SLIDERJOINT,
		BALLJOINT,
		UNIVERSALJOINT,
		PATHJOINT,
		SPRING
	} type;
	/// �L��/�����t���O
	bool bEnabled;
	/// ���̂��猩���֐߂̈ʒu�ƌX��
	Posed posePlug, poseSocket;
	PHConstraintDesc():bEnabled(true){}
};

/// �֐߂̃f�B�X�N���v�^	<	���������o��ǉ����Ȃ��ꍇ�́Ctypedef�ƕʖ���`�� FIDesc.cpp�ɒǉ��ŁD
typedef PHConstraintDesc	PHJointDesc;

/// 1���֐߂̃f�B�X�N���v�^
struct PHJoint1DDesc : public PHJointDesc{
	double	lower, upper;	///< ���͈�. lower < upper�̂Ƃ��ɗL���ƂȂ�
	double	spring;			///< �o�l�W��
	double  origin;			///< �o�l���_
	double  damper;			///< �_���p�W��
	double	torque;			///< ���[�^�g���N

	PHJoint1DDesc():lower(0.0), upper(0.0), spring(0.0), origin(0.0), damper(0.0), torque(0.0){}
};

/// �q���W�̃f�B�X�N���v�^
struct PHHingeJointDesc : public PHJoint1DDesc{
	PHHingeJointDesc(){
		type = HINGEJOINT;
	}
};

/// �X���C�_�̃f�B�X�N���v�^
struct PHSliderJointDesc : public PHJoint1DDesc{
	PHSliderJointDesc(){
		type = SLIDERJOINT;
	}
};

/// �p�X���1�̓_
struct PHPathPoint{
	double	s;			///< �X�J���[�p�����[�^
	Posed	pose;		///< �ʒu�ƌX��
};
/// �p�X�̃f�B�X�N���v�^
struct PHPathDesc{
	std::vector<PHPathPoint> points;	///< �p�X��̓_��
	bool bLoop;							///< true�Ȃ�΃��[�v�p�X�Cfalse�Ȃ�΃I�[�v���p�X�D�f�t�H���g��false�D
	PHPathDesc():bLoop(false){}
};

/// �p�X�W���C���g�̃f�B�X�N���v�^
struct PHPathJointDesc : public PHJoint1DDesc{
	PHPathJointDesc(){
		type = PATHJOINT;
	}
};

/// �{�[���W���C���g�̃f�B�X�N���v�^
struct PHBallJointDesc : public PHJointDesc{
	double	max_angle;		///< �~����̉��͈�
	Vec3d	torque;			///< ���[�^�g���N
	PHBallJointDesc(){
		type = BALLJOINT;
		max_angle = 0.0;
	}
};

/// ���j�o�[�T���W���C���g�̃f�B�X�N���v�^
struct PHUniversalJointDesc : public PHJointDesc{
	Vec2d	torque;			///< ���[�^�g���N
	PHUniversalJointDesc(){
		type = UNIVERSALJOINT;
	}
};

/// �o�l�_���p�̃f�B�X�N���v�^
struct PHSpringDesc : public PHConstraintDesc{
	Vec3d spring;		///< �o�l�W��
	Vec3d damper;		///< �_���p�W��
	PHSpringDesc(){
		type = SPRING;
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
	virtual PHConstraintDesc::ConstraintType GetConstraintType() = 0;
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

/// �p�X�̃C���^�t�F�[�X
struct PHPathIf : public SceneObjectIf{
	IF_DEF(PHPath);
	virtual void AddPoint(double s, const Posed& pose) = 0;
	virtual void SetLoop(bool bOnOff = true) = 0;
	virtual bool IsLoop() = 0;
};

/// �p�X�W���C���g�̃C���^�t�F�[�X
struct PHPathJointIf : public PHJoint1DIf{
	IF_DEF(PHPathJoint);
	virtual void SetPosition(double q)=0;
};

/// �{�[���W���C���g�̃C���^�t�F�[�X
struct PHBallJointIf : public PHConstraintIf{
	IF_DEF(PHBallJoint);

	/** @brief �ő�p�x��ݒ肷��
		@param angle �ő�p�x
	 */
	virtual void SetMaxAngle(double angle) = 0;

	/** @brief �ő�p�x���擾����
		@return �ő�p�x
	 */
	virtual double GetMaxAngle() = 0;

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	virtual void SetMotorTorque(const Vec3d& torque)=0;

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	virtual Vec3d GetMotorTorque()=0;

	/** @brief �֐ߕψʂ��擾����
		@return �֐ߕψ�
	 */
	virtual Quaterniond	GetPosition() = 0;

	/** @brief �֐ߑ��x���擾����
		@return �֐ߑ��x
	 */
	virtual Vec3d GetVelocity() = 0;

};

/// ���j�o�[�T���W���C���g�̃C���^�t�F�[�X
struct PHUniversalJointIf : public PHConstraintIf{
	IF_DEF(PHUniversalJoint);

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	virtual void SetMotorTorque(const Vec2d& torque)=0;

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	virtual Vec2d GetMotorTorque()=0;
};

/// �o�l�_���p�̃C���^�t�F�[�X
struct PHSpringIf : public PHConstraintIf{
	IF_DEF(PHSpring);
	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	virtual void SetSpring(const Vec3d& spring)=0;

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	virtual Vec3d GetSpring()=0;

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	virtual void SetDamper(const Vec3d& damper)=0;

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	virtual Vec3d GetDamper()=0;

};

///
struct PHTreeNodeIf : public SceneObjectIf{
	IF_DEF(PHTreeNode);
};

///
struct PHRootNodeIf : public PHTreeNodeIf{
	IF_DEF(PHRootNode);
};
	
//@}
//@}

}

#endif
