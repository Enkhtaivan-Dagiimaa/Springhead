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
#define EXIT_TIMER		10000		// ���s�X�e�b�v��
#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define NUM_SPHERES		1			// sphere��

// �͊o�X���b�h�̎���Hz(1000/HAPTIC_FREQ)
#ifdef _DEBUG
#define HAPTIC_FREQ		100 
#elif _OPT
#define HAPTIC_FREQ		2
#elif _WINDOWS
#define HAPTIC_FREQ		3
#endif

// �񎦗͂̃o�l�_���p�W��
#define K_force			100
#define B_force			0.5

// SPIDAR�X�V��
const float dt = 0.001f * HAPTIC_FREQ;

// ��b�ŏ��������
const int one_sec = 1000 / HAPTIC_FREQ;

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
	#include <Device/DRUsb20Simple.h>
	#include <Device/DRUsb20Sh4.h>
	#include <conio.h>

	// virtual coupling�̌W��
	#ifdef _DEBUG
		const float K = 2000;
		const float B = 120;
	#elif _OPT
		const float K = 5;
		const float B = 5;
	#elif _WINDOWS
		const float K = 20;
		const float B = 25;
	#endif
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
PHSolidIf* soPointer;

// SPIDAR�p�̕ϐ�
DVDeviceManager devMan;						//	D/A��J�E���^�Ȃǂ̃f�o�C�X�̊Ǘ�
HISpidarG6X3 spidarG6;						//	SPIDAR�ɑΉ�����N���X

// ���̑��̕ϐ�
Vec3f spidar_pos = Vec3f();
Affinef view;
Matrix3f view_rot;
bool bforce = false;
MMRESULT FTimerId;

// ���͂̉e���̌v�Z�Ɏg���ϐ�
Matrix3d effect[NUM_SPHERES+1];
Vec3d constant[NUM_SPHERES+1];

// �ċA�v�Z�����������i�[����e�[�u��
//std::map<PHConstraint*, int> process_map;
PHConstraint* process_map[NUM_SPHERES+1];
int num_process = 0;

// �|�C���^�ƐڐG�����\���b�h
PHSolid* nearest_solids[NUM_SPHERES+1];
int num_collisions = 0;

PHConstraint* nearest_constraints[NUM_SPHERES+1];

