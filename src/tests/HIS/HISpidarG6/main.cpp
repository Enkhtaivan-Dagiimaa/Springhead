/**
 Springhead2/src/tests/HIS/HISpidarG6/main.cpp
�y�T�v�z
  �O���t�B�b�N�X�����_���[�N���X�� DrawScene API���g���A�V�[�����ꊇ�Ń����_�����O����B
  DrawScene API �Őݒ肳��Ă���}�e���A���}�e���A���T���v����p���āA�J���t���ȃ{�b�N�X�������_�����O����B�@
  �܂��ASPIDAR�ƂȂ��A�͊o�C���^���N�V�������s���i�J�����j

�y�I����z
  �E5000�X�e�b�v��ɋ����I���B 

 */

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <GL/glut.h>

#include "HIS/HIS.h"
#include <iostream>
#include <stdio.h>

#include <mmsystem.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;

#define ESC				27			// ESC key
#define EXIT_TIMER		7000		// ���s�X�e�b�v��
#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define NUM_SPHERES		2			// sphere��

#define HAPTIC_FREQ		20			// �͊o�X���b�h�̎���(msec)

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
#include <Device/DRUsb20Simple.h>
#include <Device/DRUsb20Sh4.h>
#include <conio.h>

const float K = 2000;
const float B = 0;
const float dt = 0.016f;
#endif

GRSdkIf* grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

PHSdkIf* phSdk;
PHSceneIf* scene;
PHSolidIf* soFloor;
std::vector<PHSolidIf*> soSphere; 

HISpidarG6X3 spidarG6;						//	SPIDAR�ɑΉ�����N���X
Vec3f spidar_pos = Vec3f();

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	render->ClearBuffer();
	render->DrawScene(scene);

	// SPIDAR�̈ʒu��\������
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(spidar_pos.x, spidar_pos.y, spidar_pos.z);
	glutSolidSphere(0.01, 18, 9);
	glPopMatrix();

	render->EndScene();

	// �r�f�I���[�g�ɂ��킹�邽�߂�60Hz�ɂ���
	Sleep(16.66);
}

/**
 brief		�����̐ݒ�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void setLight() {
	GRLightDesc light0, light1;
	light0.position = Vec4f(10.0, 20.0, 20.0, 1.0);
	light1.position = Vec4f(-10.0, 10.0, -20.0, 1.0);
	render->PushLight(light0);
	render->PushLight(light1);
}
/**
 brief		glutReshapeFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		<in/--> w�@�@��
 param		<in/--> h�@�@����
 return		 �Ȃ�
 */
void reshape(int w, int h){
	// Viewport�Ǝˉe�s���ݒ�
	render->Reshape(Vec2f(), Vec2f(w,h));
}

MMRESULT FTimerId;

/**
 brief 		glutKeyboardFunc�Ŏw�肵���R�[���o�b�N�֐� 
 param		<in/--> key�@�@ ASCII�R�[�h
 param 		<in/--> x�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 param 		<in/--> y�@�@�@ �L�[�������ꂽ���̃}�E�X���W
 return 	�Ȃ�
 */
void keyboard(unsigned char key, int x, int y){
	if (key == ESC) 
	{
		timeKillEvent(FTimerId);
		timeEndPeriod(1);
		std::cout << "exit operation from key input" << std::endl;
		exit(0);
	}
	else if(key == 'c')
	{
		// c�ŃL�����u���[�V����
		spidarG6.Calib();
	}
}	
/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	scene->Step();
	glutPostRedisplay();
	static int count;
	count++;
	if (++count > EXIT_TIMER) 
	{
		timeKillEvent(FTimerId);
		timeEndPeriod(1);
		std::cout << "exit by count" << std::endl;
		exit(0);
	}
}

static int sec_counter = 0;
static int one_sec = 1000 / HAPTIC_FREQ;

void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	spidarG6.Update(dt);							//	USB�̒ʐM���s���D

	spidar_pos = spidarG6.GetPos();					// spidar�̈ʒu���i�[���Ă���ϐ��̍X�V

	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % one_sec == 0)
	{
		std::cout << spidar_pos << std::endl;
		sec_counter = 0;
	}
	sec_counter++;									// �J�E���^�[�̍X�V

