#include "FWSprFileLoader.h"
#include <iostream>
#include <sstream>


#define ESC 27

FWSprfileLoader::FWSprfileLoader(){
	fileName = "./SprFiles/sceneSample.spr";
	bDebug		= false;
}

void FWSprfileLoader::Init(int argc, char* argv[]){
	/// �V�~�����[�V�����̏�����
	CreateSdk();
	GetSdk()->Clear();						// SDK�̏�����
	GetSdk()->LoadScene(fileName);			// �t�@�C���̃��[�h

	///	�t�@�C���̃Z�[�u�e�X�g
	GetSdk()->SaveScene("save.spr");

	/// �`�惂�[�h�̐ݒ�
	SetGRAdaptee(TypeGLUT);					// GLUT�ŕ`��
	GRInit(argc, argv);		// ������

	/// �E�B���h�E�̍쐬
	FWWinDesc windowDesc;					// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWSprFileLoader";	// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);					// �E�B���h�E�̍쐬
	GetCurrentWin()->SetScene(GetSdk()->GetScene());

	/// �J�����r���[�̏�����
	InitCameraView();	
	CreateTimer();
}

void FWSprfileLoader::InitCameraView(){
	///�@�J�����r���[�̏�����
	std::istringstream issView(
		"((0.999816 -0.0126615 0.0144361 -0.499499)"
		"(6.50256e-010 0.751806 0.659384 13.2441)"
		"(-0.019202 -0.659263 0.751667 10.0918)"
		"(     0      0      0      1))"
		);
	issView >> cameraInfo.view;
}

void FWSprfileLoader::Reset(){
	GetSdk()->Clear();		
	GetSdk()->LoadScene("./xFiles/sceneSample.x");
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
}

void FWSprfileLoader::TimerFunc(int id){
	Step();
}

void FWSprfileLoader::IdleFunc(){
}
void FWSprfileLoader::Step(){
	GetSdk()->Step();
	PostRedisplay();
}

void FWSprfileLoader::Display(){
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(bDebug);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(bDebug);

	/// �J�������W�̎w��
	FWWin* win = GetCurrentWin();
	if (win->scene && win->scene->GetGRScene()){
		GRCameraIf* cam = win->scene->GetGRScene()->GetCamera();
		if (cam && cam->GetFrame()){
			cam->GetFrame()->SetTransform(cameraInfo.view);
		}else{
			GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
		}
	}

	/// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}


void FWSprfileLoader::Keyboard(int key, int x, int y){
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