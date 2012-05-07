/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWGLUT_H
#define FWGLUT_H

#include <Foundation/UTTimer.h>
#include <Framework/FWGraphicsHandler.h>
#include <Framework/FWWin.h>

namespace Spr{;

class DRKeyMouseGLUT;
class DRJoyStickGLUT;

/**
	FWGraphicsHandler��GLUT�ɂ�����
 */
class FWGLUT : public FWGraphicsHandler, public UTTimerProvider{
public:
	SPR_OBJECTDEF_NOIF(FWGLUT);
protected:
	bool	idleFuncFlag;	///< IdleFunc�̌Ăяo���Ɋւ���Flag	
	
	DRKeyMouseGLUT*	keyMouse;
	DRJoyStickGLUT*	joyStick;

	static FWGLUT*	GetInstance();

	/** �R�[���o�b�N�֐�*/
	static void SPR_CDECL GlutDisplayFunc();
	static void SPR_CDECL GlutReshapeFunc(int w, int h);
	static void SPR_CDECL GlutTimerFunc(int id);
	static void SPR_CDECL GlutIdleFunc();
	static void SPR_CDECL GlutMouseFunc(int button, int state, int x, int y);
	static void SPR_CDECL GlutMotionFunc(int x, int y);
	static void SPR_CDECL GlutPassiveMotionFunc(int x, int y);
	static void SPR_CDECL GlutKeyFunc(unsigned char ch, int x, int y);
	static void SPR_CDECL GlutSpecialKeyFunc(int ch, int x, int y);
	static void SPR_CDECL AtExit();

	virtual void RegisterCallbacks();
public:	
	///	GLUT�̏��������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc, char** argv);

	/// UTTimerProvider�̎���
	virtual bool StartTimer(UTTimer* timer);
	virtual bool StopTimer(UTTimer* timer);
	
	virtual void StartMainLoop();
	virtual void EnableIdleFunc(bool on = true);
	virtual void EnterGameMode();
	virtual void LeaveGameMode();

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWinIf* CreateWin(const FWWinDesc& d, FWWinIf* parent);
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWinIf* w);
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWinIf* w);
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual int GetCurrentWin();	
	///�J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay();
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int GetModifiers();
	/// �E�B���h�E��������
	virtual Vec2i	GetPosition		(FWWinBase* win);
	virtual void	SetPosition		(FWWinBase* win, int left, int top);
	virtual Vec2i	GetSize			(FWWinBase* win);
	virtual void	SetSize			(FWWinBase* win, int width, int height);
	virtual void	SetTitle		(FWWinBase* win, UTString title);
	virtual void	SetFullScreen	(FWWin* win);

	FWGLUT();
	~FWGLUT();

};

}





#endif