#if 0
		float delta = (pos.Y() - lastPos) / dt;
		float alpha = 0.1f;
		vel = (1-alpha)*vel + alpha*delta;
		lastPos = spidar_pos.Y();
		Quaternionf qt = spidarG6.GetOri();			//	�p�x�̓ǂݏo�� 4���� Quaternionf �ɂ��ẮC���������}�j���A�����Q��
		float force=0;
		Vec3f torque;
		for(int i=0; i<NVTX; ++i){
			Vec3f lp = qt*grip[i];
			Vec3f p = lp + pos;
			const float WALL = -0.001f;
			if (p.Y() < WALL){
				force += (WALL-p.Y())*K + B*vel;
				torque += lp ^ Vec3f(0, force, 0);
			}
		}
		spidarG6.SetForce(Vec3f(0, force, 0), torque);	//	�͂̎擾
#endif
}


/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){
	phSdk = CreatePHSdk();					// SDK�̍쐬�@
	PHSceneDesc sd;
	sd.contactSolver = PHSceneDesc::SOLVER_CONSTRAINT;
	sd.timeStep = 0.01;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;

	// Solid�̍쐬
	unsigned int sphereCnt;
	for (sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}

	desc.mass = 1e20f;
	desc.inertia *= 1e20f;
	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetGravity(false);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;
	{
		CDSphereDesc sd;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));

		CDBoxDesc bd;
		bd.boxsize = Vec3f (30.0, 1.0, 30.0);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	soFloor->AddShape(floor);
	soFloor->SetFramePosition(Vec3f(0,-3,0));
	for (sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere[sphereCnt]->AddShape(sphere);
		soSphere[sphereCnt]->SetFramePosition(Vec3f(0, 15+5*sphereCnt, 0));
	}
	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINSIZE_WIDTH, WINSIZE_HEIGHT);
	int window = glutCreateWindow("GRSimple");
	grSdk = CreateGRSdk();
	render = grSdk->CreateDebugRender();
	grDevice = grSdk->CreateDeviceGL(window);

	// �����ݒ�
	grDevice->Init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	
	render->SetDevice(grDevice);	// �f�o�C�X�̐ݒ�

	// ���_��ݒ肷��
	Affinef view;
	view.Pos() = Vec3f(0.0, 5.0, -15.0);								// eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));			// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);
	
	setLight();

	DVDeviceManager devMan;						//	D/A��J�E���^�Ȃǂ̃f�o�C�X�̊Ǘ�

	devMan.RPool().Register(new DRUsb20Simple(10));	//	USB2.0�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(0));		//	Sh4�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(1));		//	Sh4�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(2));		//	Sh4�ŃR���g���[�� 8���[�^

	devMan.Init();								//	�f�o�C�X�̏�����
	std::cout << devMan;						//	�������̌��ʂ�\��
	
	spidarG6.Init(devMan, false);			//	SPIDAR�̏������C�L�����u���[�V����������D

#ifdef __sh__
	cyg_thread_delay(10000);
#else
	Sleep(1000);					
#endif
	
		spidarG6.Calib();				//	�L�����u���[�V����

#define NVTX	4					//	�Փ˔��肷��_
	Vec3f grip[NVTX]={
		Vec3f( 0.02f, 0.0f, 0.02f),
		Vec3f(-0.02f, 0.0f, 0.02f),
		Vec3f( 0.02f, 0.0f,-0.02f),
		Vec3f(-0.02f, 0.0f,-0.02f)
	};

	float lastPos=0;
	float vel=0;

	timeBeginPeriod(1//����\�̍ŏ��l
            );

	// haptic�X���b�h�̐����E�J�n
	FTimerId = timeSetEvent(HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
                        1,   // ���ԕ���\
                        TimerProc,//�R�[���o�b�N�֐�
                        (DWORD)&time,//���[�U�[��`�̒l
                        TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
                        );

	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete phSdk;
}