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
	void ControlEyeToTargetDir(PHSolidIf* soEye, Vec3f aim);

	/** @brief �x�N�g����XZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����X�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngH(Vec3d v);

	/** @brief �x�N�g����YZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����Y�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngV(Vec3d v);

	// �ŏI�I�ɂ�State�N���X�Ɉړ����邱�Ƃ��]�܂��ϐ��Q
	double locLEyeAxisH, locLEyeAxisV, locREyeAxisH, locREyeAxisV;
	double locErrLH, locErrLV, locErrRH, locErrRV;
	double locDErrLH, locDErrLV, locDErrRH, locDErrRV;
	double locLastErrLH, locLastErrLV, locLastErrRH, locLastErrRV;
	double locHeadAngvelH, locHeadAngvelV;
	Quaterniond qToLoc, qToGlo;

	// -- �ዅ�̏�ԗ�
	float t1, t2, t3, t4; ///< ���������̊p�x���������l
	float eL, eR, eLV, eRV; ///< ���������ƖڕW�����̌덷��
	float vL, vR, vLV, vRV; ///< ���������ƖڕW�����̌덷�ʂ̕ω���
	float last_t1_a, last_t2_a, last_t3_a, last_t4_a; ///< �O��̖ڕW�ʒu�i���p�j
	// -- �ڕW�n�_�֘A
	Vec3f currLookatPos; ///< ���ݎ����ړ����̒����_
	Vec3f currLookatVel; ///< ���ݎ����ړ����̒����_�̈ړ����x�x�N�g��
	Vec3f nextLookatPos; ///< ���̒����_
	Vec3f nextLookatVel; ///< ���̒����_�̈ړ����x�x�N�g��
	// -- �T�b�P�[�h����֘A
	float saccadeTimer; ///< �T�b�P�[�h����̎��Ԍo�߂������^�C�}
	Vec3f saccadeFromL; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������
	Vec3f saccadeFromR; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������
	// -- �X���[�X�p�[�V���[�g����֘A
	float integrator_L, integrator_R, integrator_Lv, integrator_Rv; ///< �ϕ���

public:
	OBJECTDEF(CREyeController, SceneObject);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){
		currLookatPos = currLookatVel = Vec3f(0,0,0);
		nextLookatPos = nextLookatVel = Vec3f(0,0,0);
		saccadeTimer = 0.0f;
		t1 = t2 = t3 = t4 = 0.0f;
		eL = eR = eLV = eRV = 0.0f;
		vL = vR = vLV = vRV = 0.0f;
		last_t1_a = last_t2_a = last_t3_a = last_t4_a = 0.0f;
		integrator_L = integrator_R = integrator_Lv = integrator_Rv = 0.0f;
		locLastErrLH=0; locLastErrLV=0; locLastErrRH=0; locLastErrRV=0;
	}
	CREyeController(const CREyeControllerDesc& desc, SceneIf* s=NULL)
		:CREyeControllerDesc(desc){ 
		if(s){SetScene(s);}
		currLookatPos = currLookatVel = Vec3f(0,0,0);
		nextLookatPos = nextLookatVel = Vec3f(0,0,0);
		saccadeTimer = 0.0f;
		t1 = t2 = t3 = t4 = 0.0f;
		eL = eR = eLV = eRV = 0.0f;
		vL = vR = vLV = vRV = 0.0f;
		last_t1_a = last_t2_a = last_t3_a = last_t4_a = 0.0f;
		integrator_L = integrator_R = integrator_Lv = integrator_Rv = 0.0f;
		locLastErrLH=0; locLastErrLV=0; locLastErrRH=0; locLastErrRV=0;
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