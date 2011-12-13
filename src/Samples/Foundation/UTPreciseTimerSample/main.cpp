#include <Springhead.h>
#include <iostream>
#include <ostream>
#include <vector>
#include <conio.h>
#include <GL/glut.h>
#include <windows.h>
#include <Foundation/UTPreciseTimer.h>

using namespace Spr;

unsigned int dt = 1;	// ms
UTPreciseTimer pTimer;	// ��s�P�ʂŌv���\�ȃ^�C�}
std::vector< unsigned long > time;

void CPSCounter(double intervalms, double periodms);
void SPR_CDECL CallBack(int id, void* arg){
	time.push_back(pTimer.Stop());
	//DSTR << time.back() << std::endl;
	pTimer.Clear();
	pTimer.Start();
	//CPSCounter(dt, 1000);
	//Sleep(2000);
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
	timer1->Start();							/// �}���`���f�B�A�^�C�}�X�^�[�g

	std::cout << "Press any key to exit." << std::endl;

	while(!_kbhit()){}
	timer1->Stop();

	/// �v���f�[�^��xls�ŏo��
	std::ofstream ofs("UTPreciseTimerSample.xls");
	for(int i = 0; i < time.size(); i++){
		ofs << i << "\t" << time[i] << std::endl;
	}

	return 0;
}