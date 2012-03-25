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
	/// �V�~�����[�V�����̏�����
	FWApp::Init();									// �A�v���P�[�V�����̏�����
	UTRef<ImportIf> import = GetSdk()->GetFISdk()->CreateImport();	// 
	GetSdk()->LoadScene(fileName, import);			// �t�@�C���̃��[�h
	GetSdk()->SaveScene("save.spr", import);		// �t�@�C���̃Z�[�u�e�X�g

	PHSolidIf* solid = GetSdk()->GetScene()->GetPHScene()->CreateSolid();
	CDBoxDesc bd;
	bd.boxsize.clear(1.0);
	solid->AddShape(GetSdk()->GetPHSdk()->CreateShape(bd));
	solid->SetDynamical(false);

	/// �J�����r���[�̏�����
	InitCameraView();
	GetSdk()->GetScene()->EnableRenderAxis(true);
	GetSdk()->GetScene()->EnableRenderContact(true);

}

void FWFileLoaderSample::InitCameraView(){
}

void FWFileLoaderSample::Reset(){
	GetSdk()->Clear();		
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
				static bool bDebug = true;
				if(bDebug)	bDebug = false;
				else		bDebug = true;
				GetSdk()->SetDebugMode(bDebug);
				DSTR << "Debug Mode " << bDebug << std::endl;
			}
			break;
		default:
			break;
	}
}