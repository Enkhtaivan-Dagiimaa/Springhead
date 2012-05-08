#include "FWVrmlFileLoader.h"
#include <iostream>
#include <sstream>


#define ESC 27

FWVrmlfileLoader::FWVrmlfileLoader(){
	fileName = "./VrmlFiles/sceneSample.wrl";
	bDebug		= false;
}

void FWVrmlfileLoader::Init(int argc, char* argv[]){
	/// �V�~�����[�V�����̏�����
	CreateSdk();
	GetSdk()->Clear();						// SDK�̏�����
	GetSdk()->LoadScene(fileName);			// �t�@�C���̃��[�h

	/// �`�惂�[�h�̐ݒ�
	SetGRHandler(TypeGLUT);					// GLUT�ŕ`��
	GRInit(argc, argv);		// ������

	/// �E�B���h�E�̍쐬
	FWWinDesc windowDesc;					// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWVrmlFileLoader";	// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);					// �E�B���h�E�̍쐬
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
	
	CreateTimer();
}

void FWVrmlfileLoader::Reset(){
	GetSdk()->Clear();		
	GetSdk()->LoadScene("./xFiles/sceneSample.x");
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
}


void FWVrmlfileLoader::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();			// �t�@�C���̍ēǂݍ���
			break;
		case 'w':				// �J����������	
			break;
		case 'd':				// �f�o�b�N�\��
			bDebug = !bDebug;
			break;
		default:
			break;
	}
}