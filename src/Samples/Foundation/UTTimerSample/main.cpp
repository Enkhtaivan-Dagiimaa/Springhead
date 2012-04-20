#include <Springhead.h>
#include "Foundation/UTTimer.h"

#include <iostream>
#include <map>
#include <conio.h>
#include <GL/glut.h>
#include <windows.h>
using namespace Spr;


class TimerProviderGL: public UTTimerProvider{
	struct Arg{
		UTTimer* timer;
		bool bStop;
		Arg(UTTimer* t): timer(t), bStop(false){}
	};
	typedef std::vector<Arg*> Args;
	Args args;
	static void SPR_CDECL Callback(int a){
		Arg* arg = (Arg*)a;
		if (!arg->bStop){
			arg->timer->Call();
			glutTimerFunc(arg->timer->GetInterval(), Callback, a);
		}else{
			delete arg;	//	�����܂� arg �� delete ����킯�ɍs���Ȃ��B
		}
	}
public:
	TimerProviderGL(){
		Register();		//	�^�C�}�[�Ƀt���[�����[�N��o�^
	}
	~TimerProviderGL(){
		Unregister();	//	�^�C�}�[����t���[�����[�N������
	}
	bool StartTimer(UTTimer* timer){
		args.push_back(new Arg(timer));
		glutTimerFunc(timer->GetInterval(), Callback, (int)(void*)args.back());
		return true;
	}
	bool StopTimer(UTTimer* timer){
		for(Args::iterator it = args.begin(); it!= args.end(); ++it){
			if ((*it)->timer == timer){
				(*it)->bStop = true;
				args.erase(it);	//	�����ŁA(*it)�� delete ���Ă��܂��ƁA�^�C�}�[�̍Ō�̌Ăяo���̎��ɁAarg �̐悪�����̂Ŏ��s���G���[�ɐ����Ă��܂��B
				return true;
			}
		}
		return false;
	}
};

void CallBackHoge(int id, void* arg){
	std::cout << "hogehoge:" << id << std::endl;
}

class Fuga{
public:
	UTTimer timer2;
	int count;
	Fuga(): count(0){}
	static void CallBackFuga(int id, void* arg){
		((Fuga*)arg)->Print();
	}
	void Print(){
		count ++;
		std::cout << "         " << "fugafuga:" << id << " - "  << count << std::endl;
	}
};

UTTimer timer1;				/// �^�C�}�̐錾
Fuga fuga;									/// �N���XFuga


TimerProviderGL providerGL;	//	�^�C�}�[�Ƀt���[�����[�N��o�^ 

void SPR_CDECL keyboard(unsigned char key, int x, int y);
void SPR_CDECL display();
void SPR_CDECL reshape(int w, int h);
void SPR_CDECL idle(){
	providerGL.CallIdle();
}

int _cdecl main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
								//	���Ƃ��΁AFWApp��GLUT�� �� providerGL �Ɠ����@�\����������Ηǂ�

	timer1.SetMode(UTTimerIf::MULTIMEDIA);
	timer1.SetResolution(500);					///	 �Ăт�������\
	timer1.SetInterval(100);					/// �Ăт����p�x
	timer1.SetCallback(CallBackHoge, NULL);	/// �Ăт����֐�

	fuga.timer2.SetMode(UTTimerIf::IDLE);
	fuga.timer2.SetResolution(500);
	fuga.timer2.SetInterval(500);
	fuga.timer2.SetCallback(Fuga::CallBackFuga, &fuga);	/// �Ăі߂��֐��͐ÓI�łȂ���΂Ȃ�Ȃ�

	timer1.Start();							/// �}���`���f�B�A�^�C�}�X�^�[�g
	fuga.timer2.Start();
	
	std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;

	glutCreateWindow("UTTimerSample");
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutIdleFunc(idle);

	glutMainLoop();
	return 0;
}

void SPR_CDECL keyboard(unsigned char key, int x, int y){
	timer1.Stop();
	fuga.timer2.Stop();
	exit(0);
}	

void SPR_CDECL display(){
}
void SPR_CDECL reshape(int w, int h){
}