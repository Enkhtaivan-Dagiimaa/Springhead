/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#pragma hdrstop
#pragma comment(lib, "winmm.lib")

#include "UTQPTimer.h"
#include <Base/BaseDebug.h>

#ifdef _WIN32
#include <Windows.h>
#include <mmsystem.h>

namespace Spr{;

//���g����ǂݏo��
UTQPTimer::UTQPTimer(): stopWatch(0), startFlag(false)
{
	QueryPerformanceFrequency( &freq );
}

//�ʕb�P�ʂő҂�
void UTQPTimer::WaitUS(int time)
{
	LARGE_INTEGER time1,time2;

	QueryPerformanceCounter( &time1);
	time2.QuadPart = time1.QuadPart + time * freq.QuadPart / 1000000;
	do{
		QueryPerformanceCounter( &time1);
	}while(time1.QuadPart < time2.QuadPart);
}

//�O��Ăяo����Ă���̎��Ԃ��ʕb�P�ʂŌv��
int  UTQPTimer::CountUS()
{	
	LARGE_INTEGER now;
	int retval;

	QueryPerformanceCounter( &now);
	
	retval =  (int)((now.QuadPart-lasttime.QuadPart)*1000000 / freq.QuadPart);
	lasttime.QuadPart = now.QuadPart;
	return retval;
}

//�w�胋�[�v���Ԃ��쐬
void UTQPTimer::CountAndWaitUS(int time)
{
	int elapsedtime;
	elapsedtime = CountUS();
	WaitUS(time - elapsedtime);
}


unsigned long UTQPTimer::Start(){
	CountUS();
	startFlag = true;
	return stopWatch;
}
unsigned long UTQPTimer::GetTime(){
	if (startFlag){
		stopWatch += CountUS();
	}
	return stopWatch;
}

unsigned long UTQPTimer::Stop(){
	if (startFlag){
		stopWatch += CountUS();
		startFlag = false;
	}
	return stopWatch;
}
unsigned long UTQPTimer::Clear(){
	unsigned long rv = stopWatch;
	stopWatch = 0;
	return rv;
}

}
#endif