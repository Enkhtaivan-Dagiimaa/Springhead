/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHISpidar.h
 *	@brief �q���[�}���C���^�t�F�[�X�̊�{�N���X
*/


#ifndef SPR_HISpidar_H
#define SPR_HISpidar_H
#include <HumanInterface/SprHIBase.h>

namespace Spr{;

/**	\addtogroup	gpHumanInterface	*/
//@{

struct DRRealDeviceIf;

/**	@brief	SpidarG6�̊�{�N���X	*/
struct HISpidarG6If: public HIForceInterface6DIf{
	SPR_IFDEF(HISpidarG6);
	///	�f�o�C�X�̎��ۂ̒񎦃g���N��Ԃ�
	Vec3f GetTorque();
	///	�f�o�C�X�̎��ۂ̒񎦗͂�Ԃ�
	Vec3f GetForce();
	///	�f�o�C�X�̖ڕW�o�͂ƃg���N�o�͂�ݒ肷��
	void SetForce(const Vec3f& f, const Vec3f& t);
};

///	@name	���[�^�ɂ��Ă̒萔
struct HISpidarMotorDesc{
	//@{
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
};
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

};

///	SpidarG6�̃f�X�N���v�^
struct HISpidarG6Desc{
	SPR_DESCDEF(HISpidarG6);
	std::vector<HISpidarMotorDesc> motors;
	HISpidarG6Desc();
	///	�p�����[�^�ɂ�鏉����
	void Init(int nMotor, Vec3f* motorPos, Vec3f* knotPos, float vpn, float lpp, float minF=0.5f, float maxF=10.0f);
	///	SPIDAR�̃^�C�v�w��ɂ�鏉����
	void Init(char* type="G6X3");
};



//@}
}
#endif
