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
#ifndef SPR_HISdkIF_H
#define SPR_HISdkIF_H
#include <HumanInterface/SprHIBase.h>

namespace Spr{;

/** \addtogroup gpHumanInterface �q���[�}���C���^�t�F�[�XSDK */
//@{

struct PHSceneIf;
struct PHSceneDesc;

//	�������[�h�̂��߂̒��g�Ȃ��̃f�X�N���v�^
struct HISdkDesc{
};

///	�����V�~�����[�V����SDK
struct HISdkIf : public NameManagerIf{
	SPR_IFDEF(HISdk);

	///	SDK�̏�����
	void Init();
	///	�쐬�E�o�^����RealDevice/VirtualDevice/HumanInterface�����ׂč폜
	void Clear();
	///	Rent device.
	HIVirtualDeviceIf* RentVirtualDevice(const char* type, const char* name=NULL);
	HIVirtualDeviceIf* RentVirtualDeviceNo(const char* type, int No, const char* name=NULL );
	///	Return virutal device.
	bool ReturnVirtualDevice(HIVirtualDeviceIf* dev);
	
	///	Create and register real device.
	bool AddRealDevice(const IfInfo* keyInfo, const void* desc=NULL);

	///	Get real device.
	HIRealDeviceIf* FindRealDevice(const char* name=NULL);
	///	�q���[�}���C���^�t�F�[�X�̍쐬
	UTRef<HIBaseIf> CreateHumanInterface(const IfInfo* info);
	///	�q���[�}���C���^�t�F�[�X�̍쐬
	UTRef<HIBaseIf> CreateHumanInterface(const char* name);

	//	SDK�����A�o�^�֐�
	///	HISdk�̃C���X�^���X���쐬
	static HISdkIf* SPR_CDECL CreateSdk();
	///	HISdk���t�@�C�����[�_�[�Ȃǂɓo�^
	static void SPR_CDECL RegisterSdk();
};

//@}

}	//	namespace Spr
#endif