/**
 brief     	calculate_surround_effect����Ă΂�A�ڐG�͂��ċA�I�Ɍv�Z����֐�
 param		�Ȃ�
 return 	�Ăяo�����̍��̂ƐڐG���鍄�̂̐ڐG�͂�Ԃ�
*/
void calculate_counteractive_object_force(PHConstraints cs, int depth, PHSolid* solid, Vec3d original_r, Vec3d *output_force)
{
	std::vector<Vec3d> forces;
	std::vector<Vec3d> vec_r;

	// ���ׂĂ̐ڐG�ɂ��ď���
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
	{
		bool process = false;

		// ���݂̒l�����łɏ������ꂽ���`�F�b�N
		for(int i = 0; i < num_process; i++)
		{
			if(process_map[i] == *it)process = true;
		}
//		std::map<PHConstraint*, int>::iterator t = process_map.find(*it);

		// �܂���������Ă��Ȃ�
		if(process == false)
		{
			// ���̐ڐG�̕Е��������Ƃ������̂ł���ꍇ�ċA�I�ɂ��̊֐����Ăя������s��
			if((*it)->solid[0]->solid == solid)
			{
				//�@���łɏ��������ڐG���}�b�v�ɓo�^����
//				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
				process_map[num_process++] = *it;

				// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				Vec3d f = Vec3d();
				Vec3d r = contact->pos - (*it)->solid[1]->solid->GetCenterPosition();

				// �ċA�I�Ɏ��̏����ɓn��
				calculate_counteractive_object_force(cs, depth+1, (*it)->solid[1]->solid, r, &f);

				// ����ꂽ���̕ۑ�
				forces.push_back(f);
				vec_r.push_back(r);
			}
			else if((*it)->solid[1]->solid == solid)
			{
				//�@���łɏ��������ڐG���}�b�v�ɓo�^����
//				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
				process_map[num_process++] = *it;

				// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				Vec3d f = Vec3d();
				Vec3d r = contact->pos - (*it)->solid[0]->solid->GetCenterPosition();

				// �ċA�I�Ɏ��̏����ɓn��
				calculate_counteractive_object_force(cs, depth+1, (*it)->solid[0]->solid, r, &f);

				// ����ꂽ���̕ۑ�
				forces.push_back(f);
				vec_r.push_back(r);
			}
		}
	}

	// �ϐ��̗p��
	Vec3d sum_r = Vec3d();
	Vec3d sum_force = Vec3d();
	Vec3d sum_r_out_f = Vec3d();

	for(int i = 0; i < (int)forces.size(); i++) 
	{
		Vec3d r = (Vec3d)vec_r.at(i);
		Vec3d f = (Vec3d)forces.at(i);

		// ����ꂽ�l�̍��v�l���v�Z
		// ���̂ɉ����͂𐳂Ƃ��A
		// �O�̍��̂��瓾��ꂽ�͍͂�p������͂Ȃ̂ŕ��ɂ���
		sum_r = sum_r + r;
		sum_force = sum_force + (- f) ;

		//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
		sum_r_out_f = sum_r_out_f + r ^ (- f);
	}

	// sum_r�����͕Ԃ��͂ɑΉ����邒��������
	sum_r = sum_r + original_r;

	// �܂Ƃ߂��镔���͂܂Ƃ߂�
	Vec3d A = solid->GetAcceleration() - (solid->GetInertiaInv() * sum_r_out_f) ^ sum_r + solid->GetTorque() * solid->GetTorque() * sum_r - solid->GetMassInv() * sum_force;

	Matrix3d T = solid->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);
	Matrix3d M = solid->GetMassInv() * Matrix3d().Unit() + Matrix3d(T.Ex() ^ sum_r, T.Ey() ^ sum_r, T.Ez() ^ sum_r);

	// �������͂�Ԃ�
	*output_force = M.inv() * A;
}

