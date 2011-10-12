/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Framework/SprFWApp.h>
#include <Framework/FWGLUT.h>
#include <Framework/FWGLUI.h>
#include <Framework/FWInteractScene.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;

FWApp* FWApp::instance = 0;
UTRef<FWGraphicsAdaptee> FWGraphicsAdaptee::instance = 0;

FWApp::FWApp(){
	instance = this;
}

FWApp::~FWApp(){
	bool hasFullScreen = false;
	for(int i = 0; i < (int)wins.size(); i++){
		if(wins[i]->GetFullScreen()){
			hasFullScreen = true;
			break;
		}
	}
	if(hasFullScreen)
		FWGraphicsAdaptee::instance->LeaveGameMode();
}

void FWApp::Init(int argc, char* argv[]){
	// �ł���{�I�ȏ���������
	// SDK������
	CreateSdk();
	// �V�[�����쐬
	GetSdk()->CreateScene();
	// �E�B���h�E�}�l�W��������
	GRInit(argc, argv);
	// �E�B���h�E���쐬
	CreateWin();
	// �^�C�}���쐬
	CreateTimer();
}

void FWApp::Display(){
	GetCurrentWin()->Display();
}

void FWApp::TimerFunc(int id){
	UserFunc();
	//GetSdk()->Step();
	GetCurrentWin()->GetScene()->Step();
	PostRedisplay();
}

void FWApp::EnableIdleFunc(bool on){
	FWGraphicsAdaptee::instance->EnableIdleFunc(on);
}
void FWApp::StartMainLoop(){
	FWGraphicsAdaptee::instance->StartMainLoop();
}


// �h���N���X�Œ�`���邱�Ƃ̂ł��鉼�z�֐�/////////////////////////////////
void FWApp::Reshape(int w, int h){
}

void FWApp::MouseButton(int button, int state, int x, int y){
	// ctrl+left �J�[�\���ō��̂𓮂���
	/*if(fwSdk->GetScene() && (mouseInfo.left && mouseInfo.ctrl)){
		dragInfo.Init(fwSdk->GetScene()->GetPHScene(), cameraInfo.view, fwSdk->GetRender());
		dragInfo.Grab(x, y);
	}

	if(state == BUTTON_UP && button == LEFT_BUTTON)
		dragInfo.Release();*/
}

void FWApp::MouseMove(int x, int y){
	// ���_�ړ�(��])
	/*if(mouseInfo.left && !mouseInfo.ctrl && !mouseInfo.alt){
		cameraInfo.Rotate(mouseInfo.pos.x - mouseInfo.lastPos.x, mouseInfo.pos.y - mouseInfo.lastPos.y,
			GetSdk()->GetRender()->GetPixelSize());
	}

	// ���_�ړ�(���s�ړ�)
	if(mouseInfo.middle || mouseInfo.left && mouseInfo.alt){
		cameraInfo.Translate(mouseInfo.pos.x - mouseInfo.lastPos.x, mouseInfo.pos.y - mouseInfo.lastPos.y,
			GetSdk()->GetRender()->GetPixelSize());
	}
	
	// �Y�[��
	if(mouseInfo.right){
		cameraInfo.Zoom(mouseInfo.pos.y - mouseInfo.lastPos.y);
	}

	// ���̃h���b�O
	if(mouseInfo.left && mouseInfo.ctrl)
		dragInfo.Drag(x, y);
	*/
}

//�@FWApp�̃C���^�t�F�[�X ///////////////////////////////////////////////////////

void FWApp::CreateSdk(){
	if(fwSdk)
		return;
	fwSdk = FWSdkIf::CreateSdk();
}

void FWApp::AssignScene(FWWinIf* win){
	if (win->GetScene()) return;
	
	// �����̂ǂ̃E�B���h�E�ɂ����蓖�Ă��Ă��Ȃ��V�[����T��
	for(int i = GetSdk()->NScene() - 1; i >= 0; --i){
		FWSceneIf* scene = GetSdk()->GetScene(i);
		Wins::iterator it;
		for(it = wins.begin(); it != wins.end(); ++it){
			if ((*it)->GetScene() == scene) break;
		}
		if (it == wins.end()){
			win->SetScene(scene);
			return;
		}
	}
}

