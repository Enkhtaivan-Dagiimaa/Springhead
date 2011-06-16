/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHISpidar.h
 *	@brief �͊o�񎦃f�o�C�XSpidar
*/

#ifndef SPR_HISpidar_H
#define SPR_HISpidar_H

#include <HumanInterface/SprHIBase.h>

namespace Spr{;

/**	\addtogroup	gpHumanInterface	*/
//@{

struct HISpidarMotorIf: public HIBaseIf{
	SPR_IFDEF(HISpidarMotor);

	void SetForce(float f);
	///	���ۂɏo�͂����͂̎擾
	float GetForce();
	///	�d���l�̎擾
	float GetCurrent();
	///	���݂̎��̒����̐ݒ�
	void SetLength(float l);
	///	���݂̎��̒���
	float GetLength();
	///	���x�̌v��
	float GetVelocity();
	///	���[�^�萔�̃L�����u���[�V�����̂��߂̌v�� (100�񂭂炢������Ă�ł���)
	void GetCalibrationData(float dt);
	///	���[�^�萔�̃L�����u���[�V���� (���������Ă�ŃL�����u���[�V����)
	void Calibrate(bool bUpdate);
	///	�d���d���̌v��
	void GetVdd();
	///	 �G���R�[�_�̃J�E���g�l��Ԃ�
	int GetCount();

};
///	@name	���[�^�ɂ��Ă̒萔
struct HISpidarMotorDesc{
	//@{
	/// �|�[�g�ԍ�
	int	ch;
	///	�ő�o��(N)
	float maxForce;
	///	�ŏ��o��(N)
	float minForce;
	///	D/A�̏o�͓d�� / ���[�^�̏o��  (V/N)
	float voltPerNewton;
	///	���[�^�ɗ����d�� / D/A�̏o�͓d��
	float currentPerVolt;
	///	���̒��� / �p���X��
	float lengthPerPulse;
	///	���[�^�����t�����Ă���ʒu(���̏o��)�̍��W
	Vec3f pos;
	///	�����O���b�v�Ɏ��t�����Ă���ʒu
	Vec3f jointPos;

	HISpidarMotorDesc(){
		ch				= -1;
		maxForce		= 20.0f;
		minForce		= 0.5f;
		voltPerNewton	= 0.365296803653f;
		currentPerVolt	= 1.0f;
		lengthPerPulse	= 2.924062107079e-5f;
	}
};

struct HISpidarIf : public HIHapticIf{
	SPR_VIFDEF(HISpidar);
};
struct HISpidarDesc{
	SPR_DESCDEF(HISpidar);
	
	std::vector<HISpidarMotorDesc> motors;
	int	nButton;
};

/**	@brief	Spidar4�̊�{�N���X	*/
struct HISpidar4If: public HISpidarIf{
	SPR_VIFDEF(HISpidar4);

	///	���[�^
	HISpidarMotorIf* GetMotor(size_t i);
	///	���[�^�̐�
	size_t NMotor() const;
};
///	Spidar4�̃f�X�N���v�^
struct HISpidar4Desc : HISpidarDesc{
	SPR_DESCDEF(HISpidar4);
	
	HISpidar4Desc();
	HISpidar4Desc(char* type){
		Init(type);
	}
	HISpidar4Desc(char* type , Vec4i portNum){
		Init(type);
		for(int i = 0; i < 4; i++)
			motors[i].ch = portNum[i];
	}
	HISpidar4Desc(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF=0.5f, float maxF=10.0f){
		Init(nMotor, motorPos, knotPos, vpn, lpp, minF, maxF);
	}
	///	�p�����[�^�ɂ�鏉����
	void Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF=0.5f, float maxF=10.0f);
	///	SPIDAR�̃^�C�v�w��ɂ�鏉����
	void Init(char* type);
	void InitSpidarG(char* type);
	void InitSpidarBig(char* type);
};

/**	@brief	Spidar4�̊�{�N���X	*/
struct HISpidar4DIf: public HISpidar4If{
	SPR_VIFDEF(HISpidar4D);
};
struct HISpidar4DDesc: public HISpidar4Desc{
};

/**	@brief	SpidarG6�̊�{�N���X	*/
struct HISpidarGIf: public HISpidarIf{
	SPR_VIFDEF(HISpidarG);
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	Vec3f GetTorque();
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	Vec3f GetForce();
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	void SetForce(const Vec3f& f, const Vec3f& t);

	///	���[�^
	HISpidarMotorIf* GetMotor(size_t i);
	///	���[�^�̐�
	size_t NMotor() const;
	/// 
	int GetButton(size_t i);
	///
	size_t NButton() const;
};
///	SpidarG6�̃f�X�N���v�^
struct HISpidarGDesc : HISpidarDesc{
	SPR_DESCDEF(HISpidarG);

	HISpidarGDesc();
	HISpidarGDesc(char* type){
		Init(type);
	}
	HISpidarGDesc(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF=0.5f, float maxF=10.0f){
		Init(nMotor, motorPos, knotPos, vpn, lpp, minF, maxF);
	}
	///	�p�����[�^�ɂ�鏉����
	void Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF=0.5f, float maxF=10.0f);
	///	SPIDAR�̃^�C�v�w��ɂ�鏉����
	void Init(char* type);
};

//@}
}

#endif
