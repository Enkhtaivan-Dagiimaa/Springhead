/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWSCENE_H
#define FWSCENE_H

#include <Springhead.h>
#include <Framework/SprFWScene.h>
#include <Framework/SprFWObject.h>
#include <Foundation/Object.h>
#include <Foundation/Scene.h>

namespace Spr{;


///	FWScene�̃t�@�N�g��
class FWSceneFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return FWSceneIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, const ObjectIf*){
		return CreateFWScene();
	}
};

///	Graphics��Physics��Scene���܂Ƃ߂��V�[���O���t�S��
class FWScene : public Scene, public FWSceneDesc {
public:
	SPR_OBJECTDEF(FWScene);
	ACCESS_DESC(FWScene);
	typedef std::vector< UTRef<FWObjectIf> > FWObjects;
	FWObjects fwObjects;						///<	�����ƃO���t�B�b�N�X�̃I�u�W�F�N�g�̃����N
	UTRef<PHSceneIf> phScene;					///<	�����V�~�����[�V�����p�̃V�[��
	UTRef<GRSceneIf> grScene;					///<	�O���t�B�b�N�X�p�̃V�[��
	typedef std::vector< UTRef<HIForceDevice6D> > FWHumanInterfaces;
	//	hase TBW
	//	FWHumanInterfaces humanInterfaces;			///<	���[�U�C���^�t�F�[�X�D[0]���J�����DHIForceDevice6D�̃N���X���͕ύX����\��D
	enum HumanInterfacePurposeId{
		HI_CAMERACONTROLLER,
		HI_OBJECTMANIPULATOR0,
		HI_OBJECTMANIPULATOR1,
		HI_OBJECTMANIPULATOR2,
		HI_OBJECTMANIPULATOR3,
	};
	/// �R���X�g���N�^
	FWScene(const FWSceneDesc& d=FWSceneDesc());
	


	virtual PHSceneIf*	GetPHScene(){ return phScene; }
	virtual void		SetPHScene(PHSceneIf* s){ phScene = s; }
	virtual GRSceneIf*	GetGRScene(){ return grScene; }
	virtual void		SetGRScene(GRSceneIf* s){ grScene = s; }
	virtual FWObjectIf* CreateObject(const PHSolidDesc& soliddesc = PHSolidDesc(), const GRFrameDesc& framedesc = GRFrameDesc());
	virtual int NObject()const{return (int)fwObjects.size();}
	virtual FWObjectIf** GetObjects(){return (FWObjectIf**)&*fwObjects.begin();}
	virtual void Sync();
	virtual void Step();
	virtual void Draw(GRRenderIf* grRender, bool debug=false);
	virtual HIForceDevice6D* GetHumanInterface(size_t pos);
	//
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual NamedObjectIf* FindObject(UTString name, UTString cls);
	virtual void AddHumanInterface(HIForceDevice6D* d);
};


}

#endif
