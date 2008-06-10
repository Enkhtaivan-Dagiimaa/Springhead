/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HIMouse6D6D_H
#define HIMouse6D6D_H

#include <HumanInterface/HIBase.h>
#include <HumanInterface/DVKeyMouse.h>

namespace Spr {;

class SPR_DLL HIMouse6D: public HIPose, public DVKeyMouseHandler{
public:
	SPR_OBJECTDEF(HIMouse6D);

	UTRef<DVKeyMouse> keyMouse;
	DVKeyMouseHandler::ButtonState btnState;

	virtual DVKeyMouseIf* GetKeyMouse(){ return keyMouse->Cast(); }

	/// 
	HIMouse6D(){ bGood = false; }
	virtual ~HIMouse6D(){}

	bool IsGood(){return bGood;}

	/// �}�E�X�̏�����
	virtual bool Init(const void* desc);

	///	������Ă���{�^���ɉ�����pos,ori���X�V����BShift��������Ă���Ƃ���axis���X�V����B
	void Update(float dt);

	///	�܂Ƃ߂āC���b�Z�[�W����������ꍇ
	bool PreviewMessage(void* m);
	///@name �ʂ̃��b�Z�[�W�n���h���ŌĂяo���ꍇ
	//@{
	///	�L�[����
	virtual bool OnKeyDown(unsigned nChar);
	/// �h���b�O���̏���
	virtual void OnButtonMove(unsigned int state, int x, int y);
	/// �{�^�����������Ƃ��̏���
	virtual void OnButtonDown(int x, int y);
	/// �{�^���𗣂����Ƃ��̏���
	virtual void OnButtonUp();
	///	�_�u���N���b�N���̏���
	virtual void OnDblClick(unsigned fwKeys, int x, int y);
	/// ���_�u���N���b�N���̏���(�|�C���^�̎p���𐅕��ɏ�����)
	virtual void OnLButtonDClick();
	/// �E�_�u���N���b�N���̏���(�|�C���^�̈ʒu�����_�ɏ�����)
	virtual void OnRButtonDClick();
	/// Shift+���_�u���N���b�N���̏���(����W���̈ʒu�E�p����������)
	virtual void OnShiftLButtonDClick();
	/// �z�C�[�����܂킵���Ƃ��̏���(�|�C���^or����W����Z�������ɐ����ړ�)
	virtual void OnWheel(unsigned int state, short dz);
	//@}

	

	///	�ʒu�̎擾
	Vec3f GetPos(){ return pos; }
	///	�͂̎擾
	Vec3f GetForce(){ return force; }
	///	�g���N�̎擾
	Vec3f GetTorque(){ return torque; }
	///	�p�x�̎擾
	Quaternionf GetOri(){ return ori; }
	/// ����W���̎擾
	Affinef GetAxis(){ return axis; }

	/// �X�P�[���̐ݒ�
	void SetScale(float tp, float rp, float ta, float ra);

	/// ���ʂ̐ݒ�
	void SetMass(float m){ mass = m; }
	/// �ʒu�̐ݒ�
	void SetPos(Vec3f p){ pos = p; }
	/// �p���̐ݒ�
	void SetOri(Quaternionf o){ ori = o; }
	///	���i�́E��]�͂̐ݒ�
	void SetForce(const Vec3f& f, const Vec3f& t){ force = f; torque = t; }
	void SetForce(const Vec3f& f){ force = f; }
	///	�Œ�̗͂�ݒ肷��
	void SetMinForce(){ force = torque = Vec3f(); }	

	/// ����W���̐ݒ�
	void SetAxis(Affinef a){ axis = a; }
	/// �������p���W���̐ݒ�
	void SetAxisInit(Affinef ai){ axisInit = ai; }


	///	�L�����u���[�V����(���ɉ������Ȃ�)
	bool Calib(){ return true; }

//-> �֌��ɂ��ύX (2005/1/05)
	void SetViewSize(float inWidth, float inHeight);
//<-

protected:
	// �}�E�X���A�N�e�B�u���ǂ����̃t���O
	bool bGood;
	// �}�E�X�̍��W(old:�O�X�e�b�v�Cnew:���X�e�b�v)
	int oldX, oldY, newX, newY;
	// �}�E�X�̍��W�Ɖ��z���E�Ƃ̃X�P�[�������p�p�����[�^
	float scaleTransPointer, scaleRotPointer, scaleTransAxis, scaleRotAxis;

	// �|�C���^�Ɋւ���ϐ�
	// ����
	float mass;
	// �|�C���^�ɂ�������i��
	Vec3f force;
	// �|�C���^�ɂ������]��
	Vec3f torque;
	// �|�C���^�̈ʒu�BUpdate()�ōX�V�����
	Vec3f pos;
	// �|�C���^�̎p���BUpdate()�ōX�V�����
	Quaternionf ori;

	// ����W��(�h���b�O���쎞�͂��̍��W���ɂ��ǂÂ��ē��������)
	Affinef axis;
	// �������p���W��
	Affinef axisInit;

//-> �֌��ɂ��ύX (2005/1/05)
	float	viewCenterX, viewCenterY, trackBallRadius;

	Vec3f	GetTrackBallRotation(float inX, float inY);
//<-
};

}	//	namespace Spr
#endif
