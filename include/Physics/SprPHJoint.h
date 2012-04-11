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
#include <Physics/SprPHJointLimit.h>

namespace Spr{;

struct PHSolidIf;

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

/// �S���̃f�X�N���v�^
struct PHConstraintDesc{
	bool bEnabled;      ///< �L��/�����t���O
	Posed poseSocket;   ///< ���̂��猩���֐߂̈ʒu�ƌX��
	Posed posePlug;

	PHConstraintDesc():bEnabled(true){}
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

	// <!!>
	/** @brief �S���͂��擾
	 */
	Vec3d GetForce();

	/** @brief �S���g���N���擾
	 */
	Vec3d GetTorque();
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

// -----  -----  -----  -----  -----

/// �ڐG�_�S���̃C���^�t�F�[�X
struct PHContactPointIf : public PHConstraintIf{
	SPR_IFDEF(PHContactPoint);
};

// -----  -----  -----  -----  -----

/// �֐߂̃f�X�N���v�^
struct PHJointDesc : public PHConstraintDesc {
	double	fMax;			///< �֐߂̏o�����Ƃ��ł���ő�̗�
	PHJointDesc() {
		fMax = FLT_MAX;
	}
};

/// �֐߂̃C���^�t�F�[�X
struct PHJointIf : public PHConstraintIf{
	SPR_IFDEF(PHJoint);

	/** @brief �֐߂̏o�����Ƃ��ł���ő�S����(N)�̐�Βl��ݒ肷��
		@param max ����֐߂̍ő�S����(0�`DBL_MAX)
	*/
	void SetMaxForce(double max);

	/** @brief �֐߂̏o�����Ƃ��ł���ő�S����(N)�̐�Βl���擾����
	*/
	double GetMaxForce();
};

// -----  -----  -----  -----  -----

/// 1���֐߂̃f�X�N���v�^
struct PH1DJointDesc : public PHJointDesc {
	double spring;
	double damper;
	double secondDamper;
	double targetPosition;
	double targetVelocity;
	double offsetForce;
	double yieldStress;
	double hardnessRate;
	double secondMoment;

	PH1DJointDesc() {
		spring          = 0;
		damper          = 0;
		secondDamper    = FLT_MAX;
		targetPosition  = 0;
		targetVelocity  = 0;
		offsetForce     = 0;
		yieldStress     = FLT_MAX;
		hardnessRate    = 1.0;
	}
};

// struct PH1DJointLimitIf;
// struct PH1DJointLimitDesc;
/// 1���֐߂̃C���^�t�F�[�X
struct PH1DJointIf : public PHJointIf{
	SPR_IFDEF(PH1DJoint);

	/** @brief ���搧�����쐬����
	 */
	PH1DJointLimitIf* CreateLimit(const PH1DJointLimitDesc& desc = PH1DJointLimitDesc());

	/** @brief �֐ߕψʂ��擾����
		@return �֐ߕψ�
	 */
	double	GetPosition();

	/** @brief �֐ߑ��x���擾����
		@return �֐ߑ��x
	 */
	double	GetVelocity();

	/** @brief �֐߉���S�����擾����
		@return �֐߉���S��
	 */
	PH1DJointLimitIf* GetLimit();

	/** @brief �o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void SetSpring(double spring);

	/** @brief �o�l�W�����擾����
		@return �o�l�W��
	 */
	double GetSpring();

	/** @brief �_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void	SetDamper(double damper);

	/** @brief �_���p�W�����擾����
		@return �_���p�W��
	 */
	double	GetDamper();

	/** @brief ��ڂ̃_���p�W�����擾����
		@return ��ڂ̃_���p�W��
	 */
	double  GetSecondDamper();

	/** @brief ��ڂ̃_���p�W����ݒ肷��
		@param input ��ڂ̃_���p�W��
	 */
	void	SetSecondDamper(double input);

	/** @brief �o�l�̐���ڕW��ݒ肷��
		@param targetPosition �o�l�̐���ڕW

		�o�l���_�Ƃ̓o�l�̗͂�0�ƂȂ�֐ߕψʂ̂��ƁD
	 */
	void	SetTargetPosition(double targetPosition);

	/** @brief �o�l�̌��_���擾����
		@return �o�l���_
	 */
	double	GetTargetPosition();

	/** @brief �ڕW���x��ݒ肷��
		@param vel �ڕW���x
	 */
	void	SetTargetVelocity(double v);

	/** @brief �ڕW���x���擾����
		@return �ڕW���x
	 */
	double GetTargetVelocity();

	/** @brief �萔����������
		@param �������l
	*/
	void SetOffsetForce(double dat);

