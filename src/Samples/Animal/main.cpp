/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Springhead.h>
#include <Creature/CRCreature.h>
#include <windows.h>
#include <GL/glut.h>
#include <iostream>
#include <ctime>
#include <gl/glui.h>

#include "myGlobalValue.h"
//�֐���`���ז�������O�ɒǂ�����Ă��邾���Ȃ̂ŏ����Ă͂����Ȃ��B
//���̂��������ƃN���X�Ƃ��\���̂�����ē���悤�B
//�y�ǋL�z��2008.01.20����Framework��FWAppGLUI�N���X����������Ă���B
#include "LoadDataFromGUI.h"
#include "SetDataFromGUI.h"
#include "ResetDataFromGUI.h"
#include "DesignOfGUI.h"

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

void step(){

	phScene->ClearForce();
	// �Ǝ��̐���͂����ɓ����
	phScene->GenerateForce();
	phScene->Integrate();
	
}

void GLUT_Idle(){
	glutPostRedisplay();
}

void GLUT_Timer(int id){
	glutSetWindow(mainWindow);
	if (!bTimer) return;
	glutTimerFunc(simulationPeriod, GLUT_Timer, 0);							//�����ɏ������Ƃ�glutTimerFunc()���ċA�֐��ɂ��Ă���
	step();	
	glutPostRedisplay();
}

void createFloor(){
	
	PHSolidDesc descSolid;													//���̏������p�̃f�B�X�N���v�^�̐錾
	{
		descSolid.dynamical = false;										//�������畨���@����؂�
	}
	PHSolidIf* soFloor = phScene->CreateSolid(descSolid);					//���̃C���^�t�F�[�X�Ƀf�B�X�N���v�^�̒��g��n�����h���̂Ƃ����T�O�h��o�^����
	soFloor->SetName("Floor");
	CDBoxDesc descBox;														//���̏Փ˔���p�̃f�B�X�N���v�^�̐錾
	{
		descBox.boxsize = Vec3f(30, 2, 30);									//���̃T�C�Y�̎w��
	}
	soFloor->AddShape(phSdk->CreateShape(descBox));							//��قǓo�^�����h���̂Ƃ����T�O�h�ɏՓ˔���ł�����̂�^����
	soFloor->SetFramePosition(Vec3f(0, -1, 0));								//���̂������̂̐ݒu�ꏊ���w�肷��

}	

void CreateBody(){

	// --- --- --- --- ---
	// �N���[�`���S��

	CRCreatureDesc descCreature;
	{
		
	}
	
	creature = (DBG_NEW CRCreature(descCreature, phScene))->Cast();

	// --- --- --- --- ---
	// Body

	CRFourLegsAnimalBodyDesc bodyDesc;
	{
		//�M���ރf�[�^�ŕύX�����������l������΂�����bodyDesc.******�����ŏ��������邱�Ƃ��ł���B
		bodyDesc.goalShoulder	 = Quaterniond::Rot(Rad(-160), 'x');
		bodyDesc.goalHip		 = Quaterniond::Rot(Rad(- 30), 'x');
		bodyDesc.originElbow	 = Rad( 130);
		bodyDesc.originStifle	 = Rad(-150);
		bodyDesc.originFrontKnee = Rad(-170);
		bodyDesc.originRearKnee	 = Rad( 170);

	}
    
	animalBody = DCAST(CRFourLegsAnimalBodyIf, creature->CreateBody(bodyDesc));


//test from here

	// --- --- --- --- ---
	// �R���g���[���̓o�^
	
	// --- --- --- --- ---
	// ������

	creature->Init();

	// --- --- --- --- ---
	// Horse�̏����ʒu����
	Vec3f initialPosition = Vec3f(0, 2.0f, 1.5);
	for (int i=0; i<animalBody->NSolids(); i++) {
		PHSolidIf* so = animalBody->GetSolid(i);
		if (so) {
			so->SetFramePosition(so->GetPose().Pos() + initialPosition);
		}
	}
// to here

	phScene->Print(DSTR);
}

void GLUT_Display(){
	glutSetWindow(mainWindow);
	grRender->ClearBuffer();					//�o�b�t�@�[�̒��g������

	grRender->BeginScene();						//�`�悷��V�[���̎n�܂�
	grRender->DrawScene(phScene);				//�V�[���O���t��`�悷��B�i�{���͂��ꂢ�ȃ��f���𓮂����邯�Ǎ��̂Œu�����������E�FphScene�j

	//�@�����Ǝ��ɕ`�悵�������e������΂�����
	//////////////////////////////////////////////////
	//�����ɕ��ʂ�glBegin()�Ƃ������Əd�˕`���ł���	//
	//////////////////////////////////////////////////
	
	grRender->EndScene();						//�`�悷��V�[���̏I���

	glutSwapBuffers();							//�o�b�t�@�[�����ւ��邱�ƂŃX���[�Y�`�������
}

