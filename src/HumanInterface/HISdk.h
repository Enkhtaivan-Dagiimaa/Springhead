/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HISDK_H
#define HISDK_H
#include <Foundation/Foundation.h>
#include <HumanInterface/SprHISdk.h>
#include "IfStubHumanInterface.h"

namespace Spr {;

class HISdkFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return HISdkIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, ObjectIf* parent){
		return HISdkIf::CreateSdk();
	}
};

class HIVirtualDevicePool;
class HIRealDevicePool;

class SPR_DLL HISdk:public Sdk, HISdkIfInit{
protected:
	UTRef<HIVirtualDevicePool> vpool;
	UTRef<HIRealDevicePool> rpool;
public:
	OBJECTDEF(HISdk, Sdk);
	HISdk(const HISdkDesc& = HISdkDesc());
	ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);

	///	SDK�̏�����
	virtual void Init();
	///	�쐬�E�o�^����RealDevice/VirtualDevice/HumanInterface�����ׂč폜
	virtual void Clear();
	///	Rent virtual device.
	virtual HIVirtualDeviceIf* RentVirtualDevice(const char* type, const char* name=NULL);
	///	Return virutal device.
	virtual bool ReturnVirtualDevice(HIVirtualDeviceIf* dev);
	
	///	Register real device.
	virtual void RegisterRealDevice(HIRealDeviceIf* dev);	
	///	Get real device.
	virtual HIRealDeviceIf* FindRealDevice(const char* name=NULL);
	///	Register virutal device.
	virtual void RegisterVirtualDevice(HIVirtualDeviceIf* dev);
	///	�q���[�}���C���^�t�F�[�X�̍쐬
	HIBaseIf* CreateHumanInterface(const IfInfo* keyInfo, const void* desc);
	virtual HIBaseIf* CreateHumanInterface(const char* name, const char* desc);
	///	Create and register real device.
	virtual bool AddRealDevice(const IfInfo* keyInfo, const void* desc);

	//	SDK�����A�o�^�֐�
	///	HISdk�̃C���X�^���X���쐬
	static HISdkIf* SPR_CDECL CreateSdk();
	///	HISdk���t�@�C�����[�_�[�Ȃǂɓo�^
	static void SPR_CDECL RegisterSdk();
	///	���g�̕\��
	void Print(std::ostream& o) const;
};

}
#endif
