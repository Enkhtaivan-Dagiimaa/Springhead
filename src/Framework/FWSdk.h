/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWSDK_H
#define FWSDK_H
#include <Foundation/Foundation.h>
#include <Framework/SprFWSdk.h>

namespace Spr {;

class FWSdkFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return FWSdkIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, const ObjectIf*){
		return FWSdkIf::CreateSdk();
	}
};

class SPR_DLL FWSdk:public InheritObject<FWSdkIf, Sdk>{
protected:
	//	�^���DB
	static UTRef<UTTypeDescDb> typeDb, oldSpringheadTypeDb;
	//	scene
	typedef std::vector< UTRef<FWSceneIf> > Scenes;
	///	�V�[��
	Scenes scenes;
public:
	OBJECT_DEF(FWSdk);
	FWSdk();
	~FWSdk();
	virtual FWSceneIf* CreateScene(const FWSceneDesc& desc);
	virtual int NScene() const;
	virtual FWSceneIf* GetScene(size_t i);

	virtual size_t NChildObject() const { return NScene(); }
	virtual ObjectIf* GetChildObject(size_t i){ return GetScene(i); }
	virtual bool AddChildObject(ObjectIf* o);
public:
};

}
#endif
