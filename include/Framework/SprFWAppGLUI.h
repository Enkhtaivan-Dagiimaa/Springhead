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
		   ��{�I��FWAppGLUI�������̃A�v���P�[�V�����̃N���X�Ɍp�������C
		   DesignGUI��K���I�[�o�[���C�h���Ďg�p����D
		   �K�v�ɉ�����DesignGUI����h������R�[���o�b�N�֐����A�v���P�[�V�����N���X�ɍ쐬����K�v������D
		   �����̃A�v���P�[�V�����N���X�ɍ쐬����R�[���o�b�N�́C�Ăяo���K��ɒ��ӂ��č쐬���邱�ƁD
		   (�N���X���̊֐���`�͕K��__thiscall�ɂȂ�)
*/
class FWAppGLUI : public FWAppGLUT, public FWAppGLUIDesc{
protected:
	std::vector<GLUI*> guis;
public:
	~FWAppGLUI();
	virtual void		 Init(int argc, char* argv[]);
	virtual void __cdecl DesignGUI() = 0;
	virtual void		 Start();						// FWAppGLUT��Start()�Ƃ͒��g���قȂ�̂ŏ����Ȃ����ƁD
	virtual GLUI*		 CreateGUI(int wid = 0, FWAppGLUIDesc desc = FWAppGLUIDesc());
};

}
#endif