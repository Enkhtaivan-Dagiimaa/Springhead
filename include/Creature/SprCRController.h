/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_CRControllerIf_H
#define SPR_CRControllerIf_H

#include <SprFoundation.h>
#include <SprPhysics.h>

namespace Spr{;

//@{a

struct PHSolidIf;
struct PHHingeJointIf;

// ------------------------------------------------------------------------------

/// �R���g���[���̃C���^�[�t�F�C�X
struct CRControllerIf : SceneObjectIf{
	IF_DEF(CRController);

	/** @brief ���������s��
	*/
	virtual void Init()= 0;

	/** @brief ���䏈�������s����
	*/
	virtual void Step()= 0;

	/** @breif �L���E������؂�ւ���
	*/
	virtual void SetEnable(bool enable)= 0;

	/** @brief �L���E��������Ԃ�
	*/
	virtual bool IsEnabled()= 0;
};

/// �R���g���[���̃f�X�N���v�^
struct CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRController);

	CRControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// �ዅ�^���R���g���[���̃C���^�[�t�F�C�X
struct CREyeControllerIf : CRControllerIf{
	IF_DEF(CREyeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel)= 0;
};

/// �ዅ�^���R���g���[���̃X�e�[�g
struct CREyeControllerState{
	/// ����̏��
	enum ControlState{
		CS_SACCADE, CS_PURSUIT,
	} controlState; 
};

/// �ዅ�^���R���g���[���̃f�X�N���v�^
struct CREyeControllerDesc: public CREyeControllerState, public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CREyeController);

	CREyeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ��^���R���g���[���̃C���^�[�t�F�C�X
struct CRNeckControllerIf : CRControllerIf{
	IF_DEF(CRNeckController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;
};

/// ��^���R���g���[���̃X�e�[�g
struct CRNeckControllerState{
};

/// ��^���R���g���[���̃f�X�N���v�^
struct CRNeckControllerDesc: public CRNeckControllerState, public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRNeckController);

	CRNeckControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// �����R���g���[���i�ዅ�^���E��^���̃R���g���[���̏�ʋ@�\�j�̃C���^�[�t�F�C�X
struct CRGazeControllerIf : CRControllerIf{
	IF_DEF(CRGazeController);

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
		@param attractiveness ���ӂ������x����
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel, float attractiveness)= 0;
};

/// �����R���g���[���̃X�e�[�g
struct CRGazeControllerState{
};

/// �����R���g���[���̃f�X�N���v�^
struct CRGazeControllerDesc: public CRGazeControllerState, public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRGazeController);

	CRGazeControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ���ӃR���g���[���̃C���^�t�F�[�X
struct CRAttentionControllerIf : CRControllerIf{
	IF_DEF(CRAttentionController);

};

/// ���ӃR���g���[���̃f�X�N���v�^
struct CRAttentionControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRAttentionController);

	CRAttentionControllerDesc(){
	}
};

// ------------------------------------------------------------------------------

/// ���B�^���R���g���[���̃C���^�[�t�F�[�X
struct CRReachingControllerIf : CRControllerIf{
	IF_DEF(CRReachingController);

	/** @brief �ڕW�ʒu��ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, float t, float o)= 0;

	/** @brief �ڕW�ʒu�E�p����ݒ肷��
		@param p �ڕW�ʒu
		@param v �ڕW�̑��x
		@param q �ڕW�p��
		@param av �ڕW�p���x
		@param t �ڕW���B����
		@param o ���B��̑ҋ@����
	*/
	virtual void SetTarget(Vec3f p, Vec3f v, Quaterniond q, Vec3f av, float t, float o)= 0;

	/** @brief �쓮�����ǂ�����Ԃ�
	*/
	virtual bool IsActive()= 0;

	/** @brief ���Z�b�g����
	*/
	virtual void Reset()= 0;
};

