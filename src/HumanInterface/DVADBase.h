/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVADBASE_H
#define VR_DVADBASE_H

#include <HumanInterface/HIVirtualDevice.h>

namespace Spr {

//----------------------------------------------------------------------------
//	DVAdBase

///	A/D��1�`�����l����\���D
class DVAdBase:public HIVirtualDevice{
public:
	SPR_OBJECTDEF_ABST(DVAdBase);
	///	���̓f�W�^���l�̎擾
	virtual int Digit()=0;
	///	���͓d���̎擾
	virtual float Voltage()=0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const =0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "A/D"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif
