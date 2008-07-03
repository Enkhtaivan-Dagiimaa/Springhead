/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include "MYApp.h"
#include <Framework/SprFWAppGLUT.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <sstream>

#define ESC 27
using namespace std;

//=======================================================================================================
// �R���X�g���N�^�E�f�X�g���N�^
MYApp::MYApp(){
	instance	= this;
	dt			= 0.05;
	nIter		= 20;
	numWindow	= 10;
	for(int i = 0; i < numWindow; i++){
		stringstream sout;
		sout << "Window " << i+1 << endl;
		winNames.push_back(sout.str());
		camAngles.push_back(0.0f);
		camZooms.push_back(2.0f);
		views.push_back(Affinef());
	}
}

//=======================================================================================================
// �N���X���̊֐���`
void MYApp::MyRenderInit(FWWin* window, int num){
	GRCameraDesc cam;
	cam.size = Vec2f(0.05f, 0.0);
	GRDebugRenderIf* render = window->render->Cast();
	render->SetCamera(cam);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	GRLightDesc light[2];
	light[0].position = Vec4f(10.0,  20.0, 20.0, 1.0);
	light[1].position = Vec4f(10.0, 20.0, -20.0, 1.0);
	for(int i = 0 ; i < 2; i++){
		render->PushLight(light[i]);
		//views[i].Pos() = Vec3f(6.0f*cos(Rad(camAngle[i])), 3.0f, 6.0f*sin(Rad(camAngle[i]))) * camZoom[i];	//�J�����̍��W���w�肷��
		//views[i].LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1000.0, 0.0));
	}
	render->SetViewMatrix(views[num].inv());
}
//=======================================================================================================
// ��ʊK�w�Ő錾���ꂽ�֐��̃I�[�o�[���[�h

void MYApp::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();
	GetSdk()->SetDebugMode(true);
	for(int i = 0; i < numWindow ; i++){
		FWAppGLUTDesc winDesc;
		{
			winDesc.width			= 480;
			winDesc.height			= 320;
			winDesc.left			= 10 + 500*(i/2);
			winDesc.top				= 30 + 360*(i%2);
			winDesc.parentWindow	= 0;
			winDesc.fullscreen		= false;
			if(winNames[i].size()){
				winDesc.title		= winNames[i];
			}else{
				winDesc.title		= "Window";
			}
		}
		windows.push_back(CreateWin(winDesc));

		PHSceneDesc phDesc;
		{
			phDesc.timeStep			= dt;
			phDesc.numIteration		= nIter;
		}
		fwScenes.push_back(GetSdk()->CreateScene(phDesc, GRSceneDesc()));
		
		windows[i]->SetScene(fwScenes[i]);
		windows[i]->SetRender(GetSdk()->CreateRender());
		
		MyRenderInit(windows[i], i);		
	
		PHSolidDesc descSolid;													//���̏������p�̃f�B�X�N���v�^�̐錾
		{
			descSolid.dynamical = false;										//�������畨���@����؂�
		}
		
		PHSolidIf* soFloor = windows[i]->GetScene()->GetPHScene()->CreateSolid(descSolid);			//���̃C���^�t�F�[�X�Ƀf�B�X�N���v�^�̒��g��n�����h���̂Ƃ����T�O�h��o�^����
		soFloor->SetName("Floor");
		CDBoxDesc descBox;														//���̏Փ˔���p�̃f�B�X�N���v�^�̐錾
		{
			descBox.boxsize			= Vec3f(30*i, 2, 30);						//���̃T�C�Y�̎w��
			descBox.material.mu		= (float) 1.0;
			descBox.material.mu0	= (float) 1.0;
		}
		soFloor->AddShape(windows[i]->GetScene()->GetPHScene()->GetSdk()->CreateShape(descBox));		//��قǓo�^�����h���̂Ƃ����T�O�h�ɏՓ˔���ł�����̂�^����
		soFloor->SetFramePosition(Vec3f(0, 0, 0));								//���̂������̂̐ݒu�ꏊ���w�肷��
	}
	return;
}

void MYApp::Keyboard(int key, int x, int y){
	if(key == 'q'){
		exit(0);
	} else if(key == ESC){
		exit(0);
	} else{
	}
}