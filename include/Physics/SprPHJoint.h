/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
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
	/*enum ConstraintType{
		INVALID_CONSTRAINT,
		CONTACT,
		HINGEJOINT,
		SLIDERJOINT,
		BALLJOINT,
		PATHJOINT,
		SPRING
	} type;*/
	/// �L��/�����t���O
	bool bEnabled;
	/// ���̂��猩���֐߂̈ʒu�ƌX��
	Posed poseSocket;
	Posed posePlug;
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
	double	desiredVelocity; ///< �ڕW���x
	double	torque;			///< ���[�^�g���N
	double	fMax;			///< �֐߂ɂ�������ő�̗�
	double	fMin;			///< �֐߂ɂ�������ŏ��̗�

	PHJoint1DDesc():lower(0.0), upper(0.0), spring(0.0), origin(0.0), damper(0.0), desiredVelocity(0.0), torque(0.0), fMax(FLT_MAX), fMin(-FLT_MAX){}
};


/** @brief �c���[�m�[�h�̃f�B�X�N���v�^
 */
struct PHTreeNodeDesc{
	bool bEnabled;
	PHTreeNodeDesc(){
		bEnabled = true;
	}
};
struct PHRootNodeDesc : public PHTreeNodeDesc{
	PHRootNodeDesc(){}
};
struct PHTreeNode1DDesc : public PHTreeNodeDesc{
};
struct PHHingeJointNodeDesc : public PHTreeNode1DDesc{
};
struct PHSliderJointNodeDesc : public PHTreeNode1DDesc{
};
struct PHPathJointNodeDesc : public PHTreeNode1DDesc{
};
struct PHBallJointNodeDesc : public PHTreeNodeDesc{
};

/// �M�A�̃f�B�X�N���v�^
struct PHGearDesc{
	double ratio;		///< �M�A��
	PHGearDesc(){
		ratio = 1.0;
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

	/** @brief �\�P�b�g�̈ʒu�E�������擾�E�ݒ肷��
	 */
	virtual void GetSocketPose(Posed& pose) = 0;
	virtual void SetSocketPose(const Posed& pose) = 0;

	/** @brief �v���O�̈ʒu�E�������擾�E�ݒ肷��
	 */
	virtual void GetPlugPose(Posed& pose) = 0;
	virtual void SetPlugPose(const Posed& pose) = 0;
	
	/** @brief �S���̎�ނ��擾����
		@return �S���̎��
	 */
	//virtual PHConstraintDesc::ConstraintType GetConstraintType() = 0;

	/** @brief �S�����鍄�̊Ԃ̑��Έʒu�E�p�����擾
		@param p �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���
	 */
	virtual void GetRelativePose(Posed& p) = 0;
	
	/** @brief �S�����鍄�̊Ԃ̑��Έʒu���擾
	*/
	virtual Vec3d GetRelativePoseR() = 0;

	/** @brief �S�����鍄�̊Ԃ̑��Ύp�����擾
	*/
	virtual Quaterniond GetRelativePoseQ() = 0;
	
	/** @brief �S�����鍄�̊Ԃ̑��Α��x
		@param v �\�P�b�g�ɑ΂���v���O�̑��x
		@param w �\�P�b�g�ɑ΂���v���O�̊p���x
	 */
	virtual void GetRelativeVelocity(Vec3d& v, Vec3d& w) = 0;

	/** @brief �S���͂��擾
		@param f ���i��
		@param t ���[�����g
		�S���͂ƍS���g���N���\�P�b�g�̃��[�J�����W�n�ŕԂ�
	 */
	virtual void GetConstraintForce(Vec3d& f, Vec3d& t) = 0;
};

/// �S���̏W���̃C���^�t�F�[�X
struct PHConstraintsIf : public SceneObjectIf{
	IF_DEF(PHConstraints);

