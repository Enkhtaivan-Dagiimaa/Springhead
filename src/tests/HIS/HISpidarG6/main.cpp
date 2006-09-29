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
#define WINSIZE_WIDTH	800//480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	600//360			// �E�B���h�E�T�C�Y(height)
#define NUM_SPHERES		3			// sphere��
#define SIM_FREQ		60          // �V�~�����[�V�����̍X�V����Hz

// SPIDAR��VE���ł̓���X�P�[��
#define SPIDAR_SCALE	70

// �͊o�X���b�h�̎���Hz
#ifdef _DEBUG
#define HAPTIC_FREQ		500
#elif _OPT
#define HAPTIC_FREQ		500
#elif _WINDOWS
#define HAPTIC_FREQ		1000
#endif

// �񎦗͂̃o�l�_���p�W��
#define K_force			9
#define B_force			1

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
	#include <Device/DRUsb20Simple.h>
	#include <Device/DRUsb20Sh4.h>
	#include <conio.h>

	// virtual coupling�̌W��
	#ifdef _DEBUG
		const float K = 10;
		const float B = 10;
	#elif _OPT
		const float K = 10;
		const float B = 10;
	#elif _WINDOWS
		const float K = 10;
		const float B = 8.5;
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
Matrix3f view_rot;
bool bforce = false;
MMRESULT FTimerId;

// �ċA�v�Z�����������i�[����e�[�u��
PHConstraint* process_map[NUM_SPHERES+1];
int num_process = 0;

// �͊o�v�Z�ɕK�v�ȃf�[�^���W�߂��\����
typedef struct {
	// collision solid data
	PHSolid* nearest_solids[NUM_SPHERES+1];
	Vec3d solid_velocity[NUM_SPHERES+1];
	Vec3d solid_angular_velocity[NUM_SPHERES+1];

	// collision data
	Vec3d col_positions[NUM_SPHERES+1];
	Vec3d col_normals[NUM_SPHERES+1];
	int num_collisions;
	
	// ���͂̉e���̌��ʂ��i�[����ϐ�
	Matrix3d vel_effect[NUM_SPHERES+1];
	Vec3d vel_constant[NUM_SPHERES+1];

	Matrix3d ang_effect[NUM_SPHERES+1];
	Vec3d ang_constant[NUM_SPHERES+1];

	// pointer data
	Vec3d pointer_pos;
	Vec3d pointer_vel;
	Vec3d pointer_center;
} Penalty_info;

// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
Penalty_info info1;
Penalty_info info2;

// true�̂Ƃ��͔z��P���Q��
// false�̂Ƃ��͔z��Q���Q��
bool current_valid_data = true; 

// ���͂̉e����L���ɂ��邩�ǂ��������߂�t���O
bool bSurroundEffect = false;

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
			if(process_map[i] == *it)
			{
				process = true;
				break;
			}
		}

		// �܂���������Ă��Ȃ�
		if(process == false)
		{
			PHSolid* nearest = NULL;

			// ���̍��̂��܂ޏՓ˂��`�F�b�N
			if((*it)->solid[0]->solid == solid)
			{
				nearest = (*it)->solid[1]->solid;
			}
			else if((*it)->solid[1]->solid == solid)
			{
				nearest = (*it)->solid[0]->solid;
			}

			// ���̍��̂��܂ސڐG��������
			if(nearest != NULL)
			{
				// �����σ}�b�v�ɓo�^����
				process_map[num_process++] = *it;

				// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				Vec3d f = Vec3d();
				Vec3d r = contact->pos - nearest->GetCenterPosition();

				// �ċA�I�Ɏ��̏����ɓn��
				calculate_counteractive_object_force(cs, depth+1, nearest, r, &f);

				// ����ꂽ���̕ۑ�
				forces.push_back(f);
				vec_r.push_back(r);
			}
		}
	}

	// �ϐ��̗p��
	Vec3d sum_force = Vec3d();
	Vec3d sum_r_out_f = Vec3d();

	for(int i = 0; i < (int)forces.size(); i++) 
	{
		Vec3d r = (Vec3d)vec_r.at(i);
		Vec3d f = (Vec3d)forces.at(i);

		// ����ꂽ�l�̍��v�l���v�Z
		// ���̂ɉ����͂𐳂Ƃ��A
		// �O�̍��̂��瓾��ꂽ�͍͂�p������͂Ȃ̂ŕ��ɂ���
		sum_force = sum_force + (- f) ;

		//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
		sum_r_out_f = sum_r_out_f + r ^ (- f);
	}

	// �܂Ƃ߂��镔���͂܂Ƃ߂�
	// C = vp' - (sum(Fi) / m + w x (w x r) + I^{-1} sum(ri x Fi) x r
	Vec3d C = (solid->GetAcceleration() + ((solid->GetAngularVelocity() - solid->GetOldAngularVelocity()) ^ original_r) / scene->GetTimeStep()) - ((sum_force  / solid->GetMassInv()) + solid->GetAngularVelocity() ^ (solid->GetAngularVelocity() ^ original_r) + (solid->GetInertiaInv() * sum_r_out_f) ^ original_r);

	// T = I^(-1) * (r x F)��r���O�ς���s��ɂ��ĊO�ς�r���������́BF�͊܂܂Ȃ�
	// T(ri) = I^{-1} X(ri) 	
	Matrix3d T = solid->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);

	// M = 1/m * E + (T1 x r T2 x r T3 x r)
	Matrix3d M = solid->GetMassInv() * Matrix3d().Unit() + Matrix3d(T.Ex() ^ original_r, T.Ey() ^ original_r, T.Ez() ^ original_r);

	// �������͂�Ԃ�
	*output_force = M.inv() * C;
}

