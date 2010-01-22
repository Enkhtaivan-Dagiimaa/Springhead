#ifndef FWAPPINFO_H
#define FWAPPINFO_H
#include <Springhead.h>
#include <Framework/SprFWGraphicsAdaptee.h>
#include <Foundation/UTMMTimer.h>

namespace Spr{;

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

	FWWin(int wid, const FWWinDesc& d, GRRenderIf* r):FWWinDesc(d), id(wid), render(r){}
public:
	virtual void Position(int left, int top){}
	virtual void Reshape(int width, int height){}
	virtual void SetTitle(UTString title){}
	virtual UTString GetTitle(){ return title; }
	virtual void SetFullScreen(bool full = true){ fullscreen = full;}
	virtual bool GetFullScreen(){ return fullscreen; }
	virtual int  GetWidth(){ return width; }
	virtual int  GetHeight(){ return height; }

	int			GetID() const{ return id; }
	void		SetID(int newID){id = newID;}
	GRRenderIf*	GetRender(){ return render; }
	void		SetRender(GRRenderIf* data){render = data;}
	FWSceneIf*  GetScene(){ return scene; }
	void		SetScene(FWSceneIf* s){ scene = s; }

	virtual ~FWWin(){}
};
typedef std::vector< UTRef<FWWin> > Wins;

struct MouseInfo{
	TVec2<int> lastPos;			/// �O��̃J�[�\�����W
	bool left, middle, right;	/// �{�^����������
	bool shift, ctrl, alt;		/// �R���g���[���L�[���
	bool first;
	MouseInfo():left(false), middle(false), right(false), first(false){}
};

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

/// �J�������
/// �{����scene���Ɍʂɕێ����ׂ��B�v�C��
struct CameraInfo{
	Vec3f target;		/// ���S�_
	Vec2f rot;			/// �o�x�p�C�ܓx�p
	float zoom;			/// �g�嗦�i�Ώۂ���̋����j
	Vec2f rotRangeX, rotRangeY;
	Vec2f zoomRange;
	Affinef view;
	CameraInfo();
	void UpdateView();
};

/// ���̃h���b�O�@�\.
struct DragInfo{
	PHRayIf* ray;			/// �J�[�\�����̍��̂���肷�邽�߂�PHRay
	PHSolidIf* cursor;		/// �J�[�\������
	PHSpringIf* spring;
	float	depth;
	DragInfo():ray(NULL), cursor(NULL), spring(NULL), depth(0.0f){}
};

/// �^�C�}�D

class  FWTimer : public UTRefCount{
public:
	enum TimerType{
		GLUT = 0,
		MM = 1,
	};
	static void SPR_CDECL GLUTTimerFunc(int id);		///<	�^�C�}�[�R�[���o�b�N�֐�
	static void SPR_CDECL MultiMediaTimerFunc(int id);	///<	�^�C�}�[�R�[���o�b�N�֐�
	FWTimer(int id, TimerType t = GLUT);

private:
	UTMMTimer* mtimer;								///<�@	�}���`���f�B�A�^�C�}�[
	TimerType timerType;							///<	�^�C�}�[�̃^�C�v(GLUT�^�C�}�[�A�}���`���f�B�A�^�C�}�[)
	unsigned id;									///<	�^�C�}�[ID
	unsigned interval;								///<	�^�C�}�[�̎��ԊԊu[ms]
	unsigned resolution;							///<	�^�C�}�[�̕���\[ms](�}���`���f�B�A�^�C�}�[�̂݁j

	//void SetTimerType(TimerType t){timerType = t;}	///<	�^�C�}�[�̃^�C�v�̐ݒ�
	//TimerType GetTimerType(){return timerType;}		///<	�^�C�}�[�̃^�C�v�̎擾
public:
	int GetTimerId(){return id;}					///<	�^�C�}�[��id�擾
	void SetInterval(unsigned ms);					///<	�^�C�}�[�̎��ԊԊu�̐ݒ�
	unsigned GetInterval(){return interval;}		///<	�^�C�}�[�̎��ԊԊu�̎擾
	void SetResolution(unsigned r);					///<	�^�C�}�[�̕���\�̐ݒ�
	unsigned GetResolution(){return resolution;}	///<	�^�C�}�[�̕���\�̎擾
	void Create();								///<	�^�C�}�[�̍쐬
	void Clear();
	void Recreate();
	void Release();

};
}
#endif 