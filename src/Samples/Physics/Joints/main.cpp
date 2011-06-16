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
- �V�[��2�F�V���A�������N

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

//#include <ctime>
//#include <string>
//#include <sstream>

#include "../../SampleApp.h"

#include "ChainHandler.h"
#include "LinkHandler.h"
#include "ArmHandler.h"
//#include "PathHandler.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;
using namespace std;

#define ESC		27

class MyApp : public SampleApp{
public:
	vector< UTRef<Handler> >	handlers;
	
public:
	MyApp(){
		appName		= "Joints";
		
		// �n���h���o�^
		handlers.push_back(new ChainHandler(this));
		handlers.push_back(new LinkHandler(this));
		handlers.push_back(new ArmHandler(this));
		numScenes	= handlers.size();
	}
	virtual ~MyApp(){}

	virtual void BuildScene(){
		handlers[curScene]->BuildScene();
	}

	virtual void OnAction(int menu, int id){
		if(menu == MENU_SCENE + curScene)
			handlers[curScene]->OnAction(id);
		SampleApp::OnAction(menu, id);
	}

	virtual void OnStep(){
		SampleApp::OnStep();
		handlers[curScene]->OnStep();
	}

	virtual void OnDraw(GRRenderIf* render){
		SampleApp::OnDraw(render);
		handlers[curScene]->OnDraw(render);
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
