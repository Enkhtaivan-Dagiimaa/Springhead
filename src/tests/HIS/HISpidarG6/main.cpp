/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 Springhead2/src/tests/HIS/HISpidarG6/main.cpp
�y�T�v�z
  �O���t�B�b�N�X�����_���[�N���X�� DrawScene API���g���A�V�[�����ꊇ�Ń����_�����O����B
  DrawScene API �Őݒ肳��Ă���}�e���A���}�e���A���T���v����p���āA�J���t���ȃ{�b�N�X�������_�����O����B�@
  �܂��ASPIDAR�ƂȂ��A���̋y�ђn�ʂƗ͊o�C���^���N�V�������s���B
  
�y�I����z
  �E5000�X�e�b�v��ɋ����I���B 
 */

#include <Springhead.h>		//	Springhead�̃C���^�t�F�[�X
#include <ctime>
#include <string>
#include <GL/glut.h>
#include <iostream>
#include <stdio.h>

#include "HIS/HIS.h"
#include "Physics/PHConstraintEngine.h"
#include "Physics/PHConstraint.h"
#include "Physics/PHContactPoint.h"

#include <mmsystem.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;

#define ESC				27			// ESC key
#define EXIT_TIMER		5000		// ���s�X�e�b�v��
#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define NUM_SPHERES		10			// sphere��

#define HAPTIC_FREQ		10			// �͊o�X���b�h�̎���(1/msec)
#define POINTER_RADIUS  1.0			// �͊o�|�C���^�̔��a

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
#include <Device/DRUsb20Simple.h>
#include <Device/DRUsb20Sh4.h>
#include <conio.h>

// virtual coupling�̌W��
const float K = 60;
const float B = 30;
const float dt = HAPTIC_FREQ;
#endif

// �O���t�B�b�N�p�̕ϐ�
GRSdkIf* grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

// �����v�Z�p�̕ϐ�
PHSdkIf* phSdk;
PHSceneIf* scene;
PHSolidIf* soFloor;
std::vector<PHSolidIf*> soSphere; 

// SPIDAR�p�̕ϐ�
DVDeviceManager devMan;						//	D/A��J�E���^�Ȃǂ̃f�o�C�X�̊Ǘ�
HISpidarG6X3 spidarG6;						//	SPIDAR�ɑΉ�����N���X

// ���̑��̕ϐ�
Vec3f spidar_pos = Vec3f();
Vec3f sphere_pos = Vec3f();
Affinef pos_affine;
Affinef view;

bool bforce = false;

int time_counter = 0;
int sec_counter = 0;
const int one_sec = 1000 / HAPTIC_FREQ;

MMRESULT FTimerId;


#define NVTX	4			//	�Փ˔��肷��_
static Vec3f grip[NVTX]={
		Vec3f( 0.02f, 0.0f, 0.02f),
		Vec3f(-0.02f, 0.0f, 0.02f),
		Vec3f(-0.02f, 0.0f,-0.02f),
		Vec3f( 0.02f, 0.0f,-0.02f)
	};

int index = 0;
double global_collision_counter = 0;

std::map<PHConstraint*, int> process_map;

/**
 brief     	calculate_surround_effect����Ă΂��ċA�I�ɏ����v�Z����֐�
 param		�Ȃ�
 return 	�Ƃ肠��������Vec3f��Ԃ��Ă���i�ύX�\��j
*/
Vec3f calculate_counteractive_object_accel(PHConstraints cs, int depth, PHSolid* solid)
{
	// ���ׂĂ̐ڐG�ɂ��ď���
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
	{
		// ���݂̒l�����łɏ������ꂽ���`�F�b�N
		std::map<PHConstraint*, int>::iterator t = process_map.find(*it);

		// �܂���������Ă��Ȃ�
		if(t == process_map.end())
		{
			// ���̐ڐG�̕Е��������Ƃ������̂ł���ꍇ�ċA�I�ɂ��̊֐����Ăя������s��
			if((*it)->solid[0]->solid == solid)
			{
				//�@���łɏ��������ڐG���}�b�v�ɓo�^����
				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));

				// �ċA�I�Ɏ��̏����ɓn��
				calculate_counteractive_object_accel(cs, depth+1, (*it)->solid[1]->solid);
			}
			else if((*it)->solid[1]->solid == solid)
			{
				//�@���łɏ��������ڐG���}�b�v�ɓo�^����
				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));

				// �ċA�I�Ɏ��̏����ɓn��
				calculate_counteractive_object_accel(cs, depth+1, (*it)->solid[0]->solid);
			}
		}
	}

	// �����ɋ�̓I�ȏ�����ǉ�����----------------------------------------------------------

	return Vec3f();
}