	/** @brief �␳�͂𓾂�
		@return �␳�l
	*/
	double GetOffsetForce();

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

	/** @brief �f�ʓ񎟃��[�����g��ݒ肷��
		@param secondMoment �f�ʓ񎟃��[�����g
	 */
	void SetSecondMoment(const double& sM);

	/** @brief �f�ʓ񎟃��[�����g���擾����
		@return �f�ʓ񎟃��[�����g
	 */
	double GetSecondMoment();

	/** @brief Motor�̏o�͂����͂�Ԃ�
		@return ��
	*/
	double GetMotorForce();
};

/// �q���W�̃C���^�t�F�[�X
struct PHHingeJointIf : public PH1DJointIf{
	SPR_IFDEF(PHHingeJoint);
};

/// �q���W�̃f�X�N���v�^
struct PHHingeJointDesc : public PH1DJointDesc{
	SPR_DESCDEF(PHHingeJoint);
	PHHingeJointDesc(){}
};

/// �X���C�_�̃C���^�t�F�[�X
struct PHSliderJointIf : public PH1DJointIf{
	SPR_IFDEF(PHSliderJoint);
};

/// �X���C�_�̃f�X�N���v�^
struct PHSliderJointDesc : public PH1DJointDesc{
	SPR_DESCDEF(PHSliderJoint);
	// �ȉ��̃I�v�V�����́C����ł�ABA��L���ɂ���Ƃ��܂��@�\���Ȃ��̂Œ��ӁD<!!>
	bool bConstraintY;      ///< ���iY�����S�����邩 �f�t�H���gtrue
	bool bConstraintRollX;  ///< ��]X�����S�����邩 �f�t�H���gtrue
	bool bConstraintRollZ;  ///< ��]Z�����S�����邩 �f�t�H���gtrue

	PHSliderJointDesc() {
		bConstraintY		= true;
		bConstraintRollX	= true;
		bConstraintRollZ	= true;
	}
};

// -----  -----  -----  -----  -----

/// �p�X���1�̓_
struct PHPathPoint{
	double	s;			///< �X�J���[�p�����[�^
	Posed	pose;		///< �ʒu�ƌX��
	PHPathPoint(){}
	PHPathPoint(double _s, Posed _pose):s(_s), pose(_pose){}
};

/// �p�X�̃f�X�N���v�^
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
struct PHPathJointIf : public PH1DJointIf{
	SPR_IFDEF(PHPathJoint);
	void SetPosition(double q);
};
/// �p�X�W���C���g�̃f�X�N���v�^
struct PHPathJointDesc : public PH1DJointDesc{
	SPR_DESCDEF(PHPathJoint);
	PHPathJointDesc(){}
};

// -----  -----  -----  -----  -----

// struct PHBallJointLimitIf;
// struct PHBallJointLimitDesc;
struct PHBallJointMotorIf;
/// �{�[���W���C���g�̃C���^�t�F�[�X
struct PHBallJointIf : public PHJointIf{
	SPR_IFDEF(PHBallJoint);

	/** @brief ���搧�����쐬����
	 */
	PHBallJointLimitIf* CreateLimit(const IfInfo* ii, const PHBallJointLimitDesc& desc = PHBallJointLimitDesc());
	template <class T> PHBallJointLimitIf* CreateLimit(const T& desc){
		return CreateLimit(T::GetIfInfo(), desc);
	}

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

	/** @brief �֐߉���S�����擾����
		@return �֐߉���S��
	 */
	PHBallJointLimitIf* GetLimit();

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

	/** @brief ��ڂ̃_���p�W�����擾����
		@return ��ڂ̃_���p�W��
	 */
	Vec3d  GetSecondDamper();

	/** @brief ��ڂ̃_���p�W����ݒ肷��
		@param input ��ڂ̃_���p�W��
	 */
	void	SetSecondDamper(Vec3d damper2);

	/** @brief ����̖ڕW������ݒ肷��
	*/
	void SetTargetPosition(Quaterniond p);
	
	/** @brief ����̖ڕW�������擾����
	*/
	Quaterniond GetTargetPosition();

	/** @brief ���x����̖ڕW���x��ݒ肷��
	*/
	void  SetTargetVelocity(Vec3d q);

	/** @brief ���x����̖ڕW���x���擾����
	*/
	Vec3d GetTargetVelocity();

	/** @brief �͂̃I�t�Z�b�g��ݒ肷��	
	*/
	void SetOffsetForce(Vec3d ofst);

	/**	@brief �͂̃I�t�Z�b�g���擾����
	*/
	Vec3d GetOffsetForce();

