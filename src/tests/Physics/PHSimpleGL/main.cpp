/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/** 
 Springhead2/src/tests/Physics/PHSimpleGL/main.cpp
 
�y�T�v�z
  2�̍��̂̈ʒu�̕ω����m�F����e�X�g�v���O�����i�ʒu���o�́AGL�\���j
  �E���̂̉^�����m�F����B
  �E�ʒu���o�͂��AOpenGL�ŃV�~�����[�V�������s���B
 
�y�I����z
  �E�Ԃ��e�B�[�|�b�g�Ɛ��e�B�[�|�b�g��10�b��̈ʒu�����ꂼ��v�Z�����Ғl�Ƃ���B
  �@���̊��Ғl�ƃV�~�����[�V�������ʂ��r���āA��v�����琳��I��(success)�Ƃ���B
  
�y�����̗���z
  �E�V�~�����[�V�����ɕK�v�ȏ��(���ʁE�d�S�E�����e���\���Ȃ�)��ݒ肷��B
    ���̂̌`���OpenGL�Ńe�B�[�|�b�g���w�肷��B
  �E�V�~�����[�V�����������n�߂���A���L�����ō��̂ɗ͂�������B
      ���̐Ԃ��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�����_�Ƃ��A�d�S��1m����E��1N�̗͂�������B
      �E�̐��e�B�[�|�b�g�F�I�u�W�F�N�g�̃��[�J�����W�n�̌��_����1m�E�ɏd�S�����炵�A�d�S��1m����E��1N�̗͂�������B
  �E�^����ꂽ�����ɂ�臙t�b��̈ʒu�̕ω���ϕ����A���̂̈ʒu�����o�͂ƁAOpenGL�����_�����O���s���B
   
 */
#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <GL/glut.h>
#ifdef USE_HDRSTOP
#pragma hdrstop
#endif
using namespace Spr;

#define ESC		27

UTRef<PHSdkIf> sdk;
PHSceneIf* scene;
PHSolidIf* redTeapot, *blueTeapot;	// Solid�^�C�v
// �����̐ݒ� 
static GLfloat light_position[] = { 15.0, 30.0, 20.0, 1.0 };
static GLfloat light_ambient[]  = { 0.0, 0.0, 0.0, 1.0 };
static GLfloat light_diffuse[]  = { 1.0, 1.0, 1.0, 1.0 }; 
static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
// �ގ��̐ݒ�
static GLfloat mat_red[]        = { 1.0, 0.0, 0.0, 1.0 };
static GLfloat mat_blue[]       = { 0.0, 0.0, 1.0, 1.0 };
static GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
static GLfloat mat_shininess[]  = { 120.0 };

static double stepCnt = 0.0;
namespace {
	Vec3d redVel = Vec3d(0.0, 0.0, 0.0);	// ���x
	Vec3d redPos = Vec3d(0.0, 0.0, 0.0);	// �ʒu
	Vec3d blueVel = Vec3d(0.0, 0.0, 0.0);				
	Vec3d bluePos = Vec3d(0.0, 0.0, 0.0);				
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void SPR_CDECL display(){

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	// ���̐Ԃ��e�B�[�|�b�g
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_red);

	Affined ad;
	glPushMatrix();
	Posed pose = redTeapot->GetPose();
	pose.ToAffine(ad);
	//	redTeapot->GetOrientation().to_matrix(af);
	//	af.Pos() = redTeapot->GetFramePosition();
	// pose�ł͐��x��double�Ȃ̂�matrixd�ɂ���
	glMultMatrixd(ad);
	glutSolidTeapot(1.0);
	glPopMatrix();

	// �E�̐��e�B�[�|�b�g
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blue);

	glPushMatrix();
	pose = blueTeapot->GetPose();
	ad = Affined(pose);
	glMultMatrixd(ad);
	glutSolidTeapot(1.0);
	glPopMatrix();

	glutSwapBuffers();
}

/**
 brief     	�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glEnable(GL_LIGHT0);
}

/**
 brief     	����������
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(4.0, 3.0, 10.0, 
		      4.0, 0.0, 0.0,
		 	  0.0, 1.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	setLight();
}

/**
 brief  	glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	<in/--> w�@�@��
 param  	<in/--> h�@�@����
 return 	�Ȃ�
 */