/**
 brief     	�ڐG�_�̏�񂩂���͂̏��ɂ��e�����v�Z����֐�
 param		�Ȃ�
 return 	���͂̉e�����s��Ƃ��ĕԂ�
*/
Matrix3d calculate_surround_effect(PHConstraints cs)
{
	if(cs.size() > 0)
	{
		// �ŋߖT���̂̎擾
		PHSolidIf *nearest = soSphere[index];

		// depth�ϐ��̐錾
		int depth = 1;

		// �����`�F�b�N�Ɏg���}�b�v��������
		process_map.clear();

		// ���ׂĂ̏Փ˂ɂ��ă`�F�b�N
		for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
		{
			// ���݂̒l�����łɏ������ꂽ���`�F�b�N
			std::map<PHConstraint*, int>::iterator t = process_map.find(*it);
	
			// �܂���������Ă��Ȃ�
			if(t == process_map.end())
			{
				// solid1���ŋߖT����
				if(((*it)->solid[0]->solid) == nearest)
				{
					//�@���łɏ��������ڐG�_���}�b�v�ɓo�^����
					process_map.insert(std::pair<PHConstraint*, int>(*it, depth));

					// �ċA�I�Ɏ��̏����ɓn��	
					calculate_counteractive_object_accel(cs, depth+1, (*it)->solid[1]->solid);
//					DSTR << "solid1 is nearest" << std::endl;
				}
				// solid2���ŋߖT����
				else if(((*it)->solid[1]->solid) == nearest)
				{
					//�@���łɏ��������ڐG�_���}�b�v�ɓo�^����
					process_map.insert(std::pair<PHConstraint*, int>(*it, depth));

					// �ċA�I�Ɏ��̏����ɓn��
					calculate_counteractive_object_accel(cs, depth+1, (*it)->solid[0]->solid);
//					DSTR << "solid2 is nearest" << std::endl;
				}
				else
				{
					//�@�ŋߖT���̂��Ȃ��ꍇ�̃f�o�b�O�o��
//					DSTR << "no object is nearest " << std::endl;
				}
			}
		}
	}

	// �����ɋ�̓I�ȏ�����ǉ�����----------------------------------------------------------

	return Matrix3d();
}

