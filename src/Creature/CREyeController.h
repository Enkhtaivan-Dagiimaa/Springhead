/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef CREYECONTROLLER_H
#define CREYECONTROLLER_H

#include <Creature/CREngine.h>
#include <Creature/SprCRGazeController.h>

//@{
namespace Spr{;

struct PHSolidIf;
struct PHHingeJointIf;
struct PHBallJointIf;

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
	void StepHoriz(double destLH, double destRH, double angvelHeadH, double dt);

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
/** @brief �ዅ�R���g���[���̕������C��(CREyeController�������Ŏg���A��API�N���X)
*/
class CRPhysicalEye {
private:
	/// ����Ώۂ̃N���[�`��
	CRCreatureIf* creature;

	/// ����Ɏg�p���鍄��
	PHSolidIf *soLEye, *soREye, *soHead;

	/// ����Ɏg�p����֐�
	PHHingeJointIf *joLEyeX, *joLEyeY, *joREyeX, *joREyeY;
	PHBallJointIf  *joLEye,  *joREye;

	/// ���W�ʒu
	Vec3d targetPos, targetVel;

	/** @brief �ЖڂɊւ��Đ���
	*/
	void Control(PHHingeJointIf* joX, PHHingeJointIf* joY, Vec2d angle);
	void Control(PHBallJointIf*  jo, Vec2d angle);

	/** @brief (0,0,-1)�Ɛ����p���A�E��n��X���EY������̉�]�Ƃ��ċ��߂�
		@param v ���̓x�N�g��
		@return (ZY�ˉe��(0,0,-1)�Ɛ����p(X����]), ZX�ˉe��(0,0,-1)�Ɛ����p(Y����]))
	*/
	Vec2d Vec3ToAngle(Vec3d v);

public:
	CRPhysicalEye() {
		soLEye = soREye = soHead = NULL;
		joLEyeX = joLEyeY = joREyeX = joREyeY = NULL;
		joLEye = joREye = NULL;
	}

	void SetCreature(CRCreatureIf* c) { creature = c; }

	/** @brief �����������s����
	*/
	void Init();

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

	/** @brief �����̊p�x�i��΍��W�n�j���擾����
	*/
	Vec2d GetHeadAngle();

	/** @brief �����̊e���x�i�������W�n�j���擾����
	*/
	Vec2d GetHeadAngvel();
};

// --- --- --- --- --- --- --- --- --- --- --- --- --- --- ---
/** @brief �ዅ�^���R���g���[���̎���
*/
class CREyeController : public CREngine, public CREyeControllerDesc {
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
	SPR_OBJECTDEF(CREyeController);
	ACCESS_DESC_STATE(CREyeController);

	CREyeController(){}
	CREyeController(const CREyeControllerDesc& desc) 
		: CREyeControllerDesc(desc) 
	{
		// �]�������Ł�����Ȋ����̂��Ƃ�����Ă��������͂��Ԃ񂱂�ł͓����Ȃ�
		// �̂ŕʂ̏ꏊ�Ɉڂ��K�v������i09/06/20 mitake�j
		// physicalEye->SetCreature(GetScene()->Cast());
	}

	/** @brief �����������s����
	*/
	virtual void Init();

	/** @brief ���䏈�������s����
	*/
	virtual void Step();

	/** @brief �����_��ݒ肷��
		@param pos �����_�̂R�������W
		@param vel �����_�̈ړ����x�x�N�g��
	*/
	virtual void LookAt(Vec3f pos, Vec3f vel);

	/** @brief �T�b�P�[�h�����ǂ�����Ԃ�
	*/
	virtual bool IsSaccading();

	// ��API�֐�
	/** @brief ���݂̃R���g���[����Ԃ��擾����
	*/
	CREyeControllerState::ControlState GetControlState();
};

}//@}

#endif//CREYECONTROLLER_H