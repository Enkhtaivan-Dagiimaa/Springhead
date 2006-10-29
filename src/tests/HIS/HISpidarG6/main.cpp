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
  DrawScene API �Őݒ肳��Ă���}�e���A���T���v����p���āA�J���t���ȋ��̂������_�����O����B�@
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
#define NUM_SPHERES		2			// sphere��
#define SPIDAR_SCALE	70			// SPIDAR��VE���ł̓���X�P�[��


#ifdef _DEBUG
	#define SIMULATION_FREQ	100         // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		500			// �͊o�X���b�h�̎���Hz
	float K = 1500;						// virtual coupling�̌W��
	float B = 150;

#elif _WINDOWS
	#define SIMULATION_FREQ	150         // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000		// �͊o�X���b�h�̎���Hz
	float K = 1450;						// virtual coupling�̌W��
	float B = 200;
#endif

// �񎦗͂ƍ��̂ɒ񎦂���͂𒼐ڕω�������萔
double FORCE_COEFF=		0.1;

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
	#include <Device/DRUsb20Simple.h>
	#include <Device/DRUsb20Sh4.h>
	#include <conio.h>
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
Matrix3f view_haptic;

bool bforce = false;
MMRESULT FTimerId1;
MMRESULT FTimerId2;

// �ċA�v�Z�����������i�[����e�[�u��
PHConstraint* process_map[NUM_SPHERES+1];
int num_process = 0;

// �͊o�v�Z�ɕK�v�ȃf�[�^���W�߂��\����
typedef struct {
	// collision solid data
	PHSolid* nearest_solids[NUM_SPHERES+1];
	Vec3d solid_velocity[NUM_SPHERES+1];
	Vec3d solid_angular_velocity[NUM_SPHERES+1];
	Matrix3d solid_inertiainv[NUM_SPHERES+1];

	// collision data
	Vec3d col_positions[NUM_SPHERES+1];
	Vec3d col_normals[NUM_SPHERES+1];
	int num_collisions;
	
	Vec3d original_col_positions[NUM_SPHERES+1];
	Vec3d original_col_normals[NUM_SPHERES+1];

	// ���͂̉e���̌��ʂ��i�[����ϐ�
	Matrix3d vel_effect[NUM_SPHERES+1];
	Vec3d vel_constant[NUM_SPHERES+1];

//	Matrix3d ang_effect[NUM_SPHERES+1];
	Vec3d ang_constant[NUM_SPHERES+1];

	// pointer data
	Vec3d pointer_pos;
	Vec3d pointer_vel;
	double pointer_massinv;
} Haptic_info;

// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
Haptic_info info1;
Haptic_info info2;

// true�̂Ƃ��͍\���̂P���Q��
// false�̂Ƃ��͍\���̂Q���Q��
bool current_valid_data = true; 

// ���͂̉e����L���ɂ��邩�ǂ��������߂�t���O
bool bSurroundEffect = false;

// SPIDAR�̈ʒu��\�����邩�ǂ���
bool bDisplayPointer = true;

// ������solid�����łɏ�������Ă��邩���ׂ�֐�
bool checkProcessed(PHConstraint* target)
{
	for(int i = 0; i < num_process; i++)
	{
		if(process_map[i] == target)return true;
	}
	return false;
}

// ������solid�������ςɓo�^����
void addProcessed(PHConstraint* constraint)
{
	process_map[num_process++] = constraint;
}

// �����σ��X�g��������
void initializeProcessed()
{
	// �����σ}�b�v�̏�����
	for(int i = 0; i < num_process; i++)
	{
		process_map[i] = NULL;
	}
	num_process = 0;
}

// �^����ꂽ�ڐG�ŁA������solid�ɐڂ��Ă��鍄�̂�����ׁA����ΕԂ�
PHSolid* getAdjacentSolid(PHConstraint* constraint, PHSolid* solid, int* sign = NULL)
{
	if(constraint->solid[0]->solid == solid)
	{
		if(sign != NULL) *sign = -1;
		return constraint->solid[1]->solid;
	}
	else if(constraint->solid[1]->solid == solid)
	{
		if(sign != NULL) *sign = 1;
		return constraint->solid[0]->solid;
	}
	else return NULL;
}

