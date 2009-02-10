/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWHapticAppBase.h>
#include <GL/glut.h>
#include <sstream>

#ifdef USE_HDRSTOP
#pragma  hdrstop
#endif

namespace Spr{;

FWHapticAppBase::FWHapticAppBase(){
	bDebug = false;
//	expandedPHSolids = &eSolids;
}

void FWHapticAppBase::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);
	GetSdk()->Clear();

	FWWinDesc windowDesc;
	windowDesc.title = "(c)Springhead2    FWHaptic";
	FWWin* window = CreateWin(windowDesc);
	window->scene = GetSdk()->GetScene();
}

void FWHapticAppBase::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWHapticAppBase::Start(){
	instance = this;
	if (!NWin()){
		CreateWin();
		wins.back()->SetScene(GetSdk()->GetScene());
	}
//	glutTimerFunc(1, FWAppGLUT::GlutTimerFunc, 0);
	glutIdleFunc(FWAppGLUT::GlutIdleFunc);
	glutMainLoop();
}

void FWHapticAppBase::Display(){
	FWWin* window = GetCurrentWin();
	window->SetScene(GetSdk()->GetScene());
	GRCameraIf* cam = window->scene->GetGRScene()->GetCamera();
	GRDebugRenderIf* render = window->render->Cast();

	// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);	
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(false);			// �ڐG��Ԃ͊��荞�ݏ����œ��I�ɕω�����̂ŏ��false

	// �J�������W�̎w��
	if(cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		window->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!window) return;
	GRRenderIf*curRender =  window->GetRender();
	FWSceneIf* curScene = window->GetScene();
	GetSdk()->SwitchRender(curRender);
	GetSdk()->SwitchScene(curScene);
	if(!curRender) return;
	curRender->ClearBuffer();
	curRender->BeginScene();
	if (curScene) curScene->Draw(curRender, GetSdk()->GetDebugMode());	// �V�[����`��
	DebugDisplay(render);																			// �f�o�b�N�\�����[�h
	curRender->EndScene();
 
	glutSwapBuffers(); 
}

void FWHapticAppBase::DebugDisplay(GRDebugRenderIf* render){
	if(!bDebug) return;
	GRLightDesc ld;
	ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
	ld.specular = Vec4f(1,1,1,1) * 0.8f;
	ld.ambient = Vec4f(1,1,1,1) * 0.4f;
	ld.position = Vec4f(1,1,1,0);
	render->PushLight(ld);// �����̒ǉ�
	// �����ɕ`�悵�������̂�����
	render->PopLight();	//	�����̍폜
}

// protected�ϐ��ւ̃A�N�Z�X
void FWHapticAppBase::AddHapticPointer(PHSolidIf* ps){
	hapticPointers.push_back(ps);
}
std::vector<PHSolidIf*> FWHapticAppBase::GetHapticPointers(){
	return hapticPointers;
}
void FWHapticAppBase::AddExpandedPHSolids(ExpandedPHSolid es){
	expandedPHSolids.push_back(es);
}
std::vector<ExpandedPHSolid>* FWHapticAppBase::GetExpandedPHSolids(){
	return &expandedPHSolids;
}

// �t���O��؂�ւ��邽�߂̊֐�
void FWHapticAppBase::SetDebugMode(bool bD){
	bDebug = bD;
	if(bDebug){ 
		DSTR << "DebugMode ON" << std::endl;
	}else{
		DSTR << "DebugMode OFF" << std::endl;
	}
}

bool FWHapticAppBase::GetDebugMode(){
	return bDebug;
}
}