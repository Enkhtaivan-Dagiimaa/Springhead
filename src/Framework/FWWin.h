#ifndef FW_WINDOW_H
#define FW_WINDOW_H

#include <Framework/SprFWWin.h>
#include <Foundation/Object.h>
#include <HumanInterface/SprHITrackball.h>
#include <HumanInterface/SprHIDragger.h>

namespace Spr{;

struct FWSceneIf;

class FWWin : public Object, public FWWinDesc, public HITrackballCallback, public HIDraggerCallback{
public:
	SPR_OBJECTDEF(FWWin);

	int					id;				///< �E�B���h�EID
	bool				debugMode;		///< �f�o�b�O�`�惂�[�h
	
	GRRenderIf*			render;			///< �����_��
	FWSceneIf*			scene;			///< �V�[��

	DVKeyMouseIf*		keyMouse;		///< �L�[�{�[�h�E�}�E�X
	DVJoyStickIf*		joyStick;		///< �W���C�X�e�B�b�N
	HITrackballIf*		trackball;		///< �g���b�N�{�[��
	HIDraggerIf*		dragger;		///< �h���b�K�[

public:
	// �����ύXAPI�i�h���N���X�������j
	virtual void SetPosition(int left, int top){}
	virtual void SetSize(int width, int height){}
	virtual void SetTitle(UTString title){}
	// �����擾API
	UTString	GetTitle(){ return title; }
	void		SetFullScreen(bool full = true){ fullscreen = full;}
	bool		GetFullScreen(){ return fullscreen; }
	Vec2i		GetPosition(){ return Vec2i(left, top); }
	int			GetWidth(){ return width; }
	int			GetHeight(){ return height; }
	int			GetID() const{ return id; }
	void		SetID(int newID){id = newID;}
	// �����_���ƃV�[���̊����ĂƎ擾
	GRRenderIf*	GetRender(){ return render; }
	void		SetRender(GRRenderIf* data){render = data;}
	FWSceneIf*  GetScene(){ return scene; }
	void		SetScene(FWSceneIf* s){ scene = s; }
	// HumanInterface�f�o�C�X�̓o�^�Ǝ擾
	DVKeyMouseIf*	GetKeyMouse(){ return keyMouse; }
	void			SetKeyMouse(DVKeyMouseIf* dv){ keyMouse = dv; }
	DVJoyStickIf*	GetJoyStick(){ return joyStick; }
	void			SetJoyStick(DVJoyStickIf* dv){ joyStick = dv; }
	HITrackballIf*	GetTrackball(){ return trackball; }
	void			SetTrackball(HITrackballIf* dv){
		trackball = dv;
		trackball->SetKeyMouse(keyMouse);
		trackball->SetCallback(this);
	}
	HIDraggerIf*	GetDragger(){ return dragger; }
	void			SetDragger(HIDraggerIf* dv){
		dragger = dv;
		dragger->SetKeyMouse(keyMouse);
		dragger->SetCallback(this);
	}
	// �`�惂�[�h�̐ݒ�Ǝ擾
	void	SetDebugMode(bool ph_or_gr){ debugMode = ph_or_gr; }
	bool	GetDebugMode(){ return debugMode; }
	// �`��
	void	Display();
	// �T�C�Y�ύX������
	void	Reshape(int w, int h);

	// HITrackballCallback�̊֐�
	virtual void OnUpdatePose(HITrackballIf*, bool dir);

	FWWin(int wid = 0, const FWWinDesc& desc = FWWinDesc());
	virtual ~FWWin(){}
};

}
#endif 