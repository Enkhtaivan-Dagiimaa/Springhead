/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Foundation/UTTimer.h>

#ifdef _WIN32
# include <windows.h>
# include <mmsystem.h>
#endif

namespace Spr {;

//----------------------------------------------------------------------------------------------------------
//	UTTimerStub		UTTimer�̃C���X�^���X��ێ�����V���O���g��
//
class UTTimerStub{
	///< �}���`���f�B�A�^�C�}�̕���\
	unsigned int resolution;		///< ���݂̐ݒ�l
	unsigned int resolutionMin;		///< �V�X�e�����T�|�[�g����ŏ��l
	unsigned int resolutionMax;		///< �V�X�e�����T�|�[�g����ő�l
public:
	typedef std::vector<UTTimerProvider*> Providers;
	typedef std::vector< UTRef<UTTimer> > Timers;

	Providers	providers;		///< �v���o�C�_�̔z��
	Timers		timers;			///< �^�C�}�̔z��

	UTTimerStub(): resolution(0), resolutionMin(0), resolutionMax(0){}
	~UTTimerStub(){
		for(unsigned i=0; i<timers.size(); ++i) timers[i]->Stop();
	}
public:
	/// �B��̃C���X�^���X���擾
	static UTTimerStub& UTTimerStub::Get(){
		static UTTimerStub obj;
		return obj;
	}
	/// �^�C�}�̓o�^
	void AddTimer(UTTimer* timer){
		timer->timerId = (unsigned int)timers.size();
		timers.push_back(timer);
	}
	/// �}���`���f�B�A�^�C�}�̍ŏ�����\
	unsigned int ResolutionMin(){
		if (!resolutionMin) GetCaps();
		return resolutionMin;
	}
	/// �}���`���f�B�A�^�C�}�̍ő啪��\
	unsigned int ResolutionMax(){
		if (!resolutionMax) GetCaps();
		return resolutionMax;
	}
	/** @brief ����\�̍Đݒ�
	 */
	void UpdateResolution(){
#if defined(_WIN32)
		int resOld = resolution;
		resolution = UINT_MAX;
		for(Timers::iterator it = timers.begin(); it!=timers.end(); ++it){
			if ((*it)->IsStarted() && (*it)->GetMode() == UTTimerIf::MULTIMEDIA && (*it)->GetResolution() < resolution) 
				resolution = (*it)->GetResolution();
		}
		if (resolution == UINT_MAX) resolution = 0;

		if (resOld == resolution) return;
		if (resOld) timeEndPeriod(resOld);
		if (resolution) timeBeginPeriod(resolution);
#else
# error UTMMTimerStub: Not yet implemented.	//	������
#endif
	}

protected:
	void GetCaps(){
#ifdef _WIN32
		TIMECAPS tc;
		if (timeGetDevCaps(&tc, sizeof(TIMECAPS)) != TIMERR_NOERROR) {
			DSTR << "UTTimer::BeginPeriod()  Fail to get resolution." << std::endl;
		}
		resolutionMin = tc.wPeriodMin;
		resolutionMax = tc.wPeriodMax;
#else
# error UTMMTimerStub: Not yet implemented.	//	������
#endif
	}
};

//----------------------------------------------------------------------------------------------------------
//	UTTimerProvider
//

UTTimerProvider::UTTimerProvider(){
}

UTTimerProvider::~UTTimerProvider(){
	Unregister();
}

void UTTimerProvider::Register(){
	UTTimerStub& stub = UTTimerStub::Get();
	stub.providers.push_back(this);
}

void UTTimerProvider::Unregister(){
	UTTimerStub& stub = UTTimerStub::Get();
	// ���̃v���o�C�_�𗘗p���Ă���^�C�}���~����
	for(unsigned i=0; i<stub.timers.size(); ++i){
		UTTimer* t = stub.timers[i];
		if (t->IsStarted() && t->GetMode() == UTTimerIf::FRAMEWORK && t->provider == this){
			t->Stop();
		}
	}
	for(unsigned i=0; i<stub.providers.size(); ++i){
		UTTimerProvider* p = stub.providers[i];
		if (p == this){
			stub.providers.erase(stub.providers.begin()+i);
			break;
		}
	}
}

void UTTimerProvider::CallIdle(){
	UTTimerStub& stub = UTTimerStub::Get();
	for(UTTimerStub::Timers::iterator it = stub.timers.begin(); it != stub.timers.end(); ++it){
		if ((*it)->GetMode() == UTTimerIf::IDLE && (*it)->IsStarted()) (*it)->Call();
	}
}


//----------------------------------------------------------------------------------------------------------
//	UTTimer
//

UTTimer::UTTimer(){
	bStarted	= false;
	bRunning	= false;
	bStopThread = false;
	mode		= UTTimerIf::FRAMEWORK;
	interval	= 1;
	resolution	= 1;
	func		= 0;
	timerIdImpl = 0;
	provider	= 0;

	// ���g��Stub�ɓo�^
	UTTimerStub::Get().AddTimer(this);
	
}

UTTimer::~UTTimer(){
	Stop();
	UTTimerStub::Timers& timers = UTTimerStub::Get().timers;
	//	������AddRef���Ă����Ȃ��ƁA timers.erase()�����u�Ԃ�~UTTimer()�ɍē����Ă��܂��B
	//	new���Ă��Ȃ�UTTimer�̏ꍇ�A�ŏ�����0�Ȃ̂�2�ɂ��Ă����Ȃ��ƍē����Ă��܂��B
	AddRef(); AddRef();	
	for(unsigned int i=0; i<timers.size(); ++i){
		if (timers[i] == this){
			timers.erase(timers.begin()+i);
			break;
		}
	}
	//	�ǂ�������������̂ňӖ��Ȃ����ǁA���̌�̏����ō���悤�ɏ����Ȃ邩������Ȃ��̂ňꉞDelRef()�Ă�ǂ��B
	//	���Ƃ���0�̏ꍇ�́A2��DelRef()���ĂԂ�-1�ɂȂ��Ă��܂��̂� RefCount()���`�F�b�N����B
	DelRef(); 
	if (RefCount()>0) DelRef(); 
}

unsigned SPR_CDECL UTTimerIf::NTimers(){
	return (unsigned)UTTimerStub::Get().timers.size();
}

UTTimerIf* SPR_CDECL UTTimerIf::Get(unsigned i){
	if(0 <= i && i < NTimers())
		return DCAST(UTTimerIf, UTTimerStub::Get().timers[i]);
	return 0;
}

UTTimerIf* SPR_CDECL UTTimerIf::Create(){
	UTTimer* timer = DBG_NEW UTTimer();
	
	return DCAST(UTTimerIf, timer);
}

bool UTTimer::SetMode(UTTimerIf::Mode m){
	if (mode == m)
		return true;
	
	bool started = IsStarted();
	if (started && !Stop())
		return false;

	mode = m;
	return Start();
}

#if defined _WIN32
void (__stdcall UTTimer_MMTimerCallback)(unsigned uID, unsigned, ulong_ptr dwUser, ulong_ptr, ulong_ptr){
	UTTimerStub::Get().timers[(int)dwUser]->Call();
}

unsigned long SPR_STDCALL UTTimer_ThreadCallback(void* arg){
	UTTimer* timer = UTTimerStub::Get().timers[(int)arg];
	unsigned long lastCall = timeGetTime();
	
	while(!timer->bStopThread){
		unsigned long now = timeGetTime();
		unsigned long nextCall = lastCall + timer->GetInterval();
		int delta = (int)nextCall - (int)now;
		if (delta > 0){
			Sleep(delta);
			now = timeGetTime();
		}
 		lastCall = now;
		timer->Call();
	}
	timer->bStopThread = false;
	return 0;
}
#else	//	Windows�ȊO�̃v���b�g�t�H�[���ł̎���
# error UTTimer: Not yet implemented.		//	������
#endif


bool UTTimer::Start(){
	if (bStarted)
		return true;
	
	UTTimerStub& stub =  UTTimerStub::Get();
	if (mode == UTTimerIf::MULTIMEDIA){
#if defined _WIN32
		bStarted = true;
		stub.UpdateResolution();
		timerIdImpl = timeSetEvent(interval, resolution, UTTimer_MMTimerCallback , timerId, TIME_PERIODIC);
		if (!timerIdImpl){
			bStarted = false;
			stub.UpdateResolution();
		}
#endif
	}
	else if(mode == UTTimerIf::THREAD){
#if defined _WIN32
		unsigned long id=0;
		timerIdImpl = (unsigned int)CreateThread(NULL, 0x1000, UTTimer_ThreadCallback, (void*)timerId, 0, &id);
		if (timerIdImpl){
			SetThreadPriority((HANDLE)timerIdImpl, THREAD_PRIORITY_TIME_CRITICAL);//THREAD_PRIORITY_ABOVE_NORMAL);
			bStarted = true;
		}
#endif
	}
	else if (mode == UTTimerIf::FRAMEWORK){
		bStarted = true;
		for(UTTimerStub::Providers::iterator it = stub.providers.begin(); it != stub.providers.end(); ++it){
			if (!*it) continue;
			if ((*it)->StartTimer(this)){
				provider = *it;
				return bStarted;
			}
		}
		bStarted = false;
	}
	else if (mode == UTTimerIf::IDLE){
		bStarted = true;
	}
	return bStarted;
}

bool UTTimer::Stop(){
	if (!bStarted)
		return true;

	UTTimerStub& stub =  UTTimerStub::Get();
	
	if (mode == UTTimerIf::MULTIMEDIA){
#ifdef _WIN32
		timeKillEvent(timerIdImpl);
		for(int i=0; bRunning && i<100; i++) Sleep(10); 
		if (bRunning)
			DSTR << "UTTimer MULTIMEDIA mode: Can not stop the timer callback. There may be a dead lock problem." << std::endl;
		timerIdImpl = 0;
		bStarted = false;
		stub.UpdateResolution();
#endif
	}
	else if (mode == UTTimerIf::THREAD){
#ifdef _WIN32
		bStopThread = true;									//	�X���b�h�̒�~���w��
		for(int t=0; t<100 && bStopThread; t++) Sleep(20);	//	��~����܂ő҂��Ă݂�
		if (bStopThread)
			DSTR << "UTTimer THREAD mode: Can not stop the timer thread. There may be a dead lock problem." << std::endl;
		CloseHandle(*(HANDLE*)&timerIdImpl);
		timerIdImpl = 0;
		bStarted = false;
#endif
	}
	else if (mode == UTTimerIf::FRAMEWORK){
		if (provider && provider->StopTimer(this)){
			timerIdImpl = 0;
			provider = NULL;
			bStarted = false;
		}
		else{
			bStarted = true;
		}
	}
	else if (mode == UTTimerIf::IDLE){
		bStarted = false;
	}
	return !bStarted;
}

bool UTTimer::SetCallback(UTTimerIf::TimerFunc f, void* a){
	if (IsRunning() && !Stop())
		return false;
	func = f;
	arg = a;
	return Start();
}

bool UTTimer::SetInterval(unsigned int i){
	if (IsRunning() && !Stop())
		return false;
	interval = i;
	return Start();
}

bool UTTimer::SetResolution(unsigned r){
	r = min(r, UTTimerStub::Get().ResolutionMax());
	r = max(r, UTTimerStub::Get().ResolutionMin());
	resolution = r;
	if (bStarted && mode == UTTimerIf::MULTIMEDIA){
		UTTimerStub::Get().UpdateResolution();
	}
	return true;
}

void UTTimer::Call(){
	if (func && !bRunning){
		bRunning = true;
		(*func)(timerId, arg);
		bRunning = false;
	}
}

}	//	namespace Spr
