#include <windows.h>
#include <GL/glew.h>
#include <GL/glut.h>

#include "Framework/SprFWGLUT.h"
#include "Framework/SprFWApp.h"


namespace Spr{;
// FWWinGLUT /////////////////////////////////////////////////////////////////////
void FWWinGLUT::Position(int left, int top){
	glutPositionWindow(left, top); fullscreen = false;
}
void FWWinGLUT::Reshape(int width, int height){
	glutReshapeWindow(width, height); fullscreen = false;
	this->width = width; this->height = height;
}
void FWWinGLUT::SetTitle(UTString t){
	glutSetWindowTitle(t.c_str()); title = t;
}
void FWWinGLUT::FullScreen(){
	glutFullScreen(); fullscreen = true;
}
// FWGLUT /////////////////////////////////////////////////////////////////////
FWGLUTDesc::FWGLUTDesc(){
}

FWGLUT::FWGLUT(){
};

FWGLUT* FWGLUT::instance;
FWGLUT::FWGLUTTimer* FWGLUT::FWGLUTTimer::timerInstance;

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
void FWGLUT::GlutIdleFunc(){
	instance->fwApp->CallIdleFunc();
}

void FWGLUT::GlutKeyboardFunc(unsigned char key, int x, int y){
	instance->fwApp->CallKeyboard((int)key, x, y);
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
	if(instance && instance->fwApp->vfBridge)
		instance->fwApp->vfBridge->AtExit();
}

/** Init *////////////////////////////////////////////////////////////////

void FWGLUT::Init(int argc, char* argv[]){
	instance = this;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	atexit(FWGLUT::AtExit);
}

/** �^�C�} *////////////////////////////////////////////////////////////////

void FWGLUT::FWGLUTTimer::SetTimerFunc(GTimerFunc* f){
	func=f;
}
void FWGLUT::FWGLUTTimer::GetTimerFunc(){;
	this->func;
}
/// �^�C�}�[��ݒ肷��
void FWGLUT::Timer(int i){
	if(i < (int)glutTimer.size()){
		glutTimerFunc(1, glutTimer[i]->func, 0);
	}
}
/// �^�C�}�[���쐬����
void FWGLUT::AddTimer(){
	glutTimer.push_back(DBG_NEW FWGLUTTimer);
}
/// �Ō�ɍ쐬�����^�C�}�[�̔ԍ���Ԃ�
int FWGLUT::GetTimerNo(){
	return (int)glutTimer.size() - 1;
}
/// i�Ԗڂ̃^�C�}�[��Timerfunc��ݒ肷��
void FWGLUT::SetTimerFunc(GTimerFunc* f ,int i){
	if(i < (int)glutTimer.size()){
		glutTimer[i]->SetTimerFunc(f);
	}
}
/// mainloop���Ă�
void FWGLUT::Loop(int i,double timeStep){
	int timeSteps=1;
	if(!fwApp->instance)return;
	if(!fwApp->instance->GetSdk())return;
	if(fwApp->instance->GetSdk()->GetScene()){
		if(fwApp->instance->GetSdk()->GetScene()->GetPHScene()){
			timeSteps = (int)(fwApp->instance->GetSdk()->GetScene()->GetPHScene()->GetTimeStep() * 1000.0);
			if (timeSteps<1) timeSteps = 1;
		}
	}
	if(!timeStep==0){
		timeSteps=timeStep;
	}
	glutTimerFunc(timeSteps, glutTimer[i]->func, 0);
}
///GLUT�ɂ��Timer���X�^�[�g����
void FWGLUT::TimerStart(){
	glutIdleFunc(FWGLUT::GlutIdleFunc);
	glutMainLoop();
}
/// �^�C�}�[���擾����
FWGLUT::FWGLUTTimer* FWGLUT::GetTimer(int i){
	if(i < (int)glutTimer.size()){
		return glutTimer[i];
	}else{
		return NULL;
	}
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
	glutMouseFunc(FWGLUT::GlutMouseFunc);
	glutMotionFunc(FWGLUT::GlutMotionFunc);
	int pollInterval = 10;	// int pollInterval : glutJoystickFunc���g���Ƃ��Ɏg�������D�ǂݍ��ݎ��ԂɊ֌W���Ă���炵���D
	glutJoystickFunc(FWGLUT::GlutJoystickFunc, pollInterval);
	
	FWWin* win = DBG_NEW FWWinGLUT(wid, d, fwApp->GetSdk()->CreateRender());
	fwApp->AssignScene(win);
	fwApp->wins.push_back(win);
	return win;
}
///	�E�B���h�E��j������
void FWGLUT::DestroyWin(FWWin* w){
	if (w->fullscreen){
		glutLeaveGameMode();
	}else{
		glutDestroyWindow(w->GetID());
	}
	Wins::iterator it = std::find(fwApp->wins.begin(), fwApp->wins.end(), w);
	assert(it != fwApp->wins.end());
	fwApp->wins.erase(it);
};
///	�J�����g�E�B���h�E��ݒ肷��
void FWGLUT::SetCurrentWin(FWWin* w){
	glutSetWindow(w->GetID());
};
///	�J�����g�E�B���h�E��Ԃ��B
FWWin* FWGLUT::GetCurrentWin(){
	return fwApp->GetWinFromId(glutGetWindow());
}
///�J�����g�E�B���h�E�̃m�[�}���v���[�����C�ĕ`��̕K�v�ɉ����ă}�[�N����
void FWGLUT::PostRedisplay(){
	return glutPostRedisplay();
};
/// Shift,Ctrl,Alt�̃X�e�[�g��Ԃ�
int FWGLUT::Modifiers(){
	return glutGetModifiers();
};

}