/**
 brief     	�ڐG�_�̏�񂩂���͂̏��ɂ��e�����v�Z����֐�
 param		���̎��_�ł̏Փˏ��PHConstraints�ƏՓˏ����i�[����\����Penalty_info�^�̕ϐ������
			���͂̉e����Effect�ɁA�萔����Constant�Ɋi�[����
			�܂��APHSolid* solids[]�Ƀ|�C���^���ڂ��Ă��鍄�̂̃��X�g������
 return 	�Ȃ�
*/
void calculate_surround_effect(PHConstraints cs, Penalty_info *info)
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
		for(int i = 0; i < num_process; i++)
		{
			process_map[i] = NULL;
		}
		num_process = 0;

		// pointer�ɐڂ��Ă��鍄�̂��������A�擾����
		for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
		{
			PHSolid* nearest = NULL;
			int sign = 1;

			//�@�|�C���^���܂ޏՓ˂̏ꍇ���̂�ۑ�
			if((*it)->solid[0]->solid == soPointer)
			{
				nearest = (*it)->solid[1]->solid;
			}
			else if((*it)->solid[1]->solid == soPointer)
			{
				nearest = (*it)->solid[0]->solid;
				sign *= -1;
			}

			// �|�C���^���܂ލ��̂��������̂ŏ���
			// �͊o�v�Z�ɕK�v�ȏ����擾����
			if(nearest != NULL)
			{
				// �ߖT���̂�ۑ�
				info->nearest_solids[num_solids] = nearest;

				// �ߖT���̂̌����_�̑��x�Ɗp���x��ۑ��B
				// �ʂȕϐ���p�ӂ���̂́A�����������₷���悤�ɂ��邽��
				info->solid_velocity[num_solids] = nearest->GetVelocity();
				info->solid_angular_velocity[num_solids] = nearest->GetAngularVelocity();

				// ���̍��̂̒��S����͂̍�p�_�܂ł̃x�N�g����ۑ�
				PHContactPoint* contact = DCAST(PHContactPoint, (*it));
				info->col_positions[num_solids] = contact->pos;
				vec_r_pointer[num_solids] = contact->pos - nearest->GetCenterPosition();

				// �|�C���^���獄�̂ւ̐ڐG�ʂ̖@����ۑ�
				info->col_normals[num_solids] = sign * contact->shapePair->normal;

				// �����σ}�b�v�ɓo�^
				process_map[num_solids++] = *it;
			}
		}

		// �����|�C���^�ɐڂ��鍄�̂��Ȃ����
		// �͊o�ɉe�����Ȃ��̂Ń��^�[������
		if(num_solids == 0)
		{
			info->num_collisions = 0;
			return;
		}

		// �����ɕK�v�ȕϐ�
		num_process = num_solids;
		Vec3d C[NUM_SPHERES+1];
		Matrix3d M[NUM_SPHERES+1];
		Vec3d D[NUM_SPHERES+1];
		Matrix3d N[NUM_SPHERES+1];

		// pointer�ɐڂ��Ă��鍄�̂��ڂ��Ă��鍄�̂��������A
		// ���ꂼ�ꏈ�����Ă���
		for(int j = 0; j < num_solids; j++)
		{
			// ��قǓo�^��������(=�|�C���^�ɐڂ��Ă��鍄��)���擾
			// PHSolidIf* solid = (PHSolidIf*)solids[j];
			PHSolidIf* solid = (PHSolidIf*)info->nearest_solids[j];

			// ���ׂĂ̏Փ˂𒲍�
			for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
			{
				// ���̏Փ˂������ς݂��ǂ�������
				bool process = false;
				for(int i = 0; i < num_process; i++)
				{
					if(process_map[i] == *it)
					{
						process = true;
						break;
					}
				}

				// �����ς݃}�b�v����͌�����Ȃ�����(=������=�ړI�̍��̂̉\������j
				if(process == false)
				{
					PHSolid* nearest = NULL;

					if((*it)->solid[0]->solid == solid)
					{
						nearest = (*it)->solid[1]->solid;
					}
					else if((*it)->solid[1]->solid == solid)
					{
						nearest = (*it)->solid[0]->solid;
					}

					if(nearest != NULL)
					{
						// �����σ}�b�v�ɓo�^
						process_map[num_process++] = *it;

						// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�������߂�
						PHContactPoint* contact = DCAST(PHContactPoint, (*it));
						Vec3d f = Vec3d();
						Vec3d r = contact->pos - nearest->GetCenterPosition();

						// �ċA�I�Ɏ��̏����ɓn��
						calculate_counteractive_object_force(cs, depth+1, nearest, r, &f);

						forces.push_back(f);
						vec_r.push_back(r);
					}
				}
			}

			// �ϐ��̗p��
			Vec3d sum_force = Vec3d();
			Vec3d sum_r_out_f = Vec3d();

			for(int i = 0; i < (int)forces.size(); i++) 
			{
				Vec3d r = (Vec3d)vec_r.at(i);
				Vec3d f = (Vec3d)forces.at(i);

				// ����ꂽ�l�̍��v�l���v�Z 
				// ���̂ɉ����͂𐳂Ƃ��A
				// �O�̍��̂��瓾��ꂽ�͍͂�p������͂Ȃ̂ŕ��ɂ���
				sum_force = sum_force + (- f);

				//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
				sum_r_out_f = sum_r_out_f + r ^ (- f);
			}

			// �͊o�|�C���^�ƍŋߖT���̂Ƃ̐ڐG�_
			Vec3d original_r = vec_r_pointer[j];

			// �܂Ƃ߂��镔���͂܂Ƃ߂�
			// �萔��
			// C = sum(Fi)/m + w x (w x r) + (I^{-1} sum(ri x Fi)) x r
			D[j] = solid->GetInertiaInv() * sum_r_out_f;
			C[j] = Vec3d(sum_force / solid->GetMassInv() + solid->GetAngularVelocity() ^ (solid->GetAngularVelocity() ^ original_r) + D[j] ^ original_r);

			// T = I^(-1) * (r x F)��r���O�ς���s��ɂ��ĊO�ς�r���������́BF�͊܂܂Ȃ�
			// T(ri) = I^{-1} X(ri) 
			N[j] = solid->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);
			
			// ���߂�s��
			// M = 1/m * E + (T1 x r T2 x r T3 x r)
			M[j] = Matrix3d(solid->GetMassInv() * Matrix3d().Unit() + Matrix3d(N[j].Ex() ^ original_r, N[j].Ey() ^ original_r, N[j].Ez() ^ original_r));
		}
		
		// ���ʂ̊i�[
		for(int i = 0; i < num_solids; i++)
		{
			info->vel_effect[i] = M[i];
			info->vel_constant[i] = C[i];
			info->ang_effect[i] = N[i];
			info->ang_constant[i] = D[i];
		}

		// �|�C���^�ɐڐG���鍄�̂̐���o�^
		// �͊o�v�Z�Ŏg�p
		info->num_collisions = num_solids;

		return;
	}
	// �Փ˂��Ȃ������̂łO��Ԃ��ă��^�[��
	else
	{
		info->num_collisions = 0;
		return;
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

			if(current_valid_data == true)
			{
				for(int i = 0; i < info1.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[0]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
				}
			}
			else 
			{
				for(int i = 0; i < info2.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = &(*info2.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[0]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
				}

			}
			DSTR << std::endl;

			// solid2
			DSTR << "solid2.mass = " << (*it)->solid[1]->solid->mass << std::endl;
			DSTR << "solid2.inertia = " << std::endl << (*it)->solid[1]->solid->inertia;
			DSTR << "solid2.velocity = " << (*it)->solid[1]->solid->GetVelocity()<< std::endl;
			DSTR << "solid2.angular_velocity = " << (*it)->solid[1]->solid->GetAngularVelocity() << std::endl;
			DSTR << "solid2.position = " << (*it)->solid[1]->solid->GetFramePosition() << std::endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[1]->solid->GetCenterPosition()).unit() << std::endl;

			if(current_valid_data == true)
			{
				for(int i = 0; i < info1.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
				}
			}
			else
			{
				for(int i = 0; i < info2.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info2.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]->solid))DSTR << " --this solid is nearest object!!" << std::endl;
				}
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
	// SPIDAR�̏����i�[����ϐ�
	static Vec3d old_pos = Vec3d();
	static Vec3d last_force = Vec3d();
	static Vec3d last_torque = Vec3d();

	Penalty_info* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}

	// SPIDAR�X�V��
	static const float dt = (float)1.0f / HAPTIC_FREQ;

	// SPIDAR�̈ʒu�̍X�V
	spidarG6.Update(dt);//	USB�̒ʐM���s���D
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

	// SPIDAR�̑��x���v�Z
	Vec3d PointerVel = (spidar_pos - old_pos) / dt;
	old_pos = spidar_pos;

	// SPIDAR���W�܂ł̃x�N�g�����쐬
	Vec3d goal = spidar_pos - info->pointer_pos;

	// VR��Ԃ̃|�C���^��SPIDAR��virtual coupling�łȂ���
	Vec3d VCforce = K * goal + B * (PointerVel - info->pointer_vel);
	soPointer->AddForce(VCforce, Vec3f());

	// �|�C���^�ɉ�����́E�g���N���i�[����ϐ�
	Vec3d pointer_force = Vec3d();
	Vec3d pointer_torque = Vec3d();

	// �|�C���^�ɐ��������ׂĂ̐ڐG�ɂ��Čv�Z
	for(int i = 0; i < info->num_collisions; i++)
	{
		// �N����\���x�N�g�����쐬
		Vec3d penetration_vector = info->col_positions[i] - spidar_pos;

		// �񎦗͂��v�Z
		Vec3d feedback_force = - (K_force * dot(penetration_vector, info->col_normals[i])) * info->col_normals[i].unit() / penetration_vector.norm();

		// �񎦗͂ɂ��g���N���v�Z
		Vec3d feedback_torque = (info->col_positions[i] - info->pointer_center) ^ feedback_force;

		// �ՓˑΏۂ̃g���N���v�Z
		Vec3d solid_torque_vector = (info->col_positions[i] - info->nearest_solids[i]->GetCenterPosition()) ^ (-feedback_force);

		// �ՓˑΏۂɐڐG�́E�g���N��ǉ�
		info->nearest_solids[i]->AddForce(-feedback_force, solid_torque_vector);

		
		// ���͂̉e�����l��
		if(bSurroundEffect && info->nearest_solids[i]->IsDynamical())
		{
			// �����x���v�Z
			Vec3d accel = info->vel_effect[i] * (-feedback_force) + info->vel_constant[i];

			// ���x���v�Z���A�Փ˓_���X�V����
			info->solid_velocity[i] = info->solid_velocity[i] + accel / HAPTIC_FREQ;
			info->col_positions[i] = info->col_positions[i] + info->solid_velocity[i] / HAPTIC_FREQ;

			// �p�����x���v�Z
			Vec3d ang_accel = info->ang_effect[i] * (-feedback_force) + info->ang_constant[i];

			// �p���x���v�Z���A�@���̌������X�V����
			info->solid_angular_velocity[i] = info->solid_angular_velocity[i] + ang_accel / HAPTIC_FREQ;
//			info->col_normals[i] = info->col_normals[i] * info->solid_angular_velocity[i];
		}

		// �񎦗͂�O�̒l�ɒǉ�
		pointer_force = pointer_force + feedback_force;
		pointer_torque = pointer_torque + feedback_torque;
	}

	// spidar�ɗ͂�������
	if(info->num_collisions > 0)
	{
		// �O�̒񎦗͂ƃg���N���Q�Ƃ��ĂԂ����
		if(bforce)spidarG6.SetForce(0.95 * pointer_force + 0.005 * last_force, 0.95 * pointer_torque + 0.05 * last_torque);
		else spidarG6.SetForce(Vec3d(), Vec3d());

		// �񎦗͂�ۑ�
		last_force = pointer_force;
		last_torque = pointer_torque;
	}
	else
	{
		// �Փ˂��Ȃ��̂Œ񎦗͂����Z�b�g����
		spidarG6.SetForce(Vec3d());
		last_force = Vec3d();
		last_torque = Vec3d();
	}

