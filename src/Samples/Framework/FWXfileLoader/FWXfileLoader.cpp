#include "FWXfileLoader.h"
#include <iostream>
#include <sstream>


#define ESC 27

FWXfileLoader::FWXfileLoader(){
	fileName = "./xFiles/sceneSample.x";
	bDebug		= false;
}

void FWXfileLoader::Init(int argc, char* argv[]){
	/// �V�~�����[�V�����̏�����
	CreateSdk();
	GetSdk()->Clear();						// SDK�̏�����
	GetSdk()->LoadScene(fileName);			// �t�@�C���̃��[�h

	/// �`�惂�[�h�̐ݒ�
	SetGRAdaptee(TypeGLUT);					// GLUT�ŕ`��
	GRInit(argc, argv);		// ������

	/// �E�B���h�E�̍쐬
	FWWinDesc windowDesc;					// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWXfileLoader";		// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);					// �E�B���h�E�̍쐬
	GetCurrentWin()->SetScene(GetSdk()->GetScene());

	/// �J�����r���[�̏�����
	InitCameraView();	

	CreateTimer();
}

void FWXfileLoader::InitCameraView(){
	///�@�J�����r���[�̏�����
	std::istringstream issView(
		"((0.999816 -0.0126615 0.0144361 -0.499499)"
		"(6.50256e-010 0.751806 0.659384 13.2441)"
		"(-0.019202 -0.659263 0.751667 10.0918)"
		"(     0      0      0      1))"
		);
	issView >> cameraInfo.view;
}

void FWXfileLoader::Reset(){
	GetSdk()->Clear();		
	GetSdk()->LoadScene("./xFiles/sceneSample.x");
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
}

void FWXfileLoader::TimerFunc(int id){
	Step();
}

void FWXfileLoader::IdleFunc(){
}
void FWXfileLoader::Step(){
	GetSdk()->Step();
	PostRedisplay();
}

void FWXfileLoader::Display(){
	FWWin* win = GetCurrentWin();
	if(!win)
		return;

	GRRenderIf* render = win->GetRender();
	FWSceneIf*  scene  = win->GetScene();
	
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(bDebug);
	scene->SetRenderMode(true, false);
	scene->EnableRenderForce(bDebug, bDebug);
	scene->EnableRenderContact(bDebug);
	
	/// �J�������W�̎w��
	if (win->scene){
		GRCameraIf* cam = win->scene->GetGRScene()->GetCamera();
		if (cam && cam->GetFrame()){
			cam->GetFrame()->SetTransform(cameraInfo.view);
		}else{
			render->SetViewMatrix(cameraInfo.view.inv());
		}
	}

	/// �`��̎��s
	GetSdk()->SwitchScene(scene);
	GetSdk()->SwitchRender(render);
	GetSdk()->Draw();
	render->SwapBuffers();
}


void FWXfileLoader::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();			// �t�@�C���̍ēǂݍ���
			break;
		case 'w':				// �J����������
			InitCameraView();	
			break;
		case 'd':				// �f�o�b�N�\��
			bDebug = !bDebug;
			break;
		default:
			break;
	}
}