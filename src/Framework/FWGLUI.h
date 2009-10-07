#ifndef FWGLUI_H
#define FWGLUI_H
#include<sstream>
#include<string>
#include<vector>
#include<Springhead.h>
#include<Framework/FWGLUT.h>
#include"Framework/SprFWApp.h"
#include <GL/glui.h>

using namespace std;

namespace Spr{;


class FWGLUIDesc{
public:
	int		fromTop;					//< �ʃE�B���h�E�𐶐�����Ƃ��̏ォ���dot��
	int		fromLeft;					//< �ʃE�B���h�E�𐶐�����Ƃ��̍������dot��
	int		subPosition;				//< OpenGL�`��E�B���h�E�̒���GUI��g�ݍ���ł��܂��ꍇ�̑g�ݍ��ޏꏊ
	char*	gluiName;					//< �ʃE�B���h�E���쐬����ꍇ�̃E�B���h�E�̖��O
	bool	createOtherWindow;			//< GUI��ʃE�B���h�E�ɂ��邩�ǂ���
	
	//�f�t�H���g�R���X�g���N�^
	FWGLUIDesc();
};

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

class FWGLUI :public FWGLUT, public FWGLUIDesc{
protected:
	std::vector<GLUI*> guis;
public:
	~FWGLUI();
	virtual void SPR_CDECL DesignGUI(){};
	///GLUI�ɂ��Timer���X�^�[�g����
	virtual void StartMainLoop();			// FWAppGLUT��StartMainLoop()�Ƃ͒��g���قȂ�̂ŏ����Ȃ����ƁD
	virtual GLUI*		 CreateGUI(int wid = 0, FWGLUIDesc desc = FWGLUIDesc());
};

}

#endif