#if 0
//#if _DEBUG | _WINDOWS
	static int sec_counter = 0;
	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % HAPTIC_FREQ == 0)
	{
//		std::cout << "spidar position = " << spidar_pos << std::endl;
//		std::cout << "spidar velocity = " << PointerVel << std::endl;
//		std::cout << "force = " << -(0.95 * force + 0.05 * last_force) << "torque = " << pointer_torque << std::endl;
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
	// �P���Q�ƒ��B�Q���X�V
	if(current_valid_data == true)
	{
		calculate_surround_effect(cs, &info2);
		info2.pointer_pos = soPointer->GetFramePosition();
		info2.pointer_vel = soPointer->GetVelocity();
		info2.pointer_center = soPointer->GetCenterPosition();
	}
	// �Q���Q�ƒ��B�P���X�V
	else
	{
		calculate_surround_effect(cs, &info1);
		info1.pointer_pos = soPointer->GetFramePosition();
		info1.pointer_vel = soPointer->GetVelocity();
		info1.pointer_center = soPointer->GetCenterPosition();
	}

	// �͊o�X���b�h�̃V�~�����[�V�����̕ϐ��̎Q�Ɛ��ύX
	current_valid_data = current_valid_data?false:true;

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
void AddInactiveSolid(PHSolidIf* soPointer)
{
	scene->GetConstraintEngine()->AddInactiveSolid(soPointer);
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

	// simulation�Ɠ������g���ɂ���
	Sleep((double)1000.0/SIM_FREQ);
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

		// ���b�Z�[�W���o��
		std::cout << "force ";
		if(bforce) std::cout << "on";
		else std::cout << "off";
		std::cout << std::endl;
	}
	else if(key == 'i')
	{
		soPointer->SetFramePosition(Vec3d(0, 0, 0));
		spidarG6.Calib();
	}
	// sphere�̏ꏊ�����Z�b�g
	else if(key == 'r')
	{
		for(int i = 0; i < NUM_SPHERES; i++)
		{
			soSphere[i]->SetPose(Posed());
			soSphere[i]->SetFramePosition(Vec3f(0, 15+5*(i+1), 0));
			soSphere[i]->SetAngularVelocity(Vec3d());
			soSphere[i]->SetVelocity(Vec3d());
		}
	}
	// ���͂̉e����L���ɂ��邩�ǂ���
	else if(key == 's')
	{
		bSurroundEffect = !bSurroundEffect;

		// ���b�Z�[�W���o��
		std::cout << "surrounding effect ";

		if(bSurroundEffect)std::cout << "on";
		else std::cout << "off";

		std::cout << std::endl;
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
	FTimerId = timeSetEvent(1000 / HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
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
	sd.timeStep = (double)1.0 / SIM_FREQ;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 1.0;
	desc.inertia *= 2.0;

	// Solid�̍쐬
	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}

	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);

	desc.mass = 2.0;
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
	AddInactiveSolid(soPointer);
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

	int window = glutCreateWindow("HapticTest");
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
	Affinef view;
	view.Pos() = Vec3f(0.0, 5.0, -20.0);								// eye
	view.LookAtGL(Vec3f(0.0, 0.0, 0.0), Vec3f(0.0, 1.0, 0.0));			// center, up 
	view = view.inv();	
	render->SetViewMatrix(view);
	
	setLight();

	/// SPIDAR���W���r���[���W�ɒ����ϐ�
	view_rot = view.inv().Rot() * SPIDAR_SCALE;
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