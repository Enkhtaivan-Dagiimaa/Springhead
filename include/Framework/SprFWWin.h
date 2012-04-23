#ifndef SPR_FW_WINDOW_H
#define SPR_FW_WINDOW_H

/** SprFWWin.h
	- �E�B���h�E��GUI�̒�
	- ���ۂɗ��p�\�ȋ@�\�͑I������Ă���O���t�B�N�X�V�X�e���Ɉˑ�����

	- GUI�R���g���[���̎d�l��GLUI�Ɋ�Â��Ă���D
		- �R���g���[���̔z�u��FWColumn(���z�u)��FWPanel(�c�z�u)�𗘗p����
 **/

#include <Foundation/SprObject.h>

namespace Spr{;

struct GRRenderIf;
struct DVKeyMouseIf;
struct DVJoyStickIf;
struct HITrackballIf;
//struct HIDraggerIf;
struct FWSceneIf;
struct FWPanelIf;
struct FWStaticTextIf;
struct FWButtonIf;
struct FWTextBoxIf;
struct FWRotationControlIf;
struct FWTranslationControlIf;
struct FWListBoxIf;

/** @�E�B���h�E�̊��N���X
 */
struct FWWinBaseIf : ObjectIf{
	SPR_IFDEF(FWWinBase);

	/// ID���擾����
	int		GetID();

	/// �ʒu���擾����
	Vec2i	GetPosition();
	/// �ʒu��ύX����
	void	SetPosition(int left, int top);
	
	/// �T�C�Y���擾����
	Vec2i	GetSize();
	/// �T�C�Y��ύX����
	void	SetSize(int width, int height);
	
	/// �^�C�g�����擾����
	UTString GetTitle();
	/// �^�C�g����ύX����
	void	SetTitle(UTString title);
	
};
struct FWWinBaseDesc{
	SPR_DESCDEF(FWWinBase);

	int			width;			///<	��
	int			height;			///<	����
	int			left;			///<	���[�̍��W
	int			top;			///<	��[�̍��W
	UTString	title;			///<	�E�B���h�E�̃^�C�g��������(�g�b�v���x���E�B���h�E�̂�)

	FWWinBaseDesc(){
		width = 640;
		height = 480;
		left = 100;
		top = 100;
		title = "Springhead2";
	};
};

/** GUI�A�C�e��
 */
struct FWControlIf : FWWinBaseIf{
	SPR_IFDEF(FWControl);

	/// ���x���̎擾
	UTString	GetLabel();
	/// ���x���̐ݒ�
	void		SetLabel(UTString l);

	/// �z�u�̐ݒ�
	void	SetAlign(int align);

	/// �X�^�C���̎擾
	int		GetStyle();
	/// �X�^�C���̐ݒ�
	void	SetStyle(int style);

	/// int�l���擾
	int		GetInt();
	/// int�l��ݒ�
	void	SetInt(int val);
	/// float�l���擾
	float	GetFloat();
	/// float�l��ݒ�
	void	SetFloat(float val);

};
struct FWControlDesc{
	SPR_DESCDEF(FWControl);

	enum {
		ALIGN_LEFT,
		ALIGN_CENTER,
		ALIGN_RIGHT,
	};
	UTString	label;
	int			align;
	int			style;

	FWControlDesc(){
		align = ALIGN_LEFT;
		style = 0;
	}
};


/// �p�l��
struct FWPanelIf : FWControlIf{
	SPR_IFDEF(FWPanel);

};
struct FWPanelDesc : FWControlDesc{
	SPR_DESCDEF(FWPanel);

	enum{
		NONE,
		RAISED,
		EMBOSSED,
		ROLLOUT,
		RADIOGROUP,
	};
	bool	open;		///< �쐬����ɊJ���Ă��邩�ǂ����iROLLOUT�X�^�C������j
};

/// �{�^��
struct FWButtonIf : FWControlIf{
	SPR_IFDEF(FWButton);

	/// �`�F�b�N��Ԃ̐ݒ�
	void	SetChecked(bool on = true);
	/// �`�F�b�N��Ԃ̎擾
	bool	IsChecked();
};
struct FWButtonDesc : FWControlDesc{
	SPR_DESCDEF(FWButton);

	enum{
		PUSH_BUTTON,
		CHECK_BUTTON,
		RADIO_BUTTON,
	};
	int		style;
	bool	checked;

	FWButtonDesc(){
		style = PUSH_BUTTON;
		checked = false;
	}
};

/// �ÓI�e�L�X�g
struct FWStaticTextIf : FWControlIf{
	SPR_IFDEF(FWStaticText);
};
struct FWStaticTextDesc : FWControlDesc{
	SPR_DESCDEF(FWStaticText);
};

/// �e�L�X�g�{�b�N�X
struct FWTextBoxIf : FWControlIf{
	SPR_IFDEF(FWTextBox);

