/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DRCONTECISACOUNTER_H
#define VR_DRCONTECISACOUNTER_H

#include <HumanInterface/HIRealDevice.h>
#include "DVCounterBase.h"

namespace Spr {

///	Contec��ISA D/A�J�[�h�p�̃h���C�o.
class SPR_DLL DRContecIsaCounter:public HIRealDevice{
public:
	///	���z�f�o�C�X
	class VirtualDevice:public DVCounterBase{
	protected:
		int ch;
		DRContecIsaCounter* realDevice;
		char name[100];
	public:
		VirtualDevice(DRContecIsaCounter* r, int c);
		virtual HIRealDeviceIf* RealDevice() { return realDevice->Cast(); }
		///	�J�E���^�l�̐ݒ�
		virtual void Count(long c){ realDevice->Count(ch, c); }
		///	�J�E���^�l�̓ǂݏo��
		virtual long Count(){ return realDevice->Count(ch); }
		///	���O
		virtual const char* Name() const{ return name; }
	};
protected:
	char name[100];
	///	�J�E���^�{�[�h��IO�A�h���X
	int address;

public:
	/**	�R���X�g���N�^
		@param address		�J�E���^�{�[�h��I/O�A�h���X
	*/
	DRContecIsaCounter(int address);
	///	�f�o�C�X�̖��O
	virtual const char* Name() const { return name; }
	///	������
	virtual bool Init();
	///	���z�f�o�C�X�̓o�^
	virtual void Register(HIVirtualDevicePool& vpool);
	///	�J�E���^�l�̐ݒ�
	void Count(int ch, long c);
	///	�J�E���^�l�̓ǂݏo��
	long Count(int ch);
};
}	//	namespace Spr

#endif