// ��Ď�@�̌v�Z�|�C���^�p
void calculate_pointer_effect(std::vector<Vec3d> *forces, std::vector<Vec3d> *vec_r, Vec3d original_r, PHSolid* solid, Vec3d *C, Matrix3d *M, Vec3d *D, Matrix3d *N)
{
	// �ϐ��̗p��
	Vec3d sum_force = Vec3d();
	Vec3d sum_r_out_f = Vec3d();

	for(int i = 0; i < (int)forces->size(); i++) 
	{
		Vec3d r = (Vec3d)vec_r->at(i);
		Vec3d f = (Vec3d)forces->at(i);

		// ����ꂽ�l�̍��v�l���v�Z 
		sum_force = sum_force + f;

		//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
		sum_r_out_f = sum_r_out_f + r ^ f;
	}

	// �܂Ƃ߂��镔���͂܂Ƃ߂�
	// �萔��
	// C = sum(Fi)/m + g + w x (w x r) + (I^{-1} sum(ri x Fi)) x r
	*D = solid->GetInertiaInv() * sum_r_out_f;
	*C = Vec3d(sum_force * solid->GetMassInv() + scene->GetGravity() + solid->GetAngularVelocity() ^ (solid->GetAngularVelocity() ^ original_r) + *D ^ original_r);

	// T = I^(-1) * (r x F)��r���O�ς���s��ɂ��ĊO�ς�r���������́BF�͊܂܂Ȃ�
	// T(ri) = I^{-1} X(ri) 
	*N = solid->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);
	
	// ���߂�s��
	// M = 1/m * E + (T1 x r T2 x r T3 x r)
	*M = Matrix3d(solid->GetMassInv() * Matrix3d().Unit() + Matrix3d(N->Ex() ^ original_r, N->Ey() ^ original_r, N->Ez() ^ original_r));
}

// ���̂��ڐG���Ă��鍄�̂ɗ^����͂̌v�Z
void calculate_solid_effect(std::vector<Vec3d> *forces, std::vector<Vec3d> *vec_r, PHSolid* solid, Vec3d original_r, Vec3d *output_force)
{
	// �ϐ��̗p��
	Vec3d sum_force = Vec3d();
	Vec3d sum_r_out_f = Vec3d();

	for(int i = 0; i < (int)forces->size(); i++) 
	{
		Vec3d r = (Vec3d)vec_r->at(i);
		Vec3d f = (Vec3d)forces->at(i);

		// ����ꂽ�l�̍��v�l���v�Z
		sum_force = sum_force + f;

		//�@�͂Ɣ���������̊O�ς��v�Z�A�����̍��v�l���v�Z����
		sum_r_out_f = sum_r_out_f + r ^ f;
	}

	// �܂Ƃ߂��镔���͂܂Ƃ߂�
	// C = (v_p)' - (sum(Fi) / m + w x (w x r) + I^{-1} sum(ri x Fi) x r
	// point_accel�̌v�Z������ł悢�̂��m�F����K�v������
	Vec3d point_accel = solid->GetAcceleration() + ((solid->GetAngularVelocity() - solid->GetOldAngularVelocity()) / scene->GetTimeStep()) ^ original_r;
	Vec3d C = point_accel - ((sum_force * solid->GetMassInv()) + scene->GetGravity() + solid->GetAngularVelocity() ^ (solid->GetAngularVelocity() ^ original_r) + (solid->GetInertiaInv() * sum_r_out_f) ^ original_r);

	// T = I^(-1) * (r x F)��r���O�ς���s��ɂ��ĊO�ς�r���������́BF�͊܂܂Ȃ�
	// T(ri) = I^{-1} X(ri) 	
	Matrix3d T = solid->GetInertiaInv() * Matrix3d(0, - original_r.z, original_r.y, original_r.z, 0, - original_r.x, -original_r.y, original_r.x, 0);

	// M = 1/m * E + (T1 x r T2 x r T3 x r)
	Matrix3d M = solid->GetMassInv() * Matrix3d().Unit() + Matrix3d(T.Ex() ^ original_r, T.Ey() ^ original_r, T.Ez() ^ original_r);

	// �������͂�Ԃ�
	*output_force = M.inv() * C;
}

void calculate_counteractive_object_force(PHConstraints cs, int depth, PHSolid* solid, Vec3d original_r, Vec3d *output_force);

// ���̐ڂ��Ă鍄�̂ɐi�ނƂ��ɕK�v�ȏ������s��
void gotoNextSolid(PHConstraints cs, PHConstraint* constraint, int depth, PHSolid* solid, std::vector<Vec3d> *vec_r, std::vector<Vec3d> *force, PHSolid* origin)
{
	// �����σ}�b�v�ɓo�^����
	addProcessed(constraint);

	// ���̂̒��S����͂̍�p�_�܂ł̃x�N�g�����v�Z����
	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	Vec3d f = Vec3d();
	Vec3d r = contact->pos - solid->GetCenterPosition();

	// �ċA�I�Ɏ��̏����ɓn��
	calculate_counteractive_object_force(cs, depth, solid, r, &f);

	// ����ꂽ���̕ۑ�
	force->push_back(f);
	vec_r->push_back(contact->pos - origin->GetCenterPosition());
}

// �񎦗͂̌v�Z����ђ�Ď�@�ɕK�v�ȏ����W�߂č\���̂��쐬����
void makeInfo(PHSolid* nearest, PHConstraint* constraint, Haptic_info* info, int sign, Vec3d C, Vec3d D, Matrix3d M, Matrix3d N)
{	
	// ���̍��̂̒��S����͂̍�p�_�܂ł̃x�N�g����ۑ�
	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	info->col_positions[info->num_collisions] = contact->pos;
	info->original_col_positions[info->num_collisions] = contact->pos;

	// ���̂���|�C���^�ւ̐ڐG�ʂ̖@����ۑ�
	info->col_normals[info->num_collisions] = sign * contact->shapePair->normal;
	info->original_col_normals[info->num_collisions] = sign * contact->shapePair->normal;
	
	// �ߖT���̂̌����_�̑��x�Ɗp���x��ۑ��B
	// �ʂȕϐ���p�ӂ���̂́A�����������₷���悤�ɂ��邽��
	info->solid_velocity[info->num_collisions] = nearest->GetVelocity() +  nearest->GetAngularVelocity() ^ (info->col_positions[info->num_collisions] - nearest->GetCenterPosition());
	info->solid_angular_velocity[info->num_collisions] = nearest->GetAngularVelocity();
	info->solid_inertiainv[info->num_collisions] = nearest->GetInertiaInv();

	// �v�Z�œ���ꂽ���͂̉e����\���ϐ����R�s�[����
	info->vel_constant[info->num_collisions] = C;
	info->ang_constant[info->num_collisions] = D;
	info->vel_effect[info->num_collisions] = M;
//	info->ang_effect[info->num_collisions] = N;

	// �ڂ��Ă��鍄�̎��g��o�^
	info->nearest_solids[info->num_collisions] = nearest;

	// �Փː����ЂƂ��₷
	info->num_collisions++;
}


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
		// ���݂̒l�����łɏ������ꂽ���`�F�b�N		
		bool process = checkProcessed(*it);

		// �܂���������Ă��Ȃ�
		if(process == false)
		{
			// solid�ɐڂ��Ă��鍄�̂��擾
			PHSolid* nearest = getAdjacentSolid(*it, solid);

			// ���̍��̂��܂ސڐG��������
			if(nearest != NULL)
			{
				// ���̍��̂Ɉړ����čċA�I�ɐڐG�͂̌v�Z������
				gotoNextSolid(cs, *it, depth+1, nearest, &vec_r, &forces, solid);
			}
		}
	}
	// �ڂ��Ă��鍄�̂ɗ^����ڐG�͂̌v�Z
	calculate_solid_effect(&forces, &vec_r, solid, original_r, output_force);
}

/**
 brief     	�ڐG�_�̏�񂩂���͂̏��ɂ��e�����v�Z����֐�
 param		���̎��_�ł̏Փˏ��PHConstraints�ƏՓˏ����i�[����\����Haptic_info�^�̕ϐ������
 return 	�Ȃ�
*/
void calculate_surround_effect(PHConstraints cs, Haptic_info *info)
{
	// �Փ˂̐����`�F�b�N
	if(cs.size() > 0)
	{
		PHSolid* nearest_solids[NUM_SPHERES+1];
		int signs[NUM_SPHERES+1];
		PHConstraint* consts[NUM_SPHERES+1];

		Vec3d vec_r_pointer[NUM_SPHERES+1];
		int num_solids = 0;

		// �����σ}�b�v�̏�����
		initializeProcessed();

		// pointer�ɐڂ��Ă��鍄�̂��������A�擾����
		for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
		{
			// �@���̌������C�����镄��
			int sign = 1;

			// �|�C���^�ɐڂ��鍄�̂��擾
			PHSolid* nearest = getAdjacentSolid(*it, (PHSolid*)soPointer, &sign);

			// �|�C���^���܂ލ��̂��������̂ŏ���
			if(nearest != NULL)
			{
				// �ߖT���̂�ۑ�
				nearest_solids[num_solids] = nearest;
				signs[num_solids] = sign;
				consts[num_solids] = *it;

				PHContactPoint* contact = DCAST(PHContactPoint, *it);	
				vec_r_pointer[num_solids++] = contact->pos - nearest->GetCenterPosition();
			}
		}

		// ������
		info->num_collisions = 0;

		// �����|�C���^�ɐڂ��鍄�̂��Ȃ����
		// �͊o�ɉe�����Ȃ��̂Ń��^�[������
		if(num_solids == 0) return;

		// pointer�ɐڂ��Ă��鍄�̂��ڂ��Ă��鍄�̂��������A
		// ���ꂼ�ꏈ�����Ă���
		for(int j = 0; j < num_solids; j++)
		{
			// ���i�p�̌v�Z���ʂ��i�[����ϐ�
			Vec3d C;
			Matrix3d M;

			// ��]�p�̌v�Z���ʂ��i�[����ϐ�
			Vec3d D;
			Matrix3d N;

			std::vector<Vec3d> forces;
			std::vector<Vec3d> vec_r;

			// ��قǓo�^��������(=�|�C���^�ɐڂ��Ă��鍄��)���擾			
			PHSolidIf* solid = (PHSolidIf*)nearest_solids[j];

			// ���ׂĂ̏Փ˂𒲍�
			for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
			{
				bool process = checkProcessed(*it);

				// �����ς݃}�b�v����͌�����Ȃ�����(= ������ = �ړI�̍��̂̉\������j
				if(process == false)
				{
					PHSolid* nearest = getAdjacentSolid(*it, (PHSolid*)solid);

					// �|�C���^�ɐڂ��鍄�̂ɐڂ��鍄�̂�����΂�����ɐi��
					if(nearest != NULL && nearest != soPointer)
					{						
						// �[���̏�����
						int depth = 1;

						// �ڂ��鍄�̂Ɉړ����A�ċA�I�ɐڐG�͂��v�Z����
						gotoNextSolid(cs, *it, depth+1, nearest, &vec_r, &forces, (PHSolid*)solid);
					}
				}
			}
			
			// ��Ď�@�̌v�Z
			calculate_pointer_effect(&forces, &vec_r, vec_r_pointer[j], (PHSolid*)solid, &C, &M, &D, &N);

			// �v�Z���ʂ����Ƀf�[�^���쐬
			makeInfo(nearest_solids[j], consts[j], info, signs[j], C, D, M, N);
		}
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
			�����I�ɂ̓|�C���^��SPIDAR��virtual coupling�łȂ��A�|�C���^���X�V���Ă���
 param	 	�Ȃ�
 return 	�Ȃ�
*/
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// SPIDAR�̏����i�[����ϐ�
	static Vec3d old_pos = Vec3d();
//	static Vec3d last_force = Vec3d();
//	static Vec3d last_torque = Vec3d();

	// SPIDAR�X�V��
	static const float dt = (float)1.0f / HAPTIC_FREQ;

	// �V�~�����[�V�������瓾��������i�[����ϐ�
	Haptic_info* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}

	// SPIDAR�̈ʒu�̍X�V
	spidarG6.Update(dt);//	USB�̒ʐM���s���D
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

	// SPIDAR�̑��x���v�Z
	Vec3d PointerVel = (spidar_pos - old_pos) * dt;
	old_pos = spidar_pos;

	// SPIDAR���W�܂ł̃x�N�g�����쐬
	Vec3d goal = spidar_pos - info->pointer_pos;

	// VR��Ԃ̃|�C���^��SPIDAR��virtual coupling�łȂ���
	Vec3d VCforce = K * goal + B * (PointerVel - info->pointer_vel);

	// soPointer�̑��x���X�V
	info->pointer_vel = info->pointer_vel + info->pointer_massinv * VCforce * dt;

	// �|�C���^�ɉ�����́E�g���N���i�[����ϐ�
	Vec3d pointer_force = Vec3d();

	if(info->num_collisions > 0)
	{
		// �|�C���^�ɐ��������ׂĂ̐ڐG�ɂ��Čv�Z
		for(int i = 0; i < info->num_collisions; i++)
		{
			Vec3d feedback_force = - FORCE_COEFF * dot(VCforce, info->col_normals[i]) * info->col_normals[i];

			// �ՓˑΏۂ̃g���N���v�Z
			Vec3d solid_torque_vector = (info->col_positions[i] - info->nearest_solids[i]->GetCenterPosition()) ^ (-feedback_force);

			Vec3d accel;
			Vec3d ang_accel;

			if(bSurroundEffect == false || info->nearest_solids[i]->IsDynamical() == false)
			{
//				info->nearest_solids[i]->AddForce(-feedback_force, solid_torque_vector);
				accel = info->nearest_solids[i]->GetMassInv() * (-feedback_force);

				ang_accel = info->solid_inertiainv[i] * (solid_torque_vector);
			}

			// ���͂̉e�����l��
			else
			{
				// �����x���v�Z
				accel = info->vel_effect[i] * (-feedback_force) + info->vel_constant[i];

				// �p�����x���v�Z ang_effect���g������solid_torque_vector���^����g���N���킩���Ă���̂ł�����g�����ق�����萳�m�Ȃ͂�
	//			Vec3d ang_accel = info->ang_effect[i] * (-feedback_force) + info->ang_constant[i];
				ang_accel = info->solid_inertiainv[i] * (solid_torque_vector) + info->ang_constant[i];

			}

			// ���x���v�Z���A�Փ˓_���X�V����
			info->solid_velocity[i] = info->solid_velocity[i] + accel * dt;

			// �Փ˓_�̈ړ��������v�Z
			Vec3d dx = info->solid_velocity[i] * dt;

			// �Փ˓_���X�V
			info->col_positions[i] = info->col_positions[i] + dx;

			// �|�C���^��������ɓ������Ƃ��āA�ʒu���X�V����
			// ����̓x�N�g���Ȃ̂ŁA���ׂĂ̏Փ˓_�ɂ��đ������ƍŏI�I�ɖړI�̃x�N�g����������
			info->pointer_pos = info->pointer_pos + dx;

			// �p���x���v�Z���A�@���̌������X�V����
			info->solid_angular_velocity[i] = info->solid_angular_velocity[i] + ang_accel * dt;

			Vec3d dth = info->solid_angular_velocity[i] * dt;
			info->col_normals[i] = info->col_normals[i] + dth;
			info->col_normals[i] = info->col_normals[i].unit();

			if(fabs(feedback_force.x) > 300 || fabs(feedback_force.y) > 300 || fabs(feedback_force.z) > 300)
			{
				std::cout << "********" << std::endl;
				std::cout << i << ":force is " << feedback_force << std::endl;
				std::cout << "pointer_pos = " << info->pointer_pos << std::endl;
				std::cout << "dx = " << dx << std::endl;
				std::cout << "vel effect = " << info->vel_effect[i] << std::endl;
				std::cout << "vel const = " << info->vel_constant[i] << std::endl;
				std::cout << "accel = " << accel << std::endl;
				std::cout << "VCforce = " << VCforce << std::endl;
				std::cout << "goal = " << goal << std::endl;
				std::cout << "relative vel = " << PointerVel - info->pointer_vel << std::endl;
				std::cout << "normal = " << info->col_normals[i] << std::endl;
				std::cout << "dth = " << dth << std::endl;
				std::cout << "ang accel = " << ang_accel << std::endl;
				std::cout << std::endl;
				pointer_force = Vec3d();
			}

			
			// �񎦗͂�O�̒l�ɒǉ�
			pointer_force = pointer_force + feedback_force;
		}

		// spidar�ɗ͂�������
		if(bforce)
		{
			// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
			Vec3d f = view_haptic * pointer_force;

			if(fabs(f.x) > 300 || fabs(f.y) > 300 || fabs(f.z) > 300)
			{
				std::cout << "force is " << f << std::endl;
				f = Vec3d();
			}
				spidarG6.SetForce(f, Vec3d());	
		}
		else spidarG6.SetForce(Vec3d(), Vec3d());
	}
	else
	{
		// �Փ˂��Ȃ��̂Œ񎦗͂����Z�b�g����
		spidarG6.SetForce(Vec3d());

		// soPointer�̈ʒu���X�V
		info->pointer_pos = info->pointer_pos + info->pointer_vel * dt;
	}

#if 0
	static int sec_counter = 0;
	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % HAPTIC_FREQ == 0)
	{
//		std::cout << "spidar position = " << spidar_pos << std::endl;
//		std::cout << "spidar velocity = " << PointerVel << std::endl;
//		std::cout << " force = " << pointer_force << std::endl;
//		std::cout << "num collisions = " << info->num_collisions << std::endl;
//		std::cout << "pointer = " << soPointer->GetFramePosition() << std::endl;
//		std::cout << std::endl;
//		std::cout << "orientation = " << soPointer->GetOrientation() << std::endl;

		for(int i = 0; i < info->num_collisions; i++)
		{
			std::cout << "col pos = " << info->col_positions[i] << std::endl;
		}

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

// ��Ď�@�ɂ���Đ��������̂̈ʒu�̌덷���C������֐�
void ErrorCorrection()
{
	Haptic_info* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}
	// �|�C���^���ڂ��Ă��邷�ׂĂ̍��̂ɂ��ď���
	for(int i = 0; i < info->num_collisions; i++)
	{
		// �덷������킷�x�N�g�����v�Z
		Vec3d pos_error = info->col_positions[i] - info->original_col_positions[i];
		Vec3d ori_error = info->col_normals[i] - info->original_col_normals[i];

		if(info->nearest_solids[i]->IsDynamical())
		{
			// �덷�̕����s�ړ�����
			info->nearest_solids[i]->SetFramePosition(info->nearest_solids[i]->GetFramePosition() + pos_error);
			info->nearest_solids[i]->SetOrientation(info->nearest_solids[i]->GetOrientation() * Quaterniond::Rot(ori_error));

			info->nearest_solids[i]->SetVelocity(info->solid_velocity[i]);
			info->nearest_solids[i]->SetAngularVelocity(info->solid_angular_velocity[i]);

			// �Փ˓_���̏㏑��
			info->original_col_positions[i] = info->col_positions[i];
			info->original_col_normals[i] = info->col_normals[i];
		}
	}
}

void keyboard(unsigned char, int, int);

/**
 brief		VR���̃v���L�V�̈ʒu���X�V����֐��B
			���̊֐��Ɨ͊o�����_�����O��g�ݍ��킹�邱�ƂŃ|�C���^�̍X�V��HAPTIC_FREQ�̎��g���ōX�V�ł���B
 param		�Ȃ�
 return		�Ȃ�
 */
void UpdatePointer()
{
	Haptic_info* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}

	soPointer->SetFramePosition(info->pointer_pos);
	soPointer->SetVelocity(info->pointer_vel);
}

void CALLBACK StepSimulation(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ������덷���C������
	ErrorCorrection();

	// �V�~�����[�V�������P�X�e�b�v�i�߂�
	scene->Step();

	// soPointer�̊p���x���O�ɂ���
	keyboard('p', 0, 0);

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
		info2.pointer_massinv = soPointer->GetMassInv();
	}
	// �Q���Q�ƒ��B�P���X�V
	else
	{
		calculate_surround_effect(cs, &info1);
		info1.pointer_pos = soPointer->GetFramePosition();
		info1.pointer_vel = soPointer->GetVelocity();
		info1.pointer_massinv = soPointer->GetMassInv();
	}

	// �͊o�X���b�h�̃V�~�����[�V�����̕ϐ��̎Q�Ɛ��ύX
	current_valid_data = current_valid_data?false:true;
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	// �ĕ`��
	glutPostRedisplay();
}

/**
 brief		��͖@�̉e�����󂯂Ȃ����̂�o�^����B���̍��̂����ސڐG�͑���ɐڐG�͂�^���Ȃ�
 param		�o�^���鍄��
 return		�Ȃ�
*/
void AddInactiveSolid(PHSolidIf* soSolid)
{
	scene->GetConstraintEngine()->AddInactiveSolid(soSolid);
}

// SPIDAR�̈ʒu�ƌ��_������킷�_��`�悷��֐�
void displayPointer()
{
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat orange[] = {1.0, 0.2, 0.0, 1.0};
	GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};

	GLfloat *color;

	// ��Ԃɂ����SPIDAR�̋��̐F��ς���
	if(bforce)
	{
		if(bSurroundEffect)color = orange;
		else color = red;
	}
	else
	{
		if(bSurroundEffect)color = blue;
		else color = white;
	}

	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		static Affinef pos_affine;
		Posef::Unit(spidar_pos).ToAffine(pos_affine);
		glMultMatrixf(pos_affine);
	
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, color);

		glutSolidSphere(1.0, 12, 9);
	glPopMatrix();

	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		Vec3f te = Vec3f();
		Affinef af = Affinef();
		Posef::Unit(te).ToAffine(af);
		glMultMatrixf(af);
		glutSolidSphere(1.0, 12, 9);
	glPopMatrix();
}

/**
 brief     	glutDisplayFunc�Ŏw�肵���R�[���o�b�N�֐�
 param		�Ȃ�
 return 	�Ȃ�
 */
void display(){
	render->ClearBuffer();
	render->DrawScene(scene);

	// �t���O��ON�̂Ƃ�SPIDAR�̋O�Ղ��f�o�b�O�o��
	if(bDisplayPointer) displayPointer();

	render->EndScene();
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
		timeKillEvent(FTimerId1);
		timeKillEvent(FTimerId1);
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
	// soPointer��SPIDAR�̏ꏊ�������l�ɖ߂��A�L�����u���[�V�������s��(virtual coupling�p�j
	else if(key == 'i')
	{
		soPointer->SetVelocity(Vec3d());
		soPointer->SetAngularVelocity(Vec3d());
		soPointer->SetPose(Posed());
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
	else if(key == 'p')
	{
		soPointer->SetAngularVelocity(Vec3d());
	}
	// ���݂̃o�[�`�����J�b�v�����O��K��B�̒l��\������
	else if(key == 'l')
	{
		std::cout << "k = " << K << " b = " << B << std::endl;
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'k')
	{
		K += 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'b')
	{
		B += 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'j')
	{
		K -= 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'v')
	{
		B -= 1;
		keyboard('l', 0, 0);
	}
	// �񎦗͂𒲐߂���l��0.1��������
	else if(key == 'm')
	{
		FORCE_COEFF += 0.1;
		std::cout << "coeff = " << FORCE_COEFF << std::endl;
	}
	// �񎦗͂𒲐߂���l��0.1��������
	else if(key == 'n')
	{
		FORCE_COEFF -= 0.1;
		std::cout << "coeff = " << FORCE_COEFF << std::endl;
	}
	// SPIDAR�̈ʒu�ƌ��_������킷���̕\����ON/OFF����
	else if(key == 'd')
	{
		bDisplayPointer = !bDisplayPointer;
		std::cout << "pointer is ";
		if(!bDisplayPointer)std::cout << "not ";
		std::cout << "displayed" << std::endl;
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
	FTimerId1 = timeSetEvent(1000 / HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    HapticRendering,//�R�[���o�b�N�֐�
		                    (DWORD)&time,//���[�U�[��`�̒l
		                    TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
		                    );

	// simulation�X���b�h�̐����E�J�n
	FTimerId2 = timeSetEvent(1000 / SIMULATION_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    StepSimulation,//�R�[���o�b�N�֐�
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
	sd.timeStep = (double)1.0 / SIMULATION_FREQ;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;
	desc.mass = 0.5;
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
		sd.radius = 1.25;
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
	view_haptic = view.Rot();
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