void GLUT_Reshape(int w, int h){
	//�@Viewport�Ǝˉe�s���ݒ�
	grRender->Reshape(Vec2f(), Vec2f(w,h));	   //Window�̑傫�����ς�������A�`���Reshape�֐��̓񎟌��x�N�g���ɏ]���čs��
}

void onKey(char key){

	PHBallJointDesc ballD;
	PHHingeJointDesc hingeD;

	switch(key){
	// --- ���_�ύX
	{
	case '1'://�����ݒ艻
		camAngle = 0.0f;
		camZoom  = 3.0f;
		view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;	//�J�����̍��W���w�肷��
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));								//�J������������W, ������x�N�g�� 
		grRender->SetViewMatrix(view.inv());													//�J�������_��o�^����
		break;

	case '2'://�E���
		camAngle -= 10.0f;
		if (camAngle < 0) { camAngle -= 360.0f; }
		view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));
		grRender->SetViewMatrix(view.inv());
		break;

	case '3'://�����
		camAngle += 10.0f;
		if (camAngle < 0) { camAngle -= 360.0f; }
		view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));
		grRender->SetViewMatrix(view.inv());
		break;

	case '4'://�Y�[��
		camZoom -= 0.5;
		if (camZoom < 0.5) { camZoom = 0.5; }
		view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));
		grRender->SetViewMatrix(view.inv());
		break;

	case '5'://�L�p
		camZoom += 0.5;
		view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;
		view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));
		grRender->SetViewMatrix(view.inv());
		break;
	
	case '6'://���݂̏d�S��\��
		DSTR << "Center Of Mass : " << animalBody->GetCenterOfMass() << endl;
		break;

	case '7'://���݂̏�̏d�S��\��
		DSTR << "Center Of Mass Of Upper Part : " << animalBody->GetUpperCenterOfMass() << endl;
		break;
	}

	
	// --- �{�f�B�𓮂���
	{
	case 'a':
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_CHEST_NECK)->GetDesc(&ballD);
		ballD.goal = Quaterniond::Rot(Rad(90), 'x');
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_CHEST_NECK)->SetDesc(&ballD);
		break;
	case 'A':
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_CHEST_NECK)->GetDesc(&ballD);
		ballD.goal = Quaterniond::Rot(Rad(60), 'x');
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_CHEST_NECK)->SetDesc(&ballD);
		break;
	
	case 'z':
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_SHOULDER)->GetDesc(&ballD);
		ballD.goal = Quaterniond::Rot(Rad(-100), 'x');
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_SHOULDER)->SetDesc(&ballD);

		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_ELBOW)->GetDesc(&hingeD);
		hingeD.origin = Rad(70);
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_ELBOW)->SetDesc(&hingeD);

		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_FRONT_KNEE)->GetDesc(&hingeD);
		hingeD.origin = Rad(-60);
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_FRONT_KNEE)->SetDesc(&hingeD);
		break;

	case 'x':
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_HIP)->GetDesc(&ballD);
		ballD.goal = Quaterniond::Rot(Rad(-30), 'x');
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_HIP)->SetDesc(&ballD);

		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_STIFLE)->GetDesc(&hingeD);
		hingeD.origin = Rad(-70);
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_STIFLE)->SetDesc(&hingeD);

		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_REAR_KNEE)->GetDesc(&hingeD);
		hingeD.origin = Rad(60);
		animalBody->GetJoint(CRFourLegsAnimalBodyDesc::JO_RIGHT_REAR_KNEE)->SetDesc(&hingeD);
		break;
	}

	case 'p': 
		bTimer = false;
		step();
		break;

	case 'P':
		if(bTimer == true){
			bTimer = false;
			step();
		}
		else{
			bTimer = true;
			GLUI_Master.set_glutTimerFunc(simulationPeriod, GLUT_Timer, 0);
		}
		break;

	case 'o':
		bTimer = true;
		GLUI_Master.set_glutTimerFunc(simulationPeriod, GLUT_Timer, 0);
		break;
	}
}

void GLUT_Keyboard(unsigned char key, int x,int y){
	
	glutSetWindow(mainWindow);
	
	int i = 0;
	PHSceneDesc sceneDesc;
	PHSceneIf*	state;

	switch(key){
		case ESC: // exit this program
			PressQuit();
			break;
	
		case ' ': // Reset the scene
			phScene->Clear();
			sceneDesc = PHSceneDesc();
			sceneDesc.timeStep = dt;										//�V�~�����[�V�����̍��ݎ��Ԃ�o�^����
			sceneDesc.numIteration = niter;									//LCP�����񐔂�o�^����
			phScene = phSdk->CreateScene(sceneDesc);						//�����V�~�����[�^�̒��Ƀf�B�X�N���v�^�̕ϐ�����ꂽ�V�[���O���t���쐬
			createFloor();													//�V�[���O���t���ɏ������
			CreateBody();
			guiLoadScene();
			guiLoadInit();
			break;

		case '0':
			phScene->GetState(&state);
			DSTR << state << std::endl;
			break;

		default:
			onKey(key);
			break;	
	}
}

