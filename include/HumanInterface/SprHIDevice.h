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


///	���f�o�C�X�D�C���^�t�F�[�X�J�[�h�Ƃ��CUSB�f�o�C�X�Ƃ��C
struct HIRealDeviceIf:  public ObjectIf{
	SPR_IFDEF(HIRealDevice);
	///	�f�o�C�X�̖��O�D�N���X�ƑΉ�����{�[�h��ID�Ȃǂ��玩���Ō��܂閼�O�D�ݒ�s�D
	const char* Name();
	///	������
	bool Init();
	///	���z�f�o�C�X�̓o�^
	void Register(HISdkIf* sdk);
	///	��Ԃ̍X�V
	void Update();
};

///	�o�[�`�����f�o�C�X�DA/D, D/A��1�`�����l�����Ƃ��C�@�\�Ƃ��Ẵf�o�C�X�D
struct HIVirtualDeviceIf:  public ObjectIf{
	SPR_IFDEF(HIVirtualDevice);
public:
	///	�f�o�C�X�̖��O
	const char* Name() const;
	///	�f�o�C�X�̎��
	const char* Type() const;
	///	���f�o�C�X�ւ̃|�C���^
	HIRealDeviceIf* RealDevice();
	///	�_���v�o��
	void Print(std::ostream& o) const;
	///
	void Update();
};

struct DVAdBaseIf:public HIVirtualDeviceIf{
	SPR_IFDEF(DVAdBase);
public:
	///	���̓f�W�^���l�̎擾
	int Digit();
	///	���͓d���̎擾
	float Voltage();
	///	�f�o�C�X�̖��O
	const char* Name() const;
	///	�f�o�C�X�̎��
	const char* Type() const;
};
struct DVDaBaseIf:public HIVirtualDeviceIf{
	SPR_IFDEF(DVDaBase);
public:
	///	�o�͂���f�W�^���l�̐ݒ�
	void Digit(int d);
	///	�o�͓d���̐ݒ�
	void Voltage(float volt);
	///	�f�o�C�X�̖��O
	const char* Name() const;
	///	�f�o�C�X�̎��
	const char* Type();
};
struct DVCounterBaseIf:public HIVirtualDeviceIf{
	SPR_IFDEF(DVCounterBase);
public:
	///	�J�E���^�l�̐ݒ�
	void Count(long count);
	///	�J�E���^�l�̓ǂݏo��
	long Count();
	///	�f�o�C�X�̖��O
	const char* Name() const;
	///	�f�o�C�X�̎��
	const char* Type() const;
};

///	���o�̓|�[�g�̂��߂̒萔�̒�`�Ȃǂ������s���D
struct DVPortEnum{
	enum TLevel {LEVEL_LO, LEVEL_HI};
	enum TDir {DIR_IN, DIR_OUT};
};
///
struct DVPioBaseIf:public HIVirtualDeviceIf, public DVPortEnum{
	SPR_IFDEF(DVPioBase);
public:
	///	�|�[�g�̃��W�b�N���x���̓��́BHi:true Lo:false
	int Get();
	///	�|�[�g�̃��W�b�N���x���̏o�́BHi:true Lo:false
	void Set(int l);
	///	�f�o�C�X�̖��O
	const char* Name() const;
	///	�f�o�C�X�̎��
	const char* Type() const;
};


//@}
}

#endif