/// ���B�^���R���g���[���̃f�X�N���v�^
struct CRReachingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRReachingController);

	PHSolidIf* solid; ///< ���B������������
	CRHingeHumanBodyDesc::CRHumanSolids solidNo; ///< ���B������������

	float limitForce; ///< �͂̍ő�l
	float springPos, damperPos; ///< �ʒu����̃o�l�_���p�W��
	float springOri, damperOri; ///< �p������̃o�l�_���p�W��
	float softenRate; ///< �֐߂��_�炩����Ԃɂ��邽�߂̃o�l�_���p�{��
	float hardenRate; ///< �֐߂��ł���Ԃɂ��邽�߂̃o�l�_���p�{��

	CRReachingControllerDesc(){
		limitForce = 1000.0f;
		springPos = 500.0f;  damperPos =  20.0f;
		springOri =  10.0f;  damperOri =   0.5f;
		softenRate = 0.2f;
		hardenRate = 5.0f;
	}
};

// ------------------------------------------------------------------------------
/// ���s�R���g���[���̃C���^�t�F�[�X
struct CRWalkingControllerIf : CRControllerIf{
	IF_DEF(CRWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed)= 0;

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot)= 0;
};

/// ���s�R���g���[���̃f�X�N���v�^
struct CRWalkingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRWalkingController);

	/// ���s�R���g���[���̓��������߂�p�����[�^�������ɋL�q�D
	/// ���s���ɕω���������̂�C�ق��̗v�f�iBody�Ȃǁj����v�Z�ŋ��߂���̂͏����D

	CRWalkingControllerDesc(){
		/// �e�ϐ��Ƀf�t�H���g�̒l������D�[���Ƃ�NULL�ł������D
	}
};

// ------------------------------------------------------------------------------
/// �U���s�R���g���[���F�{���̕��s�R���g���[�����ł���܂ł̒��p��
struct CREseWalkingControllerIf : CRControllerIf{
	IF_DEF(CREseWalkingController);

	/** @brief ���s�̑��x��ݒ肷��
	*/
	virtual void SetSpeed(float speed)= 0;

	/** @brief �]��p�x��ݒ肷��
	*/
	virtual void SetRotationAngle(float rot)= 0;

	/** @brief �ʒu��ݒ肷��i������Ԍ���̂��߁j
	*/
	virtual void SetPos(Vec3f pos)= 0;
};

/// �U���s�R���g���[���̃f�X�N���v�^
struct CREseWalkingControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CREseWalkingController);

	CREseWalkingControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �ړ��R���g���[���F�ڕW�n�_�ւ̈ړ��Ə�Q�����
struct CRTravelControllerIf : CRControllerIf{
	IF_DEF(CRTravelController);

	/** @brief �ڕW�n�_��ݒ肷��
	*/
	virtual void SetGoal(Vec2f goal)= 0;
};

/// �ړ��R���g���[���̃f�X�N���v�^
struct CRTravelControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRTravelController);

	CRTravelControllerDesc(){
	}
};

// ------------------------------------------------------------------------------
/// �c���R���g���[��
struct CRGrabControllerIf : CRControllerIf{
	IF_DEF(CRGrabController);

	/** @brief �͂�
	*/
	virtual void Grab(PHSolidIf* solid, float radius)= 0;

	/** @brief ���݉����͂�ł���΁C����
	*/
	virtual void Ungrab()= 0;

	/** @brief ���݉����͂�ł���΁C����������Ĉړ�����
	*/
	virtual void MoveTo(Vec3f pos)= 0;

	/** @brief ���݂̔c���̏�Ԃ�Ԃ�
	*/
	enum GrabState{
		GS_STANDBY = 0, /// �ҋ@���
		GS_GRAB_START,  /// �c������J�n
		GS_GRAB,        /// �c����
		GS_MOVE,        /// �c�����̂��ړ���
	};
	virtual CRGrabControllerIf::GrabState GetGrabState()= 0;
};

/// �c���R���g���[���̃f�X�N���v�^
struct CRGrabControllerDesc : public CRControllerDesc{
	DESC_DEF_FOR_OBJECT(CRGrabController);

	CRGrabControllerDesc(){
	}
};

//@}

}

#endif//SPR_CRControllerIf_H
