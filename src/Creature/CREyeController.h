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
	CREyeControllerState::ControlState GetNextState(ControlState currentCS);
	void SaccadeControl();
	void PursuitControl();
	void ControlEyeToTargetDir(PHSolidIf* soEye, Vec3f aim);

	// �ŏI�I�ɂ�State�N���X�Ɉړ����邱�Ƃ��]�܂��ϐ��Q
	Vec3f currLookatPos; ///< ���ݎ����ړ����̒����_
	Vec3f currLookatVel; ///< ���ݎ����ړ����̒����_�̈ړ����x�x�N�g��
	Vec3f nextLookatPos; ///< ���̒����_
	Vec3f nextLookatVel; ///< ���̒����_�̈ړ����x�x�N�g��

	float saccadeTimer; ///< �T�b�P�[�h����̎��Ԍo�߂������^�C�}
	Vec3f saccadeFromL; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������
	Vec3f saccadeFromR; ///< �T�b�P�[�h�J�n���̍��ڂ̎�������


public:
	OBJECTDEF(CREyeController, SceneObject);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){
		currLookatPos = currLookatVel = Vec3f(0,0,0);
		nextLookatPos = nextLookatVel = Vec3f(0,0,0);
		saccadeTimer = 0.0;
	}
	CREyeController(const CREyeControllerDesc& desc, SceneIf* s=NULL)
		:CREyeControllerDesc(desc){ if(s){SetScene(s);}	}

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