	/// int�͈͐ݒ�
	void	SetIntRange(int rmin, int rmax);
	/// int�͈͎擾
	void	GetIntRange(int& rmin, int& rmax);
	/// float�͈͐ݒ�
	void	SetFloatRange(float rmin, float rmax);
	/// float�͈͎擾
	void	GetFloatRange(float& rmin, float& rmax);

	/// ��������擾
	const char*	GetString();
	/// �������ݒ�
	void		SetString(char* str);

};
struct FWTextBoxDesc : FWControlDesc{
	SPR_DESCDEF(FWTextBox);

	enum{
		TEXT,
		INT,
		FLOAT,
	};

	UTString	text;
	int			intMin;
	int			intMax;
	float		floatMin;
	float		floatMax;

	bool	spinner;

	FWTextBoxDesc(){
		style = TEXT;
		intMin = 0;
		intMax = 0;
		floatMin = 0.0f;
		floatMax = 0.0f;
	}
};

/** ���X�g�{�b�N�X
	 �I������Ă���A�C�e���̎擾��GetInt, �A�C�e���̑I����SetInt�ōs���D
	 �I���A�C�e�����ω������OnControlUpdate���Ă΂��
 **/
struct FWListBoxIf : FWControlIf{
	SPR_IFDEF(FWListBox);

	/// �A�C�e����ǉ�
	void	AddItem(UTString label);
};
struct FWListBoxDesc : FWControlDesc{
	SPR_DESCDEF(FWListBox);
};

/// ��]�R���g���[��
struct FWRotationControlIf : FWControlIf{
	SPR_IFDEF(FWRotationControl);

	/// ��]�̎擾
	Matrix3f	GetRotation();
	/// ��]�̐ݒ�
	void		SetRotation(const Matrix3f& rot);
	/// �_���s���O�W���̎擾
	float		GetDamping();
	/** @brief �_���s���O�W���̐ݒ�
		@param d �_���s���O�W��
		�}�E�X�{�^���������[�X������̉�]�̌����W����ݒ肷��D
		d��0.0���Ƒ����ɒ�~���Cd��1.0���Ɖ�]���Â���D
	 **/
	void		SetDamping(float d);
	/// ��]�̃��Z�b�g
	void		Reset();
};
struct FWRotationControlDesc : FWControlDesc{
	SPR_DESCDEF(FWRotationControl);

	Matrix3f	rot;
	float		damping;

	FWRotationControlDesc(){
		damping = 0.0f;
	}
};

/// ���s�ړ��R���g���[��
struct FWTranslationControlIf : FWControlIf{
	SPR_IFDEF(FWTranslationControl);

	/// �ψʂ̎擾
	Vec3f	GetTranslation();
	/// �ψʂ̐ݒ�
	void	SetTranslation(Vec3f p);
	/// �X�s�[�h�̎擾
	float	GetSpeed();
	/** @brief	�X�s�[�h�̐ݒ�
		@param	sp
		�}�E�X�ړ��ʂ���ψʂ̕ω��ʂɂ�����W����ݒ肷��
	 */
	void	SetSpeed(float sp);
};
struct FWTranslationControlDesc : FWControlDesc{
	SPR_DESCDEF(FWTranslationControl);

	enum{
		TRANSLATION_XY,
		TRANSLATION_X,
		TRANSLATION_Y,
		TRANSLATION_Z,
	};

	Vec3f	pos;
	float	speed;

	FWTranslationControlDesc(){
		style = TRANSLATION_XY;
		speed = 1.0f;
	}
};

/**
	�_�C�A���O�{�b�N�X(GUI�R���e�i)
 */
struct FWDialogIf : FWWinBaseIf{
	SPR_IFDEF(FWDialog);

	/** @brief GUI�A�C�e�����쐬
		@param	desc	�f�B�X�N���v�^
		@param	parent	�e�p�l��
		�쐬����GUI�A�C�e���̎�ނ̓f�B�X�N���v�^�̌^�Ŕ��肳���D
		���W�I�{�^�����쐬����ꍇ��parent�����W�I�O���[�v�łȂ���΂Ȃ�Ȃ��D
	 **/
	FWControlIf*		CreateControl(const IfInfo* ii, const FWControlDesc& desc, FWPanelIf* parent = 0);
	/** �ȉ��Z�k�`API **/
	/// �v�b�V���{�^��
	FWButtonIf*			CreatePushButton(UTString label, FWPanelIf* parent = 0);
	/// �`�F�b�N�{�^��
	FWButtonIf*			CreateCheckButton(UTString label, bool checked = false, FWPanelIf* parent = 0);
	/// ���W�I�{�^��
	FWButtonIf*			CreateRadioButton(UTString label, FWPanelIf* parent = 0);
	/// �ÓI�e�L�X�g
	FWStaticTextIf*		CreateStaticText(UTString text, FWPanelIf* parent = 0);
	/// �e�L�X�g�{�b�N�X
	FWTextBoxIf*		CreateTextBox(UTString label, UTString text = "", int style = FWTextBoxDesc::TEXT, FWPanelIf* parent = 0);
	/// �p�l��
	FWPanelIf*			CreatePanel(UTString label, int style = FWPanelDesc::RAISED, FWPanelIf* parent = 0);
	/// ���W�I�{�^���O���[�v
	FWPanelIf*			CreateRadioGroup(FWPanelIf* parent = 0);
	/// ��]�R���g���[��
	FWRotationControlIf*	CreateRotationControl(UTString label, FWPanelIf* parent = 0);
	/// ���i�R���g���[��
	FWTranslationControlIf*	CreateTranslationControl(UTString label, int style = FWTranslationControlDesc::TRANSLATION_XY, FWPanelIf* parent = 0);
	/// ���X�g�{�b�N�X
	FWListBoxIf*			CreateListBox(UTString label, FWPanelIf* parent = 0);
	/// �V��������쐬
	void	CreateColumn(bool sep = true, FWPanelIf* parent = 0);
	/// �����Z�p���[�^���쐬
	void	CreateSeparator(FWPanelIf* parent = 0);
	
};
struct FWDialogDesc : FWWinBaseDesc{
	SPR_DESCDEF(FWDialog);