	/** @brief �w�肳�ꂽ���̂̑g�ɍ�p���Ă���S����Ԃ�
		@param lhs ���̂��̂P
		@param rhs ���̂��̂Q
		@return ���̊Ԃɍ�p���Ă���S��
	*/
	virtual PHConstraintIf* FindBySolidPair(PHSolidIf* lhs, PHSolidIf* rhs) = 0;
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
	
	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)��ݒ肷��
	*/
	virtual void SetTorqueMax(double max) = 0;

	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)���擾����
	*/
	virtual double GetTorqueMax() = 0;

		/** @brief �֐߂̏o�����Ƃ��ł���ŏ��g���N(N)��ݒ肷��
	*/
	virtual void SetTorqueMin(double min) = 0;

	/** @brief �֐߂̏o�����Ƃ��ł���ŏ��g���N(N)���擾����
	*/
	virtual double GetTorqueMin() = 0;

};

/// �q���W�̃C���^�t�F�[�X
struct PHHingeJointIf : public PHJoint1DIf{
	IF_DEF(PHHingeJoint);
};
/// �q���W�̃f�B�X�N���v�^
struct PHHingeJointDesc : public PHJoint1DDesc{
	DESC_DEF_FOR_OBJECT(PHHingeJoint);
	PHHingeJointDesc(){}
};

/// �X���C�_�̃C���^�t�F�[�X
struct PHSliderJointIf : public PHJoint1DIf{
	IF_DEF(PHSliderJoint);
};
/// �X���C�_�̃f�B�X�N���v�^
struct PHSliderJointDesc : public PHJoint1DDesc{
	DESC_DEF_FOR_OBJECT(PHSliderJoint);
	PHSliderJointDesc(){}
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
/// �p�X�̃C���^�t�F�[�X
struct PHPathIf : public SceneObjectIf{
	IF_DEF(PHPath);
	/** @brief �p�X�ɓ_��ǉ�����
		@param s �֐ߍ��W
		@param pose �\�P�b�g�ɑ΂���v���O�̎p��
		�p�X�ɐ���_��ǉ�����D
		���Ȃ킿�C�֐ߍ��W��s�ł���Ƃ��̃\�P�b�g-�v���O�Ԃ̎p����pose�ɐݒ肷��D
	 */
	virtual void AddPoint(double s, const Posed& pose) = 0;

	/** @brief ���[�v�p�X���I�[�v���p�X����ݒ肷��
		@param bOnOff true�Ȃ�΃��[�v�p�X�Cfalse�Ȃ�΃I�[�v���p�X
		���[�v�p�X�̏ꍇ�C�֐ߍ��W���ŏ��̐���_�ƍő�̐���_���V�[�����X�ɂȂ��D
		�I�[�v���p�X�̏ꍇ�C�֐ߍ��W���ŏ��C�ő�̐���_�����ꂼ����͈͂̉����C����ƂȂ�D
	 */
	virtual void SetLoop(bool bOnOff = true) = 0;
	/** @brief ���[�v�p�X���I�[�v���p�X�����擾����
		@return true�Ȃ�΃��[�v�p�X�Cfalse�Ȃ�΃I�[�v���p�X
	 */
	virtual bool IsLoop() = 0;
};

/// �p�X�W���C���g�̃C���^�t�F�[�X
struct PHPathJointIf : public PHJoint1DIf{
	IF_DEF(PHPathJoint);
	virtual void SetPosition(double q)=0;
};
/// �p�X�W���C���g�̃f�B�X�N���v�^
struct PHPathJointDesc : public PHJoint1DDesc{
	DESC_DEF_FOR_OBJECT(PHPathJoint);
	PHPathJointDesc(){}
};

/// �{�[���W���C���g�̃C���^�t�F�[�X
struct PHBallJointIf : public PHConstraintIf{
	IF_DEF(PHBallJoint);

	
	/** @brief �X�C���O�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��X�C���O�p�x
		@param upper �ő�X�C���O�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	virtual void SetSwingRange(Vec2d range) = 0;
	/** @brief �X�C���O�p�̉��͈͂��擾����
		@param lower �ő�X�C���O�p�x
		@param upper �ő�X�C���O�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	virtual void GetSwingRange(Vec2d& range) = 0;

	/** @brief �c�C�X�g�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	virtual void SetTwistRange(Vec2d range) = 0;
	/** @brief �c�C�X�g�p�̉��͈͂��擾����
		@param lower �ő�c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	virtual void GetTwistRange(Vec2d& range) = 0;

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	virtual void SetMotorTorque(const Vec3d& torque)=0;

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	virtual Vec3d GetMotorTorque()=0;

	/** @brief �֐ߕψʂ��擾����
		@return �X�C���O���ʊp�C�X�C���O�p�C�c�C�X�g�p����Ȃ�x�N�g��
	 */
	virtual Vec3d GetAngle() = 0;

