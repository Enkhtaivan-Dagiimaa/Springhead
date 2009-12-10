#include <iostream>
#include <conio.h>
#include <GL/glut.h>
#include "Foundation/UTQPTimer.h"
#include <windows.h>

//���{�搶�̃\�[�X�R�[�h

using namespace std;
int _cdecl main(int argc, char* argv[]){
	Spr::UTQPTimer myTimer;
	int i,t;
	
	/*�^�C�}�[�̎��g�����o�́D�ꉞ�̕���\�ƂȂ�*/
	cout<<"�^�C�}�[�̎��g��"<<myTimer.Freq()/1000000<<"MHz"<<endl;

	/*************************************************************/
	cout<<" [��1] ���ԑ҂��̂��߂̎g����"<<endl;
	/*************************************************************/
	cout<<"3�b�҂��܂�"<<endl;
	myTimer.WaitUS(3000000);
	cout<<"3�b�o��"<<endl;
	myTimer.Clear();
	Sleep(1000);

	/*************************************************************/
	cout<<" [��2] 0.1�b�̃C���^�[�o���^�C�}�[�̎g�����i��ɐ���p�j"<<endl;
	/*************************************************************/
	myTimer.CountUS();	//(0)CountUS�֐����ĂԂ��Ƃŏ�����
	for(i=0;i<10;i++){
		Sleep(50);		//(1)����p�̌v�Z��͋[����50ms��Sleep
		myTimer.CountAndWaitUS(100000);//(2)�O��CountUS�܂���CountAndWaitUS���Ă΂�Ă���0.1�b�o�߂���܂ŃA�N�e�B�u�E�G�C�g
		cout<<"0.1�b�o�߁D"<<endl;
	}
	myTimer.Clear();
	Sleep(1000);

	/*************************************************************/
	cout<<" [��3] �����L�^�̂��߂̎g����"<<endl;
	/*************************************************************/
	myTimer.Start();
	while(1){
		double time = myTimer.GetTime(); //us�P��
		Sleep(10);
		std::cout<<time/1000<<"ms"<<std::endl;
		if(time >1000000){
			myTimer.Stop();
			break;
		}
	}

	std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;
	while(!_kbhit()){}
	return 0;
}