/**
 brief     	�ڐG�_�̏�񂩂���͂̏��ɂ��e�����v�Z����֐�
 param		���̎��_�ł̏Փˏ��PHConstraints�ƌ��ʊi�[�p��Matrix3d��Vec3d���Ƃ�B
			���͂̉e����Effect�ɁA�萔����Constant�Ɋi�[����
			�܂��APHSolid* solids[]�Ƀ|�C���^���ڂ��Ă��鍄�̂̃��X�g������
 return 	pointer�ɐڂ��Ă��镨�̂̌���Ԃ�
*/
int calculate_surround_effect(PHConstraints cs, Matrix3d *Effect, Vec3d *Constant, PHSolid* solids[NUM_SPHERES+1]) 
{
	std::vector<Vec3d> forces;
	std::vector<Vec3d> vec_r;

	Vec3d vec_r_pointer[NUM_SPHERES+1];
	int num_solids = 0;

	// �Փ˂̐����`�F�b�N
	if(cs.size() > 0)
	{
		// �[���̏�����
		int depth = 1;

		// �����σ}�b�v�̏�����
//		process_map.clear();
		for(int i = 0; i < num_process; i++)
		{
			process_map[i] = NULL;
		}
		num_process = 0;

		// pointer�ɐڂ��Ă��鍄�̂��������A�擾����
		for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
		{
			// �ڐG�̕Е��̍��̂��|�C���^
			if((*it)->solid[0]->solid == soPointer)
			{
				// ���̍��̂�ۑ�
				solids[num_solids] = (*it)->solid[1]->solid;
				nearest_constraints[num_solids] = *it;

				// ���̍��̂̒��S����͂̍�p�_�܂ł̃x�N�g����ۑ�
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				vec_r_pointer[num_solids++] = contact->pos - (*it)->solid[1]->solid->GetCenterPosition();

				// �����σ}�b�v�ɓo�^
//				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
				process_map[num_process++] = *it;
			}
			// �ڐG�̕Е��̍��̂��|�C���^
			else if((*it)->solid[1]->solid == soPointer)
			{
				// ���̍��̂�ۑ�
				solids[num_solids] = (*it)->solid[0]->solid;
				nearest_constraints[num_solids] = *it;

				// ���̍��̂̒��S����͂̍�p�_�܂ł̃x�N�g����ۑ�
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				vec_r_pointer[num_solids++] = contact->pos - (*it)->solid[0]->solid->GetCenterPosition();

				// �����σ}�b�v�ɓo�^
//				process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
				process_map[num_process++] = *it;
			}
		}

		// �����|�C���^�ɐڂ��鍄�̂��Ȃ����
		// �͊o�ɉe�����Ȃ��̂Ń��^�[������
		if(num_solids == 0)
		{
			return 0;
		}

		// pointer�ɐڂ��Ă��鍄�̂��ڂ��Ă��鍄�̂��������A
		// ���ꂼ�ꏈ�����Ă���
		for(int j = 0; j < num_solids; j++)
		{
			// ��قǓo�^�������̂��擾
			PHSolidIf* solid = (PHSolidIf*)solids[j];

			// ���̍��̂ɂ��ďՓ˂̃��X�g���猟��
			for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
			{
				// �����ς݂��ǂ�������
//				std::map<PHConstraint*, int>::iterator t = process_map.find(*it);

				bool process = false;

				for(int i = 0; i < num_process; i++)
				{
					if(process_map[i] == *it)process = true;
				}

				// �����ς݃}�b�v����͌�����Ȃ�����
				if(process == false)
				{
					// ���̍��̂��܂܂ꂽ�ڐG
					if((*it)->solid[0]->solid == solid)
					{
						// ���̐ڐG�_�������σ}�b�v�ɓo�^����
//						process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
						process_map[num_process++] = *it;

						// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
						PHContactPoint* contact = DCAST(PHContactPoint, (*it));
						Vec3d f = Vec3d();
						Vec3d r = contact->pos - (*it)->solid[1]->solid->GetCenterPosition();

						// �ċA�I�Ɏ��̏����ɓn��	
						calculate_counteractive_object_force(cs, depth+1, (*it)->solid[1]->solid, r, &f);

						// ����ꂽ�l�̕ۑ�
						forces.push_back(f);
						vec_r.push_back(r);
					}
					// ���̍��̂��܂܂ꂽ�ڐG
					else if((*it)->solid[1]->solid == solid)
					{
						//�@���̐ڐG�_�������σ}�b�v�ɓo�^����
//						process_map.insert(std::pair<PHConstraint*, int>(*it, depth));
						process_map[num_process++] = *it;

						// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
						PHContactPoint* contact = DCAST(PHContactPoint, (*it));
						Vec3d f = Vec3d();
						Vec3d r = contact->pos - (*it)->solid[0]->solid->GetCenterPosition();

						// �ċA�I�Ɏ��̏����ɓn��
						calculate_counteractive_object_force(cs, depth+1, (*it)->solid[0]->solid, r, &f);

						// ����ꂽ�l�̕ۑ�
						forces.push_back(f);
						vec_r.push_back(r);
					}
				}
			}

			// �ϐ��̗p��
			Vec3d sum_r = Vec3d();
			Vec3d sum_force = Vec3d();
			Vec3d sum_r_out_f = Vec3d();

			for(int i = 0; i < (int)forces.size(); i++) 
			{
				Vec3d r = (Vec3d)vec_r.at(i);
				Vec3d f = (Vec3d)forces.at(i);

				// ����ꂽ�l�̍��v�l���v�Z 
				// ���̂ɉ����͂𐳂Ƃ��A
				// �O�̍��̂��瓾��ꂽ�͍͂�p������͂Ȃ̂ŕ��ɂ���
				sum_r = sum_r + r;
				sum_force = sum_force + (- f);

				//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
				sum_r_out_f = sum_r_out_f + r ^ (- f);
			}

			// �͊o�|�C���^�ƍŋߖT���̂Ƃ̐ڐG�_
			Vec3d original_r = vec_r_pointer[j];

			// sum_r�����͕Ԃ��͂ɑΉ����邒��������
			sum_r = sum_r + original_r;

			// �܂Ƃ߂��镔���͂܂Ƃ߂�
			// �萔��
			// A = -w^2 * sum(r) + (I^(-1) * sum(r x f)) x sum(r) + sum(f) / m
			Vec3d A = (((PHSolid *)soPointer)->GetInertiaInv() * sum_r_out_f) ^ sum_r - ((PHSolid *)soPointer)->GetTorque() * ((PHSolid *)soPointer)->GetTorque() * sum_r + ((PHSolid *)soPointer)->GetMassInv() * sum_force;

			// T = I^(-1) * (r x f)��r���O�ς���s��ɂ��ĊO�ς�r����������
			Matrix3d T = ((PHSolid *)soPointer)->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);
			
			// ���߂�s��
			// M = 1/m * E + (T1 x sum(r) T2 x sum(r) T3 x sum(r))
			Matrix3d M = ((PHSolid *)soPointer)->GetMassInv() * Matrix3d().Unit() + Matrix3d(T.Ex() ^ sum_r, T.Ey() ^ sum_r, T.Ez() ^ sum_r);

			// ���ʂ̊i�[
			Effect[j] = M;
			Constant[j] = A;
		}
		return num_solids;
	}
	// �Փ˂��Ȃ������̂łO��Ԃ��ă��^�[��
	else
	{
		return 0;
	}
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
		// �Փ˂�\������Ƃ��̌��𐔂���J�E���^�[
		static double global_collision_counter = 0;
		int counter = 0;
		DSTR << "----------NEW COLLISION DATA No." << global_collision_counter++ << "-----------" << std::endl;
		
		// ���ׂĂ̏Փ˂ɑ΂��ď���
		for(PHConstraints::iterator it = cp.begin(); it != cp.end(); it++)
		{
			// ���[�v���̃J�E���^�[���X�V
			counter++;
			DSTR << "COLLISION" << counter << ": " << std::endl;

			// PHContactPoint��dynamic cast���������̃`�F�b�N
			PHContactPoint* contact = DCAST(PHContactPoint, (*it));
			DSTR << "collision pos = " << contact->pos << std::endl;
			DSTR << std::endl;

			// PHConstraint�̏��̕\��
			// solid1
			DSTR << "solid1.mass = " << (*it)->solid[0]->solid->mass << std::endl;
			DSTR << "solid1.inertia = " << std::endl << (*it)->solid[0]->solid->inertia;
			DSTR << "solid1.velocity = " << (*it)->solid[0]->solid->GetVelocity()<< std::endl;
			DSTR << "solid1.angular_velocity = " << (*it)->solid[0]->solid->GetAngularVelocity() << std::endl;
			DSTR << "solid1.position = " << (*it)->solid[0]->solid->GetFramePosition() << std::endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[0]->solid->GetCenterPosition()).unit() << std::endl;

			for(int i = 0; i < num_collisions; i++)
			{
				// �ŋߖT���̂̃A�h���X���擾
				PHSolidIf* nearest_address = &(*nearest_solids[i]);
				if(nearest_address == ((*it)->solid[0]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
			}
			DSTR << std::endl;

			// solid2
			DSTR << "solid2.mass = " << (*it)->solid[1]->solid->mass << std::endl;
			DSTR << "solid2.inertia = " << std::endl << (*it)->solid[1]->solid->inertia;
			DSTR << "solid2.velocity = " << (*it)->solid[1]->solid->GetVelocity()<< std::endl;
			DSTR << "solid2.angular_velocity = " << (*it)->solid[1]->solid->GetAngularVelocity() << std::endl;
			DSTR << "solid2.position = " << (*it)->solid[1]->solid->GetFramePosition() << std::endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[1]->solid->GetCenterPosition()).unit() << std::endl;

			for(int i = 0; i < num_collisions; i++)
			{
				// �ŋߖT���̂̃A�h���X���擾
				PHSolid* nearest_address = &(*nearest_solids[i]);
				if(nearest_address == ((*it)->solid[1]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
			}

			DSTR << std::endl;
		}
	}
}