	/** @brief �֐ߑ��x���擾����
		@return �X�C���O���ʊp�C�X�C���O�p�C�c�C�X�g�p�̎��ԕω�������Ȃ�x�N�g��
	 */
	virtual Vec3d GetVelocity() = 0;

	/** @brief �֐߂̏o����ő�g���N��ݒ肷��
		@param max �ő�g���N
	*/
	virtual void SetTorqueMax(double max) = 0;

	/** @brief �֐߂̏o����ő�g���N���擾����
	*/
	virtual double GetTorqueMax() = 0;

	/** @brief �֐߂̏o����ŏ��g���N��ݒ肷��
		@param min �ŏ��g���N
	*/
	virtual void SetTorqueMin(double min) = 0;

	/** @brief �֐߂̏o����ŏ��g���N���擾����
	*/
	virtual double GetTorqueMin() = 0;


};
/// �{�[���W���C���g�̃f�B�X�N���v�^
struct PHBallJointDesc : public PHJointDesc{
	DESC_DEF_FOR_OBJECT(PHBallJoint);
	double			spring;		///< �o�l�W��
	double			damper;		///< �_���p�W��
	Vec2d			limitSwing; ///< swing�p�̉���i[0] or .lower, [1] or .upper�j
	Vec2d			limitTwist;	///< twist�p�̉���i[0] or .lower, [1] or .upper�j
	Vec3d			limitDir;	///< ����̒��S�x�N�g��
	Quaterniond		goal;		///< �o�l�_���p�̐���ڕW
	Vec3d			torque;		///< ���[�^�g���N
	double			fMax;		///< �֐߂ɂ�������ő�̗�
	double			fMin;		///< �֐߂ɂ�������ŏ��̗�
	
	PHBallJointDesc();		///< �f�B�X�N���v�^�̃R���X�g���N�^
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

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	virtual void SetSpringOri(const double spring)=0;

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	virtual double GetSpringOri()=0;

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	virtual void SetDamperOri(const double damper)=0;

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	virtual double GetDamperOri()=0;
};
/// �o�l�_���p�̃f�B�X�N���v�^
struct PHSpringDesc : public PHConstraintDesc{
	DESC_DEF_FOR_OBJECT(PHSpring);
	Vec3d		spring;		///< �o�l�W��
	Vec3d		damper;		///< �_���p�W��
	double		springOri;
	double		damperOri;
	PHSpringDesc();
};

/// �c���[�m�[�h�̃C���^�t�F�[�X
struct PHTreeNodeIf : public SceneObjectIf{
	IF_DEF(PHTreeNode);
	virtual void Enable(bool bEnable = true)=0;
};
/// ���[�g�m�[�h�̃C���^�t�F�[�X
struct PHRootNodeIf : public PHTreeNodeIf{
	IF_DEF(PHRootNode);
};
/// �P���֐߃m�[�h�̃C���^�t�F�[�X
struct PHTreeNode1DIf : public PHTreeNodeIf{
	IF_DEF(PHTreeNode1D);
};
struct PHHingeJointNodeIf : public PHTreeNode1DIf{
	IF_DEF(PHHingeJointNode);
};
struct PHSliderJointNodeIf : public PHTreeNode1DIf{
	IF_DEF(PHSliderJointNode);
};
struct PHPathJointNodeIf : public PHTreeNode1DIf{
	IF_DEF(PHPathJointNode);
};
struct PHBallJointNodeIf : public PHTreeNodeIf{
	IF_DEF(PHBallJointNode);
};

/// �M�A�̃C���^�t�F�[�X
struct PHGearIf : public SceneObjectIf{
	IF_DEF(PHGear);
};
	
//@}
//@}

}

#endif
