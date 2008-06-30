/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef SPR_FWAPP_H
#define SPR_FWAPP_H

#include <Framework/SprFWSdk.h>
#include <Framework/SprFWScene.h>
#include <map>

namespace Spr{;

/** Ruby�̃v���O�������FWApp���p�����Ă������̊֐����I�[�o���C�h����ۂɁC
�@�@�t���[�����[�N�����܂��I�[�o���C�h���ꂽ�֐���F�������s���邽�߂̃��J�j�Y��
 */
class FWVFuncBridge : public UTRefCount{
public:
	virtual void Link(void* pObj) = 0;
	virtual bool Display() = 0;
	virtual bool Reshape(int w, int h) = 0;
	virtual bool Keyboard(int key, int x, int y) = 0;
	virtual bool MouseButton(int button, int state, int x, int y) = 0;
	virtual bool MouseMove(int x, int y) = 0;
	virtual bool Step() = 0;
	virtual bool Idle() = 0;
	virtual bool Joystick(unsigned int buttonMask, int x, int y, int z) = 0;
	virtual void AtExit() = 0;
	virtual ~FWVFuncBridge(){}
};

//	hase	TypeDesc���ł��Ȃ��悤�ɃN���X�ɂ��Ă���BTypeDesc���ł̑Ή����]�܂����B
class FWWinDesc{
public:
	int width;			///<	��
	int height;			///<	����
	int left;			///<	���[�̍��W
	int top;			///<	��[�̍��W
	int parentWindow;	///<	�q�E�B���h�E�����ꍇ�́A�e��ID�A�����łȂ����0
	UTString title;		///<	�E�B���h�E�̃^�C�g��������(�g�b�v���x���E�B���h�E�̂�)
	bool fullscreen;	///<	�t���X�N���[���ɂ��邩�ǂ���

	FWWinDesc(int w=640, int h=480, int l=-1, int t=-1, int p=0, bool f=false):
		width(w), height(h), left(l), top(t), parentWindow(p), fullscreen(f){
	}
};

/**	@brief �E�B���h�E
	window ID, scene, render�����D
	���������̂�FWWinGlut�Ȃ̂ŁCFWAppGlut�ł̎g�p���O��D
 */
class FWWin : public UTRefCount, public FWWinDesc{
protected:
	int id;
public:	// protected�ł悢���݊����̂��߈ꎞ�I��
	UTRef<GRRenderIf>	render;
	UTRef<FWSceneIf>	scene;

	FWWin(int wid, GRRenderIf* r):id(wid), render(r){}
public:
	virtual void Position(int left, int top){}
	virtual void Reshape(int width, int height){}
	virtual void SetTitle(UTString title){}
	virtual UTString GetTitle(){ return UTString(); }
	virtual void SetFullScreen(bool full = true){}
	virtual bool GetFullScreen(){ return false; }
	virtual int  GetWidth(){ return 0; }
	virtual int  GetHeight(){ return 0; }

	int			GetID() const{ return id; }
	GRRenderIf*	GetRender(){ return render; }
	FWSceneIf*  GetScene(){ return scene; }
	void		SetScene(FWSceneIf* s){ scene = s; }

};
typedef FWWinDesc FWAppGLUTDesc;

/** @brief �A�v���P�[�V�����N���X
	Springhead�̃N���X�͊�{�I�Ɍp�������Ɏg�p����悤�ɐ݌v����Ă��邪�C
	FWApp����т��̔h���N���X�͗�O�ł���C���[�U��FWApp���邢�͂��̔h���N���X���p�����C
	���z�֐����I�[�o���C�h���邱�Ƃɂ���ēƎ��@�\����������D

	FWApp�N���X�̓O���t�B�b�N�X�̏������@�\�������Ȃ��̂ŁC
	�ʏ�͔h���N���X�ł���FWAppGL��FWAppGLUT���g�p����D
 */
class FWApp{
protected:
	/// �}�E�X���
	struct MouseInfo{
		TVec2<int> lastPos;			/// �O��̃J�[�\�����W
		bool left, middle, right;	/// �{�^����������
		bool shift, ctrl, alt;		/// �R���g���[���L�[���
		bool first;
		MouseInfo():left(false), middle(false), right(false), first(false){}
	} mouseInfo;
	/// �J�������
	/// �{����scene���Ɍʂɕێ����ׂ��B�v�C��
	struct CameraInfo{
		Vec3f target;		/// ���S�_
		Vec2f rot;			/// �o�x�p�C�ܓx�p
		float zoom;			/// �g�嗦�i�Ώۂ���̋����j
		Vec2f rotRangeX, rotRangeY;
		Vec2f zoomRange;
		Affinef view;
		CameraInfo():
			rot(Rad(0.0), Rad(20.0)), zoom(1.0f),
			rotRangeY(Rad(-180.0), Rad(180.0)), rotRangeX(Rad(-80.0), Rad(80.0)), zoomRange(0.01f, 100.0f){}
	} cameraInfo;

	/// ���̃h���b�O�@�\
	struct DragInfo{
		PHRayIf* ray;			/// �J�[�\�����̍��̂���肷�邽�߂�PHRay
		PHSolidIf* cursor;		/// �J�[�\������
		PHSpringIf* spring;
		float	depth;
		DragInfo():ray(NULL), cursor(NULL), spring(NULL), depth(0.0f){}
	};
	std::map<FWSceneIf*, DragInfo>	dragInfo;
	
	UTRef<FWSdkIf> fwSdk;
	typedef std::vector< UTRef<FWWin> > Wins;
	Wins wins;

