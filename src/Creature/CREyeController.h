/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CREYECONTROLLER_H
#define CREYECONTROLLER_H

#include <Springhead.h>

#include <Foundation/Object.h>
#include "IfStubCreature.h"

//@{
namespace Spr{;
// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �T�b�P�[�h�p�R���g���[��(CREyeController�������Ŏg���A��API�N���X)
*/
class CRSaccadeController {
private:
	/// �p�x�w�ߒl
	double angleLH, angleRH, angleV;

public:
	CRSaccadeController() {
	}

	/** @brief ��Ԃ��[�����Z�b�g
	*/
	void Reset();

	/** @brief ���݂̏�Ԃ��w�肵�ă��Z�b�g
	*/
	void Reset(double angleLH, double angleRH, double angleV);

	/** @brief ���������̖ڕW�l���Z�b�g���Đϕ�
	*/
	void StepHoriz(double destLH, double destRH, double angleHeadH, double dt);

	/** @brief ���������̖ڕW�l���Z�b�g���Đϕ�
	*/
	void StepVert(double destV, double angleHeadV, double dt);

	/** @brief  ���ڂ̊p�x�w�ߒl���擾
		@return Vec2d(�����p�x, �����p�x) ���ʂ�0�x, ��]�����͉E����W�n�ɏ�����
	*/
	Vec2d GetLEyeAngle();

	/** @brief �E�ڂ̊p�x�w�ߒl���擾
		@return Vec2d(�����p�x, �����p�x) ���ʂ�0�x, ��]�����͉E����W�n�ɏ�����
	*/
	Vec2d GetREyeAngle();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �X���[�X�p�[�V���[�g�p�R���g���[��(CREyeController�������Ŏg���A��API�N���X)
*/
class CRPursuitController {
private:
	// ��ԕϐ�
	/// �ϕ���
	double intL, intR;
	/// �O��̌덷�l
	double lastIL, lastIR;
	/// �p�x�w�ߒl
	double angleLH, angleRH, angleV;

public:
	// �R���g���[���̌W��
	/// �o�͂ɂ�����W��
	double R1, R2;
	/// ���W�덷�̓��͂ɂ�����W��
	double N, H, S, K;
	/// �����^���̓��͂ɂ�����W��
	double A1;

	CRPursuitController() {
		// ���搶�̘_���ɂ��f�t�H���g�l
		R1 = 1.5;  R2 = 0.5;
		N  = 0.5;  H  = 0.2;  S  = 1.0;  K  = 0.5;
		A1 = 0.5;
		Reset();
	}

	/** @brief ��Ԃ��[�����Z�b�g
	*/
	void Reset();

	/** @brief ���݂̏�Ԃ��w�肵�ă��Z�b�g
	*/
	void Reset(double angleLH, double angleRH, double angleV);

	/** @brief ���������̖ڕW�l���Z�b�g���Đϕ�
	*/
	void StepHoriz(double destLH, double destRH, double angleHeadH, double dt);

	/** @brief ���������̖ڕW�l���Z�b�g���Đϕ�
	*/
	void StepVert(double destV, double angleHeadV, double dt);

	/** @brief  ���ڂ̊p�x�w�ߒl���擾
		@return Vec2d(�����p�x, �����p�x) ���ʂ�0�x, ��]�����͉E����W�n�ɏ�����
	*/
	Vec2d GetLEyeAngle();

	/** @brief �E�ڂ̊p�x�w�ߒl���擾
		@return Vec2d(�����p�x, �����p�x) ���ʂ�0�x, ��]�����͉E����W�n�ɏ�����
	*/
	Vec2d GetREyeAngle();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �ዅ�^���R���g���[���̎���
*/
class CRPhysicalEye {
private:
	/// ����Ɏg�p���鍄��
	PHSolidIf *soLEye, *soREye, *soHead;
	/// ���W�ʒu
	Vec3d targetPos, targetVel;
	/// ����̌W��
	double Kp, Kd;

	/** @brief �ЖڂɊւ��Đ���
	*/
	void Control(PHSolidIf* soEye, Vec2d angle);

	/** @brief �x�N�g����XZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����X�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngH(Vec3d v);

	/** @brief �x�N�g����YZ���ʂւ̎ˉe��Z���ƂȂ��p�iZ����Y�������̉�]�𐳂Ƃ���j
		@param v ���̓x�N�g��
	*/
	double Vec3ToAngV(Vec3d v);

public:
	CRPhysicalEye() : soLEye(NULL), soREye(NULL), soHead(NULL) {
		Kp = Kd = 0.0;
	}

	CRPhysicalEye(PHSolidIf* lEye, PHSolidIf* rEye, PHSolidIf* head, double kp, double kd) : soLEye(lEye), soREye(rEye), soHead(head), Kp(kp), Kd(kd) {
	}

	/** @brief �R������ԓ��̎��W�ʒu��ݒ肷��i��Έʒu�j
	*/
	void SetTarget(Vec3d pos, Vec3d vel);

	/** @brief �ዅ�^����������s����
		@param angleLEye ���ڂ̊p�x�w�ߒl
		@param angleREye �E�ڂ̊p�x�w�ߒl
	*/
	void Control(Vec2d angleLEye, Vec2d angleREye);

	/** @brief ���ڂ̌��݂̎����p�x���擾����
	*/
	Vec2d GetAxisL();

	/** @brief �E�ڂ̌��݂̎����p�x���擾����
	*/
	Vec2d GetAxisR();

	/** @brief ���ڂ��猩�����W�����̊p�x���擾����
	*/
	Vec2d GetTargetFromL();

	/** @brief �E�ڂ��猩�����W�����̊p�x���擾����
	*/
	Vec2d GetTargetFromR();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �ዅ�^���R���g���[���̎���
*/
class CREyeController : public SceneObject,	public CREyeControllerIfInit, public CREyeControllerDesc {
private:
	/** @brief ���̐����Ԃ����߂�
	*/
	CREyeControllerState::ControlState GetNextState(ControlState currentCS);

	/** @brief ��������s����
	*/
	void Control();

	/// �������E���ł̊ዅ����舵�����C���[
	CRPhysicalEye physicalEye;
	/// �X���[�X�p�[�V���[�g�R���g���[��
	CRPursuitController pursuitCtrl;
	/// �T�b�P�[�h�R���g���[��
	CRSaccadeController saccadeCtrl;

public:
	OBJECTDEF(CREyeController, SceneObject);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){}
	CREyeController(const CREyeControllerDesc& desc, SceneIf* s=NULL) : CREyeControllerDesc(desc) {
		if(s){SetScene(s);}
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

}//@}

#endif//CREYECONTROLLER_H