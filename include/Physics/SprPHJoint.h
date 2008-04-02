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
	SPR_IFDEF(PHConstraint);

	/** @brief �S����L��/����������
		@param bEnable true�Ȃ�ΗL�����Cfalse�Ȃ�Ζ���������
	 */
	void Enable(bool bEnable = true);

	/** @brief �L�������������擾����
		@return �L���Ȃ��true, �����Ȃ��false
	 */
	bool IsEnabled();

	/** @brief �\�P�b�g�̈ʒu�E�������擾�E�ݒ肷��
	 */
	void GetSocketPose(Posed& pose);
	void SetSocketPose(const Posed& pose);

	/** @brief �v���O�̈ʒu�E�������擾�E�ݒ肷��
	 */
	void GetPlugPose(Posed& pose);
	void SetPlugPose(const Posed& pose);
	
	/** @brief �S���̎�ނ��擾����
		@return �S���̎��
	 */
	//PHConstraintDesc::ConstraintType GetConstraintType();

	/** @brief �S�����鍄�̊Ԃ̑��Έʒu�E�p�����擾
		@param p �\�P�b�g�ɑ΂���v���O�̈ʒu�ƌ���
	 */
	void GetRelativePose(Posed& p);
	
	/** @brief �S�����鍄�̊Ԃ̑��Έʒu���擾
	*/
	Vec3d GetRelativePoseR();

	/** @brief �S�����鍄�̊Ԃ̑��Ύp�����擾
	*/
	Quaterniond GetRelativePoseQ();
	
	/** @brief �S�����鍄�̊Ԃ̑��Α��x
		@param v �\�P�b�g�ɑ΂���v���O�̑��x
		@param w �\�P�b�g�ɑ΂���v���O�̊p���x
	 */
	void GetRelativeVelocity(Vec3d& v, Vec3d& w);

	/** @brief �S���͂��擾
		@param f ���i��		: PHConstraint::f.v()
		@param t ���[�����g : PHConstraint::f.w()
		�S���͂ƍS���g���N���\�P�b�g�̃��[�J�����W�n�ŕԂ�
	 */
	void GetConstraintForce(Vec3d& f, Vec3d& t);
};

/// �S���̏W���̃C���^�t�F�[�X
struct PHConstraintsIf : public SceneObjectIf{
	SPR_IFDEF(PHConstraints);

	/** @brief �w�肳�ꂽ���̂̑g�ɍ�p���Ă���S����Ԃ�
		@param lhs ���̂��̂P
		@param rhs ���̂��̂Q
		@return ���̊Ԃɍ�p���Ă���S��
	*/
	PHConstraintIf* FindBySolidPair(PHSolidIf* lhs, PHSolidIf* rhs);
};

/// �ڐG�_�S���̃C���^�t�F�[�X
struct PHContactPointIf : public PHConstraintIf{
	SPR_IFDEF(PHContactPoint);
};

/// �֐߂̃C���^�t�F�[�X
struct PHJointIf : public PHConstraintIf{
	SPR_IFDEF(PHJoint);
};

/// 1���֐߂̃C���^�t�F�[�X
struct PHJoint1DIf : public PHJointIf{
	SPR_IFDEF(PHJoint1D);

	/** @brief ���͈͂�ݒ肷��
		@param lower ���͈͂̉���
		@param upper ���͈͂̏��
	 */
	void	SetRange(double lower, double upper);
	
	/** @brief ���͈͂��擾����
		@param lower ���͈͂̉������擾����ϐ��ւ̎Q��
		@param upper ���͈͂̏�����擾����ϐ��ւ̎Q��
	 */
	void	GetRange(double& lower, double& upper);

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	void	SetMotorTorque(double t);

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	double	GetMotorTorque();

	//void	SetDesiredPosition(double p, double t);	/// �ڕW�ψʂ�ݒ肷��
	//double	GetDesiredPosition();				/// �ڕW�ψʂ��擾����

	/** @brief �ڕW���x��ݒ肷��
		@param vel �ڕW���x
	 */
	void	SetDesiredVelocity(double v);

