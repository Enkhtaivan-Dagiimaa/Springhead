/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FWScene.h"
#include "FWObject.h"
#include <Graphics/GRDebugRender.h>
#include <Graphics/GRScene.h>
#include <Physics/PHScene.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

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
	if (phScene){
		rv = DCAST(PHScene, phScene)->FindObjectFromDescendant(name, cls);
		if (rv) return rv;
	}
	if (grScene){
		rv = DCAST(GRScene, grScene)->FindObjectFromDescendant(name, cls);
		if (rv) return rv;
	}

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

FWObjectIf* FWScene::CreateFWObject(){
	FWObjectDesc desc;
	FWObjectIf* obj = DCAST(FWObjectIf, CreateObject(FWObjectIf::GetIfInfoStatic(), &desc));
	obj->SetPHSolid(NULL);
	obj->SetGRFrame(NULL);
	AddChildObject(obj);
	return obj;
}

bool FWScene::AddChildObject(ObjectIf* o){
	FWSdkIf* sdk = DCAST(FWSdkIf, GetNameManager());
	bool ok = false;
	if (!ok) {
		FWObject* obj = DCAST(FWObject, o);
		if (obj) {
			fwObjects.push_back(obj->Cast());
			obj->SetScene(Cast());
			// �f�t�H���g�l�[��
			if(strcmp(obj->GetName(), "") == 0){
				char name[256];
				sprintf(name, "object%d", NObject()-1);
				obj->SetName(name);
			}
			ok = true;
		}
	}
	if (!ok) {
		PHScene* obj = DCAST(PHScene, o);
		if (obj) {
			phScene = obj->Cast();
			sdk->GetPHSdk()->AddChildObject(obj->Cast());
			ok = true;
		}
	}
	if (!ok) {
		GRScene* obj = DCAST(GRScene, o);
		if (obj) {
			grScene = obj->Cast();
			sdk->GetGRSdk()->AddChildObject(obj->Cast());
			ok = true;
		}
	}
	if (!ok && phScene) {
		ok = phScene->AddChildObject(o);
	}
	if (!ok && grScene) {
		ok = grScene->AddChildObject(o);
	}
	return ok;
}

bool FWScene::DelChildObject(ObjectIf* o){
	FWObjectIf* obj = DCAST(FWObjectIf, o);
	if(obj){
		FWObjects::iterator it = find(fwObjects.begin(), fwObjects.end(), obj);
		if(it != fwObjects.end()){
			fwObjects.erase(it);
			return true;
		}
	}
	return false;
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
	if (phScene){
		if (pos == 0) return phScene;
		pos--;
	}
	if (grScene){
		if (pos == 0) return grScene;
		pos--;
	}	
	if (pos < fwObjects.size()) return fwObjects[pos];
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
	if (grScene){
		HIForceDevice6D* device = GetHumanInterface(HI_CAMERACONTROLLER);
		GRCameraIf* camera = grScene->GetCamera();
		if(camera && device){
			Posed pose;
	/*		hase:	TBW	
			pose.Pos() = device->GetPos();	
			pose.Ori() = device->GetOri();
	*/		if (!camera->GetFrame()){
				GRSceneIf* scene = DCAST(GRSceneIf, camera->GetNameManager());
				if (scene) camera->SetFrame(DCAST(GRFrameIf, 
					scene->CreateVisual(GRFrameIf::GetIfInfoStatic(), GRFrameDesc())));
			}
			if (camera->GetFrame()){
				Affinef af;
				pose.ToAffine(af);
				camera->GetFrame()->SetTransform(af);
			}
		}
	}
}
void FWScene::Step(){
	if (phScene) phScene->Step();
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
		if (grScene) grScene->Render(grRender);
	}
}

void FWScene::AddHumanInterface(HIForceDevice6D* d){
//	hase	TBW
	//	humanInterfaces.push_back(d);
}

}
