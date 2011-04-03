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

struct PHConstraintEngineDesc{
	int		numIter;					///< ���x�X�VLCP�̔�����
	int		numIterCorrection;			///< �덷�C��LCP�̔�����
	int		numIterContactCorrection;	///< �ڐG�_�̌덷�C��LCP�̔�����
	double	velCorrectionRate;			///< ���x��LCP�Ŋ֐ߍS���̌덷���C������ꍇ�̌덷�C���䗦
	double	posCorrectionRate;			///< �ʒu��LCP�ŁC�֐ߍS���̌덷���C������ꍇ�̌덷�C���䗦
	double  contactCorrectionRate;		///< �ڐG�̐N�������̂��߂́C���x��LCP�ł̕␳�䗦�D
	double	shrinkRate;					///< LCP�����l��O��̉��ɑ΂��ďk��������䗦
	double	shrinkRateCorrection;
	double	freezeThreshold;			///< ���̂��t���[�Y����臒l
	double	accelSOR;					///< SOR�@�̉����W��
	bool	bGearNodeReady;				///< �M�A�m�[�h�����܂��\������Ă��邩�̃t���O�D�m�[�h��M�A��ǉ��E�폜���邽�т�false�ɂȂ�
	bool	bSaveConstraints;			///< SaveState, LoadState�ɁC constraints ���܂߂邩�ǂ����D�{���s�v�����Cf, F���ω�����D
	bool	bUpdateAllState;			///< ���̂̑��x�C�ʒu�̑S�Ă̏�Ԃ��X�V����D
	bool	bUseContactSurface;			///< �ʐڐG���g��
	PHConstraintEngineDesc();
};

/** \defgroup gpJoint �W���C���g*/
//@{

/// �S���̃f�B�X�N���v�^
struct PHConstraintDesc{
	/// �L��/�����t���O
	bool bEnabled;
	/// ���̂��猩���֐߂̈ʒu�ƌX��
	Posed poseSocket;
	Posed posePlug;
	PHConstraintDesc():bEnabled(true){}
};

/// �֐߂̃f�B�X�N���v�^	<	���������o��ǉ����Ȃ��ꍇ�́Ctypedef�ƕʖ���`�� FIDesc.cpp�ɒǉ��ŁD
//typedef PHConstraintDesc	PHJointDesc;
struct PHJointDesc : public PHConstraintDesc{
	/*enum PHControlMode{
		MODE_TORQUE=0,
		MODE_POSITION,
		MODE_VELOCITY,
		MODE_TRAJ,
	} mode;*/
	enum PHDeformationType{
		ELASTIC =0,
		PLASTIC =1,
		ELASTIC_PLASTIC =2
	}type;

		PHJointDesc();	
};
/// 1���֐߂̃f�B�X�N���v�^
struct PHJoint1DDesc : public PHJointDesc{
	double	lower, upper;	///< ���͈�. lower < upper�̂Ƃ��ɗL���ƂȂ�
	double	spring;			///< �o�l�W��
	double  targetPosition;	///< �o�l�̐���ڕW
	double  damper;			///< �_���p�W��
	double	targetVelocity; ///< �ڕW���x
	double	offsetForce;	///< ���������v�Z���ē����ꍇ�Ɏg�p�D
	//double	torque;		///< ���[�^�g���N
	double	rangeSpring;	///< ���͈̓o�l
	double	rangeDamper;	///< ���͈̓_���p
	double	fMax;			///< �֐߂ɂ�������ő�̗�
	double	secondDamper;	///< ��ڂ̃_���p�W��
	double  yieldStress;	///< �~������
	double  hardnessRate;	///< �~�����͈ȉ��̏ꍇ�ɓ�ڂ̃_���p�W���Ɋ|����䗦
	PHJoint1DDesc();
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

struct PHSceneIf;
/// �S���̃C���^�t�F�[�X
struct PHConstraintIf : public SceneObjectIf{
	SPR_IFDEF(PHConstraint);

	/** @brief �\�P�b�g���̍��̂��擾����
	 */
	PHSolidIf* GetSocketSolid();

	/** @brief �v���O���̍��̂��擾����
	 */
	PHSolidIf* GetPlugSolid();

