/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_PHIKIf_H
#define SPR_PHIKIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

// ------------------------------------------------------------------------------
/// IK�̐���_
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
};

struct PHIKControlPointDesc{
	SPR_DESCDEF(PHIKControlPoint);

	PHSolidIf* solid;  ///< ����_�̂��鍄��
};

// --- �ʒu����_
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

struct PHIKPosCtlDesc : PHIKControlPointDesc{
	SPR_DESCDEF(PHIKPosCtl);

	Vec3d pos;  ///< ����_�̈ʒu�i���̃��[�J�����W�n�ɂ�����j
};

// --- �p������_
struct PHIKOriCtlIf : PHIKControlPointIf{
	SPR_IFDEF(PHIKOriCtl);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	void SetGoal(Quaterniond goal);

	/** @brief �ڕW�n�_���擾����
	*/
	Quaterniond GetGoal();
};

struct PHIKOriCtlDesc : PHIKControlPointDesc{
	SPR_DESCDEF(PHIKOriCtl);
};

// ------------------------------------------------------------------------------
/// IK�̐���ɗp���邱�Ƃ̂ł�������i�i�֐߁E���̂Ȃǁj
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
};

struct PHIKNodeDesc{
	SPR_DESCDEF(PHIKNode);

	float bias;  ///< �������ɂ����̌W��

	PHIKNodeDesc(){ bias = 1.0f; }
};

// --- �ʒu����\�ȍ���
struct PHIKSolidIf : PHIKNodeIf{
	SPR_IFDEF(PHIKSolid);
};

struct PHIKSolidDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKSolid);

	PHSolidIf* solid;  ///< ����Ώۂ̍���
};

// --- �p�x����\�ȃ{�[���W���C���g
struct PHIKBallJointIf : PHIKNodeIf{
	SPR_IFDEF(PHIKBallJoint);
};

struct PHIKBallJointDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKBallJoint);

	PHBallJointIf* joint;  ///< ����Ώۂ̊֐�
};

// --- �p�x����\�ȃq���W�W���C���g
struct PHIKHingeJointIf : PHIKNodeIf{
	SPR_IFDEF(PHIKHingeJoint);
};

struct PHIKHingeJointDesc : PHIKNodeDesc{
	SPR_DESCDEF(PHIKHingeJoint);

	PHHingeJointIf *joint;  ///< ����Ώۂ̊֐�
};

}

#endif