/**
 brief  	�񎦗͂̌v�Z��Haptic Device	�֔��f. multimedia timer�ɂ���ČĂяo�����
 param	 	�Ȃ�
 return 	�Ȃ�
*/
void CALLBACK TimerProc(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// pointer�̈ʒu�̍X�V
	static Vec3d old_pos = Vec3d();
	static Vec3d last_force = Vec3d();
	static Vec3d last_last_force = Vec3d();

	spidarG6.Update(dt);//	USB�̒ʐM���s���D
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

	// pointer�̑��x���v�Z
	Vec3d PointerVel = (spidar_pos - old_pos) / dt;
	old_pos = spidar_pos;

	// spidar���W�܂ł̃x�N�g�����쐬
	Vec3d goal = spidar_pos - soPointer->GetFramePosition();

	// VR��Ԃ̃|�C���^��SPIDAR��virtual coupling�łȂ���
	soPointer->AddForce(K * goal + B * (PointerVel - soPointer->GetVelocity()), Vec3f());

	// --- �����Փ˂̏ꍇ�͂�������ύX���� ---
	// ������͂��v�Z ---> ���������K�v
	
	Vec3d force = Vec3d();
	Vec3d pointer_torque = Vec3d();

	for(int i = 0; i < num_collisions; i++)
	{
		PHConstraint *constraint = nearest_constraints[i];
		if(constraint == NULL)break;
		PHContactPoint* cp = DCAST(PHContactPoint, constraint);
		
		Vec3d local_force = (cp->pos - spidar_pos);
		Vec3d local_torque = (cp->pos - nearest_solids[i]->GetCenterPosition()) ^ local_force;
		nearest_solids[i]->AddForce(local_force, local_torque);

		force = force + local_force;
		pointer_torque = pointer_torque + (cp->pos - soPointer->GetCenterPosition()) ^ (-local_force);
	}

	// spidar�ɗ͂�������
	if(num_collisions > 0)
	{
		// �O��̒񎦗͂ƕ��ς�����ĂԂ����
		if(bforce)spidarG6.SetForce(-(0.6 * force + 0.25 * last_force + 0.15 * last_last_force), pointer_torque);
		else spidarG6.SetForce(Vec3d(), Vec3d());

		// �񎦗͂�ۑ�
		last_last_force = last_force;
		last_force = force;
	}
	else
	{
		// �Փ˂��Ȃ��̂Œ񎦗͂����Z�b�g����
		spidarG6.SetForce(Vec3d());
		last_last_force = last_force;
		last_force = Vec3d();
	}

#if 0
//#if _DEBUG | _WINDOWS
	static int sec_counter = 0;
	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % one_sec == 0)
	{
//		std::cout << "spidar position = " << spidar_pos << std::endl;
//		std::cout << "spidar velocity = " << PointerVel << std::endl;
		std::cout << "force = " << force << "torque = " << pointer_torque << std::endl;
		sec_counter = 0;	
	}
	sec_counter++;									// �J�E���^�[�̍X�V
