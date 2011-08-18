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

namespace Spr{;

class FWGLUI : public FWGLUT{
public:
	SPR_OBJECTDEF_NOIF(FWGLUI);
protected:
	// �R�[���o�b�N�������s���R���g���[���̔z��
	// �{�^���C�e�L�X�g�{�b�N�X
	std::vector<FWControl*>	ctrls;

	/// �C���X�^���X�擾
	static FWGLUI* GetInstance();

	/** �R�[���o�b�N�֐�*/
	static void SPR_CDECL GLUIUpdateFunc(int id);
	
	virtual void RegisterCallbacks();
public:
	virtual void EnableIdleFunc(bool on = true);
	virtual void CalcViewport(int& l, int& t, int& w, int& h);

	virtual FWDialog*	CreateDialog	(FWWin* owner, const FWDialogDesc& desc);
	virtual FWControl*	CreateControl	(FWDialog* owner, const IfInfo* ii, const FWControlDesc& desc, FWPanel* parent);
	virtual void		CreateColumn	(FWDialog* owner, bool sep, FWPanel* panel);
	virtual void		CreateSeparator	(FWDialog* owner, FWPanel* panel);
	virtual void		SetSize			(FWWinBase* win, int width, int height);
	virtual void		SetLabel		(FWControl* win, UTString label);
	virtual void		SetAlign		(FWControl* ctrl, int align);
	virtual int			GetInt			(FWControl* ctrl);
	virtual void		SetInt			(FWControl* ctrl, int val);
	virtual float		GetFloat		(FWControl* ctrl);
	virtual void		SetFloat		(FWControl* ctrl, float val);
	virtual void		SetChecked		(FWButton*	btn,  bool on);
	virtual bool		IsChecked		(FWButton*	btn);
	virtual void		SetIntRange		(FWTextBox* text, int rmin, int rmax);
	virtual void		SetFloatRange	(FWTextBox* text, float rmin, float rmax);
	virtual const char*	GetString		(FWTextBox* text);
	virtual void		SetString		(FWTextBox* text, char* str);
	virtual Matrix3f	GetRotation		(FWRotationControl* rotCtrl);
	virtual void		SetRotation		(FWRotationControl* rotCtrl, const Matrix3f& rot);
	virtual void		SetDamping		(FWRotationControl* rotCtrl, float d);
	virtual void		Reset			(FWRotationControl* rotCtrl);
	virtual	Vec3f		GetTranslation	(FWTranslationControl* trnCtrl);
	virtual	void		SetTranslation	(FWTranslationControl* trnCtrl, Vec3f p);
	virtual void		SetSpeed		(FWTranslationControl* trnCtrl, float sp);
	virtual void		AddItem			(FWListBox*	listBox, UTString label);

	FWGLUI(){}
	~FWGLUI();
};
/*
class FWWinGLUI : public FWWinGLUT{
public:
	virtual FWDialogIf*		CreateDialog(const FWDialogDesc& desc);
};

class FWDialogGLUI : public FWDialog{
public:
	virtual FWControlIf*	CreateControl(const FWControlDesc& desc);
};

class FWControlGLUI : public FWControl{
public:
	virtual UTString	GetLabel();
	virtual void		SetLabel(UTString label);
	virtual void		SetAlign(int align);
	virtual int			GetStyle();
	virtual void		SetStyle(int style);
};

class FWPanelGLUI : public FWPanel{
public:
	virtual void	Open(bool on = true);
	virtual void	CreateColumn(bool sep = true);
	virtual void	CreateSeparator();
};

class FWButtonGLUI : public FWButton{
public:
	virtual void	SetChecked(bool on = true);
	virtual bool	IsChecked();

	virtual	void		SetRadioGroup(FWPanelIf*);
	virtual	FWPanelIf*	GetRadioGroup();
};

class FWStaticTextGLUI : public FWStaticText{
public:

};

class FWTextBoxGLUI : public FWTextBox{
public:
	virtual void	SetIntRange(int rmin, int rmax);
	virtual void	GetIntRange(int& rmin, int& rmax);
	virtual void	SetFloatRange(int rmin, int rmax);
	virtual void	GetFloatRange(int& rmin, int& rmax);

};

class FWListBoxGLUI : public FWListBox{
public:

};

class FWRotationControlGLUI : public FWRotationControl{
public:
};

class FWTranslationControlGLUI : public FWTranslationControl{
public:
};
*/
}

#endif