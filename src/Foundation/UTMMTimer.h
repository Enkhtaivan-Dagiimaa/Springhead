#ifndef UTMMTIMER_H
#define UTMMTIMER_H
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <Base/Env.h>
#include <iostream>
using namespace std; 

namespace Spr {

/**	�}���`���f�B�A�^�C�}�[.
	�^�C�}�[���쒆�Ƀf�o�b�K���g����OS��������̂ŁC�f�o�b�O�p��
	�X���b�h�ɂ��[�����샂�[�h��p�ӂ��Ă���D	*/
typedef void MMTimerFunc(void* arg);
typedef void  SPR_CDECL MMTimerIdFunc(int id);

class SPR_DLL UTMMTimer{
	static int count;
	int tick;
public:
	//typedef void MMTimerFunc(void* arg);

protected:
	static unsigned resolution;		///<	�^�C�}�[�̎��Ԑ��x
	unsigned interval;					///<	�^�C�}�[�̎��ԊԊu
	MMTimerFunc* func;				///<	���Ԃ����邽�тɌĂ΂��R�[���o�b�N�֐��D
	MMTimerIdFunc* idFunc;				///<	���Ԃ����邽�тɌĂ΂��R�[���o�b�N�֐��D
	void* arg;								///<	�R�[���o�b�N�֐��ɓn������
	int id;								///<	�R�[���o�b�N�֐��ɓn��int�^����
	unsigned timerID;
	volatile bool bCreated;				///<	�^�C�}�[�����삵�Ă��邩�ǂ���
	volatile bool bThread;				///<	�^�C�}�[���X���b�h�Ƃ��ċ[�����삵�Ă��邩�ǂ���
    volatile bool bRun;					///<	�R�[���o�b�N�֐������s�����ǂ���
	void* hThread;						///<	�[������p�X���b�h
	static void SPR_STDCALL TimerCallback(unsigned uID, unsigned, unsigned long dwUser, unsigned long, unsigned long);
	static unsigned long SPR_STDCALL ThreadCallback(void* arg);
	void BeginPeriod();
	void EndPeriod();

	public:
	volatile int heavy;

	UTMMTimer();							///<	�R���X�g���N�^
	~UTMMTimer();							///<	�f�X�g���N�^

	unsigned Resolution();					///<	�^�C�}�[�̎��Ԑ��x
	void Resolution(unsigned res);		///<	�^�C�}�[�̎��Ԑ��x�̐ݒ�
	unsigned Interval();						///<	�^�C�}�[�̎��ԊԊu
	void Interval(unsigned i);				///<	�^�C�}�[�̎��ԊԊu�̐ݒ�
	void Set(MMTimerFunc* f, void* arg);
	void Set(MMTimerIdFunc* f, int i);
													///<	�R�[���o�b�N�֐��̐ݒ�
	void Init(unsigned int r, unsigned int i, MMTimerFunc* f, void* arg);				///< ���Ԑ��x�C�Ԋu�C�R�[���o�b�N�֐�����x�Őݒ� 
	bool Create();								///<	�^�C�}�[����J�n
	bool Thread();							///<	�^�C�}�[�[������J�n
	bool IsCreated(){return bCreated;}
													///<	�^�C�}�[�����삵�Ă��邩�ǂ���
	bool IsThread(){return bThread;}
													///<	�^�C�}�[���[�����삵�Ă��邩�ǂ���
	void Release();							///<	�^�C�}�[�̒�~
};

}	//	namespace Spr
#endif
