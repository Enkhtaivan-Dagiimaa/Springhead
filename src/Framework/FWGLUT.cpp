/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifdef _MSC_VER
# include <windows.h>
# undef CreateDialog
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

FWGLUT::FWGLUT(){
	idleFuncFlag = true;
};

FWGLUT::~FWGLUT(){
}

FWGLUT* FWGLUT::GetInstance(){
	return (FWGLUT*)&*(FWGraphicsAdaptee::instance);
}

/** �R�[���o�b�N�֐�*///////////////////////////////////////////////////////
void FWGLUT::GlutDisplayFunc(){
	FWApp::GetApp()->Display();	
}
void FWGLUT::GlutReshapeFunc(int w, int h){
	FWWinIf* win = FWApp::GetApp()->GetCurrentWin();
	// ���ߍ���GUI�R���g���[��������ꍇ��z�肵�ăr���[�|�[�g�̌v�Z���s��
	int l = 0, t = 0;
	FWGraphicsAdaptee::instance->CalcViewport(&l, &t, &w, &h);
	win->GetRender()->Reshape(Vec2f(l, t), Vec2f(w,h));
	// �V�����E�B���h�E�T�C�Y���L��
	FWWinBase* winBase = win->Cast();
	FWWinBaseDesc desc;
	winBase->FWWinBase::GetDesc(&desc);
	desc.width = w;
	desc.height = h;
	winBase->FWWinBase::SetDesc(&desc);
}
void FWGLUT::GlutTimerFunc(int value){
	UTTimerIf* timer = UTTimerIf::Get(value);
	if(!timer)
		return;
	timer->Call();

	// �^�C�}�[�̍Đݒ�
	if(GetInstance()->timerRestart)
		glutTimerFunc(timer->GetInterval(), GlutTimerFunc, timer->GetID());

}
void FWGLUT::GlutIdleFunc(){
	// ���ȉ�2�n����Idle�����͓��ꂷ�ׂ�	tazz

	// UTTimerProvider�Ƃ��Ă̋@�\�DIDLE���[�h�̃^�C�}��TimerFunc���Ă΂��
	UTTimerProvider::CallIdle();

	// FWApp::IdleFunc���Ă�
	FWApp::GetApp()->IdleFunc();
}
void FWGLUT::AtExit(){
	FWApp::GetApp()->AtExit();
}

/** Init *////////////////////////////////////////////////////////////////

void FWGLUT::Init(int argc, char** argv){
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
	DSTR << FWApp::GetApp()->GetSdk() << endl;
	grDevice = FWApp::GetApp()->GetSdk()->GetGRSdk()->CreateDeviceGL();
	grDevice->Init();

	// �L�[�{�[�h�E�}�E�X�ƃW���C�X�e�B�b�N�f�o�C�X�̓o�^
	HISdkIf* hiSdk = FWApp::GetApp()->GetSdk()->GetHISdk();
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
FWWinIf* FWGLUT::CreateWin(const FWWinDesc& desc, FWWinIf* parent){
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
		if(parent){
			wid = glutCreateSubWindow(parent->GetID(), desc.left, desc.top, desc.width, desc.height);
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

	// �E�B���h�E���쐬
	FWWin* win = DBG_NEW FWWin();
	win->SetDesc(&desc);
	win->id = wid;
	
	// �L�[�{�[�h�E�}�E�X�ƃW���C�X�e�B�b�N�̉��z�f�o�C�X���쐬���Ċ֘A�t��
	HISdkIf* hiSdk = FWApp::GetApp()->GetSdk()->GetHISdk();
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
int FWGLUT::GetCurrentWin(){
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

void FWGLUT::SetPosition(FWWinBase* win, int left, int top){
	glutSetWindow(win->id);
	glutPositionWindow(left, top);
}
void FWGLUT::SetSize(FWWinBase* win, int width, int height){
	glutSetWindow(win->id);
	glutReshapeWindow(width, height);
}
void FWGLUT::SetTitle(FWWinBase* win, UTString t){
	glutSetWindow(win->id);
	glutSetWindowTitle(t.c_str());
}
void FWGLUT::SetFullScreen(FWWin* win){
	glutSetWindow(win->id);
	glutFullScreen();
}


}