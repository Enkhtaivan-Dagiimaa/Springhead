/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "FWMultiWinSample.h"
#include <iostream>

using namespace std;

FWMultiWinSample::FWMultiWinSample(){
}

void FWMultiWinSample::Init(int argc, char* argv[]){
	/// �V�~�����[�V�����̏�����
	CreateSdk();
	GRInit(argc, argv);

	/// �V�[�������[�h
	GetSdk()->LoadScene("./xFiles/cookie/sceneCookie.spr");
	//GetSdk()->LoadScene("./xFiles/scene1.spr");

	/// �`��E�B���h�E�̍쐬�C������
	FWWinDesc winDesc;
	winDesc.width	= 480;
	winDesc.height	= 320;
	winDesc.left	= 10;
	winDesc.top		= 30;
	winDesc.fullscreen = false;
	
	for(int i = 0; i < 2; i++)
		CreateWin(winDesc);
	
	// �J�����ʒu�̎w��
	GetWin(0)->GetTrackball()->SetPosition(Vec3f(-0.0050556, 0.155619, 0.191252));
	GetWin(1)->GetTrackball()->SetPosition(Vec3f(-0.0050556, 0.155619, 0.191252));

	// �E�B���h�E�̃^�C�g���̎w��
	GetWin(0)->SetTitle("Window1: PHScene");
	GetWin(1)->SetTitle("Window2: GRScene");

	// �E�B���h�E�̈ʒu���߁D�^�C�g���o�[��t���[�����̃}�[�W���͎����Ōv�Z����K�v������
	GetWin(0)->SetPosition(  100,  100);
	GetWin(1)->SetPosition( 600,  100);
	
	// �E�B���h�E�ɕ`�悷��V�[�������蓖��
	GetWin(0)->SetScene(GetSdk()->GetScene(0));
	GetWin(1)->SetScene(GetSdk()->GetScene(0));

	/// �`�惂�[�h�ݒ�
	GetWin(0)->SetDebugMode(true);
	GetWin(1)->SetDebugMode(false);
	
	// �`�惂�[�h�̎w��
	for(int i = 0; i < GetSdk()->NScene(); i++){
		FWSceneIf* scene = GetSdk()->GetScene(i);
		scene->SetRenderMode(true, false);
		//scene->EnableRenderAxis();
		//scene->EnableRenderForce();
		scene->EnableRenderContact();
		//scene->EnableRenderGrid();
	}

	/// �^�C�}�̍쐬
	CreateTimer();
}

void FWMultiWinSample::TimerFunc(int id){	
	// �S�E�B���h�E�ɑ΂��čĕ`��v��
	Vec3f p = GetCurrentWin()->GetTrackball()->GetPosition();
	GetSdk()->GetScene()->Step();
	for(int i = 0; i < NWin(); i++){
		SetCurrentWin(GetWin(i));
		GetCurrentWin()->GetTrackball()->SetPosition(p);
		PostRedisplay();
	}
}

void FWMultiWinSample::Keyboard(int key, int x, int y){
	switch(key){
	case DVKeyCode::ESC:
	case 'q':
		exit(0);
		break;
	case 'r':
		Reset();
		break;
	case ' ':{
			FWObjectIf* fwCookie;
			GetSdk()->GetScene()->FindObject(fwCookie, "fwCookie");
			if(!fwCookie) break;
			FWObjectIf* clone = fwCookie->CloneObject()->Cast();
			clone->GetPHSolid()->SetFramePosition(Vec3d(0.0, 0.1, 0.0));
			 }
		break;
	default:
		break;
	}
}


