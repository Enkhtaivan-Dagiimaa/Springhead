#ifndef FW_GRAPHICSADAPTEE_H
#define FW_GRAPHICSADAPTEE_H

/** FWGraphicsAdaptee
	�����n�Ɉˑ������E�B���h�E�̍쐬�E�Ǘ��@�\
	���@�\���l�����FWWindowManager�̕��������C������ tazz
 */

#include <Foundation/Object.h>

namespace Spr{;

class FWWinBase;
class FWWin;
class FWDialog;
class FWControl;
class FWPanel;
class FWButton;
class FWTextBox;
class FWRotationControl;
class FWTranslationControl;
class FWListBox;
struct GRDeviceIf;

class FWGraphicsAdaptee : public Object{
public:
	SPR_OBJECTDEF_ABST_NOIF(FWGraphicsAdaptee);

	static UTRef<FWGraphicsAdaptee>	instance;

	GRDeviceIf*		grDevice;		///< �O���t�B�N�X�f�o�C�X�ւ̎Q��
public:
	///	���������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc = 0, char** argv = NULL){}	
	/// ���C�����[�v���J�n
	virtual void StartMainLoop()=0;
	/// Idle callback�̗L����/������
	virtual void EnableIdleFunc(bool on = true)=0;
	/// �t���X�N���[�����[�h�ւ̐؂�ւ��DAPI���͎b��I��GLUT�ɏ���
	virtual void EnterGameMode()=0;
	virtual void LeaveGameMode()=0;

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWinIf* CreateWin(const FWWinDesc& d, FWWinIf* parent){ return NULL; }
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWinIf* w){}
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWinIf* w){}
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual int GetCurrentWin(){ return 0; }
	/// �J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay(){}
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int GetModifiers(){ return NULL; }
	/// �ĕ`�掞�̃r���[�|�[�g�v�Z
	virtual void CalcViewport(int& l, int& t, int& w, int& h){}

	/// �_�C�A���O�쐬
	virtual FWDialog*	CreateDialog(FWWin* owner, const FWDialogDesc& desc){ return 0; }
	/// GUI�A�C�e���쐬
	virtual FWControl*	CreateControl(FWDialog* owner, const IfInfo* ii, const FWControlDesc& desc, FWPanel* parent){ return 0; }
	virtual void	CreateColumn	 (FWDialog* owner, bool sep, FWPanel* panel){}
	virtual void	CreateSeparator	 (FWDialog* owner, FWPanel* panel){}
	/// ��������
	virtual Vec2i	GetPosition		(FWWinBase* win){ return Vec2i(); }
	virtual void	SetPosition		(FWWinBase* win, int left, int top){}
	virtual Vec2i	GetSize			(FWWinBase* win){ return Vec2i(); }
	virtual void	SetSize			(FWWinBase* win, int width, int height){}
	virtual void	SetTitle		(FWWinBase* win, UTString title){}
	virtual void	SetFullScreen	(FWWin* win){}
	virtual void	SetLabel		(FWControl* ctrl, UTString label){}
	virtual void	SetAlign		(FWControl* ctrl, int align){}
	virtual void	SetStyle		(FWControl* ctrl, int style){}
	virtual int		GetInt			(FWControl* ctrl){ return 0; }
	virtual void	SetInt			(FWControl* ctrl, int val){}
	virtual float	GetFloat		(FWControl* ctrl){ return 0.0f; }
	virtual void	SetFloat		(FWControl* ctrl, float val){}
	virtual void	SetChecked		(FWButton*	btn,  bool on){}
	virtual bool	IsChecked		(FWButton*	btn){ return false; }
	virtual void	SetIntRange		(FWTextBox* text, int rmin, int rmax){}
	virtual void	SetFloatRange	(FWTextBox* text, float rmin, float rmax){}
	virtual const char*	GetString	(FWTextBox* text){ return 0; }
	virtual void	SetString		(FWTextBox* text, char* str){}
	virtual Matrix3f GetRotation	(FWRotationControl* rotCtrl){ return Matrix3f(); }
	virtual void	SetRotation		(FWRotationControl* rotCtrl, const Matrix3f& rot){}
	virtual void	SetDamping		(FWRotationControl* rotCtrl, float d){}
	virtual void	Reset			(FWRotationControl* rotCtrl){}
	virtual	Vec3f	GetTranslation	(FWTranslationControl* trnCtrl){ return Vec3f(); }
	virtual	void	SetTranslation	(FWTranslationControl* trnCtrl, Vec3f p){}
	virtual void	SetSpeed		(FWTranslationControl* trnCtrl, float sp){}
	virtual void	AddItem			(FWListBox*	listBox, UTString label){}
	
	/// �O���t�B�N�X�f�o�C�X�̎擾
	GRDeviceIf* GetGRDevice(){ return grDevice; }

};

}

#endif