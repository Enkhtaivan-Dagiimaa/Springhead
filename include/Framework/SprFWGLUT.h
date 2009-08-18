#ifndef FWGLUT_H
#define FWGLUT_H
#include<sstream>
#include<string>
#include<vector>
#include<Springhead.h>
#include"Framework/FWGraphicsAdaptee.h"
#include"Framework/SprFWApp.h"
#include"Framework/FWAppInfo.h"
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

class FWGLUT :public FWGraphicsAdaptee, public FWGLUTDesc{

protected:
	/** glutTimerfunc���Ǘ�����N���X */
	class FWGLUTTimer : public UTRefCount{
	public:
		static FWGLUTTimer* timerInstance;
		GTimerFunc* func;					//glutTimerFunc�ŌĂяo���֐�
		void SetTimerFunc(GTimerFunc* f);	//func��ݒ肷��
		void GetTimerFunc();				//func���擾����
	};
	typedef UTRef<FWGLUTTimer> UTRef_FWGLUTTimer;
	typedef vector<UTRef_FWGLUTTimer> FWGLUTTimers;
	FWGLUTTimers glutTimer;

	/** �R�[���o�b�N�֐�*/
	static FWGLUT* instance;
	static void SPR_CDECL GlutDisplayFunc();
	static void SPR_CDECL GlutReshapeFunc(int w, int h);
	static void SPR_CDECL GlutIdleFunc();
	static void SPR_CDECL GlutKeyboardFunc(unsigned char key, int x, int y);
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
	/// �^�C�}�[��ݒ肷��
	virtual void Timer(int i);
	/// �^�C�}�[���쐬����
	virtual void AddTimer();
	/// �Ō�ɍ쐬�����^�C�}�[�̔ԍ���Ԃ�
	virtual int GetTimerNo();
	/// i�Ԗڂ̃^�C�}�[��Timerfunc��ݒ肷��
	virtual void SetTimerFunc(GTimerFunc* f ,int i=0);
	/// mainloop���Ă�
	virtual void Loop(int i,double teimeStep);
	///GLUT�ɂ��Timer���X�^�[�g����
	virtual void TimerStart();
	/// �^�C�}�[���擾����
	virtual FWGLUTTimer* GetTimer(int i);

	/** �E�B���h�E */
	///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
	virtual FWWin* CreateWin(const FWWinDesc& d=FWWinDesc());
	///	�E�B���h�E��j������
	virtual void DestroyWin(FWWin* w);
	///	�J�����g�E�B���h�E��ݒ肷��
	virtual void SetCurrentWin(FWWin* w);
	///	�J�����g�E�B���h�E��Ԃ��B
	virtual FWWin* GetCurrentWin();	
	///�J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
	virtual void PostRedisplay();
	/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
	virtual int Modifiers();

};

}





#endif