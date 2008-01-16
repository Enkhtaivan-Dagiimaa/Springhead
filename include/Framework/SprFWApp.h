/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPP_H
#define SPR_FWAPP_H

#include <Framework/SprFWSdk.h>
#include <Framework/SprFWScene.h>

namespace Spr{;

/** Ruby�̃v���O�������FWApp���p�����Ă������̊֐����I�[�o���C�h����ۂɁC
�@�@�t���[�����[�N�����܂��I�[�o���C�h���ꂽ�֐���F�������s���邽�߂̃��J�j�Y��
 */
class FWVFuncBridge : public UTRefCount{
public:
	virtual void Link(void* pObj) = 0;
	virtual bool Display() = 0;
	virtual bool Reshape(int w, int h) = 0;
	virtual bool Keyboard(unsigned char key, int x, int y) = 0;
	virtual bool MouseButton(int button, int state, int x, int y) = 0;
	virtual bool MouseMove(int x, int y) = 0;
	virtual bool Step() = 0;
	virtual bool Idle() = 0;
	virtual void AtExit() = 0;
	virtual ~FWVFuncBridge(){}
};

/** @brief �A�v���P�[�V�����N���X
	Springhead�̃N���X�͊�{�I�Ɍp�������Ɏg�p����悤�ɐ݌v����Ă��邪�C
	FWApp����т��̔h���N���X�͗�O�ł���C���[�U��FWApp���邢�͂��̔h���N���X���p�����C
	���z�֐����I�[�o���C�h���邱�Ƃɂ���ēƎ��@�\����������D

	FWApp�N���X�̓O���t�B�b�N�X�̏������@�\�������Ȃ��̂ŁC
	�ʏ�͔h���N���X�ł���FWAppGL��FWAppGLUT���g�p����D
 */
class FWApp{
protected:
	UTRef<FWSdkIf> fwSdk;
	
	void CallDisplay(){
		if(!vfBridge || !vfBridge->Display())
			Display();
	}
	void CallReshape(int w, int h){
		if(!vfBridge || !vfBridge->Reshape(w, h))
			Reshape(w, h);
	}
	void CallKeyboard(unsigned char key, int x, int y){
		if(!vfBridge || !vfBridge->Keyboard(key, x, y))
			Keyboard(key, x, y);
	}
	void CallMouseButton(int button, int state, int x, int y){
		if(!vfBridge || !vfBridge->MouseButton(button, state, x, y))
			MouseButton(button, state, x, y);
	}
	void CallMouseMove(int x, int y){
		if(!vfBridge || !vfBridge->MouseMove(x, y))
			MouseMove(x, y);
	}
	void CallStep(){
		if(!vfBridge || !vfBridge->Step())
			Step();
	}
	void CallIdle(){
		if(!vfBridge || !vfBridge->Idle())
			Idle();
	}
public:
	UTRef<FWVFuncBridge>	vfBridge;

	/** @brief SDK���擾����
	 */
	FWSdkIf*	GetSdk(){ return fwSdk; }

	/** @brief ������
		FWApp�I�u�W�F�N�g�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	virtual void Init(int argc = 0, char* argv[] = NULL);

	/** @brief �R�}���h���C�������̏���
		�A�v���P�[�V�����ɓn���ꂽ�R�}���h���C�������������������ꍇ�ɃI�[�o���C�h����
	 */
	virtual void ProcessArguments(int argc, char* argv[]){}

	/** @brief �V�~�����[�V�����̎��s
		�f�t�H���g�ł�FWSdk::Step���Ă΂��D
	 */
	virtual void Step();

	/** @brief idle�֐�
	 */
	virtual void Idle(){}

	/** @brief �V�[���̕`��
		�V�[�����\�������Ƃ��ɌĂ΂��D
		�`�揈�����J�X�^�}�C�Y�������ꍇ�ɃI�[�o���C�h����D
		�f�t�H���g�ł�FWSdk::Draw���Ă΂��D
	 */
	virtual void Display();

	/** @brief �`��̈�̃T�C�Y�ύX
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
		�f�t�H���g�ł�FWSdk::Reshape���Ă΂��D
	 */
	virtual void Reshape(int w, int h);

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
	 */
	virtual void Keyboard(unsigned char key, int x, int y){}

	/** @brief �}�E�X�C�x���g�̃n���h��
	 */
	virtual void MouseButton(int button, int state, int x, int y){}

	/** @brief �}�E�X�C�x���g�̃n���h��
	 */
	virtual void MouseMove(int x, int y){}

	virtual ~FWApp();

	//	���݂�Window��Ԃ�
	virtual FWWin* GetWin(){ return NULL; }
};

}
#endif
