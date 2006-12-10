/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "Framework.h"
#include "Framework/FWSdk.h"
#include "Framework/FWOldSpringheadNode.h"
#include "Physics/PHSdk.h"
#include "Physics/PHScene.h"
#include "Graphics/GRSdk.h"
#include "Graphics/GRScene.h"
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
#include <stdlib.h>
#include <GL/glut.h>

namespace Spr{;

void SPR_CDECL FWRegisterTypeDescs();
void SPR_CDECL FWSdkIf::RegisterSdk(){
	static bool bFirst = true;
	if (!bFirst) return;
	bFirst=false;
	FWRegisterTypeDescs();
	FWRegisterOldSpringheadNode();

	FWSdkIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(FWScene));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(FWObject));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(PHSdk));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(PHScene));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(GRSdk));
	FWSceneIf::GetIfInfoStatic()->RegisterFactory(new FactoryImp(GRScene));
}

FWSdkIf* SPR_CDECL FWSdkIf::CreateSdk(){
	FWSdk* rv = DBG_NEW FWSdk;
	FWSdkIf::RegisterSdk();
	PHSdkIf::RegisterSdk();
	GRSdkIf::RegisterSdk();
	return rv->Cast();
}

//----------------------------------------------------------------------------
//	FWSdk
IF_OBJECT_IMP(FWSdk, Sdk);

FWSdk::FWSdk(){
	name="fwSdk";
	CreateSdks();
}
void FWSdk::CreateSdks(){
	phSdk = PHSdkIf::CreateSdk();
	DCAST(PHSdk, phSdk)->SetNameManager(this);
	phSdk->SetName("phSdk");
	grSdk = GRSdkIf::CreateSdk();
	DCAST(GRSdk, grSdk)->SetNameManager(this);
	grSdk->SetName("grSdk");
	fiSdk = FISdkIf::CreateSdk();
}

FWSdk::~FWSdk(){
}
FWSceneIf* FWSdk::CreateScene(const FWSceneDesc& desc){
	FWSceneIf* rv = DCAST(FWSceneIf, CreateObject(FWSceneIf::GetIfInfoStatic(), &desc));
	AddChildObject(rv); 
	return rv;
}
int FWSdk::NScene() const{
	return (int)scenes.size();
}
FWSceneIf* FWSdk::GetScene(size_t i){
	if (i < scenes.size()) return scenes[i];
	return NULL;
}
bool FWSdk::AddChildObject(ObjectIf* o){
	FWSceneIf* s = DCAST(FWSceneIf, o);
	if (s){
		if (std::find(scenes.begin(), scenes.end(), s) == scenes.end()){
			scenes.push_back(s);
			return true;
		}
	}
	PHSdkIf* ps = DCAST(PHSdkIf, o);
	if (ps) {
		phSdk = ps;
		return true;
	}
	GRSdkIf* gs = DCAST(GRSdkIf, o);
	if (gs) {
		grSdk = gs;
		return true;
	}
	return false;
}
void FWSdk::ClearObjects(){
	// �ă��[�h�O�ȂǁA�S�I�u�W�F�N�g�̃N���A���K�v�Ȏ��ɌĂ�
	// ������
}
void FWSdk::Step(){
	for (unsigned i=0; i<scenes.size(); i++) {
		scenes[i]->Step();
	}
}
void FWSdk::CreateRenderGL(){
	// Window�̐���
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1000, 700);
	int window = glutCreateWindow("Springhead Application");
	// Render�̐���
	grRender = grSdk->CreateDebugRender();
	// Device�̐ݒ�
	grDevice = grSdk->CreateDeviceGL(window);
	grDevice->Init();
	grRender->SetDevice(grDevice);
}
void FWSdk::Draw(){
	// �`��B�`��v���Z�X���̂��̂������ƍו����������\�b�h�ɂ��邩���B
	// {ClearBuffer,BeginScene},{EndScene}�͕������ق����悩�Ǝv��

	// ToDo : isLoadComplete�ł͂Ȃ��A�K�v�ȃC���X�^���X���m�ۂ���Ă��邩��
	// ����Ď��s���邩�ǂ������f����悤�ɂ���B

	grRender->ClearBuffer();
	grRender->BeginScene();

	for (unsigned i=0; i<scenes.size(); i++) {
		scenes[i]->Draw(grRender);
	}

	grRender->EndScene();	
}
void FWSdk::Reshape(int w, int h){
	grRender->Reshape(Vec2f(), Vec2f(w,h));
}
void FWSdk::Keyboard(unsigned char key, int x, int y){
	if (key == 27) {
		exit(0);
	}
}
}
