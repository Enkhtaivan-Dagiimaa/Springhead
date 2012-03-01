
#include "..\..\include\EmbPython\SprEPInterpreter.h"

using namespace Spr;

//�}���`�X���b�h�p
#include <process.h>

#ifdef _WIN32
#define NOMINMAX //std::max�Ȃǂ��g���̂ɕK�v(windows.h�Ƌ������邩��)
#include <windows.h>
#endif

EPInterpreter* EPInterpreter::instance = NULL;

EPInterpreter* EPInterpreter::Create()
{
	if( EPInterpreter::instance == NULL)
		EPInterpreter::instance = new EPInterpreter();
	
	return EPInterpreter::instance;
}

void EPInterpreter::Destroy()
{
	if( EPInterpreter::instance != NULL)
		delete(EPInterpreter::instance);
}

EPInterpreter::EPInterpreter()
{
	this->State = UNSTARTED;
}

EPInterpreter::~EPInterpreter()
{
	this->State = STOP;
}

// �v���O�����I������EPInterpreter::instance��delete���Ă����Ђ�
class EPInterpreterDestroyer {
public:
	~EPInterpreterDestroyer() {
		EPInterpreter::Destroy();
	}
};
static EPInterpreterDestroyer epid;

extern "C" { __declspec(dllexport) PyObject* PyInit__SprPy(void); }


void EPInterpreter::Initialize()
{
	//PYTHONPATH��ύX����
	//SPRPYTHONPATH��SpringheadPython�Ŏg��LIB��PATH��ǉ����Ă���
	std::string newPath;
	char buff[1024];
	if (!GetEnvironmentVariable("SPRPYTHONPATH",buff,1024) || !strlen(buff)){
		DSTR << "Warning: " << "Can not find environment variable of 'SPRPYTHONPATH'. " 
			<< "Embeded python may cause serious error." << std::endl;
	}
	newPath.append(buff);
	newPath.append(";");
	buff[0] = '\0';
	GetEnvironmentVariable("PYTHONPATH", buff,1024);
	newPath.append(buff);
	if (newPath.length() <= 1){
		DSTR << "Warning: " << "Can not find environment variable of 'PYTHONPATH' and 'SPRPYTHONPATH'. " 
			<< "Embeded python will causes buffer over run error." << std::endl;
		printf("Error: Python Lib not found ");
		assert(0);
	}
	
	SetEnvironmentVariable("PYTHONPATH", newPath.c_str());
	// SetEnvironmentVariable("PYTHONHOME", newPath.c_str());


	//�p�C�\��������
	Py_Initialize();
	

	////���W���[���ǂݍ���
	initUtility();
	initBase();
	initFoundation();
	initFileIO();
	initCollision();
	initPhysics();
	initGraphics();
	initCreature();
	initHumanInterface();
	initFramework();
}

void EPInterpreter::Run(void* arg)
{
	if( this->State == UNSTARTED || this->State == STOP)
	{
		_beginthread(EPLoopLauncher, 0 , arg);
		this->State = RUN;
	}
}

void EPInterpreter::Stop()
{
	if( this->State != UNSTARTED)
		this->State = STOP;
}

bool EPInterpreter::BindInt(int i)
{
	return false;
}

void EPInterpreter::EPLoopLauncher(void* arg)
{
	EPInterpreter::instance->EPLoopInit(arg);
	while(EPInterpreter::instance->State == RUN)
	{
		EPInterpreter::instance->EPLoop(arg);
	}
}