	/** @brief ��������V�[���̎擾	 */
	PHSceneIf* GetScene() const;

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
	
	/** @biref �S�����鍄�̊Ԃ̃��[���h���W�n�Ō����������擾
	*/
	Quaternionf GetAbsolutePoseQ();
	
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

	/** @brief PHMotor�̍S���͂��擾
		@return �S�����[�����g
	*/
	Vec3d GetMotorf();

	/** @brief PHJointLimit�̍S���͂��擾
		@return �S�����[�����g
	*/
	Vec3d GetLimitf();

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

	/** @brief �w�肳�ꂽ���̂̑g�ɍ�p���Ă��鑍���I�ȗ͂�Ԃ�
		@param lhs ���̂��̂P
		@param rhs ���̂��̂Q
		@return ���̊Ԃɍ�p���Ă��鑍���I�ȗ�
	*/
	Vec3d GetTotalForce(PHSolidIf* lhs, PHSolidIf* rhs);
};

/// �ڐG�_�S���̃C���^�t�F�[�X
struct PHContactPointIf : public PHConstraintIf{
	SPR_IFDEF(PHContactPoint);
};

/// �֐߂̃C���^�t�F�[�X
struct PHJointIf : public PHConstraintIf{
	SPR_IFDEF(PHJoint);

	/**�֐߂�ControlMode�̎擾,�ݒ肷��*/
	//PHJointDesc::PHControlMode	GetMode();
	//void	SetMode(PHJointDesc::PHControlMode mode);

	/** @brief �ό`�̃^�C�v���擾����
		@return 0:ELASTIC,1:PLASTIC,2:ELASTIC_PLASTIC
	 */
	int		GetDefomationType();	
	/** @brief �ό`�̃^�C�v��ݒ肷��
		@param input 0:ELASTIC,1:PLASTIC,2:ELASTIC_PLASTIC
	 */
	void	SetDefomationType(int t);	


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
	void	SetTargetVelocity(double v);

	/** @brief �ڕW���x���擾����
		@return �ڕW���x
	 */
	double	GetTargetVelocity();
	
	/** @brief �ڕW�O���̑��x��ݒ肷��
		@param v �ڕW�O���̑��x
	*/
	void SetTrajectoryVelocity(double v);