FWWinIf* FWApp::CreateWin(const FWWinDesc& desc, FWWinIf* parent){
	FWWinIf* win = FWGraphicsAdaptee::instance->CreateWin(desc, parent);

	// ���g���L�[�{�[�h�E�}�E�X�n���h���ɓo�^
	win->GetKeyMouse()->AddHandler(this);

	HISdkIf* hiSdk = GetSdk()->GetHISdk();
	// �g���b�N�{�[���ƃh���b�K�[���쐬
	if(desc.useKeyMouse && desc.useTrackball)
		win->SetTrackball(hiSdk->CreateHumanInterface(HITrackballIf::GetIfInfoStatic())->Cast());
	if(desc.useKeyMouse && desc.useDragger)
		win->SetDragger(hiSdk->CreateHumanInterface(HIDraggerIf::GetIfInfoStatic())->Cast());

	wins.push_back(win);

	// �V�[���̊�����
	AssignScene(win);
	// �����_��������
	GRRenderIf* render = GetSdk()->GetGRSdk()->CreateRender();
	render->SetDevice(FWGraphicsAdaptee::instance->GetGRDevice());
	// ���̃R���e�L�X�g�ɑ΂���GRDeviceGL::Init���Ă�
	render->GetDevice()->Init();
	win->SetRender(render);

	return win;
}

FWWinIf* FWApp::GetWinFromId(int wid){
	for(Wins::iterator i = wins.begin(); i != wins.end(); i++){
		if((*i)->GetID() == wid)
			return *i;
	}
	return NULL;
}

FWWinIf* FWApp::GetWin(int pos){
	if(0 <= pos && pos < NWin())
		return wins[pos];
	return NULL;
}

FWWinIf* FWApp::GetCurrentWin(){
	return GetWinFromId(FWGraphicsAdaptee::instance->GetCurrentWin());
}

void FWApp::DestroyWin(FWWinIf* win){
	FWGraphicsAdaptee::instance->DestroyWin(win);
}

void FWApp::SetCurrentWin(FWWinIf* win){
	FWGraphicsAdaptee::instance->SetCurrentWin(win);
}

void FWApp::PostRedisplay(){
	FWGraphicsAdaptee::instance->PostRedisplay();
}

int FWApp::GetModifier(){
	return FWGraphicsAdaptee::instance->GetModifiers();
}

// �`��p�[�g////////////////////////////////////////////////////////////////////

void FWApp::SetGRAdaptee(int type){
	switch (type) {
	case TypeNone:
		FWGraphicsAdaptee::instance = 0;
		break;
	case TypeGLUT:
		FWGraphicsAdaptee::instance = DBG_NEW FWGLUT();
		break;
	case TypeGLUI:
		FWGraphicsAdaptee::instance = DBG_NEW FWGLUI();
		break;
	}
}

void FWApp::GRInit(int argc, char* argv[], int type){
	if(!FWGraphicsAdaptee::instance)
		SetGRAdaptee(type);
	FWGraphicsAdaptee::instance->Init(argc, argv);
}

/*void FWApp::CallKeyboard(int key, int x, int y){
	for(int i = 0; i < GetSdk()->NIAScenes(); i++){
		FWInteractScene* iaScene = GetSdk()->GetIAScene(i)->Cast();
		iaScene->BeginKeyboard();
	}

	Keyboard(key, x, y);

	for(int i = 0; i < GetSdk()->NIAScenes(); i++){
		FWInteractScene* iaScene = GetSdk()->GetIAScene(i)->Cast();
		iaScene->EndKeyboard();
	}
}*/

//�^�C�}///////////////////////////////////////////////////////////////////////////

/// UTTimer�ɓo�^����R�[���o�b�N�֐�
void SPR_CDECL FWApp_TimerCallback(int id, void* arg){
	FWApp* app = (FWApp*)arg;
	if(!app)
		return;
	app->TimerFunc(id);
}

UTTimerIf*  FWApp::CreateTimer(UTTimerIf::Mode mode){
	/// �C���X�^���X�̓R���X�g���N�^�̒���UTTimerStub�Ɋi�[�����
	UTTimerIf* timer = UTTimerIf::Create();
	timer->SetMode(mode);
	timer->SetCallback(FWApp_TimerCallback, this);
	return timer;
}

}
