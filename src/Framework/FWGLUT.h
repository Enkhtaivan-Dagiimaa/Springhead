#ifndef FWGLUT_H
#define FWGLUT_H
#include <vector>
#include <GL/glut.h>
#include <Foundation/UTTimer.h>
#include <Framework/SprFWGraphicsAdaptee.h>
#include <Framework/SprFWApp.h>

using namespace std;

namespace Spr{;

/**
	FWGraphicsAdaptee��GLUT�ɂ�����
 */

class FWWinGLUT : public FWWin{
public:
	FWWinGLUT(int wid, const FWWinDesc& d, GRRenderIf* r):
	  FWWin(wid, d, r){}
	virtual void FullScreen();
	virtual void Position(int left, int top);
	virtual void Reshape(int width, int height);
	virtual void SetTitle(UTString t);
};

class FWGLUTDesc{
public:
	FWGLUTDesc();
};

class FWGLUT : public FWGraphicsAdaptee, public UTTimerProvider, public FWGLUTDesc{
protected:
	bool	idleFuncFlag;	///< IdleFunc�̌Ăяo���Ɋւ���Flag	
	bool	timerRestart;	///< �^�C�}�R�[���o�b�N���Ń^�C�}�̍ēo�^�����邩

	/** �R�[���o�b�N�֐�*/
	static FWGLUT* instance;
	static void SPR_CDECL GlutDisplayFunc();
	static void SPR_CDECL GlutReshapeFunc(int w, int h);
	static void SPR_CDECL GlutTimerFunc(int id);
	static void SPR_CDECL GlutIdleFunc();
	static void SPR_CDECL GlutKeyboardFunc(unsigned char key, int x, int y);
	static void SPR_CDECL GlutSpecialFunc(int key, int x, int y);
	static void SPR_CDECL GlutMouseFunc(int button, int state, int x, int y);
	static void SPR_CDECL GlutMotionFunc(int x, int y);
	static void SPR_CDECL GlutJoystickFunc(unsigned int buttonMask, int x, int y, int z);
	static void SPR_CDECL AtExit();
public:	
	FWGLUT();
	~FWGLUT();

	///	GLUT�̏��������s���B�ŏ��ɂ�����ĂԕK�v������B
	virtual void Init(int argc, char* argv[]);

	/// UTTimerProvider�̎���
	virtual bool StartTimer(UTTimer* timer);
	virtual bool StopTimer(UTTimer* timer);
	
	virtual void StartMainLoop();
	virtual void EnableIdleFunc(bool on = true);
	virtual void EnterGameMode();
	virtual void LeaveGameMode();

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWin* CreateWin(const FWWinDesc& d=FWWinDesc());
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWin* w);
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWin* w);
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual FWWin* GetCurrentWin();	
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual int GetWinFromId();
	///�J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay();
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int GetModifiers();

};

}





#endif