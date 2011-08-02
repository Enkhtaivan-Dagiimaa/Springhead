#ifndef UTTimer_H
#define UTTimer_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Base/Env.h>
#include <Foundation/Object.h>
#include <Foundation/SprUTTimer.h>

namespace Spr {

class UTTimerProvider;


/**	�����^�C�}�[
	�E�i�댯��������Ȃ����j1ms ���x�̐��x�����}���`���f�B�A�^�C�}�[
	�E�X���b�h���g�p�����[���^�C�}�[
	�EGLUT�Ȃǂ̃t���[�����[�N�Ɉˑ��������b�Z�[�W�p�b�V���O�^�̃^�C�}�[
	�E�A�C�h������
	�Ȃǂ𓝍������^�C�}�[	*/

class SPR_DLL UTTimer : public Object{
public:
	SPR_OBJECTDEF(UTTimer);

protected:
	unsigned					interval;			///<	�^�C�}�[�Ăяo���Ԋu(ms)
	unsigned					resolution;			///<	�^�C�}�[�Ăяo���̉𑜓x(ms)
	UTTimerIf::TimerFunc		func;				///<	���Ԃ����邽�тɌĂ΂��R�[���o�b�N�֐��D
	void*						arg;				///<	�R�[���o�b�N�֐��ɓn������
	volatile UTTimerIf::Mode	mode;				///<	�^�C�}�[�̓��샂�[�h
	volatile bool				bStarted;			///<	�^�C�}�[��ON�i����I�ɃR�[���o�b�N���Ăяo����ԁj�Ȃ� true
	volatile bool				bRunning;			///<	�R�[���o�b�N�Ăяo�����Ȃ�� true

public:
	unsigned					timerId;			///<	UTTimer�̒ʂ��ԍ�
	unsigned					timerIdImpl;		///<	���[�h�ʂɎg����^�C�}ID�D�ʂ��ԍ��ł͂Ȃ�
	UTTimerProvider*			provider;			///<	�t���[�����[�N�^�C�}�[�������Ă���Ƃ��A�^�C�}�[��񋟂��Ă���t���[�����[�N���o���Ă������߂̃|�C���^
	volatile bool				bStopThread;		///<	�X���b�h���[�h�̃X���b�h�ɒ�~���w�����邽�߂̃t���O

public:
	UTTimer();
	~UTTimer();

	unsigned GetID(){return timerId; }

	unsigned GetResolution(){return resolution; }
	bool SetResolution(unsigned r);
	
	unsigned GetInterval(){ return interval;}
	bool SetInterval(unsigned i);
	
	bool SetCallback(UTTimerIf::TimerFunc f, void* arg=NULL);
	
	UTTimerIf::Mode GetMode() { return mode; }
	bool SetMode(UTTimerIf::Mode m);

	bool IsStarted(){return bStarted;}
	bool IsRunning(){return bRunning;}
	
	bool Start();
	bool Stop();
	void Call();

protected:
	friend class UTTimerProvider;
	friend unsigned long SPR_STDCALL UTTimer_ThreadCallback(void* arg);
	friend void SPR_STDCALL UTTimer_MMTimerCallback(unsigned int uID, unsigned int, ulong_ptr dwUser, ulong_ptr, ulong_ptr);
};

///	�t���[�����[�N�ɂ��^�C�}�[�����̂��߂̃R�[���o�b�N�N���X
class SPR_DLL UTTimerProvider{
public:
	UTTimerProvider();
	virtual ~UTTimerProvider();
	///	�t���[�����[�N��UTTimer�ɓo�^
	void Register();
	///	�t���[�����[�N��UTTiemr����o�^����
	void Unregister();
	
	///	�^�C�}�[�̐ݒ�
	virtual bool StartTimer(UTTimer* timer)=0;
	///	�^�C�}�[�̍폜
	virtual bool StopTimer(UTTimer* timer)=0;
	///	Idle�����̌Ăяo�����Bmode == IDLE�̃^�C�}�[�̃R�[���o�b�N����ʂ�Ăяo���B
	static void CallIdle();
};


}	//	namespace Spr
#endif
