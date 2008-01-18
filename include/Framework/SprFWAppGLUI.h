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
	int		fromTop;					//< �ʃE�B���h�E�𐶐�����Ƃ��̏ォ���dot��
	int		fromLeft;					//< �ʃE�B���h�E�𐶐�����Ƃ��̍������dot��
	int		subPosition;				//< OpenGL�`��E�B���h�E�̒���GUI��g�ݍ���ł��܂��ꍇ�̑g�ݍ��ޏꏊ
	char*	gluiName;					//< �ʃE�B���h�E���쐬����ꍇ�̃E�B���h�E�̖��O
	bool	createOtherWindow;			//< GUI��ʃE�B���h�E�ɂ��邩�ǂ���
	
	//�f�t�H���g�R���X�g���N�^
	FWAppGLUIDesc();
};

/** @brief GLUI��p����A�v���P�[�V�����N���X
*/
class FWAppGLUI : public FWAppGLUT, public FWAppGLUIDesc{
protected:
	std::vector<GLUI*> guis;
public:
	~FWAppGLUI();
	virtual void	Init(int argc, char* argv[]);
	virtual void	DesignGUI() = 0;
	virtual void	Display() = 0;
	virtual void	Start();
	virtual GLUI*	CreateGUI(int wid = 0, FWAppGLUIDesc desc = FWAppGLUIDesc());
};

}
#endif