void setLight() {
	GRLightDesc light0, light1;
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, 10.0, 1.0);
	grRender->PushLight(light0);
	grRender->PushLight(light1);

}

void initialize(){
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	setLight();
}

int main(int argc, char* argv[]){
	
	//�@SDK�̍쐬�@
	phSdk = PHSdkIf::CreateSdk();									//�����V�~�����[�^���v��
	grSdk = GRSdkIf::CreateSdk();									//�`�悪�v��

//:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//fMax�t��simulator

	//�@�V�[���I�u�W�F�N�g�̍쐬
	PHSceneDesc sceneDesc;											//�V�[���O���t�i�ݒ���݂����Ȃ��́H�j������������f�B�X�N���v�^
	sceneDesc.timeStep = dt;										//�V�~�����[�V�����̍��ݎ��Ԃ�o�^����
	sceneDesc.numIteration = niter;									//LCP�����񐔂�o�^����
	phScene = phSdk->CreateScene(sceneDesc);						//�����V�~�����[�^�̒��Ƀf�B�X�N���v�^�̕ϐ�����ꂽ�V�[���O���t���쐬
	createFloor();													//�V�[���O���t���ɏ������
	CreateBody();													//�V�[���O���t���ɑ̂����

	glutInit(&argc, argv);											//GLUT������������
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);		//GLUT�̃f�B�X�v���C���[�h���w�肷��
	glutInitWindowSize(480, 320);									//OpenGL��Window�T�C�Y���w�肷��
	
	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//window1 : simulator window
	mainWindow = glutCreateWindow("Horse body Simulator");			//OpenGL��Window�^�C�g�����w�肷��
	glutPositionWindow(10, 30);
	///---- GLUI�̍쐬
	glui = GLUI_Master.create_glui("Menu", 0, 510, 30);				//< �ʃE�B���h�E���쐬����Ƃ��͂�����
	//glui = GLUI_Master.create_glui_subwindow(window, GLUI_SUBWINDOW_RIGHT);			//< �E�B���h�E���ɍ쐬����Ƃ��͂�����
	guiLoadScene();
	DesignGUI();
	guiLoadInit();

	// GLUT���Ȃ��݂̊֐��̓o�^(GLUI>GLUT>OpenGL)
	GLUI_Master.set_glutDisplayFunc(GLUT_Display);
	GLUI_Master.set_glutIdleFunc(GLUT_Idle);
	GLUI_Master.set_glutTimerFunc(simulationPeriod, GLUT_Timer, 0);
	GLUI_Master.set_glutKeyboardFunc(GLUT_Keyboard);
	GLUI_Master.set_glutReshapeFunc(GLUT_Reshape);
	
	glui->set_main_gfx_window(mainWindow);	

	//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//Springhead��render�̓o�^
	grRender = grSdk->CreateDebugRender();				//GraphicSDK�Ńf�o�b�O��񃌃��_���[���쐬����
	grDevice = grSdk->CreateDeviceGL();					//OpenGL �����_���[�p�f�o�C�X���쐬����
	grDevice->Init();									//OpenGL �����_���[�̏�����������

	grRender->SetDevice(grDevice);						// ���������������̂�GLUT���Ȃ��݂̊֐����f�o�C�X�ɐݒ�
	
	//	�J�����̐ݒ�
	GRCameraDesc cam;									//�`�悷�邽�߂ɂ̓J�������v��̂Ńf�B�X�N���v�^�̐錾������
	cam.size = Vec2d(0.05, 0);							//�f�B�X�N���v�^�ɃJ�����̑傫�����㏑������
	grRender->SetCamera(cam);							//�J�����f�B�X�N���v�^�Őݒ肵������o�^����B
	
	initialize();										//�J�����̎g�p���i�����Ƃ��j�̏�����

	//�@���_�̐ݒ�
	view.Pos() = Vec3f(6.0f*cos(Rad(camAngle)), 3.0f, 6.0f*sin(Rad(camAngle))) * camZoom;	//�J�����̍��W���w�肷��
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));								//�J������������W, ������x�N�g�� 
	grRender->SetViewMatrix(view.inv());													//�J�������_��o�^����

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
	//�@OpenGL�̃��C�����[�v
	cout << "************************" << endl;
	cout << "* horse body simulator *" << endl;
	cout << "************************" << endl;
	cout << endl;
	cout << "< Ver 0.7 beta > Last update: 2008.01.10 Toki" << endl;
	cout << "This program is an easy sample of GLUI." << endl;
	glutMainLoop();													//GLUT�̃��[�v����

	return 0;
}