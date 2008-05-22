/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVPIOBASE_H
#define VR_DVPIOBASE_H

#include <HumanInterface/HIVirtualDevice.h>
namespace Spr {;
//----------------------------------------------------------------------------
//	DVPioBase

///	���̓|�[�g��1�[�q������킷�D
class SPR_DLL DVInputPortBase:public HIVirtualDevice, public DVPortEnum{
public:
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	���̓|�[�g�̓d�����x��(Hi:1, Lo:0) ��Ԃ��D
	virtual int Get()=0;

	///	�f�o�C�X�̎��
	static const char* TypeS() { return "input port"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

///	�o�̓|�[�g��1�[�q������킷�D
class SPR_DLL DVOutputPortBase:public HIVirtualDevice, public DVPortEnum{
public:
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	���̓|�[�g�̓d�����x��(Hi:1, Lo:0) ��Ԃ��D
	virtual void Set(int l)=0;

	///	�f�o�C�X�̎��
	static const char* TypeS() { return "output port"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};


///	PIO��1�[�q��\���D
class SPR_DLL DVPioBase:public HIVirtualDevice, public DVPortEnum{
public:
	SPR_OBJECTDEF_ABST(DVPioBase);
	///	�|�[�g�̃��W�b�N���x���̓��́BHi:true Lo:false
	virtual int Get()=0;
	///	�|�[�g�̃��W�b�N���x���̏o�́BHi:true Lo:false
	virtual void Set(int l)=0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "PIO"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr
#endif
