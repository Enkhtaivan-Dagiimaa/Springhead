/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPP_SIMPLE_H
#define SPR_FWAPP_SIMPLE_H

#include <Springhead.h>
#include <Framework/SprFWApp.h>
using namespace Spr;								// Springhead�̖��O���

/** @brief �A�v���P�[�V�����N���X
	FWAppSimple��FWApp�̔h���N���X�ł���C���[�U�[�ɂ��������������������N���X�ł���D
	BuildObject()���I�[�o�[���C�h���邱�ƂŃV�~�����[�V�������쐬���邱�Ƃ��ł���D
	�Ǝ��̋@�\�͉��z�֐����I�[�o���C�h���邱�Ƃɂ���Ď�������D
 */

class FWAppSimple;
class FWAppSimple : public FWApp{
public:
	FWAppSimple();									// �R���X�g���N�^�i���̃N���X�̃C���X�^���X���������ꂽ���ɌĂ΂��֐��j
	virtual void Init(int argc, char* argv[]);		// GLUT�̏�����
	virtual void Timer();							// �^�C�}�[�̍쐬
	virtual void IdleFunc(){};						// IdleFunc�̍쐬
	virtual void InitCameraView();					// �J�������W		
	virtual void BuildObject()=0;					// ���̂��쐬
	virtual void Step();							// �V�~�����[�V������1�X�e�b�v�̏���
	virtual void Display();							// �`��
	virtual void Reset();							// �V�[���̃��Z�b�g
	virtual void Keyboard(int key, int x, int y){};	// �L�[�{�[�h�֐�
	void				CallStep();
	static void SPR_CDECL	TimerFunc(int id);			// ���[�v�ŌĂ΂��֐�

	bool bDrawInfo;
}; 

#endif
