#include "FWScene.h"
#include "FWObject.h"
#include <Graphics/GRDebugRender.h>
#include <Graphics/GRScene.h>
#include <Physics/PHScene.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

IF_OBJECT_IMP(FWScene, Scene);

FWScene::FWScene(const FWSceneDesc& d/*=FWSceneDesc()*/)
: phScene(NULL), grScene(NULL)
{
}
NamedObjectIf* FWScene::FindObject(UTString name, UTString cls){
	//	�]����phScene,grScene��2�̃V�[������������̂ŁCNameManager::FindObject�Ƃ�����ƈႤ�D
	//	2�̃V�[���́CSdk�ɏ��L����Ă���̂ŁC���ʂ�NameManager�Ƃ��āCFWScene���w�肷�邱�Ƃ͂ł��Ȃ��D

	//	�܂������Ǝq��������
	NamedObjectIf* rv = FindObjectFromDescendant(name, cls);
	if (rv) return rv;
	//	��c������
	rv = FindObjectFromAncestor(name, cls);
	if (rv) return rv;

	//	�Ȃ���΁CphScene��grScene�ɂ��Ď����Ǝq����T���B
	rv = DCAST(PHScene, phScene)->FindObjectFromDescendant(name, cls);
	if (rv) return rv;
	rv = DCAST(GRScene, grScene)->FindObjectFromDescendant(name, cls);
	if (rv) return rv;

	//	����ł��Ȃ��Ȃ�΁Anamespace������āA������x����
	size_t pos = name.find('/');
	if (pos != UTString::npos){	//	 ���O��Ԃ̎w�肪����ꍇ
		UTString n = name.substr(pos+1);
		rv = FindObject(n, cls);
	}
	return rv;
}

ObjectIf* FWScene::CreateObject(const IfInfo* info, const void* desc){
	ObjectIf* rv = Scene::CreateObject(info, desc);
	if (!rv && phScene) rv = phScene->CreateObject(info, desc);
	if (!rv && grScene) rv = grScene->CreateObject(info, desc);
	return rv;
}
bool FWScene::AddChildObject(ObjectIf* o){
	bool rv = false;
	if (!rv) {
		FWObjectIf* obj = DCAST(FWObjectIf, o);
		if (obj) {
			fwObjects.push_back(obj);
			rv = true;
		}
	}
	if (!rv) {
		PHScene* obj = DCAST(PHScene, o);
		if (obj) {
			phScene = obj->Cast();
			rv = true;
		}
	}
	if (!rv) {
		GRScene* obj = DCAST(GRScene, o);
		if (obj) {
			grScene = obj->Cast();
			rv = true;
		}
	}
	if (!rv && phScene) {
		rv = phScene->AddChildObject(o);
	}
	if (!rv && grScene) {
		rv = grScene->AddChildObject(o);
	}
	return rv;
}

HIForceDevice6D* FWScene::GetHumanInterface(size_t pos){
/*	hase:	TBW
	if (pos < humanInterfaces.size()) return humanInterfaces[pos];
*/	return NULL;
}

size_t FWScene::NChildObject() const{
	return fwObjects.size() + (grScene?1:0) + (phScene?1:0);
}

ObjectIf* FWScene::GetChildObject(size_t pos){
	if (pos < fwObjects.size()) return fwObjects[pos];
	if (pos - fwObjects.size() == 0) return phScene ? phScene : grScene;
	if (pos - fwObjects.size() == 1) return phScene ? grScene : NULL;
	return NULL;
}

FWSceneIf* SPR_CDECL CreateFWScene(){
	FWScene* rv = DBG_NEW FWScene;
	return rv->Cast();
}

FWSceneIf* SPR_CDECL CreateFWScene(const void* desc){
	FWScene* rv = DBG_NEW FWScene(*(FWSceneDesc*)desc);
	return rv->Cast();
}
void FWScene::Sync(){
	//	�I�u�W�F�N�g�ʒu�E�p���̓���
	for(FWObjects::iterator it = fwObjects.begin(); it!=fwObjects.end(); ++it){
		DCAST(FWObject, *it)->Sync();
	}
	
	//	�J�����̓���
	HIForceDevice6D* device = GetHumanInterface(HI_CAMERACONTROLLER);
	GRCameraIf* camera = grScene->GetCamera();
	if (grScene && camera && device){
		Posed pose;
/*		hase:	TBW	
		pose.Pos() = device->GetPos();	
		pose.Ori() = device->GetOri();
*/		if (!camera->GetFrame()){
			GRSceneIf* scene = DCAST(GRSceneIf, camera->GetNameManager());
			if (scene) camera->SetFrame(scene->CreateFrame(GRFrameDesc()));
		}
		if (camera->GetFrame()){
			Affinef af;
			pose.ToAffine(af);
			camera->GetFrame()->SetTransform(af);
		}
	}
}
void FWScene::Step(){
	phScene->Step();
}
void FWScene::Draw(GRRenderIf* grRender, bool debug/*=false*/){
	Sync();
	if (debug){
		GRDebugRenderIf* render = DCAST(GRDebugRenderIf, grRender);
		if (render){
			GRCameraIf* cam = NULL;
			if (grScene) cam = grScene->GetCamera();
			if (cam) cam->Render(render);
			
			GRLightDesc ld;
			ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
			ld.specular = Vec4f(1,1,1,1) * 0.8f;
			ld.ambient = Vec4f(1,1,1,1) * 0.4f;
			ld.position = Vec4f(1,1,1,0);

			render->PushLight(ld);
			render->DrawScene(phScene);
			render->PopLight();
			if (cam) cam->Rendered(render);
		}
	}else{
		grScene->Render(grRender);
	}
}

void FWScene::AddHumanInterface(HIForceDevice6D* d){
//	hase	TBW
	//	humanInterfaces.push_back(d);
}

}
