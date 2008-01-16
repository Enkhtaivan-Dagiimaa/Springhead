/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPPGLUT_H
#define SPR_FWAPPGLUT_H

#include <Framework/SprFWAppGL.h>

namespace Spr{;

class FWWinDesc{	//	hase	TypeDesc���ł��Ȃ��悤�ɃN���X�ɂ��Ă���BTypeDesc���ł̑Ή����]�܂����B
public:
	int width;			///<	��
	int height;			///<	����
	int left;			///<	���[�̍��W
	int top;			///<	��[�̍��W
	int parentWindow;	///<	�q�E�B���h�E�����ꍇ�́A�e��ID�A�����łȂ����0
	UTString title;		///<	�E�B���h�E�̃^�C�g��������(�g�b�v���x���E�B���h�E�̂�)
	bool fullscreen;
	FWWinDesc(int w=640, int h=480, int l=-1, int t=-1, int p=0, bool f=false):
		width(w), height(h), left(l), top(t), parentWindow(p), fullscreen(f){
	}
};
	struct Win{
		int id;
		GRRenderIf* render;
	};
/** @brief GLUT��p����A�v���P�[�V�����N���X
 */
class FWAppGLUT : public FWAppGL{
public:

protected:
	static FWAppGLUT* instance;
	static void SPR_CDECL GlutDisplayFunc();
	static void SPR_CDECL GlutReshapeFunc(int w, int h);
	static void SPR_CDECL GlutTimerFunc(int id);
	static void SPR_CDECL GlutIdleFunc();
	static void SPR_CDECL GlutKeyboardFunc(unsigned char key, int x, int y);
	static void SPR_CDECL AtExit();
	std::vector<Win> windows;
public:
	~FWAppGLUT();
	///	�`��R�[���o�b�N
	virtual void Display();

	///	GLUT�̏��������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc, char* argv[]);
	///	�쐬�ς݂̃E�B���h�E�̐�
	virtual int NWindow(){ return (int)windows.size(); }
	///	�E�B���h�EID�̎擾
	virtual int GetWin(int i){ return windows[i].id; }
	/// �E�B���h�E��Render�̎擾
	virtual GRRenderIf* GetWinRender(int i){ return windows[i].render; }
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual int CreateGLWin(const FWWinDesc d=FWWinDesc());
	///	�E�B���h�E��j������
	virtual void DestroyWin(int wid);
	///	�J�����g�E�B���h�E��ݒ肷��B�ݒ���ID��Ԃ��B
	virtual int SetWin(int wid);
	///	�J�����g�E�B���h�E��ID��Ԃ��B
	virtual int GetWin();

	///	�t���[�����[�N���X�^�[�g����B
	virtual void Start();
};


}

#endif