	/** @brief �ڕW���x���擾����
		@return �ڕW���x
	 */
	double	GetDesiredVelocity();

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void	SetSpring(double spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double	GetSpring();

	/** @brief �o�l�̌��_��ݒ肷��
		@param origin �o�l���_

		�o�l���_�Ƃ̓o�l�̗͂�0�ƂȂ�֐ߕψʂ̂��ƁD
	 */
	void	SetSpringOrigin(double origin);

	/** @brief �o�l�̌��_���擾����
		@return �o�l���_
	 */
	double	GetSpringOrigin();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void	SetDamper(double damper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	double	GetDamper();

	/** @brief �֐ߕψʂ��擾����
		@return �֐ߕψ�
	 */
	double	GetPosition();

	/** @brief �֐ߑ��x���擾����
		@return �֐ߑ��x
	 */
	double	GetVelocity();
	
	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)��ݒ肷��
	*/
	void SetTorqueMax(double max);

	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)���擾����
	*/
	double GetTorqueMax();

		/** @brief �֐߂̏o�����Ƃ��ł���ŏ��g���N(N)��ݒ肷��
	*/
	void SetTorqueMin(double min);

	/** @brief �֐߂̏o�����Ƃ��ł���ŏ��g���N(N)���擾����
	*/
	double GetTorqueMin();

};

/// �q���W�̃C���^�t�F�[�X
struct PHHingeJointIf : public PHJoint1DIf{
	SPR_IFDEF(PHHingeJoint);
};
/// �q���W�̃f�B�X�N���v�^
struct PHHingeJointDesc : public PHJoint1DDesc{
	SPR_DESCDEF(PHHingeJoint);
	PHHingeJointDesc(){}
};

/// �X���C�_�̃C���^�t�F�[�X
struct PHSliderJointIf : public PHJoint1DIf{
	SPR_IFDEF(PHSliderJoint);
};
/// �X���C�_�̃f�B�X�N���v�^
struct PHSliderJointDesc : public PHJoint1DDesc{
	SPR_DESCDEF(PHSliderJoint);
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
	SPR_IFDEF(PHPath);
	/** @brief �p�X�ɓ_��ǉ�����
		@param s �֐ߍ��W
		@param pose �\�P�b�g�ɑ΂���v���O�̎p��
		�p�X�ɐ���_��ǉ�����D
		���Ȃ킿�C�֐ߍ��W��s�ł���Ƃ��̃\�P�b�g-�v���O�Ԃ̎p����pose�ɐݒ肷��D
	 */
	void AddPoint(double s, const Posed& pose);

	/** @brief ���[�v�p�X���I�[�v���p�X����ݒ肷��
		@param bOnOff true�Ȃ�΃��[�v�p�X�Cfalse�Ȃ�΃I�[�v���p�X
		���[�v�p�X�̏ꍇ�C�֐ߍ��W���ŏ��̐���_�ƍő�̐���_���V�[�����X�ɂȂ��D
		�I�[�v���p�X�̏ꍇ�C�֐ߍ��W���ŏ��C�ő�̐���_�����ꂼ����͈͂̉����C����ƂȂ�D
	 */
	void SetLoop(bool bOnOff = true);
	/** @brief ���[�v�p�X���I�[�v���p�X�����擾����
		@return true�Ȃ�΃��[�v�p�X�Cfalse�Ȃ�΃I�[�v���p�X
	 */
	bool IsLoop();
};

/// �p�X�W���C���g�̃C���^�t�F�[�X
struct PHPathJointIf : public PHJoint1DIf{
	SPR_IFDEF(PHPathJoint);
	void SetPosition(double q);
};
/// �p�X�W���C���g�̃f�B�X�N���v�^
struct PHPathJointDesc : public PHJoint1DDesc{
	SPR_DESCDEF(PHPathJoint);
	PHPathJointDesc(){}
};

