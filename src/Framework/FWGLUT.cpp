#ifdef _MSC_VER
#	include <windows.h>
#endif
#include <GL/glew.h>
#include <Framework/FWGLUT.h>
#include <sstream>

using namespace std;

namespace Spr{;

// FWWinGLUT /////////////////////////////////////////////////////////////////////
void FWWinGLUT::Position(int left, int top){
	glutPositionWindow(left, top);
	fullscreen = false;
}
void FWWinGLUT::Reshape(int width, int height){
	glutReshapeWindow(width, height);
	fullscreen = false;
	this->width = width;
	this->height = height;
}
void FWWinGLUT::SetTitle(UTString t){
	glutSetWindowTitle(t.c_str());
	title = t;
}
void FWWinGLUT::FullScreen(){
	glutFullScreen();
	fullscreen = true;
}

// FWGLUT /////////////////////////////////////////////////////////////////////
FWGLUTDesc::FWGLUTDesc(){
}

FWGLUT::FWGLUT(){
	idleFuncFlag = true;
};

FWGLUT* FWGLUT::instance;

FWGLUT::~FWGLUT(){
	FWGLUT::AtExit();
	instance = NULL;	
}

/** �R�[���o�b�N�֐�*///////////////////////////////////////////////////////
void FWGLUT::GlutDisplayFunc(){
	instance->fwApp->CallDisplay();	
}
void FWGLUT::GlutReshapeFunc(int w, int h){
	instance->fwApp->CallReshape(w, h);
}
void FWGLUT::GlutTimerFunc(int value){
	//instance->fwApp->CallTimerFunc(id);
	UTTimerIf* timer = UTTimerIf::Get(value);
	timer->Call();

	// �^�C�}�[�̍Đݒ�
	if(instance->timerRestart)
		glutTimerFunc(timer->GetInterval(), GlutTimerFunc, timer->GetID());

}
void FWGLUT::GlutIdleFunc(){
	// ���ȉ�2�n����Idle�����͓��ꂷ�ׂ�	tazz

	// UTTimerProvider�Ƃ��Ă̋@�\�DIDLE���[�h�̃^�C�}��TimerFunc���Ă΂��
	UTTimerProvider::CallIdle();

	// FWApp::IdleFunc���Ă�
	instance->fwApp->CallIdleFunc();
}
void FWGLUT::GlutKeyboardFunc(unsigned char key, int x, int y){
	instance->fwApp->CallKeyboard((int)key, x, y);
}
void FWGLUT::GlutSpecialFunc(int key, int x, int y){
	// GlutKeyboardFunc�Əd�����Ȃ��悤�Ƀr�b�g�𗧂Ă�
	instance->fwApp->CallKeyboard(key | 0x100, x, y);
}
void FWGLUT::GlutMouseFunc(int button, int state, int x, int y){
	instance->fwApp->CallMouseButton(button, state, x, y);
}
void FWGLUT::GlutMotionFunc(int x, int y){
	instance->fwApp->CallMouseMove(x, y);
}
void FWGLUT::GlutJoystickFunc(unsigned int buttonMask, int x, int y, int z){
	instance->fwApp->CallJoystick(buttonMask, x, y, z);
}
void FWGLUT::AtExit(){
	instance->fwApp->AtExit();
}

/** Init *////////////////////////////////////////////////////////////////

void FWGLUT::Init(int argc, char* argv[]){
	instance = this;
	if(argc == 0){
		argc = 1;
		char* dummy[] = {"", NULL};
		glutInit(&argc, dummy);
	}
	else glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	atexit(FWGLUT::AtExit);

	// UTTimerProvider�Ƃ��ēo�^
	Register();
}

bool FWGLUT::StartTimer(UTTimer* timer){
	glutTimerFunc(timer->GetInterval(), GlutTimerFunc, timer->timerId);
	timerRestart = true;
	return true;
}

bool FWGLUT::StopTimer(UTTimer* timer){
	timerRestart = false;
	return true;
}

void FWGLUT::StartMainLoop(){
	// CPU���펞100%�ɂȂ��肠��
	EnableIdleFunc(idleFuncFlag);
	glutMainLoop();
}

void FWGLUT::EnableIdleFunc(bool on){
	idleFuncFlag = on;
	glutIdleFunc(on ? FWGLUT::GlutIdleFunc : NULL);
}
void FWGLUT::EnterGameMode(){
	glutEnterGameMode();
}
void FWGLUT::LeaveGameMode(){
	glutLeaveGameMode();
}

/** �E�B���h�E *////////////////////////////////////////////////////////////////

///	�E�B���h�E���쐬���A�E�B���h�E ID��Ԃ�
FWWin* FWGLUT::CreateWin(const FWWinDesc& d){
	int wid=0;
	if(d.fullscreen){	//< �t���X�N���[���̏ꍇ�̃E�B���h�E����
		std::stringstream gameMode;
		gameMode << d.width << "x" << d.height << ":32@60";
		glutGameModeString(gameMode.str().c_str());
#ifdef _MSC_VER
		Sleep(100);
#endif
		wid	= glutEnterGameMode();
	}else{				//< �E�B���h�E���[�h�̏ꍇ�̐���
		if (d.parentWindow){
			wid = glutCreateSubWindow(d.parentWindow, d.left, d.top, d.width, d.height);
		}else{
			glutInitWindowSize(d.width, d.height);
			glutInitWindowPosition(d.left, d.top);
			wid = glutCreateWindow(d.title.c_str());
		}
	}
	//	����Window��glew�R���e�L�X�g�̏�����
	int rv = glewInit();
	//	window�Ɋ֘A����R�[���o�b�N�̐ݒ�
	glutDisplayFunc(FWGLUT::GlutDisplayFunc);
	glutReshapeFunc(FWGLUT::GlutReshapeFunc);
	glutKeyboardFunc(FWGLUT::GlutKeyboardFunc);
	glutSpecialFunc(FWGLUT::GlutSpecialFunc);
	glutMouseFunc(FWGLUT::GlutMouseFunc);
	glutMotionFunc(FWGLUT::GlutMotionFunc);
	int pollInterval = 10;	// int pollInterval : glutJoystickFunc���g���Ƃ��Ɏg�������D�ǂݍ��ݎ��ԂɊ֌W���Ă���炵���D
	glutJoystickFunc(FWGLUT::GlutJoystickFunc, pollInterval);
	
	FWWin* win = DBG_NEW FWWinGLUT(wid, d, fwApp->GetSdk()->CreateRender());
	win->SetID(glutGetWindow());	//���݂�window��ID��ݒ�
	return win;
}
///	�E�B���h�E��j������
void FWGLUT::DestroyWin(FWWin* w){
	if (w->fullscreen){
		glutLeaveGameMode();
	}else{
		glutDestroyWindow(w->GetID());
	}
};
///	�J�����g�E�B���h�E��ݒ肷��
void FWGLUT::SetCurrentWin(FWWin* w){
	glutSetWindow(w->GetID());
};
///	�J�����g�E�B���h�E��Ԃ��B
FWWin* FWGLUT::GetCurrentWin(){
	return fwApp->GetWinFromId(glutGetWindow());
}
///	�J�����g�E�B���h�E��ID��Ԃ��B
int FWGLUT::GetWinFromId(){
	return glutGetWindow();
}
///�J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
void FWGLUT::PostRedisplay(){
	return glutPostRedisplay();
};
/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
int FWGLUT::GetModifiers(){
	return glutGetModifiers();
};

}