/**
 brief     	���̂̐ڐG�_�̏���\������֐�
 param		�Ȃ�
 return 	�Ȃ�
*/
void show_collision_info(PHConstraints cp)
{
	// �ڐG�_�����݂���Ώ�������
	if(cp.size() > 0)
	{
		int counter = 0;
		DSTR << "----------NEW COLLISION DATA No." << global_collision_counter++ << "-----------" << std::endl;
		
		// ���ׂĂ̏Փ˂ɑ΂��ď���
		for(PHConstraints::iterator it = cp.begin(); it != cp.end(); it++)
		{
			// ���[�v���̃J�E���^�[���X�V
			counter++;
			DSTR << "COLLISION" << counter << ": " << std::endl;


			// �ŋߖT���̂̃A�h���X���擾
			PHSolidIf* nearest_address = &(*soSphere[index]);

			// PHContactPoint��dynamic cast���������̃`�F�b�N
			PHContactPoint* contact = DCAST(PHContactPoint, (*it));
			DSTR << "collision pos = " << contact->pos << std::endl;
//			DSTR << "address of solid1 = " << &(*contact->solid[0]->solid) << std::endl;
//			DSTR << "address of solid2 = " << &(*contact->solid[1]->solid) << std::endl;
			DSTR << std::endl;

			// PHConstraint�̏��̕\��
			// solid1
			DSTR << "solid1.mass = " << (*it)->solid[0]->solid->mass << std::endl;
			DSTR << "solid1.inertia = " << std::endl << (*it)->solid[0]->solid->inertia;
			DSTR << "solid1.velocity = " << (*it)->solid[0]->solid->GetVelocity()<< std::endl;
			DSTR << "solid1.angular_velocity = " << (*it)->solid[0]->solid->GetAngularVelocity() << std::endl;
			DSTR << "solid1.position = " << (*it)->solid[0]->solid->GetFramePosition() << std::endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[0]->solid->GetCenterPosition()).unit() << std::endl;
//			DSTR << "solid1.address = " << ((*it)->solid[0]->solid) << std::endl;
			if(nearest_address == ((*it)->solid[0]->solid))DSTR << " --this solid is nearest object!!" << std::endl;

			DSTR << std::endl;

			// solid2
			DSTR << "solid2.mass = " << (*it)->solid[1]->solid->mass << std::endl;
			DSTR << "solid2.inertia = " << std::endl << (*it)->solid[1]->solid->inertia;
			DSTR << "solid2.velocity = " << (*it)->solid[1]->solid->GetVelocity()<< std::endl;
			DSTR << "solid2.angular_velocity = " << (*it)->solid[1]->solid->GetAngularVelocity() << std::endl;
			DSTR << "solid2.position = " << (*it)->solid[1]->solid->GetFramePosition() << std::endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[1]->solid->GetCenterPosition()).unit() << std::endl;
//			DSTR << "solid2.address = " << ((*it)->solid[1]->solid) << std::endl;
			if(nearest_address == ((*it)->solid[1]->solid))DSTR << " --this solid is nearest object!!" << std::endl;

			DSTR << std::endl;

//			DSTR << "index = " << index << "; address = " << &(*soSphere[index]) << std::endl;

			DSTR << std::endl;
		}
	}
}

/**
 brief     	���̂̐ڐG�_���擾����֐��iAPI�Ɉړ��\��j
 param		�Ȃ�
 return 	�ڐG�_�̏W��
*/
PHConstraints GetContactPoints()
{
	return scene->GetConstraintEngine()->GetContactPoints();
}


/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	render->ClearBuffer();
	render->DrawScene(scene);

	// SPIDAR�̈ʒu�����`�ŕ\������
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	Posef::Unit(spidar_pos).ToAffine(pos_affine);
	glMultMatrixf(pos_affine);
	glutSolidSphere(POINTER_RADIUS, 12, 9);
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
	// toggle force output
	else if(key == ' ')
	{
		bforce = !bforce;
	}
}

double dist;
double min_dist;
double d = 10000;
Vec3f so_pos;

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	scene->Step();

	// �|�C���^�̍ŋߖT�̕��̂�T������
	min_dist = 10000;

	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		so_pos = soSphere[sphereCnt]->GetFramePosition();

		dist = (spidar_pos - so_pos).norm();

		if(dist < min_dist)
		{
			// �ŋߖT�̕��̂��擾
			min_dist = dist;
			sphere_pos = so_pos; 
			index = sphereCnt;
		}
	}

	// �ŏ��̋�����ݒ�
	d = min_dist;

	// �Փ˓_���擾
	PHConstraints cs = GetContactPoints();

	// �Փ˓_����\��
//	show_collision_info(cs);
	
	// �Փ˓_������͂̉e�����v�Z
	calculate_surround_effect(cs);

	glutPostRedisplay();

	time_counter++;

	if (++time_counter > EXIT_TIMER) 
	{
		timeKillEvent(FTimerId);
		timeEndPeriod(1);
		std::cout << "exit by count" << std::endl;
		exit(0);
	}
}

Vec3f lastPos = Vec3f();
Vec3f vel = Vec3f();
Vec3f delta = Vec3f();
const float alpha = 0.1f;
Quaternionf qt;
Vec3f force;
Vec3f torque;
const float WALL = -0.0f;
Vec3f lp;
Vec3f p;