#endif
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
 brief		����̃\���b�h����d�͂��Ȃ���
 param		�d�͂��Ȃ����\���b�h
 return		�Ȃ�
*/
void RemoveGravity(PHSolidIf* solid)
{
	scene->RemoveGravity(solid);
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	scene->Step();

	// �Փ˓_���擾
	PHConstraints cs = GetContactPoints();

	// �Փ˓_����\��
//	show_collision_info(cs);

	// �Փ˓_������͂̉e�����v�Z
	num_collisions = calculate_surround_effect(cs, effect, constant, nearest_solids);

	// �ĕ`��
	glutPostRedisplay();

/*
	// �o�ߎ��Ԃ��`�F�b�N
	static int time_counter = 0;
	time_counter++;
	if (time_counter > EXIT_TIMER) 
	{
		timeKillEvent(FTimerId);
		timeEndPeriod(1);
		std::cout << "exit by count" << std::endl;
		exit(0);
	}
*/
}

/**
 brief		��͖@�̉e�����󂯂Ȃ����̂�o�^����B���̍��̂����ސڐG�͂��ׂď�������Ȃ�
 param		�o�^���鍄��
 return		�Ȃ�
*/
void AddInteractiveSolid(PHSolidIf* soPointer)
{
	scene->GetConstraintEngine()->AddInteractiveSolid(soPointer);
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	render->ClearBuffer();
	render->DrawScene(scene);

//#if _DEBUG | _WINDOWS
	// SPIDAR�̋O�Ղ��f�o�b�O�o��
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	static Affinef pos_affine;
	Posef::Unit(spidar_pos).ToAffine(pos_affine);
	glMultMatrixf(pos_affine);
	
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, white);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, white);

	glutSolidSphere(0.3, 12, 9);
	glPopMatrix();
