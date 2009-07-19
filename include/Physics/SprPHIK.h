/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHIK.h
 *	@brief �t�^���w(IK)�v�Z
*/
#ifndef SPR_PHIKIf_H
#define SPR_PHIKIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

/** \addtogroup gpPhysics */
//@{

/** \defgroup gpJoint �t�^���w(IK)�v�Z*/
//@{

// ------------------------------------------------------------------------------
/// IK�̐���_�i�ڕW�ʒu���������́j
struct PHIKControlPointIf : SceneObjectIf{
	SPR_IFDEF(PHIKControlPoint);

	/** @brief �͂�ݒ肷��
	*/
	void SetForce(Vec3d force);

	/** @brief �͂��擾����
	*/
	Vec3d GetForce();

	/** @brief ����Ώۂ̍��̂�ݒ肷��
	*/
	void SetSolid(PHSolidIf* solid);

	/** @brief ����Ώۂ̍��̂��擾����
	*/
	PHSolidIf* GetSolid();

	/** @brief �ԍ���ݒ肷��
	*/
	void SetNumber(int i);

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled();
};

/// IK����_�̃f�B�X�N���v�^
struct PHIKControlPointDesc{
	SPR_DESCDEF(PHIKControlPoint);

	bool isEnabled;  ///< �L�����ǂ���
};

/// �ڕW�ʒu���w�肷�鐧��_
struct PHIKPosCtlIf : PHIKControlPointIf{
	SPR_IFDEF(PHIKPosCtl);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	void SetGoal(Vec3d goal);

	/** @brief �ڕW�n�_���擾����
	*/
	Vec3d GetGoal();

	/** @brief ����_�̈ʒu��ݒ肷��
	*/
	void SetPos(Vec3d pos);

	/** @brief ����_�̈ʒu���擾����
	*/
	Vec3d GetPos();
};

/// �ʒu����_�̃f�B�X�N���v�^
struct PHIKPosCtlDesc : PHIKControlPointDesc{
	SPR_DESCDEF(PHIKPosCtl);

	Vec3d pos;  ///< ����_�̈ʒu�i���̃��[�J�����W�n�ɂ�����j
};

/// �ڕW�p�����w�肷�鐧��_
struct PHIKOriCtlIf : PHIKControlPointIf{
	SPR_IFDEF(PHIKOriCtl);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	void SetGoal(Quaterniond goal);

	/** @brief �ڕW�n�_���擾����
	*/
	Quaterniond GetGoal();
};

/// �p������_�̃f�B�X�N���v�^
struct PHIKOriCtlDesc : PHIKControlPointDesc{
	SPR_DESCDEF(PHIKOriCtl);
};

// ------------------------------------------------------------------------------
/// IK�m�[�h�i�ڕW�̒B���̂��߂ɗp���邱�Ƃ̂ł�������i�i�֐߁E���̂Ȃǁj�j
struct PHIKNodeIf : SceneObjectIf{
	SPR_IFDEF(PHIKNode);

	/** @brief IK�̌v�Z����������
	*/
	void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	void ProceedSolve();

	/** @brief �v�Z���ʂ��擾����
	*/
	PTM::VVector<double> GetDTheta();

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	void Move();

	/** @brief ���R�ʒu�ɖ߂�
	*/
	void MoveNatural();

	/** @brief ����_��ǉ�����
	*/
	void AddControlPoint(PHIKControlPointIf* control);

	/** @brief �������ɂ�����ݒ肷��
	*/
	void SetBias(float bias);

	/** @brief �������ɂ������擾����
	*/
	float GetBias();

	/** @brief �ԍ���ݒ肷��
	*/
	void SetNumber(int i);

	/** @brief �쓮�̂��߂̃o�l�W����ݒ肷��
	*/
	void SetSpring(double spring);

	/** @brief �쓮�̂��߂̃o�l�W�����擾
	*/
	double GetSpring();

	/** @brief �쓮�̂��߂̃_���p�W����ݒ肷��
	*/
	void SetDamper(double damper);

	/** @brief �쓮�̂��߂̃_���p�W�����擾
	*/
	double GetDamper();

	/** @brief �L���E������ݒ肷��
	*/
	void Enable(bool enable);

	/** @brief �L���E�������擾����
	*/
	bool IsEnabled();
};

/// IK�m�[�h�̃f�B�X�N���v�^
struct PHIKNodeDesc{
	SPR_DESCDEF(PHIKNode);

	bool isEnabled;  ///< �L�����ǂ���

	float	bias;	///< �������ɂ����̌W��
	double	spring;	///< �쓮�p�o�l�̃o�l�W��
	double	damper;	///< �쓮�p�o�l�̃_���p�W��

	PHIKNodeDesc(){ bias = 1.0f; }
};

/// �ʒu�����ȍ���
struct PHIKSolidIf : PHIKNodeIf{
	SPR_IFDEF(PHIKSolid);
};

/// IK�p���̂̃f�B�X�N���v�^
struct PHIKSolidDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKSolid);
};

/// �p�������ȃ{�[���W���C���g
struct PHIKBallJointIf : PHIKNodeIf{
	SPR_IFDEF(PHIKBallJoint);
};

/// IK�p�{�[���W���C���g�̃f�B�X�N���v�^
struct PHIKBallJointDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKBallJoint);
};

/// �p�x�����ȃq���W�W���C���g
struct PHIKHingeJointIf : PHIKNodeIf{
	SPR_IFDEF(PHIKHingeJoint);
};

/// IK�p�q���W�W���C���g�̃f�B�X�N���v�^
struct PHIKHingeJointDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKHingeJoint);
};

//@}
//@}

}

#endif