	/** @brief �ڕW�O���̑��x���擾����
		@return �ڕW�O���̑��x
	*/
	double GetTrajectoryVelocity();

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void	SetSpring(double spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double	GetSpring();

	/** @brief �o�l�̐���ڕW��ݒ肷��
		@param targetPosition �o�l�̐���ڕW

		�o�l���_�Ƃ̓o�l�̗͂�0�ƂȂ�֐ߕψʂ̂��ƁD
	 */
	void	SetTargetPosition(double targetPosition);

	/** @brief �o�l�̌��_���擾����
		@return �o�l���_
	 */
	double	GetTargetPosition();

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
	
	/** @brief �萔����������
		@param �������l
	*/
	void	SetOffsetForce(double dat);

	/** @brief �␳�͂𓾂�
		@return �␳�l
	*/
	double	GetOffsetForce();

	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)�̐�Βl��ݒ肷��
		@param max ����֐߂̍ő�g���N(0�`DBL_MAX)
	*/
	void SetTorqueMax(double max);

	/** @brief �֐߂̏o�����Ƃ��ł���ő�g���N(N)�̐�Βl���擾����
	*/
	double GetTorqueMax();

	/** @brief ����ɂ������Ă��邩�ǂ������擾����
		@return �������Ă����true
	*/
	bool IsLimit();

		/** @brief ��ڂ̃_���p�W�����擾����
		@return ��ڂ̃_���p�W��
	 */
	double  GetSecondDamper();

	/** @brief ��ڂ̃_���p�W����ݒ肷��
		@param input ��ڂ̃_���p�W��
	 */
	void	SetSecondDamper(double input);
	/** @brief �~�����͂�ݒ肷��
		@return �~������
	 */
	double GetYieldStress();
	/** @brief �~�����͂��擾����
		@param input �~������
	 */
    void SetYieldStress(const double yS);
	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{����ݒ肷��
		@return �d������{��
	 */
	double GetHardnessRate();
	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{�����擾����
		@param input �d������{��
	 */
	void SetHardnessRate(const double hR);
	
	/** @brief �ό`�̃^�C�v��\������
		@return �ό`�̃^�C�v
	 */
	PHJointDesc::PHDeformationType 	GetDeformationMode();


	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void	SetRangeSpring(double rSpring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double	GetRangeSpring();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void	SetRangeDamper(double rDamper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	double	GetRangeDamper();
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
	
	bool bConstraintY;
	bool bConstraintRollX;
	bool bConstraintRollZ;

	PHSliderJointDesc();
};

/// �p�X���1�̓_
struct PHPathPoint{
	double	s;			///< �X�J���[�p�����[�^
	Posed	pose;		///< �ʒu�ƌX��
	PHPathPoint(){}
	PHPathPoint(double _s, Posed _pose):s(_s), pose(_pose){}
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
struct PHBallJointIf : public PHJointIf{
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
	/** @brief �ɒl�ł̃c�C�X�g�p�̉��͈͂�ݒ肷��BConstLine�ƂƂ��Ɏg�p
		@param lower �ŏ��c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
		���͈͐����𖳌�������ɂ̓f�B�X�N���v�^�ŏ��������Ȃ���Ηǂ�
		�i�f�t�H���g�ŉ��搧���͖����ɂȂ��Ă���j
	 */
	void SetTwistPole(Vec2d range);
	/**@brief �S���ʒu��ݒ肷��
	   @param �S���ʒu�̃t�@�C����ǂݍ���
    */
	bool SetConstLine(char* fileName, bool i);
	/**@brief �S���ʒu��ݒ肷��
	   @param [way][0]�X�C���O���ʊp�@[1]�X�C���O�p�@[2]�c�C�X�g�pmin�@[3]�c�C�X�g�pmax�@[4]�X��
    */
	void SetConstPoint(int Num, int way, double a);
	/**@brief �S���ʒu���擾����
	   @param [way][0]�X�C���O���ʊp�@[1]�X�C���O�p�@[2]�c�C�X�g�pmin�@[3]�c�C�X�g�pmax�@[4]�X��
    */
	double GetConstLine(int Num, int way);

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

	/**  @brief �֐ߕψʂ��擾����
		 @return Xjrel.q(�e���̂��猩���q���̂̑��Εψʂ�Quaternion�ŕ\����������)
	*/
	Quaterniond GetPosition();

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

	/** @brief ����̖ڕW������ݒ肷��
	*/
	void SetTargetPosition(Quaterniond p);
	
	/** @brief ����̖ڕW�������擾����
	*/
	Quaterniond GetTargetPosition();

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void	SetSpring(double spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double	GetSpring();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void	SetDamper(double damper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	double	GetDamper();

	/** @brief ���x����ɐ؂�ւ��C���x��ݒ肷��
	*/
	void  SetTargetVelocity(Vec3d q);

	/** @brief ���x����̖ڕW���x���擾����
	*/
	Vec3d GetTargetVelocity();

	/** @brief �O���Ǐ]����ɐ؂�ւ��C�ڕW���x��ݒ肷��
	*/
	void SetTrajectoryVelocity(Vec3d q);
	
	/** @brief �O���Ǐ]����̖ڕW���x�����擾����
	*/
	Vec3d GetTrajectoryVelocity();

	/** @brief �萔����ݒ肷��	
	*/
	void SetOffsetForce(Vec3d ofst);

	/**	@brief �萔�����擾����
	*/
	Vec3d GetOffsetForce();

	/** @brief ����ɂ������Ă��邩�ǂ������擾����
		@return �������Ă����true
	*/
	bool IsLimit();

	/** @brief ��ڂ̃_���p�W�����擾����
		@return ��ڂ̃_���p�W��
	 */
	double  GetSecondDamper();

	/** @brief ��ڂ̃_���p�W����ݒ肷��
		@param input ��ڂ̃_���p�W��
	 */
	void	SetSecondDamper(double input);
	/** @brief �~�����͂�ݒ肷��
		@return �~������
	 */
	double GetYieldStress();
	/** @brief �~�����͂��擾����
		@param input �~������
	 */
    void SetYieldStress(const double yS);
	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{����ݒ肷��
		@return �d������{��
	 */
	double GetHardnessRate();
	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{�����擾����
		@param input �d������{��
	 */
	void SetHardnessRate(const double hR);
	
	/** @brief �f��2�����[�����g��ݒ肷��
		@return �f��2�����[�����gVec3d(x,y,z)
	 */
	Vec3d	GetInertia();	
	/** @brief �f��2�����[�����g��ݒ肷��
		@param input �f��2�����[�����gVec3d(x,y,z)
	 */
	void	SetInertia(const Vec3d i);
	
	/** @brief �ό`�̃^�C�v��\������
		@return �ό`�̃^�C�v
	 */
	PHJointDesc::PHDeformationType 	GetDeformationMode();

	/** @brief Joint�ɉ�����Ă���͂̉ߋ�5�X�e�b�v���𕽋ς����m�������擾����
		@return Joint�ɉ�����Ă���͂̉ߋ�5�X�e�b�v���𕽋ς����m����
	 */
	double GetmotorfNorm();	

	void SetConstraintMode(int t);

};

/// �{�[���W���C���g�̃f�B�X�N���v�^
struct PHBallJointDesc : public PHJointDesc{
	SPR_DESCDEF(PHBallJoint);
	double			spring;			 ///< �o�l�W��
	double			damper;			 ///< �_���p�W��
	Vec2d			limitSwing;		 ///< swing�p�̉���i[0] or .lower, [1] or .upper�j
	Vec2d			limitSwingDir;	 ///< swing���ʊp�̉���i[0] or .lower, [1] or .upper�j
	Vec2d			limitTwist;		 ///< twist�p�̉���i[0] or .lower, [1] or .upper�j
	Vec3d			limitDir;		 ///< ����̒��S�x�N�g��
	Quaterniond		targetPosition;  ///< �o�l�_���p�̐���ڕW
	Vec3d			targetVelocity;  ///< �ڕW�ƂȂ��]�x�N�g��
	Vec3d			offsetForce;	 ///< �萔���i�O���Ǐ]����̉����x�̍�������̂Ɏg�����肷��j
	//Vec3d			torque;			 ///< ���[�^�g���N
	double			fMax;			 ///< �֐߂ɂ�������ő�̗�(��Βl)
	Vec2d			poleTwist;		 ///< spline�S����Swing�p0�̂Ƃ���Twist�p����i[0] or .lower, [1] or .upper�j
	
	double secondDamper;			///< ��Q�_���p�W��
	double yieldStress;				///< �~������
	double hardnessRate;			///< �~�����͈ȉ��̏ꍇ�ɓ�ڂ̃_���p�W���Ɋ|����䗦
	Vec3d  Inertia;					///< �f�ʂQ�����[�����g
	
	enum PHConstraintType{
		non = 0,
		SwingTwist =1,
		Spline =2,
		Jacobian =3
	}ConstMode;

	PHBallJointDesc();		///< �f�B�X�N���v�^�̃R���X�g���N�^
};


/// �o�l�_���p�̃C���^�t�F�[�X
struct PHSpringIf : public PHJointIf{
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
struct PHSpringDesc : public PHJointDesc{
	SPR_DESCDEF(PHSpring);
	Vec3d		spring;		///< �o�l�W��
	Vec3d		damper;		///< �_���p�W��
	double		springOri;
	double		damperOri;
	double		fMax;			///< �֐߂ɂ�������ő�̗�
	Vec3d		secondDamper;		// ��ڂ̃_���p�W��
	double		yieldStress;		// �~������
	double		hardnessRate;		// �~�����͈ȉ��̏ꍇ�ɓ�ڂ̃_���p�W���Ɋ|����䗦
	
	PHSpringDesc();
};

/// �c���[�m�[�h�̃C���^�t�F�[�X
struct PHTreeNodeIf : public SceneObjectIf{
	SPR_IFDEF(PHTreeNode);

	/** @brief �L���E������؂�ւ���
	 */
	void Enable(bool bEnable = true);

	/** @brief �L���E������Ԃ�
	 */
	bool IsEnabled();

	/** @brief �q�m�[�h�̐����擾����
	 */
	int NChildren();

	/** @brief �q�m�[�h���擾����
	 */
	PHTreeNodeIf* GetChild(int i);

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
