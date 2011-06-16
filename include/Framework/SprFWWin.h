#ifndef SPR_FW_WINDOW_H
#define SPR_FW_WINDOW_H

#include <Foundation/SprObject.h>

namespace Spr{;

struct GRRenderIf;
struct DVKeyMouseIf;
struct DVJoyStickIf;
struct HITrackballIf;
struct HIDraggerIf;
struct FWSceneIf;

struct FWWinDesc{
	int			width;			///<	��
	int			height;			///<	����
	int			left;			///<	���[�̍��W
	int			top;			///<	��[�̍��W
	int			parentWindow;	///<	�q�E�B���h�E�����ꍇ�́A�e��ID�A�����łȂ����0
	UTString	title;			///<	�E�B���h�E�̃^�C�g��������(�g�b�v���x���E�B���h�E�̂�)
	bool		fullscreen;		///<	�t���X�N���[���ɂ��邩�ǂ���

	bool		useKeyMouse;	///<	�L�[�{�[�h�E�}�E�X�f�o�C�X���g�p���邩
	bool		useJoyStick;	///<	�W���C�X�e�B�b�N�f�o�C�X���g�p���邩
	bool		useTrackball;	///<	�g���b�N�{�[���C���^�t�F�[�X���g�p���邩
	bool		useDragger;		///<	�h���b�K�[�C���^�t�F�[�X���g�p���邩

	FWWinDesc(int w=640, int h=480, int l=-1, int t=-1, int p=0, bool f=false):
		width(w), height(h), left(l), top(t), parentWindow(p), fullscreen(f){
		title			= "Springhead2";
		useKeyMouse		= true;
		useJoyStick		= false;
		useTrackball	= true;
		useDragger		= true;
	}
};

/**	@brief �E�B���h�E
	window ID, scene, render�����D
 */
struct FWWinIf : ObjectIf{
	SPR_IFDEF(FWWin);

	/// �ʒu��ύX����
	void	SetPosition(int left, int top);
	/// �T�C�Y��ύX����
	void	SetSize(int width, int height);
	/// �^�C�g����ύX����
	void	SetTitle(UTString title);
	/// �^�C�g�����擾����
	UTString GetTitle();
	/// �t���X�N���[���ɂ���
	void	SetFullScreen(bool full = true);
	/// �t���X�N���[�����ǂ���
	bool	GetFullScreen();
	/// �ʒu���擾����
	Vec2i	GetPosition();
	/// �����擾����
	int		GetWidth();
	/// �������擾����
	int		GetHeight();
	/// ID���擾����
	int		GetID();
	/// ID��ݒ肷��
	void	SetID(int newID);

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
	HIDraggerIf*	GetDragger();
	void			SetDragger(HIDraggerIf* dv);

	/// �`�惂�[�h�ݒ�
	void	SetDebugMode(bool ph_or_gr);
	bool	GetDebugMode();
	/// �`��
	void	Display();
	/// �T�C�Y�ύX���̏���
	void	Reshape(int w, int h);
};

}
#endif 
