/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRInverseKinematicsIf_H
#define SPR_CRInverseKinematicsIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

// ------------------------------------------------------------------------------
/// �{�f�B��IK�œ��삳���邽�߂̐���_
struct CRIKControlIf : SceneObjectIf{
	SPR_IFDEF(CRIKControl);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	void SetGoal(Vec3d goal);

	/** @brief �ڕW�n�_���擾����
	*/
	Vec3d GetGoal();

	/** @brief ����Ώۂ̍��̂�ݒ肷��
	*/
	void SetSolid(PHSolidIf* solid);

	/** @brief ����Ώۂ̍��̂��擾����
	*/
	PHSolidIf* GetSolid();
};

struct CRIKControlDesc{
	SPR_DESCDEF(CRIKControl);

	PHSolidIf* solid;  ///< ����_�̂��鍄��
};

// --- �ʒu����_
struct CRIKControlPosIf : CRIKControlIf{
	SPR_IFDEF(CRIKControlPos);
};

struct CRIKControlPosDesc : CRIKControlDesc{
	SPR_DESCDEF(CRIKControlPos);

	Vec3d pos;  ///< ����_�̈ʒu�i���̃��[�J�����W�n�ɂ�����j
};

// --- �p������_
struct CRIKControlOriIf : CRIKControlIf{
	SPR_IFDEF(CRIKControlOri);
};

struct CRIKControlOriDesc : CRIKControlDesc{
	SPR_DESCDEF(CRIKControlOri);
};

// ------------------------------------------------------------------------------
/// �{�f�B��IK�œ��삳���邽�߂̐���Ώہi�֐߁E���́j
struct CRIKMovableIf : SceneObjectIf{
	SPR_IFDEF(CRIKMovable);

	/** @brief IK�̌v�Z����������
	*/
	void PrepareSolve();

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	void ProceedSolve();

	/** @brief �v�Z���ʂ��擾����
	*/
	PTM::VVector<double> GetValue();

	/** @brief �v�Z���ʂɏ]���Đ���Ώۂ𓮂���
	*/
	void Move();

	/** @brief ����_��ǉ�����
	*/
	void AddIKControl(CRIKControlIf* control);
};

struct CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovable);

	float bias;  ///< �������₷���̌W��

	CRIKMovableDesc(){ bias = 1.0f; }
};

// --- �ʒu����\�ȍ���
struct CRIKMovableSolidPosIf : CRIKMovableIf{
	SPR_IFDEF(CRIKMovableSolidPos);
};

struct CRIKMovableSolidPosDesc : CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovableSolidPos);

	PHSolidIf* solid;  ///< ����Ώۂ̍���
};

// --- �p������\�ȍ���
struct CRIKMovableSolidOriIf : CRIKMovableIf{
	SPR_IFDEF(CRIKMovableSolidOri);
};

struct CRIKMovableSolidOriDesc : CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovableSolidOri);

	PHSolidIf* solid;  ///< ����Ώۂ̍���
};

// --- �p�x����\�ȃ{�[���W���C���g
struct CRIKMovableBallJointOriIf : CRIKMovableIf{
	SPR_IFDEF(CRIKMovableBallJointOri);
};

struct CRIKMovableBallJointOriDesc : CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovableBallJointOri);

	PHBallJointIf* joint;  ///< ����Ώۂ̊֐�
};

// --- �p�x����\�ȎO�A�q���W�W���C���g
struct CRIKMovable3HingeJointOriIf : CRIKMovableIf{
	SPR_IFDEF(CRIKMovable3HingeJointOri);
};

struct CRIKMovable3HingeJointOriDesc : CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovable3HingeJointOri);
	PHHingeJointIf *joint1;
	PHHingeJointIf *joint2;
	PHHingeJointIf* joint3;  ///< ����Ώۂ̊֐�
};

// --- �p�x����\�ȃq���W�W���C���g
struct CRIKMovableHingeJointOriIf : CRIKMovableIf{
	SPR_IFDEF(CRIKMovableHingeJointOri);
};

struct CRIKMovableHingeJointOriDesc : CRIKMovableDesc{
	SPR_DESCDEF(CRIKMovableHingeJointOri);

	PHHingeJointIf *joint;  ///< ����Ώۂ̊֐�
};

}

#endif//SPR_CRInverseKinematicsIf_H