	void AssignScene(FWWin* win);
	
	void CallDisplay(){
		if(!vfBridge || !vfBridge->Display())
			Display();
	}
	void CallReshape(int w, int h){
		if(!vfBridge || !vfBridge->Reshape(w, h))
			Reshape(w, h);
	}
	void CallKeyboard(int key, int x, int y){
		if(!vfBridge || !vfBridge->Keyboard(key, x, y))
			Keyboard(key, x, y);
	}
	void CallMouseButton(int button, int state, int x, int y){
		if(!vfBridge || !vfBridge->MouseButton(button, state, x, y))
			MouseButton(button, state, x, y);
	}
	void CallMouseMove(int x, int y){
		if(!vfBridge || !vfBridge->MouseMove(x, y))
			MouseMove(x, y);
	}
	void CallStep(){
		if(!vfBridge || !vfBridge->Step())
			Step();
	}
	void CallIdle(){
		if(!vfBridge || !vfBridge->Idle())
			Idle();
	}
	void CallJoystick(unsigned int buttonMask, int x, int y, int z){
		if(!vfBridge || !vfBridge->Joystick(buttonMask, x, y, z))
			Joystick(buttonMask, x, y, z);
	}

	/** @brief Ctrl, Shift, Alt�̏�Ԃ�Ԃ�
		�X�̔h���N���X�Ŏ��������
	 */
	virtual int GetModifier(){ return 0; }

public:
	// �ȉ��̒萔��GLUT�ɍ��킹�Ă���
	enum MouseButtonType{
		LEFT_BUTTON = 0,
		MIDDLE_BUTTON = 1,
		RIGHT_BUTTON = 2,
	};
	enum MouseButtonState{
		BUTTON_DOWN = 0,
		BUTTON_UP = 1,
	};
	enum ModifierMask{
		ACTIVE_SHIFT = 1,
		ACTIVE_CTRL = 2,
		ACTIVE_ALT = 4,
	};

	UTRef<FWVFuncBridge>	vfBridge;

	/** @brief SDK���擾����
	 */
	FWSdkIf*	GetSdk(){ return fwSdk; }

	/** @brief ������
		FWApp�I�u�W�F�N�g�̏��������s���D�ŏ��ɕK���ĂԁD
	 */
	virtual void Init(int argc = 0, char* argv[] = NULL);

	/** @brief �R�}���h���C�������̏���
		�A�v���P�[�V�����ɓn���ꂽ�R�}���h���C�������������������ꍇ�ɃI�[�o���C�h����
	 */
	virtual void ProcessArguments(int argc, char* argv[]){}

	/** @brief �E�B���h�E�ɑΉ�����R���e�L�X�g�����
		@param desc	�f�B�X�N���v�^
		�E�B���h�E���쐬����D�Ή����郌���_���͐V�����쐬����C
		�V�[���̓A�N�e�B�u�ȃV�[�����֘A������D
	 */
	virtual FWWin* CreateWin(const FWWinDesc& desc){ return NULL; }
	
	/** @brief �E�B���h�E���폜����
	 */
	virtual void DestroyWin(FWWin* win){}

	/** @brief �E�B���h�E�̐�
	 */
	virtual int NWin(){ return (int)wins.size(); }
	
	/**	@brief �E�B���h�E��ID����T��
	 */
	virtual FWWin* GetWinFromId(int wid);
 	
	/** @brief �E�B���h�E���擾����
		index�Ŏw�肳�ꂽ�E�B���h�E���擾����D
	 */
	virtual FWWin* GetWin(int index);

	/** @brief ���݂̃E�B���h�E���擾����
	 */
	virtual FWWin* GetCurrentWin(){ return NULL;}

	/** @brief ���݂̃E�B���h�E��ݒ肷��
	 */
	virtual void SetCurrentWin(FWWin* win){}

	/** @brief �V�~�����[�V�����̎��s
		�f�t�H���g�ł�FWSdk::Step���Ă΂��D
	 */
	virtual void Step();

	/** @brief idle�֐�
	 */
	virtual void Idle(){}

	/** @brief �V�[���̕`��
		�V�[�����\�������Ƃ��ɌĂ΂��D
		�`�揈�����J�X�^�}�C�Y�������ꍇ�ɃI�[�o���C�h����D
		�f�t�H���g�ł�FWSdk::Draw���Ă΂��D
	 */
	virtual void Display();

	/** @brief �`��̈�̃T�C�Y�ύX
		@param w �`��̈�̉���
		@param h �`��̈�̏c��
		���[�U�ɂ���ăE�B���h�E�T�C�Y���ύX���ꂽ�Ƃ��ȂǂɌĂ΂��D
		�f�t�H���g�ł�FWSdk::Reshape���Ă΂��D
	 */
	virtual void Reshape(int w, int h);

	/** @brief �L�[�{�[�h�C�x���g�̃n���h��
	 */
	virtual void Keyboard(int key, int x, int y){}

	/** @brief �}�E�X�C�x���g�̃n���h��
	 */
	virtual void MouseButton(int button, int state, int x, int y);

	/** @brief �}�E�X�C�x���g�̃n���h��
		�f�t�H���g�ł͍��h���b�O�Ŏ��_�ύX�C�E�h���b�O�ŃY�[���C��/�A�E�g
	 */
	virtual void MouseMove(int x, int y);

	/** @brif �W���C�X�e�B�b�N�̃n���h��
	*/
	virtual void Joystick(unsigned int buttonMask, int x, int y, int z){}

	virtual ~FWApp();

};

}
#endif