	enum{
		DOCK_LEFT,
		DOCK_RIGHT,
		DOCK_TOP,
		DOCK_BOTTOM,
	};

	/// �h�b�L���O�^�C�v���ǂ���
	bool	dock;

	/// �h�b�L���O�^�C�v�̏ꍇ�̈ʒu
	int		dockPos;

	FWDialogDesc(){
		dock	= false;
		dockPos = DOCK_TOP;
	}

};

/**	@brief �E�B���h�E
	window ID, scene, render�����D
 */
struct FWWinIf : FWWinBaseIf{
	SPR_IFDEF(FWWin);

	/// �t���X�N���[���ɂ���
	void	SetFullScreen();
	/// �t���X�N���[�����ǂ���
	bool	GetFullScreen();

	/// �o�^����Ă��郌���_�����擾����
	GRRenderIf*	GetRender();
	/// �����_����o�^����
	void		SetRender(GRRenderIf* data);
	/// �o�^����Ă���V�[�����擾����
	FWSceneIf*  GetScene();
	/// �V�[����o�^����
	void		SetScene(FWSceneIf* s);

	/// �L�[�{�[�h�E�}�E�X�f�o�C�X
	DVKeyMouseIf*	GetKeyMouse();
	void			SetKeyMouse(DVKeyMouseIf* dv);
	/// �W���C�X�e�B�b�N�f�o�C�X
	DVJoyStickIf*	GetJoyStick();
	void			SetJoyStick(DVJoyStickIf* dv);
	/// �g���b�N�{�[��
	HITrackballIf*	GetTrackball();
	void			SetTrackball(HITrackballIf* dv);
	/// �h���b�K�[
	//HIDraggerIf*	GetDragger();
	//void			SetDragger(HIDraggerIf* dv);

	/// �`�惂�[�h�ݒ�
	void	SetDebugMode(bool ph_or_gr);
	bool	GetDebugMode();
	/// �`��
	void	Display();
	
	/** @brief �_�C�A���O�̍쐬
		@param desc	�f�B�X�N���v�^
		���̃E�B���h�E�̎q�Ƃ��ă_�C�A���O���쐬����
	 **/
	FWDialogIf*		CreateDialog(const FWDialogDesc& desc = FWDialogDesc());

	/** @brief	�r���[�|�[�g�̌v�Z
		@param	left
		@param	top
		@param	width
		@param	height
		�h�b�L���O�^�C�v�̃_�C�A���O������ꍇ�C������������c��̗̈���v�Z����D
		�J�����g�E�B���h�E�ł���K�v������
	 */
	void	CalcViewport(int& left, int& top, int& width, int& height);
};
struct FWWinDesc : FWWinBaseDesc{
	SPR_DESCDEF(FWWin);

	bool		debugMode;		///<	�f�o�b�O�`�惂�[�h��
	bool		fullscreen;		///<	�t���X�N���[���ɂ��邩�ǂ���
	bool		useKeyMouse;	///<	�L�[�{�[�h�E�}�E�X�f�o�C�X���g�p���邩
	bool		useJoyStick;	///<	�W���C�X�e�B�b�N�f�o�C�X���g�p���邩
	int			joyStickPollInterval;
	bool		useTrackball;	///<	�g���b�N�{�[���C���^�t�F�[�X���g�p���邩
	//bool		useDragger;		///<	�h���b�K�[�C���^�t�F�[�X���g�p���邩

	FWWinDesc(){
		debugMode		= true;
		fullscreen		= false;
		useKeyMouse		= true;
		useJoyStick		= false;
		joyStickPollInterval = 10;
		useTrackball	= true;
		//useDragger		= true;
	}
};

}
#endif 
