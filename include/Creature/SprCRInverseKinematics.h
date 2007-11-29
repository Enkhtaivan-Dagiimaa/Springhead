/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
	IF_DEF(CRIKControl);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec3d goal)= 0;
};

struct CRIKControlDesc{
	DESC_DEF_FOR_OBJECT(CRIKControl);

	PHSolidIf* solid;  ///< ����_�̂��鍄��
};

// --- �ʒu����_
struct CRIKControlPosIf : CRIKControlIf{
	IF_DEF(CRIKControlPos);
};

struct CRIKControlPosDesc : CRIKControlDesc{
	DESC_DEF_FOR_OBJECT(CRIKControlPos);

	Vec3d pos;  ///< ����_�̈ʒu�i���̃��[�J�����W�n�ɂ�����j
};

// --- �p������_
struct CRIKControlOriIf : CRIKControlIf{
	IF_DEF(CRIKControlOri);
};

struct CRIKControlOriDesc : CRIKControlDesc{
	DESC_DEF_FOR_OBJECT(CRIKControlOri);
};

// ------------------------------------------------------------------------------
/// �{�f�B��IK�œ��삳���邽�߂̐���Ώہi�֐߁E���́j
struct CRIKMovableIf : SceneObjectIf{
	IF_DEF(CRIKMovable);

	/** @brief IK�̌v�Z����������
	*/
	virtual void PrepareSolve()= 0;

	/** @brief IK�̌v�Z�J�Ԃ��̂P�X�e�b�v�����s����
	*/
	virtual void ProceedSolve()= 0;

	/** @brief �v�Z���ʂ��擾����
	*/
	virtual PTM::VVector<double> GetValue()= 0;

	/** @brief ����_��ǉ�����
	*/
	virtual void AddIKControl(CRIKControlIf* control)= 0;
};

struct CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovable);

	float bias;  ///< �������₷���̌W��

	CRIKMovableDesc(){ bias = 1.0f; }
};

// --- �ʒu����\�ȍ���
struct CRIKMovableSolidPosIf : CRIKMovableIf{
	IF_DEF(CRIKMovableSolidPos);
};

struct CRIKMovableSolidPosDesc : CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovableSolidPos);

	PHSolidIf* solid;  ///< ����Ώۂ̍���
};

// --- �p������\�ȍ���
struct CRIKMovableSolidOriIf : CRIKMovableIf{
	IF_DEF(CRIKMovableSolidOri);
};

struct CRIKMovableSolidOriDesc : CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovableSolidOri);

	PHSolidIf* solid;  ///< ����Ώۂ̍���
};

// --- �p�x����\�ȃ{�[���W���C���g
struct CRIKMovableBallJointOriIf : CRIKMovableIf{
	IF_DEF(CRIKMovableBallJointOri);
};

struct CRIKMovableBallJointOriDesc : CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovableBallJointOri);

	PHBallJointIf* joint;  ///< ����Ώۂ̊֐�
};

// --- �p�x����\�ȎO�A�q���W�W���C���g
struct CRIKMovable3HingeJointOriIf : CRIKMovableIf{
	IF_DEF(CRIKMovable3HingeJointOri);
};

struct CRIKMovable3HingeJointOriDesc : CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovable3HingeJointOri);

	PHHingeJointIf *joint1, *joint2, *joint3;  ///< ����Ώۂ̊֐�
};

// --- �p�x����\�ȃq���W�W���C���g
struct CRIKMovableHingeJointOriIf : CRIKMovableIf{
	IF_DEF(CRIKMovableHingeJointOri);
};

struct CRIKMovableHingeJointOriDesc : CRIKMovableDesc{
	DESC_DEF_FOR_OBJECT(CRIKMovableHingeJointOri);

	PHHingeJointIf *joint;  ///< ����Ώۂ̊֐�
};

}

#endif//SPR_CRInverseKinematicsIf_H
