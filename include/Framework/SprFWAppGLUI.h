/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPPGLUI_H
#define SPR_FWAPPGLUI_H

#include <Framework/SprFWAppGLUT.h>

class GLUI;

namespace Spr{;

class FWAppGLUIDesc {
public:
	GLUI*	guiID;
	int		fromTop;
	int		fromLeft;
	int		subPosition;
	char*	gluiName;
	bool	createOtherWindow;
	//�ʃE�B���h�E���쐬����ꍇ�̃R���X�g���N�^
	//glui�̏����l�͂ǂ�����΂����HNULL�Ƃ��ɂ���Ƒ��点���u�Ԏ��ʁB
	//�f�X�N���v�^�̃R���X�g���N�^�ɖ���o�^����̂͂�������
	FWAppGLUIDesc(GLUI* glui, int top = 500, int left = 30,	char* name = "Menu");
	//�T�u�E�B���h�E���쐬����ꍇ�̃R���X�g���N�^
	FWAppGLUIDesc(GLUI* glui, int pos = 0x20);
};

/** @brief GLUI��p����A�v���P�[�V�����N���X
*/
class FWAppGLUI : public FWAppGLUT, public FWAppGLUIDesc{
protected:
	static FWAppGLUI* instance;
	static void SPR_CDECL GluiDisplayFunc();
	static void SPR_CDECL GluiReshapeFunc(int w, int h);
	static void SPR_CDECL GluiTimerFunc(int id);
	static void SPR_CDECL GluiIdleFunc();
	static void SPR_CDECL GluiKeyboardFunc(unsigned char key, int x, int y);
	static void SPR_CDECL AtExit();
	std::vector<GLUI*> guis;
public:
	~FWAppGLUI();
	virtual void	Init(int argc, char* argv[]);
	virtual void	DesignGLUI();
	virtual void	Display();
	virtual void	Start();
	virtual GLUI*	CreateGUI(int wid=0, int subPos=((long)(1<<5)));
};

}
#endif