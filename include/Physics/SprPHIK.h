/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/** \addtogroup gpPhysics */
//@{

/**
 *	@file SprPHIK.h
 *	@brief �t�^���w(IK)�v�Z
*/
#ifndef SPR_PHIKIf_H
#define SPR_PHIKIf_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct PHSolidIf;
struct PHHingeJointIf;
struct PHBallJointIf;

/** \defgroup gpIK �t�^���w(IK)�v�Z*/
//@{

// ------------------------------------------------------------------------------
/// IK�̃G���h�G�t�F�N�^�i���B�ڕW�Ɍ����ē����������́j
struct PHIKEndEffectorIf : SceneObjectIf{
	SPR_IFDEF(PHIKEndEffector);

	/** @brief ����Ώۂ̍��̂�ݒ肷��i�P�G���h�G�t�F�N�^�ɂ��P���̂��K���Ή�����j
	*/
	void SetSolid(PHSolidIf* solid);

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ���̂��擾����
	*/
	PHSolidIf* GetSolid();

	// --- --- --- --- ---

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled();

	// --- --- --- --- ---

	/** @brief �ʒu�̐���̗L���E������؂�ւ���
	*/
	void EnablePositionControl(bool enable);

	/** @brief �ʒu����̃v���C�I���e�B��ݒ肷��
	*/
	void SetPositionPriority(double priority);

	/** @brief �ʒu�̖ڕW�l��ݒ肷��
	*/
	void SetTargetPosition(Vec3d position);

	/** @brief ���̂ɂ����ē��B���������ʒu�̍��̃��[�J�����W��ݒ肷��
	*/
	void SetTargetLocalPosition(Vec3d localPosition);

	/** @brief �ݒ肳�ꂽ�ʒu�̖ڕW�l���擾����
	*/
	Vec3d GetTargetPosition();

	/** @brief �G���h�G�t�F�N�^�ɂ����铞�B���������ʒu�̐ݒ肳�ꂽ�ڕW�l���擾����
	*/
	Vec3d GetTargetLocalPosition();

	// --- --- --- --- ---

	/** @brief �p���̐���̗L���E������؂�ւ���
	*/
	void EnableOrientationControl(bool enable);

	/** @brief �p������̃v���C�I���e�B��ݒ肷��
	*/
	void SetOrientationPriority(double priority);

	/** @brief �p���̖ڕW�l��ݒ肷��
	*/
	void SetTargetOrientation(Quaterniond orientation);

	/** @brief �ݒ肳�ꂽ�p���̖ڕW�l���擾����
	*/
	Quaterniond GetTargetOrientation();

	// --- --- --- --- ---

	/** @brief �͂̐���̗L���E������؂�ւ���
	*/
	void EnableForceControl(bool enable);

	/** @brief �̖͂ڕW�l��ݒ肷��
	*/
	void SetTargetForce(Vec3d force, Vec3d workingPoint = Vec3d());

	/** @brief �ݒ肳�ꂽ�̖͂ڕW�l���擾����
	*/
	Vec3d GetTargetForce();

	/** @brief �ݒ肳�ꂽ�͂̍�p�_���擾����
	*/
	Vec3d GetTargetForceWorkingPoint();

	// --- --- --- --- ---

	/** @brief �g���N�̐���̗L���E������؂�ւ���
	*/
	void EnableTorqueControl(bool enable);

	/** @brief �g���N�̖ڕW�l��ݒ肷��
	*/
	void SetTargetTorque(Vec3d torque);

	/** @brief �ݒ肳�ꂽ�g���N�̖ڕW�l���擾����
	*/
	Vec3d GetTargetTorque();

};

/// IK�G���h�G�t�F�N�^�̃f�X�N���v�^
struct PHIKEndEffectorDesc {
	SPR_DESCDEF(PHIKEndEffector);

	bool   bEnabled;				///< �G���h�G�t�F�N�^���쓮�����邩�ǂ���

	bool   bPosition;				///< �ʒu�����L���ɂ��邩�ǂ���
	bool   bOrientation;			///< �p�������L���ɂ��邩�ǂ���
	bool   bForce;					///< �͐����L���ɂ��邩�ǂ���
	bool   bTorque;					///< �g���N�����L���ɂ��邩�ǂ���

