/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
	IF_DEF(HIRealDevice);
	///	�f�o�C�X�̖��O�D�N���X�ƑΉ�����{�[�h��ID�Ȃǂ��玩���Ō��܂閼�O�D�ݒ�s�D
	virtual const char* Name()=0;
	///	������
	virtual bool Init()=0;
	///	���z�f�o�C�X�̓o�^
	virtual void Register(HISdkIf* sdk)=0;
	///	��Ԃ̍X�V
	virtual void Update()=0;
};

///	�o�[�`�����f�o�C�X�DA/D, D/A��1�`�����l�����Ƃ��C�@�\�Ƃ��Ẵf�o�C�X�D
struct HIVirtualDeviceIf:  public ObjectIf{
	IF_DEF(HIVirtualDevice);
public:
	///	�f�o�C�X�̖��O
	virtual const char* Name() const=0;
	///	�f�o�C�X�̎��
	virtual const char* Type() const=0;
	///	���f�o�C�X�ւ̃|�C���^
	virtual HIRealDeviceIf* RealDevice()=0;
	///	�_���v�o��
	virtual void Print(std::ostream& o) const=0;
	///
	virtual void Update()=0;
};

//@}
}

#endif