	/** @brief �~�����͂��擾����
		@param input �~������
	 */
    void SetYieldStress(const double yS);

	/** @brief �~�����͂�ݒ肷��
		@return �~������
	 */
	double GetYieldStress();

	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{�����擾����
		@param input �d������{��
	 */
	void SetHardnessRate(const double hR);
	
	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{����ݒ肷��
		@return �d������{��
	 */
	double GetHardnessRate();

	/** @brief �f��2�����[�����g��ݒ肷��
		@param input �f��2�����[�����gVec3d(x,y,z)
	 */
	void	SetSecondMoment(const Vec3d m);

	/** @brief �f��2�����[�����g��ݒ肷��
		@return �f��2�����[�����gVec3d(x,y,z)
	 */
	Vec3d	GetSecondMoment();	

	/** @brief Motor�̏o�͂����͂�Ԃ�
		@return ��
	*/
	Vec3d GetMotorForce();
};

/// �{�[���W���C���g�̃f�X�N���v�^
struct PHBallJointDesc : public PHJointDesc {
	SPR_DESCDEF(PHBallJoint);
	double spring;
	double damper;
	Vec3d  secondDamper;
	Quaterniond targetPosition;
	Vec3d  targetVelocity;
	Vec3d  offsetForce;
	double yieldStress;
	double hardnessRate;
	Vec3d  secondMoment;

	PHBallJointDesc() {
		spring          = 0;
		damper          = 0;
		secondDamper    = Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
		targetPosition  = Quaterniond();
		targetVelocity  = Vec3d();
		offsetForce     = Vec3d();
		yieldStress     = FLT_MAX;
		hardnessRate    = 1.0;
	}
};

// -----  -----  -----  -----  -----

struct PHSpringMotorIf;
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

	/** @brief ���i�̑��_���p�W����ݒ肷��
		@param secondDamper ���i�̑��_���p�W��
	 */
	void SetSecondDamper(const Vec3d& secondDamper);

	/** @brief ���i�̑��_���p�W�����擾����
		@return ���i�̑��_���p�W��
	 */
	Vec3d GetSecondDamper();

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

	/** @brief ��]�̑��_���p�W����ݒ肷��
		@param secondDamperOri ��]�̑��_���p�W��
	 */
	void SetSecondDamperOri(const double& secondDamperOri);

	/** @brief ��]�̑��_���p�W�����擾����
		@return ��]�̑��_���p�W��
	 */
	double GetSecondDamperOri();

	/** @brief �~�����͂�ݒ肷��
		@param yieldStress �~������
	 */
	void SetYieldStress(const double& yieldStress);

	/** @brief �~�����͂��擾����
		@return �~������
	 */
	double GetYieldStress();

	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{����ݒ肷��
		@return �d������{��
	 */
	void SetHardnessRate(const double& hardnessRate);

	/** @brief �~�����͈ȉ��̏ꍇ�Ƀ_���p���d������{�����擾����
		@param input �d������{��
	 */
	double GetHardnessRate();

	/** @brief �f�ʓ񎟃��[�����g��ݒ肷��
		@param secondMoment �f�ʓ񎟃��[�����g
	 */
	void SetSecondMoment(const Vec3d& secondMoment);

	/** @brief �f�ʓ񎟃��[�����g���擾����
		@return �f�ʓ񎟃��[�����g
	 */
	Vec3d GetSecondMoment();
};

/// �o�l�_���p�̃f�X�N���v�^
struct PHSpringDesc : public PHJointDesc {
	SPR_DESCDEF(PHSpring);
	Vec3d  spring;
	Vec3d  damper;
	Vec3d  secondDamper;
	double springOri;
	double damperOri;
	double secondDamperOri;
	double yieldStress;
	double hardnessRate;
	Vec3d  secondMoment;

	PHSpringDesc() {
		spring          = Vec3d();
		damper          = Vec3d();
		secondDamper    = Vec3d(FLT_MAX, FLT_MAX, FLT_MAX);
		springOri       = 0;
		damperOri       = 0;
		secondDamperOri = FLT_MAX;
		yieldStress     = FLT_MAX;
		hardnessRate    = 1.0;
	}
};

// -----  -----  -----  -----  -----

/// �c���[�m�[�h�̃f�X�N���v�^
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

// -----  -----  -----  -----  -----

/// �M�A�̃f�X�N���v�^
struct PHGearDesc{
	double ratio;		///< �M�A��
	PHGearDesc(){
		ratio = 1.0;
	}
};

/// �M�A�̃C���^�t�F�[�X
struct PHGearIf : public SceneObjectIf{
	SPR_IFDEF(PHGear);
};
	
//@}
//@}

}

#endif
