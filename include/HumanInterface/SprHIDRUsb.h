/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprHISdk.h
 *	@brief �q���[�}���C���^�t�F�[�XSDK
*/
#ifndef SPR_HIDRUsbIF_H
#define SPR_HIDRUsbIF_H
#include <HumanInterface/SprHIBase.h>

namespace Spr{;

/** \addtogroup gpPhysics */
//@{


//	�f�X�N���v�^
struct DRUsb20SimpleDesc{
	int number;
	DRUsb20SimpleDesc(): number(0){}
};

///	�����V�~�����[�V����SDK
struct DRUsb20SimpleIf : public HIRealDeviceIf{
	IF_DEF(DRUsb20Simple);
};

//@}

}	//	namespace Spr
#endif
