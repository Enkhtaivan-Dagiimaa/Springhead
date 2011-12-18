#include <Springhead.h>
#include <iostream>
#include <ostream>
#include <vector>
#include <conio.h>
#include <GL/glut.h>
#include <windows.h>
#include <Foundation/UTPreciseTimer.h>
#include <Foundation/UTQPTimer.h>

using namespace Spr;

unsigned int dt = 1;	// �J��Ԃ��Ԋums
int mt = 5000;			// ���莞��ms

UTPreciseTimer pTimer;	// ��s�P�ʂŌv���\�ȃ^�C�}
UTQPTimer qTimer;		// �}���`�X���b�h�Ή���
std::vector< Vec2d > time;

void CPSCounter(double intervalms, double periodms);
void SPR_CDECL CallBack(int id, void* arg){
	Vec2d sec;
#if 1
	// 1���[�v�Ԃ̌v���p
	sec.x = pTimer.Stop() * 1e-6;
	sec.y = qTimer.Stop() * 1e-6;
	time.push_back(sec);
	pTimer.Clear();
	qTimer.Clear();
	pTimer.Start();
	qTimer.Start();
#else
	// ����A���S���Y�����I������܂łɂ����鎞�Ԍv���p
	pTimer.Clear();
	qTimer.Clear();
	pTimer.Start();
	qTimer.Start();
	for(int i = 0; i < 10; i++)
		std::cout << "Springhead2!" << std::endl;
	sec.x = pTimer.Stop() * 1e-6;
	sec.y = qTimer.Stop() * 1e-6;
	time.push_back(sec);
#endif
	//CPSCounter(dt, 1000);
}

// �i�I�v�V�����jCPS��\������֐�
// intervalms:���z���荞�ݎ��� ms
// periodms:�v���Ԋu ms �i1000����1sec�Ԃ̕��ς�\���j
void CPSCounter(double intervalms, double periodms){
	static int cnt=0;
	static DWORD lastTick;
	cnt++;
	double interval = timeGetTime() - lastTick;
	if (interval > periodms) {
		double cps = cnt / interval * 1000.0 / intervalms;
		lastTick = timeGetTime();
		cnt = 0;
		std::cout << "cps : " << cps << std::endl;		// cycle per sec
		std::cout << "sec : " << 1 /cps << std::endl;	// sec
	}
}

int _cdecl main(int argc, char* argv[]){
	pTimer.Init();								/// �v���p�^�C�}�̏�����

	UTTimerIf* timer1;
	timer1 = UTTimerIf::Create();
	timer1->SetMode(UTTimerIf::MULTIMEDIA);
	timer1->SetResolution(1);					///	�Ăт�������\ms
	timer1->SetInterval(dt);					/// �Ăт����p�xms
	timer1->SetCallback(CallBack, NULL);		/// �Ăт����֐�
	timer1->Start();							/// �^�C�}�X�^�[�g
	std::cout << "Start the mearsurement." << std::endl;
	std::cout << "Pleas wait " << mt * 0.001 << " seconds." << std::endl; 
	Sleep(mt);								/// 5sec�Ԍv��
	timer1->Stop();							/// �^�C�}�X�g�b�v

	std::cout << "Saving the data to a file." << std::endl;
	/// �v���f�[�^��csv�ŏo��
	CSVOUT << "count" << "," << "Precise timer [s]" << "," << "QPTimer [s]" << std::endl;
	for(int i = 0; i < time.size(); i++){
		CSVOUT << i << "," << time[i].x << "," << time[i].y << std::endl;
	}
	std::cout << "Complete!" << std::endl;

	std::cout << "Press any key to exit." << std::endl;
	while(!_kbhit()){}

	return 0;
}