	double positionPriority;		///< �ʒu����̒B���D��x�i1�`0�A�傫���قǗD��x�������j
	double orientationPriority;		///< �p������̒B���D��x�i1�`0�A�傫���قǗD��x�������j
	double forcePriority;			///< �͐���̒B���D��x�i1�`0�A�傫���قǗD��x�������j
	double torquePriority;			///< �g���N����̒B���D��x�i1�`0�A�傫���قǗD��x�������j

	Vec3d		targetPosition;				///< ���B�ڕW�ʒu
	Vec3d		targetLocalPosition;		///< �G���h�G�t�F�N�^�ɂ����铞�B�����������ʂ̈ʒu
	Quaterniond	targetOrientation;			///< ���B�ڕW�p��
	Vec3d		targetForce;				///< �̖͂ڕW�l
	Vec3d		targetForceWorkingPoint;	///< �o�������͂̍�p�_
	Vec3d		targetTorque;				///< �g���N�̖ڕW�l

	PHIKEndEffectorDesc();
};

// ------------------------------------------------------------------------------
/// IK�p�̃A�N�`���G�[�^�i�ڕW�̒B���̂��߂ɗp���邱�Ƃ̂ł���쓮���i�i�����Ɋ֐߂ɕt���j�j
struct PHIKActuatorIf : SceneObjectIf{
	SPR_IFDEF(PHIKActuator);

	/** @brief IK�̌v�Z����������
	*/
	void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	void ProceedSolve();

	/** @brief �[���t�����������܂܂�"����"�v�Z���ʂ��擾����
	*/
	PTM::VVector<double> GetRawSolution();

	// --- --- --- --- ---

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	void Move();

	// --- --- --- --- ---

	/** @brief �������ɂ�����ݒ肷��
	*/
	void SetBias(float bias);

	/** @brief �������ɂ������擾����
	*/
	float GetBias();

	/** @brief ���x����̔��W����ݒ肷��
	*/
	void SetVelocityGain(double velocityGain);

	/** @brief ���x����̔��W�����擾����
	*/
	double GetVelocityGain();

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled();
};

/// IK�A�N�`���G�[�^�̃f�B�X�N���v�^
struct PHIKActuatorDesc{
	SPR_DESCDEF(PHIKActuator);

	bool bEnabled;        ///< �L�����ǂ���
	float	bias;	      ///< �������ɂ����̌W��
	double  velocityGain; ///< ���x����̔��W��

	PHIKActuatorDesc() {
		bEnabled     = true;
		bias         = 1.0;
		velocityGain = 10.0;
	}
};

/// �R���A�N�`���G�[�^�iPHBallJoint���쓮����j
struct PHIKBallActuatorIf : PHIKActuatorIf{
	SPR_IFDEF(PHIKBallActuator);

	/** @brief ����Ώۂ̊֐߂�ݒ肷��i�P�A�N�`���G�[�^�ɂ��P�֐߂��K���Ή�����j
	*/
	void SetJoint(PHBallJointIf* joint);

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ�֐߂��擾����
	*/
	PHBallJointIf* GetJoint();
};

/// �R���A�N�`���G�[�^�̃f�B�X�N���v�^
struct PHIKBallActuatorDesc : PHIKActuatorDesc{
	SPR_DESCDEF(PHIKBallActuator);
};

/// �P���A�N�`���G�[�^�iPHHingeJoint���쓮����j
struct PHIKHingeActuatorIf : PHIKActuatorIf{
	SPR_IFDEF(PHIKHingeActuator);

	/** @brief ����Ώۂ̊֐߂�ݒ肷��i�P�A�N�`���G�[�^�ɂ��P�֐߂��K���Ή�����j
	*/
	void SetJoint(PHHingeJointIf* joint);

	/** @brief ����ΏۂƂ��Đݒ肳�ꂽ�֐߂��擾����
	*/
	PHHingeJointIf* GetJoint();
};

/// �P���A�N�`���G�[�^�̃f�B�X�N���v�^
struct PHIKHingeActuatorDesc : PHIKActuatorDesc{
	SPR_DESCDEF(PHIKHingeActuator);
};

//@}
//@}

}

#endif
