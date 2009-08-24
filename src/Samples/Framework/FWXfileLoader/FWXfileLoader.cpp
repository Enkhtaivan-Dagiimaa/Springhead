#include "FWXfileLoader.h"
#include <iostream>
#include <sstream>


#define ESC 27

FWXfileLoader::FWXfileLoader(){
	fileName = "./xFiles/sceneSample.x";
	bDebug		= false;
}

void FWXfileLoader::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUT);
	GetGRAdaptee()->Init(argc, argv);				// SDK�̍쐬
	CreateSdk();
	GetSdk()->Clear();								// SDK�̏�����
	GetSdk()->LoadScene(fileName);

	FWWinDesc windowDesc;							// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWXfileLoader";				// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);							// �E�B���h�E�̍쐬
	InitWindow();
	InitCameraView();								// �J�����r���[�̏�����
}

void FWXfileLoader::Reset(){
	GetSdk()->Clear();								// SDK�̏�����
	GetSdk()->LoadScene("./xFiles/sceneSample.x");
}

void FWXfileLoader::Timer(){
//	GetGRAdaptee()->Timer();
}
void FWXfileLoader::IdleFunc(){
	Step();
}
void FWXfileLoader::Step(){
	GetSdk()->Step();
	glutPostRedisplay();
}

void FWXfileLoader::Display(){
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(bDebug);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(bDebug);

	/// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	/// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}

void FWXfileLoader::InitCameraView(){
	///	Affinef �^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.999816 -0.0126615 0.0144361 -0.499499)"
		"(6.50256e-010 0.751806 0.659384 13.2441)"
		"(-0.019202 -0.659263 0.751667 10.0918)"
		"(     0      0      0      1))"
		);
	issView >> cameraInfo.view;
}

void FWXfileLoader::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
//		case 'r':
//			Reset();			// �t�@�C���̍ēǂݍ���(�^�C�}���~�߂ĂȂ��Ɨ�����̂Ŗ�����)
//			break;
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