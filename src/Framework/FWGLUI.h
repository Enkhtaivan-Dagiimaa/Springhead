/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWGLUI_H
#define FWGLUI_H

#include <Framework/FWGLUT.h>
#include <GL/glui.h>

namespace Spr{;
/*
struct FWGLUIDesc{
	
	//�f�t�H���g�R���X�g���N�^
	FWGLUIDesc();
};
*/
/** @brief GLUI��p����A�v���P�[�V�����N���X
		   ��{�I��FWAppGLUI�������̃A�v���P�[�V�����̃N���X�Ɍp�������C
		   DesignGUI��K���I�[�o�[���C�h���Ďg�p����.
		   CreateGUI��FWAppGLUIDesc�̏������GLUI*�^�̕ϐ����擾����D
		   GLUI*�^�̕ϐ�����c���[���UI�̃p�[�c�ƃR�[���o�b�N���`����iDesignGUI�̎����j
		   glutTimerFunc��GLUI_Master.set_glutTimerFunc�ɂ��Ȃ��ƌł܂�D
		   ���Ƃ�glutMainLoop();
		   GLUI*��NULL����glut�Ɠ��������ɂȂ�̂�DesignGUI���Ă΂Ȃ��̂Ȃ�CreateGUI���ĂԕK�v�������D
		   �K�v�ɉ�����DesignGUI����h������R�[���o�b�N�֐����A�v���P�[�V�����N���X�ɍ쐬����K�v������D
		   �����̃A�v���P�[�V�����N���X�ɍ쐬����R�[���o�b�N�́C�Ăяo���K��ɒ��ӂ��č쐬���邱�ƁD
		   (�N���X���̊֐���`�͕K��__thiscall�ɂȂ�)   
	*/

class FWGLUI : public FWGLUT/*, public FWGLUIDesc*/{
//public:
//	SPR_OBJECTDEF_NOIF(FWGLUI);
protected:
	std::vector<GLUI*> guis;

	int		fromTop;					//< �ʃE�B���h�E�𐶐�����Ƃ��̏ォ���dot��
	int		fromLeft;					//< �ʃE�B���h�E�𐶐�����Ƃ��̍������dot��
	int		subPosition;				//< OpenGL�`��E�B���h�E�̒���GUI��g�ݍ���ł��܂��ꍇ�̑g�ݍ��ޏꏊ
	char*	gluiName;					//< �ʃE�B���h�E���쐬����ꍇ�̃E�B���h�E�̖��O
	bool	createOtherWindow;			//< GUI��ʃE�B���h�E�ɂ��邩�ǂ���

public:
	virtual void SPR_CDECL DesignGUI(){};
	/** GLUI�̃��C�����[�v���X�^�[�g����
		FWGLUT��StartMainLoop()�Ƃ͒��g���قȂ�̂ŏ����Ȃ����ƁD
	 */
	virtual void	StartMainLoop();			
	//virtual GLUI*	CreateGUI(int wid = 0, FWGLUIDesc desc = FWGLUIDesc());
	GLUI* CreateGUI(int wid = 0, int top = 50, int left = 30, int subPos = GLUI_SUBWINDOW_RIGHT,
		char* name = "Menu", bool other = true);

	FWGLUI(FWApp* a=0):FWGLUT(a){}
	~FWGLUI();
};

}

#endif