//#endif

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
	else if(key == 'i')
	{
		soPointer->SetFramePosition(Vec3d(0, 0, 0));
		spidarG6.Calib();
	}
}

void InitScene();
void InitRendering(int*, char**);
void InitDeviceManager();

/**
 brief		���C���֐�
 param		<in/--> argc�@�@�R�}���h���C�����͂̌�
 param		<in/--> argv�@�@�R�}���h���C������
 return		0 (����I��)
 */
int main(int argc, char* argv[]){

	// object, solid�Ȃǂ̏�����
	InitScene();

	// rendering�֌W�̏�����
	InitRendering(&argc, argv);

	// device manager�̏�����
	InitDeviceManager();

	// device�̏�����
	spidarG6.Init(devMan, false);			//	SPIDAR�̏������C�L�����u���[�V����������D

	timeBeginPeriod(1);							//����\�̍ŏ��l

	// haptic�X���b�h�̐����E�J�n
	FTimerId = timeSetEvent(HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    TimerProc,//�R�[���o�b�N�֐�
		                    (DWORD)&time,//���[�U�[��`�̒l
		                    TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
		                    );

	// openGL main loop
	glutMainLoop();

	//	SDK�͊J�����Ȃ��Ă��ǂ��D���Ȃ��Ă�main�𔲂��Ă���J�������D
	delete phSdk;
}

/**
 brief  	scene�̏��������s���֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void InitScene()
{
	phSdk = PHSdkIf::CreateSdk();					// SDK�̍쐬�@
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

	soPointer = scene->CreateSolid(desc);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;

	{
		CDSphereDesc sd;
		sd.radius = 1.0;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));
		soPointer->AddShape(sphere);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));

		sd.radius = 2.0;
		sphere = DCAST(CDSphereIf,phSdk->CreateShape(sd));

		CDBoxDesc bd;
		bd.boxsize = Vec3f (30.0, 1.0, 30.0);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soFloor->AddShape(floor);
		soFloor->SetFramePosition(Vec3f(0,-2.5,0));
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere[sphereCnt]->AddShape(sphere);
		soSphere[sphereCnt]->SetFramePosition(Vec3f(0, 15+5*sphereCnt, 0));
	}
	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	// pointer�̏d�͂𖳌��ɂ���
	RemoveGravity(soPointer);

	// pointer�ƐڐG���鍄�̂Ƃ̉�͖@�ɂ��v�Z����߁A
	// �͊o�ŏ�������悤�ɂ���
	AddInteractiveSolid(soPointer);

	for(int i = 0; i < NUM_SPHERES+1; i++) nearest_constraints[i] = NULL;
}

/**
 brief  	rendering, window�܂��̏��������s���֐�
 param	 	main�̈���argc, argv
 return 	�Ȃ�
 */
void InitRendering(int *argc, char *argv[])
{
	glutInit(argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(WINSIZE_WIDTH, WINSIZE_HEIGHT);

	int window = glutCreateWindow("GRSimple");
	grSdk = GRSdkIf::CreateSdk();
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

	/// SPIDAR���W���r���[���W�ɒ����ϐ�
	view_rot = view.inv().Rot() * 60;
	
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