/// �{�[���W���C���g�̃C���^�t�F�[�X
struct PHBallJointIf : public PHConstraintIf{
	SPR_IFDEF(PHBallJoint);

	
	/** @brief �X�C���O�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��X�C���O�p�x
		@param upper �ő�X�C���O�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	void SetSwingRange(Vec2d range);
	/** @brief �X�C���O�p�̉��͈͂��擾����
		@param lower �ő�X�C���O�p�x
		@param upper �ő�X�C���O�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	void GetSwingRange(Vec2d& range);

	/** @brief �c�C�X�g�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	void SetTwistRange(Vec2d range);
	/** @brief �c�C�X�g�p�̉��͈͂��擾����
		@param lower �ő�c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	void GetTwistRange(Vec2d& range);

	/** @brief ���[�^�g���N��ݒ肷��
		@param torque ���[�^�g���N
	 */
	void SetMotorTorque(const Vec3d& torque);

	/** @brief ���[�^�g���N���擾����
		@return ���[�^�g���N
	 */
	Vec3d GetMotorTorque();

	/** @brief �֐ߕψʂ��擾����
		@return �X�C���O���ʊp�C�X�C���O�p�C�c�C�X�g�p����Ȃ�x�N�g��
	 */
	Vec3d GetAngle();

	/** @brief �֐ߑ��x���擾����
		@return �X�C���O���ʊp�C�X�C���O�p�C�c�C�X�g�p�̎��ԕω�������Ȃ�x�N�g��
	 */
	Vec3d GetVelocity();

	/** @brief �֐߂̏o����ő�g���N��ݒ肷��
		@param max �ő�g���N
	*/
	void SetTorqueMax(double max);

	/** @brief �֐߂̏o����ő�g���N���擾����
	*/
	double GetTorqueMax();

	/** @brief �֐߂̏o����ŏ��g���N��ݒ肷��
		@param min �ŏ��g���N
	*/
	void SetTorqueMin(double min);

	/** @brief �֐߂̏o����ŏ��g���N���擾����
	*/
	double GetTorqueMin();


};
/// �{�[���W���C���g�̃f�B�X�N���v�^
struct PHBallJointDesc : public PHJointDesc{
	SPR_DESCDEF(PHBallJoint);
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
	SPR_IFDEF(PHSpring);
	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void SetSpring(const Vec3d& spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	Vec3d GetSpring();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void SetDamper(const Vec3d& damper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	Vec3d GetDamper();

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void SetSpringOri(const double spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double GetSpringOri();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void SetDamperOri(const double damper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	double GetDamperOri();
};
/// �o�l�_���p�̃f�B�X�N���v�^
struct PHSpringDesc : public PHConstraintDesc{
	SPR_DESCDEF(PHSpring);
	Vec3d		spring;		///< �o�l�W��
	Vec3d		damper;		///< �_���p�W��
	double		springOri;
	double		damperOri;
	PHSpringDesc();
};

/// �c���[�m�[�h�̃C���^�t�F�[�X
struct PHTreeNodeIf : public SceneObjectIf{
	SPR_IFDEF(PHTreeNode);
	void Enable(bool bEnable = true);
};
/// ���[�g�m�[�h�̃C���^�t�F�[�X
struct PHRootNodeIf : public PHTreeNodeIf{
	SPR_IFDEF(PHRootNode);
};
/// �P���֐߃m�[�h�̃C���^�t�F�[�X
struct PHTreeNode1DIf : public PHTreeNodeIf{
	SPR_IFDEF(PHTreeNode1D);
};
struct PHHingeJointNodeIf : public PHTreeNode1DIf{
	SPR_IFDEF(PHHingeJointNode);
};
struct PHSliderJointNodeIf : public PHTreeNode1DIf{
	SPR_IFDEF(PHSliderJointNode);
};
struct PHPathJointNodeIf : public PHTreeNode1DIf{
	SPR_IFDEF(PHPathJointNode);
};
struct PHBallJointNodeIf : public PHTreeNodeIf{
	SPR_IFDEF(PHBallJointNode);
};

/// �M�A�̃C���^�t�F�[�X
struct PHGearIf : public SceneObjectIf{
	SPR_IFDEF(PHGear);
};
	
//@}
//@}

}

#endif
