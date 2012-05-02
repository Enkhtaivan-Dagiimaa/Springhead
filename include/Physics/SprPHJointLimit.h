/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHJointLimit.h
 *	@brief �֐߉��搧��
*/
#ifndef SPR_PHJOINTLIMITIf_H
#define SPR_PHJOINTLIMITIf_H

#include <Foundation/SprObject.h>

namespace Spr{;

/** \addtogroup gpJoint */
//@{

/// 1���R�x�֐߉���S���̃C���^�t�F�[�X
struct PH1DJointLimitIf : public SceneObjectIf{
	SPR_IFDEF(PH1DJointLimit);

	/** @brief ���͈͂�ݒ肷��
		@param range ���͈�
	 */
	void SetRange(Vec2d range);
	
	/** @brief ���͈͂��擾����
		@return ���͈�
	 */
	void GetRange(Vec2d& range);

	/** @brief ���搧���̂��߂̃o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void SetSpring(double spring);

	/** @brief ���搧���̂��߂̃o�l�W�����擾����
		@return �o�l�W��
	 */
	double GetSpring();

	/** @brief ���搧���̂��߂̃_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void SetDamper(double damper);

	/** @brief ���搧���̂��߂̃_���p�W�����擾����
		@return �_���p�W��
	 */
	double GetDamper();

	/** @brief ����ɂ������Ă��邩�ǂ������擾����
		@return �������Ă����true
	*/
	bool IsOnLimit();
};

///	1���R�x�֐߂̉���S��
struct PH1DJointLimitDesc{
	double	spring;		///< ���͈̓o�l
	double	damper;		///< ���͈̓_���p
	Vec2d	range;		///< ���͈�. range[0]:lower < range[1]:upper �̂Ƃ��ɗL��

	PH1DJointLimitDesc() {
		spring = 10000.0;
		damper =   100.0;
		range  = Vec2d(1, -1); /// �f�t�H���g�ł͍S������
	}
};

// -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  ----- 

/// �{�[���W���C���g���搧���̃C���^�t�F�[�X
struct PHBallJointLimitIf : public SceneObjectIf {
	SPR_IFDEF(PHBallJointLimit);

	/** @brief ���搧���̂��߂̃o�l�W����ݒ肷��
		@param spring �o�l�W��
	 */
	void SetSpring(double rSpring);

	/** @brief ���搧���̂��߂̃o�l�W�����擾����
		@return �o�l�W��
	 */
	double GetSpring();

	/** @brief ���搧���̂��߂̃_���p�W����ݒ肷��
		@param damper �_���p�W��
	 */
	void SetDamper(double rDamper);

	/** @brief ���搧���̂��߂̃_���p�W�����擾����
		@return �_���p�W��
	 */
	double GetDamper();

	/** @brief ����̊������ݒ肷��
		@param limDir ����̊����
	 */
	void SetLimitDir(Vec3d limDir);

	/** @brief ����̊�������擾����
		@return ��������
	 */
	Vec3d GetLimitDir();

	/** @brief ����ɂ������Ă��邩�ǂ������擾����
		@return �������Ă����true
	*/
	bool IsOnLimit();
};
	
/// �{�[���W���C���g�̉��搧���̃X�e�[�g
struct PHBallJointLimitState {
	Vec3d f;   ///< �S���̗͂͐�
};

/// �{�[���W���C���g�̉��搧���̃f�X�N���v�^
struct PHBallJointLimitDesc : public PHBallJointLimitState {
	SPR_DESCDEF(PHBallJointLimit);
	double	spring;			///< ���͈̓o�l
	double	damper;			///< ���͈̓_���p
	Vec3d   limitDir;		///< ����̊�����i�~������Ȃ璆�S���C�Ȃǁj

	PHBallJointLimitDesc() {
		spring    = 10000.0;
		damper    =   100.0;
		limitDir  = Vec3d(0,0,1);
	}
};

// -----  -----  -----  -----  -----

/// �{�[���W���C���g�̉~�����搧���̃C���^�t�F�[�X
struct PHBallJointConeLimitIf : public PHBallJointLimitIf{
	SPR_IFDEF(PHBallJointConeLimit);

