/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifdef _MSC_VER
#	include <windows.h>
#endif
#include <GL/glew.h>
#include <GL/glut.h>
#include <Framework/SprFWApp.h>
#include <Framework/FWGLUT.h>
#include <Framework/FWSdk.h>
#include <Graphics/GRSdk.h>
#include <HumanInterface/HISdk.h>
#include <sstream>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace std;

namespace Spr{;

// FWWinGLUT /////////////////////////////////////////////////////////////////////
void FWWinGLUT::SetPosition(int left, int top){
	glutSetWindow(id);
	glutPositionWindow(left, top);
	fullscreen = false;
}
void FWWinGLUT::SetSize(int width, int height){
	glutSetWindow(id);
	glutReshapeWindow(width, height);
	fullscreen = false;
	this->width = width;
	this->height = height;
}
void FWWinGLUT::SetTitle(UTString t){
	glutSetWindow(id);
	glutSetWindowTitle(t.c_str());
	title = t;
}
void FWWinGLUT::SetFullScreen(){
	glutSetWindow(id);
	glutFullScreen();
	fullscreen = true;
}

// FWGLUT /////////////////////////////////////////////////////////////////////
FWGLUT::FWGLUT(FWApp* a):FWGraphicsAdaptee(a){
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
	UTTimerIf* timer = UTTimerIf::Get(value);
	if(!timer)
		return;
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
/*
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
*/
void FWGLUT::AtExit(){
	instance->fwApp->AtExit();
}

/** Init *////////////////////////////////////////////////////////////////

void FWGLUT::Init(int argc, char** argv){
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

	// �O���t�B�N�X�f�o�C�X���쐬
	grDevice = fwApp->GetSdk()->GetGRSdk()->CreateDeviceGL();
	grDevice->Init();

	// �L�[�{�[�h�E�}�E�X�ƃW���C�X�e�B�b�N�f�o�C�X�̓o�^
	HISdkIf* hiSdk = fwApp->GetSdk()->GetHISdk();
	hiSdk->AddRealDevice(DRKeyMouseGLUTIf::GetIfInfoStatic());
	hiSdk->AddRealDevice(DRJoyStickGLUTIf::GetIfInfoStatic());
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
FWWinIf* FWGLUT::CreateWin(const FWWinDesc& desc){
	int wid=0;

	// �t���X�N���[���̏ꍇ�̃E�B���h�E����
	if(desc.fullscreen){	
		std::stringstream gameMode;
		gameMode << desc.width << "x" << desc.height << ":32@60";
		glutGameModeString(gameMode.str().c_str());
#ifdef _MSC_VER
		Sleep(100);
#endif
		wid	= glutEnterGameMode();
	}
	// �E�B���h�E���[�h�̏ꍇ�̐���
	else{
		if (desc.parentWindow){
			wid = glutCreateSubWindow(desc.parentWindow, desc.left, desc.top, desc.width, desc.height);
		}
		else{
			glutInitWindowSize(desc.width, desc.height);
			glutInitWindowPosition(desc.left, desc.top);
			wid = glutCreateWindow(desc.title.c_str());
		}
	}
	// ����Window��glew�R���e�L�X�g�̏�����
	int rv = glewInit();

	// window�Ɋ֘A����R�[���o�b�N�̐ݒ�
	glutDisplayFunc(FWGLUT::GlutDisplayFunc);
	glutReshapeFunc(FWGLUT::GlutReshapeFunc);
	/*
	glutKeyboardFunc(FWGLUT::GlutKeyboardFunc);
	glutSpecialFunc(FWGLUT::GlutSpecialFunc);
	glutMouseFunc(FWGLUT::GlutMouseFunc);
	glutMotionFunc(FWGLUT::GlutMotionFunc);
	int pollInterval = 10;	// int pollInterval : glutJoystickFunc���g���Ƃ��Ɏg�������D�ǂݍ��ݎ��ԂɊ֌W���Ă���炵���D
	glutJoystickFunc(FWGLUT::GlutJoystickFunc, pollInterval);
	*/

	// �E�B���h�E���쐬
	FWWinGLUT* win = DBG_NEW FWWinGLUT(wid, desc);
	
	// �L�[�{�[�h�E�}�E�X�ƃW���C�X�e�B�b�N�̉��z�f�o�C�X���쐬���Ċ֘A�t��
	HISdkIf* hiSdk = fwApp->GetSdk()->GetHISdk();
	HIRealDeviceIf* dr;
	if(desc.useKeyMouse){
		dr = hiSdk->FindRealDevice(DRKeyMouseGLUTIf::GetIfInfoStatic());
		win->keyMouse = dr->Rent(DVKeyMouseIf::GetIfInfoStatic(), "", wid)->Cast();
	}
	if(desc.useJoyStick){
		dr = hiSdk->FindRealDevice(DRJoyStickGLUTIf::GetIfInfoStatic());
		win->joyStick = dr->Rent(DVJoyStickIf::GetIfInfoStatic(), "", wid)->Cast();
	}

	return win->Cast();
}

///	�E�B���h�E��j������
void FWGLUT::DestroyWin(FWWinIf* w){
	if (w->GetFullScreen()){
		glutLeaveGameMode();
	}
	else{
		glutDestroyWindow(w->GetID());
	}
};
///	�J�����g�E�B���h�E��ݒ肷��
void FWGLUT::SetCurrentWin(FWWinIf* w){
	glutSetWindow(w->GetID());
};
///	�J�����g�E�B���h�E��Ԃ��B
FWWinIf* FWGLUT::GetCurrentWin(){
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