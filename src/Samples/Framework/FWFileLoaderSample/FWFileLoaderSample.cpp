/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include "FWFileLoaderSample.h"

#define USE_SPRFILE
#define ESC 27

FWFileLoaderSample::FWFileLoaderSample(){
#ifdef USE_SPRFILE
	fileName = "./files/sceneSample.spr";	// spr�t�@�C��
#else
	fileName = "./files/sceneSample.x";		// x�t�@�C��
#endif
}

void FWFileLoaderSample::Init(int argc, char* argv[]){
	CreateSdk();			// SDK�̍쐬
	UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();
	GetSdk()->LoadScene(fileName, import);			// �t�@�C���̃��[�h
	GetSdk()->SaveScene("save.spr", import);		// �t�@�C���̃Z�[�u�e�X�g
	GRInit(argc, argv);		// �E�B���h�E�}�l�W��������
	CreateWin();			// �E�B���h�E���쐬
	CreateTimer();			// �^�C�}���쐬

	InitCameraView();		// �J�����r���[�̏�����
	GetSdk()->SetDebugMode(false);						// �f�o�b�N���[�h�̖�����
	GetSdk()->GetScene()->EnableRenderAxis(true);		// ���W���̕\��
	GetSdk()->GetScene()->EnableRenderContact(true);	// �ڐG�̈�̕\��
}

void FWFileLoaderSample::InitCameraView(){
	Vec3d pos = Vec3d(-0.978414, 11.5185, 24.4473);		// �J���������ʒu
	GetCurrentWin()->GetTrackball()->SetPosition(pos);	// �J���������ʒu�̐ݒ�
}

void FWFileLoaderSample::Reset(){
	GetSdk()->ClearScenes();
	GetSdk()->LoadScene(fileName);
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
}


void FWFileLoaderSample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			// �A�v���P�[�V�����̏I��
			exit(0);
			break;
		case 'r':
			// �t�@�C���̍ēǂݍ���
			Reset();
			break;
		case 'w':
			// �J�����ʒu�̏�����
			InitCameraView();	
			break;
		case 'd':				
			{
				// �f�o�b�N�\��
				static bool bDebug = GetSdk()->GetDebugMode();
				if(bDebug)	bDebug = false;
				else		bDebug = true;
				GetSdk()->SetDebugMode(bDebug);
				DSTR << "Debug Mode " << bDebug << std::endl;
				//DSTR << "CameraPosition" << std::endl;
				//DSTR << GetCurrentWin()->GetTrackball()->GetPosition() << std::endl;
			}
			break;
		default:
			break;
	}
}