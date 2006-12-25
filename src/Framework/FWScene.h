#ifndef FWSCENE_H
#define FWSCENE_H

#include <Framework/SprFWScene.h>
#include <Framework/SprFWObject.h>
#include <Foundation/Object.h>
#include <Foundation/Scene.h>
#include "IfStubFramework.h"

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
class FWScene : public Scene, public FWSceneIfInit, public FWSceneDesc {
public:
	OBJECTDEF(FWScene, Scene);
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
	//
	FWScene(const FWSceneDesc& d=FWSceneDesc()); // �R���X�g���N�^
	//
	PHSceneIf* GetPHScene(){ return phScene; }
	GRSceneIf* GetGRScene(){ return grScene; }
	///	�I�u�W�F�N�g�̈ʒu�E�p���𓯊�
	void Sync();
	void Step();
	void Draw(GRRenderIf* grRender, bool debug=false);
	HIForceDevice6D* GetHumanInterface(size_t pos);
	//
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual bool AddChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual NamedObjectIf* FindObject(UTString name, UTString cls);
	virtual void AddHumanInterface(HIForceDevice6D* d);
};


}

#endif
