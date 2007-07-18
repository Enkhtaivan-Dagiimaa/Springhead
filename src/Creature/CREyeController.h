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

//@{
namespace Spr{;

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CRPursuitController
// �X���[�X�p�[�V���[�g�p�R���g���[���̏�ԗʒ�`�N���X
class CRPursuitControllerState {
public:
	double angleLH, angleRH, angleV;
	double intL, intR;
	double lastIL, lastIR;

	CRPursuitControllerState() {
		angleLH = angleRH = angleV = 0.0;
		lastIL = lastIR = 0.0;
	}

	void Reset(){
		intL = intR = 0.0;
		lastIL = lastIR = 0.0;
	}
};

// �X���[�X�p�[�V���[�g�p�R���g���[���̃p�����[�^��`�N���X
class CRPursuitControllerDesc : public CRPursuitControllerState {
public:
	double R1, R2;
	double N, H, S, K;
	double A1;
	CRPursuitControllerDesc() : CRPursuitControllerState() {
		R1 = 1.5;
		R2 = 0.5;
		N  = 0.5;
		H  = 0.2;
		S  = 1.0;
		K  = 0.5;
		A1 = 0.5;
	}
};

// �X���[�X�p�[�V���[�g�p�R���g���[��(CREyeController�������Ŏg���A��API�N���X)
class CRPursuitController : public CRPursuitControllerDesc {
private:
public:
	CRPursuitController() : CRPursuitControllerDesc() {}
	CRPursuitController(CRPursuitControllerDesc& desc) : CRPursuitControllerDesc(desc){}
	void StepHoriz(double destLH, double destRH, double angleHeadH, double dt);
	void StepVert(double destV, double angleHeadV, double dt);
};


// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
// CREyeController
/// �ዅ�^���R���g���[���̎���
class CREyeController : public SceneObject,	public CREyeControllerIfInit, public CREyeControllerDesc {
private:
	/** @brief ���o����ɗp����ዅ�̏�ԗʂ��v�Z����
	*/
	void CalcEyeStatusValue();

	/** @brief ���̐����Ԃ����߂�
	*/
	CREyeControllerState::ControlState GetNextState(ControlState currentCS);

	/** @brief �ዅ�̌�����ڕW�̕�����PD���䂷��
		@param soEye ����Ώۂ̊ዅ
		@param aim   �ዅ�����̖ڕW�l
	*/
	void ControlEyeToTargetDir(PHSolidIf* soEye, Vec3d aim);

	/** @brief �ዅ�̌�����ڕW�̕�����PD���䂷��
		@param soEye ����Ώۂ̊ዅ
		@param horiz �ڕW�p�x�i���������j
		@param vert  �ڕW�p�x�i���������j
	*/
	void ControlEyeToTargetDir(PHSolidIf* soEye, double horiz, double vert);

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
	Quaterniond qToLoc, qToGlo;
	double locLEyeAxisH, locLEyeAxisV, locREyeAxisH, locREyeAxisV;
	double locLLookatH,  locLLookatV,  locRLookatH,  locRLookatV;
	// -- �ڕW�n�_
	Vec3f lookatPos; ///< �����_
	Vec3f lookatVel; ///< �����_�̈ړ����x�x�N�g��
	
	// �X���[�X�p�[�V���[�g�R���g���[��
	CRPursuitController pursuitCtrl;

public:
	OBJECTDEF(CREyeController, SceneObject);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){}
	CREyeController(const CREyeControllerDesc& desc, SceneIf* s=NULL) : CREyeControllerDesc(desc) {if(s){SetScene(s);}}

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel);

	/** @brief ���䏈�������s����
	*/
	virtual void Step();
};

}//@}

#endif//CREYECONTROLLER_H