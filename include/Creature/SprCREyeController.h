/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CREyeControllerIf_H
#define SPR_CREyeControllerIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{

struct PHSolidIf;
struct PHHingeJointIf;
struct CREyeControllerIf;
struct CRNeckControllerIf;

// ------------------------------------------------------------------------------

/// �ዅ�^���R���g���[���̃X�e�[�g
struct CREyeControllerState{
	/// ����̏��
	enum ControlState{
		CS_SACCADE, CS_PURSUIT,
	} controlState; 
};

/// �ዅ�^���R���g���[���̃f�X�N���v�^
struct CREyeControllerDesc: public CREyeControllerState{
	PHSolidIf* soLEye; ///< ���ڂ̃\���b�h
	PHSolidIf* soREye; ///< �E�ڂ̃\���b�h
	PHSolidIf* soHead; ///< ���̃\���b�h
	
	PHHingeJointIf* joLEyeX; ///< ���ڂ��Œ肷��{�[���W���C���g
	PHHingeJointIf* joLEyeY; ///< ���ڂ��Œ肷��{�[���W���C���g
	PHHingeJointIf* joREyeX; ///< �E�ڂ��Œ肷��{�[���W���C���g
	PHHingeJointIf* joREyeY; ///< �E�ڂ��Œ肷��{�[���W���C���g

	CREyeControllerDesc(){
		soHead = soLEye = soREye = NULL; 
		joLEyeX = joLEyeY = joREyeX = joREyeY = NULL;

		controlState = CS_PURSUIT;
	}
};

/// �ዅ�^���R���g���[���̃C���^�[�t�F�C�X
struct CREyeControllerIf : SceneObjectIf{
	IF_DEF(CREyeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel)= 0;

	/** @brief ���䏈�������s����
	*/
	virtual void Step()= 0;
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState{
	PHSolidIf* soChest; ///< �����̃\���b�h
	PHSolidIf* soNeck;  ///< ��̃\���b�h
	PHSolidIf* soHead;  ///< ���̃\���b�h
	PHHingeJointIf* joChestNeckX; ///< ���Ǝ���Œ肵�Ă���W���C���g
	PHHingeJointIf* joChestNeckY; ///< ���Ǝ���Œ肵�Ă���W���C���g
	PHHingeJointIf* joChestNeckZ; ///< ���Ǝ���Œ肵�Ă���W���C���g
	PHHingeJointIf* joNeckHeadX;  ///< ��Ɠ����Œ肵�Ă���W���C���g
	PHHingeJointIf* joNeckHeadZ;  ///< ��Ɠ����Œ肵�Ă���W���C���g

	CRNeckControllerDesc(){
		soChest = soNeck = soHead = NULL;
		joChestNeckX = joChestNeckY = joChestNeckZ = NULL;
		joNeckHeadX  = joNeckHeadZ  = NULL;
	}
};

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : SceneObjectIf{
	IF_DEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;

	/** @brief ���䏈�������s����
	*/
	virtual void Step()= 0;
};

// ------------------------------------------------------------------------------

/// �����R���g���[���̃X�e�[�g
struct CRGazeControllerState{
};

/// �����R���g���[���̃f�X�N���v�^
struct CRGazeControllerDesc: public CRGazeControllerState{
	CREyeControllerIf*  crEyeCtrl;
	CRNeckControllerIf* crNeckCtrl;
	CRGazeControllerDesc(){
		crEyeCtrl  = NULL;
		crNeckCtrl = NULL;
	}
};

/// �����R���g���[���i�ዅ�^���E��^���̃R���g���[���̏�ʋ@�\�j�̃C���^�[�t�F�C�X
struct CRGazeControllerIf : SceneObjectIf{
	IF_DEF(CRGazeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
		@param attractiveness ���ӂ������x����
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;

	/** @brief ���䏈�������s����
	*/
	virtual void Step()= 0;
};

// ------------------------------------------------------------------------------

//@}

}

#endif//SPR_CREyeControllerIf_H
