#include <iostream>
#include <conio.h>
#include <GL/glut.h>						/// UTMMtimer.h���g���̂ɕK�v
#include "Foundation/UTMMTimer.h"

void CallBackHoge(void* arg){
	std::cout << "hogehoge" << std::endl;
}

class Fuga{
public:
	Spr::UTMMTimer timer2;
	static void CallBackFuga(void* arg){
		std::cout << "         " << "fugafuga" << std::endl;
	}
};

int _cdecl main(int argc, char* argv[]){
	Spr::UTMMTimer timer1;				/// �}���`���f�B�A�^�C�}�̐錾
	timer1.Resolution(1);					///	 �Ăт�������\
	timer1.Interval(1000);					/// �Ăт����p�x
	timer1.Set(CallBackHoge, NULL);	/// �Ăт����֐�

	Fuga fuga;									/// �N���XFuga
	fuga.timer2.Resolution(1);
	fuga.timer2.Interval(5000);
	fuga.timer2.Set(Fuga::CallBackFuga, NULL);	/// �Ăі߂��֐��͐ÓI�łȂ���΂Ȃ�Ȃ�

	timer1.Create();							/// �}���`���f�B�A�^�C�}�X�^�[�g
	fuga.timer2.Create();
	
	std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;
	while(!_kbhit()){}
	return 0;
}