	/** @brief �X�C���O�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��X�C���O�p�x
		@param upper �ő�X�C���O�p�x
	 */
	void SetSwingRange(Vec2d range);

	/** @brief �X�C���O�p�̉��͈͂��擾����
		@param lower �ő�X�C���O�p�x
		@param upper �ő�X�C���O�p�x
	 */
	void GetSwingRange(Vec2d& range);

	/** @brief �c�C�X�g�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
	 */
	void SetTwistRange(Vec2d range);

	/** @brief �c�C�X�g�p�̉��͈͂��擾����
		@param lower �ő�c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
	 */
	void GetTwistRange(Vec2d& range);
};

/// �{�[���W���C���g�̉~�����搧���̃f�X�N���v�^
struct PHBallJointConeLimitDesc : public PHBallJointLimitDesc {
	SPR_DESCDEF(PHBallJointConeLimit);
	Vec2d	limitSwing;		///< swing�p�̉���i[0] or .lower, [1] or .upper�j
	Vec2d	limitSwingDir;	///< swing���ʊp�̉���i[0] or .lower, [1] or .upper�j <!!> ���������s����
	Vec2d	limitTwist;		///< twist�p�̉���i[0] or .lower, [1] or .upper�j

	PHBallJointConeLimitDesc() {
		limitSwing    = Vec2d(-FLT_MAX, FLT_MAX);
		limitSwingDir = Vec2d(-FLT_MAX, FLT_MAX);
		limitTwist    = Vec2d(-FLT_MAX, FLT_MAX);
	}
};

// -----  -----  -----  -----  -----

/// �X�v���C������_
/// �X�v���C���Ȑ��̐���_
struct PHSplineLimitNode {
	double swing;
	double swingDir;
	double dSwing;
	double dSwingDir;
	double twistMin;
	double twistMax;

	PHSplineLimitNode(){
		swing     =  0;       swingDir  =  0;
		dSwing    =  0;       dSwingDir =  0;
		twistMin  = -FLT_MAX; twistMax  =  FLT_MAX;
	};
	PHSplineLimitNode(double s,double sd,double ds, double dsd, double tmn, double tmx) {
		swing     = s;   swingDir  = sd;
		dSwing    = ds;  dSwingDir = dsd;
		twistMin  = tmn; twistMax  = tmx;
	}
};

/// �{�[���W���C���g�̃X�v���C�����搧���̃C���^�t�F�[�X
struct PHBallJointSplineLimitIf : public PHBallJointLimitIf{
	SPR_IFDEF(PHBallJointSplineLimit);

	/** @brief ����_��ǉ�����
		@param node �ǉ����鐧��_
	 */
	void AddNode(PHSplineLimitNode node);
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax);

	/** @brief ����_��}������
		@param node �ǉ����鐧��_
		@param pos  �}���ʒu
	 */
	void AddNode(PHSplineLimitNode node, int pos);
	void AddNode(double S, double SD, double dS, double dSD, double tMin, double tMax, int pos);

	/** @brief �ɂɂ�����c�C�X�g�p�̉��͈͂�ݒ肷��
		@param lower �ŏ��c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
	 */
	void SetPoleTwistRange(Vec2d range);

	/** @brief �ɂɂ�����c�C�X�g�p�̉��͈͂��擾����
		@param lower �ő�c�C�X�g�p�x
		@param upper �ő�c�C�X�g�p�x
	 */
	void GetPoleTwistRange(Vec2d& range);
};

/// �{�[���W���C���g�̃X�v���C�����搧���̃f�X�N���v�^
struct PHBallJointSplineLimitDesc : public PHBallJointLimitDesc {
	SPR_DESCDEF(PHBallJointSplineLimit);
	std::vector<PHSplineLimitNode> nodes;  ///< �����\������X�v���C������_�̔z��
	Vec2d poleTwist;                       ///< �Ɂi���_�j�ɂ�����Twist�̃��~�b�g

	PHBallJointSplineLimitDesc() {
		poleTwist = Vec2d(-FLT_MAX, FLT_MAX);
	}
};

//@}

}

#endif//SPR_PHJOINTLIMITIf_H
