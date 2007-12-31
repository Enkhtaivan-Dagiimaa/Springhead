/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVCOUNTERBASE_H
#define VR_DVCOUNTERBASE_H

#include <HumanInterface/HIVirtualDevice.h>

namespace Spr {

//----------------------------------------------------------------------------
//	DVCountBase

///	�J�E���^��1�`�����l����\���D
class SPR_DLL DVCounterBase:public HIVirtualDevice{
public:
	DVCounterBase();
	///	�J�E���^�l�̐ݒ�
	virtual void Count(long count)=0;
	///	�J�E���^�l�̓ǂݏo��
	virtual long Count()=0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const=0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "Counter"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif
