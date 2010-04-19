#ifndef UTTimer_H
#define UTTimer_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Base/Env.h>

namespace Spr {

class UTTimerProvider;


/**	�����^�C�}�[
	�E�i�댯��������Ȃ����j1ms ���x�̐��x�����}���`���f�B�A�^�C�}�[
	�E�X���b�h���g�p�����[���^�C�}�[
	�EGLUT�Ȃǂ̃t���[�����[�N�Ɉˑ��������b�Z�[�W�p�b�V���O�^�̃^�C�}�[
	�E�A�C�h������
	�Ȃǂ𓝍������^�C�}�[	*/

class SPR_DLL UTTimer{
public:
	///	�^�C�}�[�̃��[�h
	enum Mode{
		MULTIMEDIA,
		THREAD,
		FRAMEWORK,
		IDLE
	};
	///	�R�[���o�b�N�֐��̌^
	typedef void TimerFunc(void* arg);

protected:
	unsigned interval;								///<	�^�C�}�[�Ăяo���Ԋu(ms)
	unsigned resolution;							///<	�^�C�}�[�Ăяo���̉𑜓x(ms)
	TimerFunc* func;								///<	���Ԃ����邽�тɌĂ΂��R�[���o�b�N�֐��D
	void* arg;										///<	�R�[���o�b�N�֐��ɓn������
	volatile Mode mode;								///<	�^�C�}�[�̓��샂�[�h
	volatile bool bStarted;							///<	�^�C�}�[��ON�i����I�ɃR�[���o�b�N���Ăяo����ԁj�Ȃ� true
	volatile bool bRunning;							///<	�R�[���o�b�N�Ăяo�����Ȃ�� true
	volatile int heavy;								///<	���������t���O�B���������Ԃ��I�[�o�[�����ꍇ�ɐ��l��������

	//	�ȉ��A�����p��Ɨ̈�
public:
	unsigned timerId;								///<	�^�C�}�[�����ʂ��邽�߂�Id
	UTTimerProvider* provider;						///<	�t���[�����[�N�^�C�}�[�������Ă���Ƃ��A�^�C�}�[��񋟂��Ă���t���[�����[�N���o���Ă������߂̃|�C���^
	volatile bool bStopThread;						///<	�X���b�h���[�h�̃X���b�h�ɒ�~���w�����邽�߂̃t���O

public:
	UTTimer();										///<	�R���X�g���N�^
	~UTTimer();										///<	�f�X�g���N�^

	unsigned GetResolution(){return resolution; }	///<	MULTIMEDIA���̃^�C�}�[���x�̎擾
	bool SetResolution(unsigned r);					///<	MULTIMEDIA���̃^�C�}�[���x�̐ݒ�iMULTIMEDIA���T�|�[�g���Ȃ��ꍇ�A�ݒ肵���l���珑������邱�Ƃ�����B�j
	unsigned GetInterval(){ return interval;}		///<	�^�C�}�[�̎��ԊԊu(ms)
	bool SetInterval(unsigned i);					///<	�^�C�}�[�̎��ԊԊu(ms)�̐ݒ�
	bool SetCallback(TimerFunc* f, void* arg);		///<	�R�[���o�b�N�֐��̐ݒ�
	Mode GetMode() { return mode; }					///<	�^�C�}�[�̓��샂�[�h�̎擾
	bool SetMode(Mode m);							///<	�^�C�}�[�̓��샂�[�h�̐ݒ�
	bool IsStarted(){return bStarted;}				///<	�^�C�}�[��ON�i����I�ɃR�[���o�b�N���Ăяo����ԁj�Ȃ� true
	bool IsRunning(){return bRunning;}				///<	�R�[���o�b�N�Ăяo�����Ȃ�� true
	
	bool Start();									///<	�^�C�}�[����J�n
	bool Stop();									///<	�^�C�}�[�̒�~
	void Call();

protected:
	friend class UTTimerProvider;
	friend unsigned long SPR_STDCALL UTTimer_ThreadCallback(void* arg);
	friend void SPR_STDCALL UTTimer_MMTimerCallback(unsigned uID, unsigned, unsigned long dwUser, unsigned long, unsigned long);

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
	void CallIdle();
};


}	//	namespace Spr
#endif
