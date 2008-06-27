/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Framework/SprFWAppGLUT.h>
#include "FWScene.h"
#include "FWSdk.h"
#include "FWOldSpringheadNode.h"
#include <Physics/PHSdk.h>
#include <Graphics/GRSdk.h>
#include <sstream>
#include <windows.h>

#include <GL/glew.h>
#include <GL/glut.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

namespace Spr{;
void FWWinGLUT::Position(int left, int top){
	glutPositionWindow(left, top); fullScreen = false;
}
void FWWinGLUT::Reshape(int width, int height){
	glutReshapeWindow(width, height); fullScreen = false;
	this->width = width; this->height = height;
}
void FWWinGLUT::SetTitle(UTString t){
	glutSetWindowTitle(t.c_str()); title = t;
}
void FWWinGLUT::FullScreen(){
	glutFullScreen(); fullScreen = true;
}

//-----------------------------------------------------------------------

FWAppGLUT* FWAppGLUT::instance;

FWAppGLUT::~FWAppGLUT(){
	FWAppGLUT::AtExit();
	instance = NULL;
}
void FWAppGLUT::GlutDisplayFunc(){
	FWAppGLUT::instance->CallDisplay();
}

void FWAppGLUT::GlutReshapeFunc(int w, int h){
	FWAppGLUT::instance->CallReshape(w, h);
}
void FWAppGLUT::GlutIdleFunc(){
	FWAppGLUT::instance->CallIdle();
}

void FWAppGLUT::GlutTimerFunc(int id){	
	// �^�C�}�����������V�~�����[�V�����̃C���^�[�o��
	int timeStep=1;
	if(!FWAppGLUT::instance)return;
	if(!FWAppGLUT::instance->GetSdk())return;
	if(FWAppGLUT::instance->GetSdk()->GetScene()){
		if(FWAppGLUT::instance->GetSdk()->GetScene()->GetPHScene()){
			timeStep = (int)(FWAppGLUT::instance->GetSdk()->GetScene()->GetPHScene()->GetTimeStep() * 1000.0);
			if (timeStep<1) timeStep = 1;
		}
	}
	glutTimerFunc(timeStep, GlutTimerFunc, 0);

	FWAppGLUT::instance->CallStep();
	glutPostRedisplay();
}
void FWAppGLUT::GlutKeyboardFunc(unsigned char key, int x, int y){
	FWAppGLUT::instance->CallKeyboard((int)key, x, y);
}

void FWAppGLUT::GlutMouseFunc(int button, int state, int x, int y){
	FWAppGLUT::instance->CallMouseButton(button, state, x, y);
}
void FWAppGLUT::GlutMotionFunc(int x, int y){
	FWAppGLUT::instance->CallMouseMove(x, y);
}

void FWAppGLUT::GlutJoystickFunc(unsigned int buttonMask, int x, int y, int z){
	FWAppGLUT::instance->CallJoystick(buttonMask, x, y, z);
}

void FWAppGLUT::AtExit(){
	if(FWAppGLUT::instance && FWAppGLUT::instance->vfBridge)
		FWAppGLUT::instance->vfBridge->AtExit();
}

//-----------------------------------------------------------------------

void FWAppGLUT::Init(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	FWAppGL::Init(argc, argv);
	atexit(FWAppGLUT::AtExit);
}

void FWAppGLUT::Start(){
	instance = this;
	if (!NWin()){
		CreateWin();
		wins.back()->SetScene(GetSdk()->GetScene());
	}
	glutTimerFunc(1, FWAppGLUT::GlutTimerFunc, 0);
	glutIdleFunc(FWAppGLUT::GlutIdleFunc);
	glutMainLoop();
}

FWWin* FWAppGLUT::CreateWin(const FWWinDesc& d){
	int wid=0;
	if(d.fullscreen){	//< �t���X�N���[���̏ꍇ�̃E�B���h�E����
		//memo:-------------------------------------------------------
		// fullScreen���g�p����ꍇ�ɂ̓E�B���h�E�j��������ۂ�
		// glutLeaveGameMode()���Ă΂Ȃ��Ɗ댯�Dby Toki
		//------------------------------------------------------------
		std::stringstream gameMode;
		gameMode << d.width << "x" << d.height << ":32@60";
		glutGameModeString(gameMode.str().c_str());
		Sleep(100);
		wid	= glutEnterGameMode();
	}
	else{				//< �E�B���h�E���[�h�̏ꍇ�̐���
		if (d.parentWindow){
			wid = glutCreateSubWindow(d.parentWindow, d.left, d.top, d.width, d.height);
		}else{
			glutInitWindowSize(d.width, d.height);
			glutInitWindowPosition(d.left, d.top);
			wid = glutCreateWindow(d.title.c_str());
		}
	}
	int rv = glewInit();
	glutDisplayFunc(FWAppGLUT::GlutDisplayFunc);
	glutReshapeFunc(FWAppGLUT::GlutReshapeFunc);
	glutKeyboardFunc(FWAppGLUT::GlutKeyboardFunc);
	glutMouseFunc(FWAppGLUT::GlutMouseFunc);
	glutMotionFunc(FWAppGLUT::GlutMotionFunc);
	int pollInterval = 0.01;	// int pollInterval : glutJoystickFunc���g���Ƃ��Ɏg�������D�ǂݍ��ݎ��ԂɊ֌W���Ă���炵���D
	glutJoystickFunc(FWAppGLUT::GlutJoystickFunc, pollInterval);
	// �E�B���h�EID���w�肵�ă^�C�}���n��
	//glutTimerFunc(1, FWAppGLUT::GlutTimerFunc, wid);
	
	FWWin* win = DBG_NEW FWWinGLUT(wid, fwSdk->CreateRender());
	AssignScene(win);
	wins.push_back(win);
	return win;
}
void FWAppGLUT::DestroyWin(FWWin* w){
	glutDestroyWindow(w->GetID());
}
void FWAppGLUT::SetCurrentWin(FWWin* w){
	glutSetWindow(w->GetID());
}
FWWin* FWAppGLUT::GetCurrentWin(){
	return GetWinFromId(glutGetWindow());
}
void FWAppGLUT::Display(){
	FWAppGL::Display();
	/// �_�u���o�b�t�@���[�h���A�J�����g�E�B���h�E�̃o�b�t�@�������s��
	glutSwapBuffers();
}

int FWAppGLUT::GetModifier(){
	return glutGetModifiers();
}

}
