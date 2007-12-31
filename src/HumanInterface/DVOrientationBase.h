/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef VR_DVORIENTATION_BASE_H
#define VR_DVORIENTATION_BASE_H

#include <HumanInterface/HIVirtualDevice.h>
#include <base/Affine.h>

namespace Spr {
//----------------------------------------------------------------------------
//	DVOrientationBase

///	������Ԃ��f�o�C�X
class SPR_DLL DVOrientationBase:public HIVirtualDevice{
public:
	///	�����̎擾(��]�s��ł���킷)
	virtual Vec3f GetEuler() = 0;
	///	�����̎擾(��]�s��ł���킷)
	virtual void GetMatrix(Matrix3f& ori) = 0;
	///	�f�o�C�X�̖��O
	virtual const char* Name() const = 0;
	///	�f�o�C�X�̎��
	static const char* TypeS() { return "Orientation"; }
	///	�f�o�C�X�̎��
	virtual const char* Type() const{ return TypeS(); }
};

}	//	namespace Spr

#endif