#include <iostream>
#include <conio.h>
#include <GL/glut.h>						/// UTMMtimer.h���g���̂ɕK�v
#include "Foundation/UTMMTimer.h"

void CallBackHoge(void* arg){
	std::cout << "hogehoge" << std::endl;
}

void CallBackFuga(void* arg){
	std::cout << "         " << "fugafuga" << std::endl;
}

int _cdecl main(int argc, char* argv[]){
	Spr::UTMMTimer timer1;				/// �}���`���f�B�A�^�C�}�̐錾
	timer1.Resolution(1);					///	 �Ăт�������\
	timer1.Interval(1000);					/// �Ăт����p�x
	timer1.Set(CallBackHoge, NULL);	/// �Ăт����֐�

	Spr::UTMMTimer timer2;
	timer2.Resolution(1);
	timer2.Interval(5000);
	timer2.Set(CallBackFuga, NULL);

	timer1.Create();							/// �}���`���f�B�A�^�C�}�X�^�[�g
	timer2.Create();

	while(!_kbhit()){}
	return 0;
}