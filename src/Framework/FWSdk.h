/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWSDK_H
#define FWSDK_H

#include <Framework/SprFWSdk.h>
#include <Foundation/Scene.h>

namespace Spr {;

struct FIFileIf;

class FWSdkFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return FWSdkIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, const ObjectIf*){
		return FWSdkIf::CreateSdk();
	}
};

class SPR_DLL FWSdk:public Sdk{
protected:
	// SDKs
	UTRef<PHSdkIf>	phSdk;
	UTRef<GRSdkIf>	grSdk;
	UTRef<FISdkIf>	fiSdk;
	UTRef<HISdkIf>	hiSdk;

	///	�V�[��
	typedef std::vector< UTRef<FWSceneIf> > FWScenes;
	FWScenes scenes;
	/// �A�N�e�B�u�V�[��
	//FWSceneIf*		curScene;

	/// �C���^���N�V�����p�V�[��
	FWInteractSceneIf*					curIAScene;
	FWInteractScenes					iaScenes;
	
	/// �����_��
	typedef std::vector< UTRef<GRRenderIf> > Renders;
	Renders renders;
	//GRRenderIf*		curRender;
	
	//bool debugMode;
	//bool DSTRFlag;

protected:
	FIFileIf* CreateFile(UTString ext, const IfInfo* ii);
	void CreateSdks();

public:
	SPR_OBJECTDEF(FWSdk);
	FWSdk();
	~FWSdk();

	/// SDK�֌W
	PHSdkIf* GetPHSdk(){ return phSdk; }
	GRSdkIf* GetGRSdk(){ return grSdk; }
	FISdkIf* GetFISdk(){ return fiSdk; }
	HISdkIf* GetHISdk(){ return hiSdk; }

	/// �V�[���֌W
	FWSceneIf* CreateScene(const PHSceneDesc& phdesc = PHSceneDesc(), const GRSceneDesc& grdesc = GRSceneDesc());
	bool LoadScene(UTString filename, ImportIf* ex = NULL, const IfInfo* ii = NULL, ObjectIfs* objs = NULL);
	bool SaveScene(UTString filename, ImportIf* ex = NULL, const IfInfo* ii = NULL, ObjectIfs* objs = NULL);
	int NScene() const{	return (int)scenes.size(); }
	//void SwitchScene(FWSceneIf* scene){ curScene = scene; }
	FWSceneIf* GetScene(int i = -1);
	void MergeScene(FWSceneIf* scene0, FWSceneIf* scene1);
	
	/// �C���^���N�V�����V�[���֌W
	FWInteractSceneIf*				CreateIAScene(const FWInteractSceneDesc& desc);
	FWInteractSceneIf*				GetIAScene(int i = -1);
	int								NIAScenes(){ return iaScenes.size(); }
	void							ClearIAScenes();
	
	/// �����_���֌W
	//GRRenderIf*	CreateRender();
	//int NRender() const{return (int)renders.size();}
	//void SwitchRender(GRRenderIf* render){ curRender = render; }
	
	bool GetDebugMode();
	void SetDebugMode(bool debug);
	void Step();
	void Draw();
	void Reshape(int w, int h);
	GRRenderIf* GetRender();
	//void SetDSTR(bool f){ DSTRFlag = f; }
	
	// Object�̉��z�֐��̎���
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t i);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
	virtual void Clear();
	
};

}
#endif
