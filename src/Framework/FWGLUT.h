#ifndef FWGLUT_H
#define FWGLUT_H
#include <vector>
#include <GL/glut.h>
#include <Framework/SprFWGraphicsAdaptee.h>
#include <Framework/SprFWApp.h>

using namespace std;

namespace Spr{;

class FWWinGLUT : public FWWin{
public:
	FWWinGLUT(int wid, const FWWinDesc& d, GRRenderIf* r):
	  FWWin(wid, d, r){}
	void FullScreen();
	void Position(int left, int top);
	void Reshape(int width, int height);
	void SetTitle(UTString t);
};


class FWGLUTDesc{
public:
	FWGLUTDesc();
};

class FWGLUT : public FWGraphicsAdaptee, public FWGLUTDesc{
protected:
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
	
	/** �^�C�} */
	///GLUT�ɂ��IdleFunc���X�^�[�g����
	virtual void StartIdleFunc();
	///GLUT�ɂ��Timer���X�^�[�g����
	virtual void StartMainLoop();

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
	virtual int Modifiers();

};

}





#endif