/**
 brief  	�񎦗͂̌v�Z��Haptic Device	�֔��f. multimedia timer�ɂ���ČĂяo�����
 param	 	�Ȃ�
 return 	�Ȃ�
*/
void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	spidarG6.Update(dt);							//	USB�̒ʐM���s���D
	spidar_pos = view.inv().Rot() * spidarG6.GetPos() * 60;		// SPIDAR�̍��W�����[���h���W�ł͂Ȃ��̂Ń��[���h���W�ɂ���

#if _DEBUG
	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % one_sec == 0)
	{
//		std::cout << "spidar position = " << spidar_pos << std::endl;
//		std::cout << "force = " << force << "torque = " << torque << std::endl;
//		std::cout << "distance = " << d << std::endl;
		sec_counter = 0;	
	}
	sec_counter++;									// �J�E���^�[�̍X�V
#endif

	// ���x�E�p���̗p��
	delta = (spidar_pos - lastPos) / dt;
	vel = (1-alpha)*vel + alpha*delta;
	lastPos = spidar_pos;
	qt = spidarG6.GetOri();			//	�p�x�̓ǂݏo�� 4���� Quaternionf �ɂ��ẮC���������}�j���A�����Q��

	// �ϐ��̏�����
	force = Vec3f();
	torque = Vec3f();

	// �񎦗͂̌v�Z --> solid���g�������@�ɕς���
	for(int i=0; i<NVTX; ++i){
		lp = qt*grip[i];
		p = lp + spidar_pos;

		// �n�ʂƂ̐ڐG���v�Z
		if (p.y-POINTER_RADIUS < WALL){
			// virtual coupling�Œ񎦗͂̌v�Z
			double temp_force = (WALL - p.y + POINTER_RADIUS) * K + B * vel.y;
			force.y += temp_force;
			torque += lp ^ Vec3f(0, temp_force, 0);
		}

		// ���̂Ƃ̐ڐG���v�Z
		if(d < DCAST(CDSphereIf, soSphere[index]->GetShape(0))->GetRadius() + 1.0)
		{
			Vec3f temp_force = (Vec3f(3, 3, 3) + p - sphere_pos) + (vel - soSphere[index]->GetVelocity());
		
			force += -temp_force;
			torque += lp ^ (-temp_force);

			soSphere[index]->AddForce(-temp_force, lp ^ (-temp_force));
		}
	}

	// grip�ɗ͂𔭐�������
	if(bforce)spidarG6.SetForce(force, torque);
	else spidarG6.SetForce(Vec3f(), Vec3f());
}

/**
 brief  	device manager�̏��������s���֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void InitDeviceManager()
{
	devMan.RPool().Register(new DRUsb20Simple(10));	//	USB2.0�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(0));		//	Sh4�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(1));		//	Sh4�ŃR���g���[�� 8���[�^
	devMan.RPool().Register(new DRUsb20Sh4(2));		//	Sh4�ŃR���g���[�� 8���[�^

	devMan.Init();								//	�f�o�C�X�̏�����
	std::cout << devMan;						//	�������̌��ʂ�\��	
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
//	sd.ContactMode = PHSceneDesc::ContactMode::MODE_LCP;
	sd.timeStep = 0.017;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 2.0;
	desc.inertia *= 2.0;

	// Solid�̍쐬
	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}

	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;

	{
		CDSphereDesc sd;
		sd.radius = 2.0;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));

		CDBoxDesc bd;
		bd.boxsize = Vec3f (30.0, 1.0, 30.0);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	soFloor->AddShape(floor);
	soFloor->SetFramePosition(Vec3f(0,-0.5,0));

	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
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
	view.Pos() = Vec3f(0.0, 5.0, -20.0);								// eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));			// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);
	
	setLight();

	InitDeviceManager();
	spidarG6.Init(devMan, false);			//	SPIDAR�̏������C�L�����u���[�V����������D

	timeBeginPeriod(1);							//����\�̍ŏ��l
/*
	// haptic�X���b�h�̐����E�J�n
	FTimerId = timeSetEvent(HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
                        1,   // ���ԕ���\
                        TimerProc,//�R�[���o�b�N�֐�
                        (DWORD)&time,//���[�U�[��`�̒l
                        TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
                        );
*/
	// openGL main loop
	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete phSdk;
}