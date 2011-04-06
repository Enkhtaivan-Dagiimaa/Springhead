/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** \page pageJoints �֐ߋ@�\�̃f��
 Springhead2/src/Samples/Joints/main.cpp

\ref pagePhysics �̊֐ߋ@�\�̃f���v���O�����B

\section secTestPatternJoints �e�X�g�p�^�[��
- �V�[��0�F��
- �V�[��1�F�`�F�r�V�F�t�����N
- �V�[��2�F�o�l�_���p����W���C���g�ƃo�l�_���p
- �V�[��3�F�{�[���W���C���g1�̃e�X�g
- �V�[��4�F�p�X�W���C���g
- �V�[��5�F�o�l�_���p�i����ȃo�l�_���p�̊m�F�f���j 


\section secSpecJoints �d�l
- ���ʂ̑���F
 - �V�[���ԍ�0, 1, ...���^�C�v����ƑΉ�����V�[���ɐ؂�ւ��i�f�t�H���g�F�V�[��0)
  - 'x'���^�C�v����Ɗ֐߂̗L��/�������؂�ւ��
  - 'z'�ŁA�E��������{�b�N�X���΂��A�Փ˂�����		
  - 'Z'�ŁA��O��������{�b�N�X���΂��A�Փ˂�����
  - 'C'�ŁA�E��������J�v�Z�����΂��A�Փ˂�����
  - 'S'�ŁA�E�������狅���΂��A�Փ˂�����
  - 'P'�ŁA�V�~�����[�V�������~�߂�
- �V�[��0�F ��
  - '0'�ŁA�q���W�V�[���̐ݒ���s���i�V�[���؊����j
  - ' '���邢��'b'�Ńq���W�p��box�𐶐�
		�@' '�̏ꍇ�͓����A���S���Y����ABA�ƂȂ�
- �V�[��1�F
  - '1'�ŁA�`�F�r�V�F�t�����N�𐶐�����i�V�[���؊����j
  - 'a'�ŁA���[�^�[�g���N��1.0�ɐݒ肷��
  - 's'�ŁA���[�^�[�g���N��0.0�ɐݒ肷��
  - 'd'�ŁA���[�^�[�g���N��-1.0�ɐݒ肷��
  - 'f'�ŁA�ڕW���x��90.0�ɐݒ肷��
  - 'g'�ŁA�ڕW���x��0.0�ɐݒ肷��
  - 'h'�ŁA�ڕW���x��-90.0�ɐݒ肷��
  - 'j'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��1.0�ɐݒ肷��
  - 'k'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��0.0�ɐݒ肷��
  - 'l'�ŁA�o�l���_(�o�l�̗͂�0�ƂȂ�֐ߕψ�)��-1.0�ɐݒ肷��
- �V�[��4�F
  - '4'�ŁA�p�X�W���C���g�V�[���̐ݒ���s���i�V�[���؊����j
  - 'a'�ŁA�d�͕������A(0.0, -9.8, 0.0)�ɐݒ肷��
  - 'd'�ŁA�d�͕������A(0.0,  9.8, 0.0)�ɐݒ肷��
  - 
- �V�[��5�F
  - '5'�ŁA�o�l�E�_���p�V�[���̐ݒ���s���i�V�[���؊����j
  - 'a'�ŁA���̂̃t���[�����_���A�ʒu(-20.0, 30.0, 0.0)�ɐݒ肷��
  - 's'�ŁA���̂̃t���[�����_���A�ʒu(-10.0, 20.0, 0.0)�ɐݒ肷��
  - 'd'�ŁA���̂̃t���[�����_���A�ʒu( -5.0, 10.0, 0.0)�ɐݒ肷��
  - 'f'�ŁA���̂̃t���[�����_���A�ʒu(  0.0, 10.0, 0.0)�ɐݒ肷��
  - 'g'�ŁA���̂̃t���[�����_���A�ʒu(  5.0, 10.0, 0.0)�ɐݒ肷��
  - 'h'�ŁA���̂̃t���[�����_���A�ʒu( 10.0, 20.0, 0.0)�ɐݒ肷��
  - 'j'�ŁA���̂̃t���[�����_���A�ʒu( 20.0, 30.0, 0.0)�ɐݒ肷��
  - '.'�ŁA�R�[���o�b�N�֐��̌Ăяo�����Ԃ�1/2�{����
  - ','�ŁA�R�[���o�b�N�֐��̌Ăяo�����Ԃ�2�{����
  - 'n'�ŁA�o�l���_��+0.01����
  - 'm'�ŁA�o�l���_��-0.01����
*/

#include <ctime>
#include <string>
#include <sstream>

#include "ChainHandler.h"
#include "LinkHandler.h"
#include "PathHandler.h"
#include "ArmHandler.h"
#include "BallJointHandler.h"
#include "HingeJointHandler.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;
using namespace std;

#define ESC		27

class MyApp : public FWApp{
public:
	UTRef<ObjectStatesIf>		state;
	vector< UTRef<Handler> >	handlers;
	Handler*					activeHandler;
	FWEditorOverlay				editor;

	bool	bAutoStep;		///< �����X�e�b�v

public:
	void SwitchScene(int id){
		if(id < 0 || (int)handlers.size() <= id)
			return;

		GetSdk()->SwitchScene(GetSdk()->GetScene(id));
		editor.SetObject(GetSdk()->GetScene()->GetPHScene());
		activeHandler = handlers[id];
		if(!activeHandler->bReady)
			activeHandler->Build();
		cameraInfo.Fit(GetSdk()->GetRender()->GetCamera(), activeHandler->GetSceneRadius());
	}

	/**
	 brief 		�L�[�{�[�h�R�[���o�b�N�֐� 
	 param		<in/--> key�@�@ ASCII�R�[�h
	 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
	 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
	 return 	�Ȃ�
	 */
	virtual void Keyboard(int key, int x, int y){
		if(key == ESC || key == 'q')
			exit(0);
		if(key == ' ')
			bAutoStep = !bAutoStep;
		if(key == ';')
			Step();

		PHSceneIf* phScene = activeHandler->GetPHScene();
		if (key == 'W'){
			phScene->WriteState("state.bin");
		}
		else if (key == 'R'){
			phScene->ReadState("state.bin");
		}
		else if(key == ','){
			state->SaveState(phScene);
		}
		else if(key == '.'){
			state->LoadState(phScene);
		}
		else if(key == '/'){
			state->ReleaseState(phScene);
		}

		// FWEditor�ŏ�������Ȃ������L�[���͂̂ݎ��ɉ�
		if(!editor.Key(key)){
			//�V�[���؂�ւ�
			if(key <= 0xff && isdigit(key)){
				SwitchScene(key - '0');
			}
			else{
				activeHandler->OnKey(key);
				// FWEditor���X�V
				editor.SetObject(GetSdk()->GetScene()->GetPHScene());
			}
		}
	}

	void Step(){
		GetSdk()->Step();
		activeHandler->OnTimer();
	}

	virtual void TimerFunc(int id){
		if(bAutoStep)
			Step();
		PostRedisplay();
	}	

	virtual void Init(int argc = 0, char* argv[] = NULL){
		SetGRAdaptee(TypeGLUT);
		GRInit(argc, argv);
		CreateSdk();
		
		FWWinDesc windowDesc;
		windowDesc.title = "Joints";
		CreateWin(windowDesc);
		InitWindow();

		UTTimerIf* timer = CreateTimer();				// �^�C�}�[�̐���
		timer->SetInterval(50);

		cameraInfo.zoomRange[1] = 1000.0f;
		GetSdk()->SetDebugMode(true);

		/*GRRenderIf* render = GetSdk()->GetRender();
		GRLightDesc light;
		light.position = Vec4f(10.0, 20.0, 20.0, 1.0);
		light.diffuse = Vec4f(1,1,1,1) * 0.5f;
		light.specular = Vec4f(1,1,1,1) * 0.2f;
		light.ambient = Vec4f(1,1,1,1) * 0.0f;
		render->PushLight(light);*/

		// �V�[���̍\�z
		handlers.push_back(new ChainHandler());
		handlers.push_back(new LinkHandler());
		handlers.push_back(new ArmHandler());
		handlers.push_back(new BallJointHandler());
		handlers.push_back(new HingeJointHandler());
		handlers.push_back(new PathHandler());
		for(unsigned i = 0; i < handlers.size(); i++){
			FWSceneIf* fwScene = GetSdk()->CreateScene();
			fwScene->GetPHScene()->SetTimeStep(0.05);
			fwScene->GetPHScene()->SetNumIteration(20);
			fwScene->SetRenderMode(true, false);
			fwScene->EnableRenderAxis(true, false, false);
			fwScene->EnableRenderForce();
			fwScene->EnableRenderContact();
		}

		state = ObjectStatesIf::Create();
		SwitchScene(0);
		bAutoStep = false;
	}

	virtual void Display(){
		// �`�惂�[�h�̐ݒ�
		GRRenderIf* render = GetCurrentWin()->render->Cast();
		GRCameraDesc cam = render->GetCamera();
		cam.front = 0.5f;
		render->SetCamera(cam);
		render->SetViewMatrix(cameraInfo.view.inv());

		// �`��̎��s
		GetSdk()->Draw();

		// ���̃I�[�o���C
		editor.Update();
		editor.Draw(render);
		
		glutSwapBuffers();
	}

} app;

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	app.Init(argc, argv);
	app.StartMainLoop();
}
