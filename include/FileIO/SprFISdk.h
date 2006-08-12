/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FISDKIF_H
#define SPR_FISDKIF_H
#include <Foundation/SprObject.h>

namespace Spr{;


/** \addtogroup gpFileIO	*/
//@{

struct FIFileXIf;

///	�t�@�C�����o��SDK
struct FISdkIf : public ObjectIf{
	IF_DEF(FISdk);
	virtual FIFileXIf* CreateFileX()=0;
	virtual void Clear()=0;
};
///	�t�@�C�����o��SDK�̍쐬
FISdkIf* SPR_CDECL CreateFISdk();
///	���낢���SDK�̍쐬
ObjectIf* SPR_CDECL CreateSdk(const IfInfo* info, const void* desc);
/// Sdk�̃t�@�N�g���̓o�^
void SPR_CDECL RegisterSdk(SdkFactoryBase* sdkFactory);

//@}

}	//	namespace Spr
#endif
