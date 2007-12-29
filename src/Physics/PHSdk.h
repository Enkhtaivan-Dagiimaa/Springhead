/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PHSDK_H
#define PHSDK_H
#include <Foundation/Foundation.h>
#include <Physics/SprPHSdk.h>
#include "IfStubPhysics.h"

namespace Spr {;

class PHSdkFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return PHSdkIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, ObjectIf* parent){
		return PHSdkIf::CreateSdk();
	}
};

class SPR_DLL PHSdk:public Sdk, public PHSdkIfInit{
protected:
	//	scene
	typedef std::vector< UTRef<PHSceneIf> > Scenes;
	///	�V�[��
	Scenes scenes;
	//	shape
	typedef std::vector< UTRef<CDShapeIf> > Shapes;
	///	�`��
	Shapes shapes;
	///	���̃I�u�W�F�N�g����
	ObjectRefs objects;
public:

	OBJECTDEF(PHSdk, Sdk);
	PHSdk(const PHSdkDesc& = PHSdkDesc());
	~PHSdk();

	virtual void			Clear();
	virtual PHSceneIf*		CreateScene();
	virtual PHSceneIf*		CreateScene(const PHSceneDesc& desc);
	virtual int				NScene();
	virtual PHSceneIf*		GetScene(size_t i);
	virtual void			MergeScene(PHSceneIf* scene0, PHSceneIf* scene1);
	virtual CDShapeIf*		CreateShape(const IfInfo* ii, const CDShapeDesc& desc);
	virtual int				NShape(){ return (int)shapes.size(); }
	virtual CDShapeIf*		GetShape(int i){ return (0 <= i && i < (int)shapes.size()) ? shapes[i] : NULL; }
	
	ObjectIf*				CreateObject(const IfInfo* info, const void* desc);
	virtual size_t			NChildObject() const { return shapes.size() + scenes.size(); }
	virtual ObjectIf*		GetChildObject(size_t i);
	virtual bool			AddChildObject(ObjectIf* o);
	virtual bool			DelChildObject(ObjectIf* o);

public:
};

}
#endif
