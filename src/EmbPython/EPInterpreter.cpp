
#include "..\..\include\EmbPython\SprEPInterpreter.h"

using namespace Spr;

//�}���`�X���b�h�p
#include <process.h>

#ifdef _WIN32
#define NOMINMAX //std::max�Ȃǂ��g���̂ɕK�v(windows.h�Ƌ������邩��)
#include <windows.h>
#endif

EPInterpreter* EPInterpreter::instance = 0;

EPInterpreter* EPInterpreter::Create()
{
	if( EPInterpreter::instance == 0)
		EPInterpreter::instance = new EPInterpreter();
	
	return EPInterpreter::instance;
}

EPInterpreter::EPInterpreter()
{
	this->State = EPState::UNSTARTED;
}

EPInterpreter::~EPInterpreter()
{
	this->State = EPState::STOP;
}

void EPInterpreter::Initialize()
{
	//PYTHONPATH��ύX����
	//SPRPYTHONPATH��SpringheadPython�Ŏg��LIB��PATH��ǉ����Ă���
	std::string newPath;
	char buff[1024];
	GetEnvironmentVariable("SPRPYTHONPATH",buff,1024);
	newPath.append(buff);
	newPath.append(";");
	buff[0] = '\0';
	GetEnvironmentVariable("PYTHONPATH", buff,1024);
	newPath.append(buff);
	
	SetEnvironmentVariable("PYTHONPATH", newPath.c_str());
	// SetEnvironmentVariable("PYTHONHOME", newPath.c_str());


	//�p�C�\��������
	Py_Initialize();

	////����N���X�̏�����
	initUtility();

	////���W���[���ǂݍ���
	initFoundation();
	initCollision();
	initPhysics();
	//initGraphics();
	//initCreature();
}

void EPInterpreter::Run(void* arg)
{
	if( this->State == EPState::UNSTARTED || this->State == EPState::STOP)
	{
		_beginthread(EPLoopLauncher, 0 , arg);
		this->State = EPState::RUN;
	}
}

void EPInterpreter::Stop()
{
	if( this->State != EPState::UNSTARTED)
		this->State = EPState::STOP;
}

bool EPInterpreter::BindInt(int i)
{
	return false;
}

void EPInterpreter::EPLoopLauncher(void* arg)
{
	EPInterpreter::instance->EPLoopInit(arg);
	while(EPInterpreter::instance->State == EPState::RUN)
	{
		EPInterpreter::instance->EPLoop(arg);
	}
}
