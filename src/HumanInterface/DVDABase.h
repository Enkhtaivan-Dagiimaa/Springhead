/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVDABASE_H
#define VR_DVDABASE_H

#include <HumanInterface/HIVirtualDevice.h>

namespace Spr {

//----------------------------------------------------------------------------
//	DVDaBase

///	D/A��1�`�����l����\���D
class SPR_DLL DVDaBase:public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVDaBase);
	DVDaBase();
	///	�o�͂���f�W�^���l�̐ݒ�
	virtual void Digit(int d)=0;
	///	�o�͓d���̐ݒ�
	virtual void Voltage(float volt)=0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "D/A"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif
