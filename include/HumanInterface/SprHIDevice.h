/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprDRDevice.h
 *	@brief �q���[�}���C���^�t�F�[�X����邽�߂̍ޗ��ɂȂ�f�o�C�X�̃N���X�D���Ƃ��΁CD/A,A/D,PIO�ȂǁD
*/

#ifndef SPR_HIDevice_H
#define SPR_HIDevice_H
#include <SprFoundation.h>

namespace Spr{;
struct HISdkIf;

/**	\addtogroup	gpHumanInterface	*/
//@{

/**
	�f�o�C�X�̊�{�N���X
 **/
struct HIDeviceIf : NamedObjectIf{
	SPR_IFDEF(HIDevice);

	///	�f�o�C�X�̖��O�D�N���X�ƑΉ�����{�[�h��ID�Ȃǂ��玩���Ō��܂閼�O�D�ݒ�s�D
	//const char* GetDeviceName() const;	

};

struct HIVirtualDeviceIf;

/**
	���f�o�C�X�D�C���^�t�F�[�X�J�[�h�Ƃ��CUSB�f�o�C�X�Ƃ��C
 **/
struct HIRealDeviceIf : HIDeviceIf{
	SPR_IFDEF(HIRealDevice);

	/// �f�o�C�X�����擾
	//UTString	GetDeviceName();

	///	�������Ɖ��z�f�o�C�X�̓o�^
	bool Init();
	///	���z�f�o�C�X�̓o�^
	//void Register(HISdkIf* sdk);

	/// ���z�f�o�C�X�̎擾
	HIVirtualDeviceIf*	Rent(const IfInfo* ii, const char* name, int portNo);
	/// ���z�f�o�C�X�̕ԋp
	bool				Return(HIVirtualDeviceIf* dv);

	///	��Ԃ̍X�V
	void Update();
};

/**
	�o�[�`�����f�o�C�X�DA/D, D/A��1�`�����l�����Ƃ��C�@�\�Ƃ��Ẵf�o�C�X�D
 **/
struct HIVirtualDeviceIf : HIDeviceIf{
	SPR_IFDEF(HIVirtualDevice);

	/// �|�[�g�ԍ����擾�i�|�[�g�ԍ������f�o�C�X�̂݁j
	int	GetPortNo() const;
	
	/// �g�p��Ԃ̎擾
	bool IsUsed();

	///	���f�o�C�X�ւ̃|�C���^
	HIRealDeviceIf* GetRealDevice();
	
	///
	void Update();
};

/**
	A/D�ϊ�
 **/
struct DVAdIf : HIVirtualDeviceIf{
	SPR_IFDEF(DVAd);

	///	���̓f�W�^���l�̎擾
	int Digit();
	///	���͓d���̎擾
	float Voltage();
};

/**
	D/A�ϊ�
 **/
struct DVDaIf : HIVirtualDeviceIf{
	SPR_IFDEF(DVDa);

	///	�o�͂���f�W�^���l�̐ݒ�
	void Digit(int d);
	///	�o�͓d���̐ݒ�
	void Voltage(float volt);
};

/**
	�J�E���^
 **/
struct DVCounterIf : HIVirtualDeviceIf{
	SPR_IFDEF(DVCounter);

	///	�J�E���^�l�̐ݒ�
	void Count(long count);
	///	�J�E���^�l�̓ǂݏo��
	long Count();
};

///	���o�̓|�[�g�̂��߂̒萔�̒�`�Ȃǂ������s���D
struct DVPortEnum{
	enum TLevel {LEVEL_LO, LEVEL_HI};
	enum TDir {DIR_IN, DIR_OUT};
};

/**
	�p������I/O
 **/
struct DVPioIf : public HIVirtualDeviceIf, public DVPortEnum{
	SPR_IFDEF(DVPio);

	///	�|�[�g�̃��W�b�N���x���̓��́BHi:true Lo:false
	int Get();
	///	�|�[�g�̃��W�b�N���x���̏o�́BHi:true Lo:false
	void Set(int l);
};

/**
	�͂̓���
 */
struct DVForceIf : public HIVirtualDeviceIf{
	SPR_IFDEF(DVForce);

	///	���R�x�̎擾
	int GetDOF();
	///	�͂̎擾
	float GetForce(int ch);
	///	�͂̎擾
	void GetForce3(Vec3f& f);
	///	�͂̎擾
	void GetForce6(Vec3f& f, Vec3f& t);
};

//@}
}

#endif
