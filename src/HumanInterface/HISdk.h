/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef HISDK_H
#define HISDK_H

#include <Foundation/Scene.h>
#include <HumanInterface/SprHISdk.h>
#include <HumanInterface/HIBase.h>

#include <vector>

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

typedef	std::vector< UTRef<HIRealDeviceIf> >	DRPool;
typedef std::vector< UTRef<HIBaseIf> >			HIPool;

class SPR_DLL HISdk: public Sdk{
protected:
	//HIVirtualDevicePool				dvPool;		///< ���z�f�o�C�X�v�[��
	//HIRealDevicePool					drPool;		///< ���f�o�C�X�v�[��
	DRPool					drPool;
	HIPool					hiPool;		///< �q���[�}���C���^�t�F�[�X�̃v�[��

public:
	SPR_OBJECTDEF(HISdk);
	HISdk(const HISdkDesc& = HISdkDesc());

	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual void Clear();

	/// �C���^�t�F�[�X�̎���
	//void Init();	
	HIVirtualDeviceIf* RentVirtualDevice(const IfInfo* ii, const char* name=NULL, int portNo = -1);
	//HIVirtualDeviceIf* RentVirtualDeviceNo(const char* type,int No=0 ,const char* name=NULL);
	bool ReturnVirtualDevice(HIVirtualDeviceIf* dev);
	
	void			RegisterRealDevice(HIRealDeviceIf* dev);	
	HIRealDeviceIf* FindRealDevice(const char* name);
	HIRealDeviceIf*	FindRealDevice(const IfInfo* ii);
	void			RegisterVirtualDevice(HIVirtualDeviceIf* dev);
	HIBaseIf*		CreateHumanInterface(const IfInfo* keyInfo);
	HIBaseIf*		CreateHumanInterface(const char* name);
	HIRealDeviceIf* AddRealDevice(const IfInfo* keyInfo, const void* desc=NULL);

	static HISdkIf* SPR_CDECL CreateSdk();
	//static void SPR_CDECL RegisterSdk();
	//void Print(std::ostream& o) const;
};

}
#endif
