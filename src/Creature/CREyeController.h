/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CREYECONTROLLER_H
#define CREYECONTROLLER_H

#include <SprCreature.h>
#include <SprFoundation.h>
#include <SprPhysics.h>

#include <Foundation/Object.h>
#include "IfStubDumpCreature.h"

namespace Spr{;

/// �ዅ�^���R���g���[���̎���
class CREyeController : public SceneObject,	public CREyeControllerIfInit, 	public CREyeControllerDesc {
private:
	/** @brief ���o����ɗp����ዅ�̏�ԗʂ��v�Z����
	*/
	void CalcEyeStatusValue();

	/** @brief ���̐����Ԃ����߂�
	*/
	CREyeControllerState::ControlState GetNextState(ControlState currentCS);

	/** @brief�T�b�P�[�h��������s����
	*/
	void SaccadeControl();

	/** @brief �X���[�X�p�[�V���[�g��������s����
	*/
	void PursuitControl();

	/** @brief �ዅ�̌�����ڕW�̕�����PD���䂷��
		@param soEye ����Ώۂ̊ዅ
		@param aim   �ዅ�����̖ڕW�l
	*/
	void ControlEyeToTargetDir(PHSolidIf* soEye, Vec3d aim);

	/** @brief �x�N�g����XZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����X�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngH(Vec3d v);

	/** @brief �x�N�g����YZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����Y�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngV(Vec3d v);

	// �ŏI�I�ɂ�State�N���X�Ɉړ����邱�Ƃ��]�܂��ϐ��Q
	// -- �ዅ����ѓ����̏�ԗ�
	double locLEyeAxisH, locLEyeAxisV, locREyeAxisH, locREyeAxisV;
	double locErrLH, locErrLV, locErrRH, locErrRV;
	double locDErrLH, locDErrLV, locDErrRH, locDErrRV;
	double locHeadAngvelH, locHeadAngvelV;
	Quaterniond qToLoc, qToGlo;
	double locLastErrLH, locLastErrLV, locLastErrRH, locLastErrRV;
	bool bContLocErr;
	// -- �ڕW�n�_
	Vec3f lookatPos; ///< �����_
	Vec3f lookatVel; ///< �����_�̈ړ����x�x�N�g��
	// -- �T�b�P�[�h����֘A
	double saccadeTimer; ///< �T�b�P�[�h����̎��Ԍo�߂������^�C�}
	Vec3d  saccadeFromL; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������
	Vec3d  saccadeFromR; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������
	// -- �X���[�X�p�[�V���[�g����֘A
	double integrator_L, integrator_R, integrator_Lv, integrator_Rv; ///< �ϕ���

public:
	OBJECTDEF(CREyeController, SceneObject);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){
		saccadeTimer = 0.0f;
		integrator_L = integrator_R = integrator_Lv = integrator_Rv = 0.0f;
		locLastErrLH=0; locLastErrLV=0; locLastErrRH=0; locLastErrRV=0;
		bContLocErr = false;
	}
	CREyeController(const CREyeControllerDesc& desc, SceneIf* s=NULL)
		:CREyeControllerDesc(desc){ 
		if(s){SetScene(s);}
		saccadeTimer = 0.0f;
		integrator_L = integrator_R = integrator_Lv = integrator_Rv = 0.0f;
		locLastErrLH=0; locLastErrLV=0; locLastErrRH=0; locLastErrRV=0;
		bContLocErr = false;
	}

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel);

	/** @brief ���䏈�������s����
	*/
	virtual void Step();
};

}

#endif//CREYECONTROLLER_H