void SPR_CDECL reshape(int w, int h){
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (double)(w/h), 1.0, 500);
	glMatrixMode(GL_MODELVIEW);
}

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void SPR_CDECL keyboard(unsigned char key, int x, int y){
	if (key == ESC) exit(0);
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void SPR_CDECL idle(){
	//	���̂̏d�S��1m����E�ɉ����D
	Vec3d force = Vec3d(1, 0, 0);
	redTeapot->AddForce( force, Vec3f(0,1,0)+redTeapot->GetCenterPosition());
	blueTeapot->AddForce( force, Vec3f(0,1,0)+blueTeapot->GetCenterPosition());

	scene->Step();
	stepCnt += scene->GetTimeStep();	// ��������[msec]
	double dt = scene->GetTimeStep();

	// ���x�𓱂��A�������Ԍo�ߌ�̈ʒu���v�Z�i���Ғl)
	redVel		+= force * redTeapot->GetMassInv() * dt;	// (force/m)*dt
	redPos		+= redVel * dt;
	blueVel		+= force * blueTeapot->GetMassInv() * dt;	// (force/m)*dt
	bluePos		+= blueVel * dt;

	std::cout << redTeapot->GetCenterPosition() << blueTeapot->GetCenterPosition() << std::endl;
	
	// 10sec�ɃV�~�����[�V�������ʂƊ��Ғl���r
	if (stepCnt >= 10.0) {	
		bool redApprox, blueApprox;
		redApprox	= approx(redPos, redTeapot->GetFramePosition());
		blueApprox	= approx(bluePos, blueTeapot->GetFramePosition());

		if (redApprox && blueApprox) {	// �V�~�����[�V�������ʂ͐��������ʂƂȂ�܂����B
			DSTR << "\nPHSimpleGL success (redTeapot:success, blueTeapot:success)" << std::endl;
			exit(EXIT_SUCCESS);
		} else if (redApprox) {			// �Ԃ��e�B�[�|�b�g�͐��������ʂ������܂������A���e�B�[�|�b�g�͐��������ʂ������܂���ł����B
			DSTR << "\nPHSimpleGL failure (redTeapot:success, blueTeapot:failure)" << std::endl;
			exit(EXIT_FAILURE);
		} else if (blueApprox) {		// ���e�B�[�|�b�g�͐��������ʂ������܂������A�Ԃ��e�B�[�|�b�g�͐��������ʂ������܂���ł����B		
			DSTR << "\nPHSimpleGL failure (redTeapot:failure, blueTeapot:success)" << std::endl;
			exit(EXIT_FAILURE);
		} else {						// ���e�B�[�|�b�g�ƐԂ��e�B�[�|�b�g�͂Ƃ��ɐ��������ʂ������܂���ł����B
			DSTR << "\nPHSimpleGL failure (redTeapot:failure, blueTeapot:failure)" << std::endl;
			exit(EXIT_FAILURE);
		}
	} 
	
	glutPostRedisplay();
}

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int SPR_CDECL main(int argc, char* argv[]){
	sdk = PHSdkIf::CreateSdk();					//	SDK�̍쐬
	scene = sdk->CreateScene();				//	�V�[���̍쐬
	
	PHSolidDesc desc;						// ����teapot
	desc.mass = 2.0;						// ����	
	desc.inertia *=2.0;						// �����e���\��
	desc.center = Vec3f(0,0,0);				// ���ʒ��S�̈ʒu
	redTeapot = scene->CreateSolid(desc);	// ���̂�desc�Ɋ�Â��č쐬
	redTeapot->SetGravity(false);			// �d�͂𖳌��ɂ���
	redPos = redTeapot->GetFramePosition();

	desc.center = Vec3f(1,0,0);			//	�d�S�̈ʒu��Solid�̌��_����1m�E�ɂ��炷�D
	desc.pose.Pos() = Vec3f(3.5, 0.0, 0.0);
	//desc.pose = desc.pose * Posed::Trn(0.0, 0.0, 3.0);	// ��O�ɕ��s�ړ�	
	Vec3d trn = Vec3d(0.0, 0.0, 3.0);
	desc.pose = desc.pose * Posed::Trn(trn);				// ��O�ɕ��s�ړ�
	blueTeapot = scene->CreateSolid(desc);	
	blueTeapot->SetGravity(false);
	bluePos = blueTeapot->GetFramePosition();

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("PHSimpleGL");

	initialize();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glutMainLoop();
	
	return 0;
}

