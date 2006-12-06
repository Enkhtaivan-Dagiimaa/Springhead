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
#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include <stdio.h>

// debug�o�͗p
#include <fstream>
#include "Foundation/UTPreciseTimer.h"

#include "HIS/HIS.h"
#include "Physics/PHConstraintEngine.h"
#include "Physics/PHConstraint.h"
#include "Physics/PHContactPoint.h"
#include <Physics/PHSpatial.h>

#include <mmsystem.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;

#define ESC				27			// ESC key
//#define EXIT_TIMER		10000		// ���s�X�e�b�v��
#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define NUM_SPHERES		2			// sphere��
#define SPIDAR_SCALE	70			// SPIDAR��VE���ł̓���X�P�[��


#define LIMIT_DEPTH 100				// �\���V�~�����[�V�������s�����̎擾�̐[�����
#define NUM_PREDICT_ITERATE 5		// �\���V�~�����[�V�����̃C�e���[�g��

#define NUM_COLLISIONS 20			// �|�C���^�ւ̋��e�ڐG��
#define NUM_COL_SOLIDS 20			// �|�C���^�ւ̋��e�ڐG���̐��@NUM_COLLISIONS�Ƌ�ʂ���̂̓v���O������ǂ݂₷�����邽�߁B�����I�ȑ��݈Ӌ`�͂Ȃ�


#ifdef _DEBUG
	#define SIMULATION_FREQ	60         // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		500			// �͊o�X���b�h�̎���Hz
	float Km = 550;						// virtual coupling�̌W��
	float Bm = 30;						// ���i

	float Kr = 150;						// ��]
	float Br = 13;

#elif _WINDOWS
	#define SIMULATION_FREQ	60          // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000		// �͊o�X���b�h�̎���Hz
	float Km = 1000;						// virtual coupling�̌W��
	float Bm = 60;						// ���i

	float Kr = 1000;						// ��]
	float Br = 150;
#endif

// �񎦗͂ƍ��̂ɒ񎦂���͂𒼐ڕω�������萔
double FORCE_COEFF =		0.5;

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

// ���z���E�ň�������
PHSolidIf* soFloor;
vector<PHSolidIf*> soSphere; 
PHSolidIf* soPointer;

// SPIDAR�p�̕ϐ�
DVDeviceManager devMan;						//	D/A��J�E���^�Ȃǂ̃f�o�C�X�̊Ǘ�
HISpidarG6X3 spidarG6;						//	SPIDAR�ɑΉ�����N���X

// ���̑��̕ϐ�
Vec3f spidar_pos = Vec3f();
Matrix3f view_rot;
Matrix3f view_haptic;

// multimedia timer�̃n���h��
MMRESULT FTimerId1;


// �񎦗͂��o�����ǂ���
bool bforce = false;

// �߂荞�ݕ␳�̃t���O
bool bCorrectPenetration = true;

// �������f�����̃t���O
bool bGradualReflection = true;

// �Ǐ��I���͊w�v�Z
bool bLocalDynamics = true;

// debug�o�͗pfstream
ofstream ofs;
bool bOutput = false;

// debug�p�^�C�}�[
UTPreciseTimer timer;

// �V�~�����[�V���������邩�ǂ���������킷�t���O
// mmtimer�œ���HapticRendering�����Ԃ��J�E���g����
// �K�؂Ȏ�����ON�ɂ���
// idle function���V�~�����[�V�������s��
bool bSimulation = true;

// �͊o�v�Z�ɕK�v�ȃf�[�^���W�߂��\����
// �͊o�v�Z�������ōs����悤��
// �V�~�����[�V�����Ńf�[�^���i�[���A
// HapticRendering�ɓn��
typedef struct {
	// collision data
	PHConstraint* constraint[NUM_COLLISIONS];

	// collision solid data
	PHSolid* nearest_solids[NUM_COL_SOLIDS];

	// map from physical data to rigid body itself
	int ColToSol[NUM_COLLISIONS];
	int SolToCol[NUM_COL_SOLIDS][NUM_COLLISIONS];
	int num_col_per_sol[NUM_COL_SOLIDS];
	int num_solids;

	// physical data
	Vec3d solid_velocity[NUM_COL_SOLIDS];
	Vec3d solid_angular_velocity[NUM_COL_SOLIDS];

	Vec3d solid_current_center_positions[NUM_COL_SOLIDS];
	Vec3d solid_future_center_positions[NUM_COL_SOLIDS];
	Quaterniond solid_current_orientations[NUM_COL_SOLIDS];
	Quaterniond solid_future_orientations[NUM_COL_SOLIDS];

	double solid_massinvs[NUM_COL_SOLIDS];
	Matrix3d solid_inertiainvs[NUM_COL_SOLIDS];

	// collision data
	Vec3d future_col_positions[NUM_COLLISIONS];
	Vec3d future_col_normals[NUM_COLLISIONS];
	int num_collisions;
	
	Vec3d current_col_positions[NUM_COLLISIONS];
	Vec3d current_col_normals[NUM_COLLISIONS];

	Vec3d pointer_current_col_positions[NUM_COLLISIONS];
	Vec3d pointer_future_col_positions[NUM_COLLISIONS];

	// ���͂̉e���̌��ʂ��i�[����ϐ�
	Matrix3d vel_effect[NUM_COL_SOLIDS][NUM_COLLISIONS];
	Vec3d vel_constant[NUM_COLLISIONS];

	Matrix3d ang_effect[NUM_COL_SOLIDS][NUM_COLLISIONS];
	Vec3d ang_constant[NUM_COLLISIONS];

	// pointer data
	Vec3d pointer_pos;

	Vec3d pointer_vel;
	double pointer_massinv;

	Quaterniond pointer_ori;
	Vec3d pointer_angvel;
	Matrix3d pointer_inertiainv;

	// flag for gradual reflection
	bool bLocalReflection1[NUM_COL_SOLIDS];
	bool bLocalReflection2[NUM_COL_SOLIDS];
} HapticInfo;

// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
HapticInfo info1;
HapticInfo info2;

// true�̂Ƃ��͍\���̂P���Q��
// false�̂Ƃ��͍\���̂Q���Q��
bool current_valid_data = true; 

// ���͂̉e����L���ɂ��邩�ǂ��������߂�t���O
bool bSurroundEffect = false;

// SPIDAR�̈ʒu��\�����邩�ǂ���
bool bDisplayPointer = true;

// K��B�̒l�ǂ���̕ύX��L���ɂ��邩
// true�̏ꍇ���i
// false�̏ꍇ��]
bool bMode = true;

// �^����ꂽ�ڐG�ŁA������solid�ɐڂ��Ă��鍄�̂�����ׁA����ΕԂ�
PHSolid* getAdjacentSolid(PHConstraint* constraint, PHSolid* solid, int* sign = NULL)
{
	if(constraint->solid[0] == solid)
	{
		if(sign != NULL) *sign = -1;
		return constraint->solid[1];
	}
	else if(constraint->solid[1] == solid)
	{
		if(sign != NULL) *sign = 1;
		return constraint->solid[0];
	}
	else return NULL;
}

// �񎦗͂̌v�Z����ђ�Ď�@�ɕK�v�ȏ����W�߂č\���̂��쐬����
void MakeHapticInfo(HapticInfo *info, HapticInfo *prev_info, vector<pair<PHConstraint *, int> > pointer_consts, map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices, map<PHSolid *, SpatialVector> vecs, vector<pair<PHConstraint *, int> > pointer_static_consts)
{
	// ����������
	// �O�X��̃f�[�^�͂����K�v�Ȃ��̂ŏ���������
	// ����ȊO�̃f�[�^�͏㏑�����A����Ɍ��ŃA�N�Z�X�𐧌�����̂�
	// �������̕K�v�͂Ȃ�
	for(int i = 0; i < info->num_solids; i++) 
	{
		info->nearest_solids[i] = NULL;
		info->num_col_per_sol[i] = 0;
	}
	info->num_solids = 0;

	int consts_size = (int)pointer_consts.size();
	int static_consts_size = (int)pointer_static_consts.size();

	// �͊o�����_�����O�ɕK�v�ȃf�[�^�̍쐬
	for(int i = 0; i < consts_size + static_consts_size; i++)
	{
		// ���e���𒴂���ꍇ�͑ł��؂�
		if(i > NUM_COLLISIONS) break;

		PHConstraint* constraint;
		int sign;
		PHSolid* so;

		// �������̂��܂ސڐG
		if(consts_size > i)
		{
			constraint = pointer_consts[i].first;
			sign = pointer_consts[i].second;

			if(sign > 0) so = pointer_consts[i].first->solid[0];
			else so = pointer_consts[i].first->solid[1];
		}
		// �����Ȃ����̂��܂ސڐG
		else
		{
			constraint = pointer_static_consts[i - consts_size].first;
			sign = pointer_static_consts[i - consts_size].second;

			if(sign > 0) so = pointer_static_consts[i - consts_size].first->solid[0];
			else so = pointer_static_consts[i - consts_size].first->solid[1];
		}


		int counter = 0;
		// �z���PHSolid��ǉ�
		// ���̂ƐڐG���P�΂P�ł͂Ȃ����߁A�ڐG���獄�̂֓Y������ϊ�����}�b�v���쐬����
		while(1)
		{
			// �܂��ǉ�����Ă��Ȃ����̂͒ǉ�����
			if(!info->nearest_solids[counter])
			{
				info->nearest_solids[counter] = so;
				
				// ���̂���ڐG��
				// �܂��ڐG���獄�̂֎��Ԃ��������ɕϊ��ł���悤��
				// �z��ɓY�������i�[���Ă���
				info->SolToCol[counter][info->num_col_per_sol[counter]] = i;
				info->num_col_per_sol[counter]++;

				info->ColToSol[i] = counter;
				info->num_solids = counter+1;

				// ���x�������̂��Ƃɒǉ�
				info->solid_velocity[counter] = so->GetVelocity();
				info->solid_angular_velocity[counter] = so->GetAngularVelocity();
				info->solid_massinvs[counter] = so->GetMassInv();
				info->solid_inertiainvs[counter] = so->GetInertiaInv();

				// ���͂̉e��������킷�ϐ����R�s�[����
				// ���̕ϐ��͍��̌ŗL�̒l�Ȃ̂ŁA
				// ���̂��ǉ����ꂽ���_�Ŋi�[�������s��
				if(so->IsDynamical())
				{
					map<PHSolid *, SpatialVector>::iterator it1 = vecs.find(so);
					SpatialVector sv = (*it1).second;
					info->vel_constant[counter] = sv.v;
					info->ang_constant[counter] = sv.w;

					map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >::iterator it2 = matrices.find(so);
					vector<pair<Matrix3d, Matrix3d> > mm_vector = (*it2).second;
					for(int j = 0; j < (int)mm_vector.size(); j++)
					{
						info->vel_effect[counter][j] = mm_vector[j].first;
						info->ang_effect[counter][j] = mm_vector[j].second;
					}
				}
				else
				{
					info->vel_constant[counter] = Vec3d();
					info->ang_constant[counter] = Vec3d();
				}


				bool current_exists = false;
				// �O������̍��̂ƃ|�C���^�̐ڐG�����݂������`�F�b�N���A
				// ���݂����ꍇ�͏��X�ɖ߂��悤�Ƀf�[�^���i�[����
				for(int j = 0; j < prev_info->num_solids; j++)
				{
					if(prev_info->nearest_solids[j] == so && so->IsDynamical() && bLocalDynamics)
					{
						info->solid_current_center_positions[counter] = prev_info->solid_current_center_positions[j];
						info->solid_future_center_positions[counter] = so->GetCenterPosition() - info->solid_current_center_positions[counter];
						info->solid_current_orientations[counter] = prev_info->solid_current_orientations[j];
						info->solid_future_orientations[counter] = so->GetOrientation();

						current_exists = true;				
						break;
					}
				}

				// �O��͑��݂��Ȃ�����
				if(!current_exists)
				{
					info->solid_current_center_positions[counter] = so->GetCenterPosition();
					info->solid_future_center_positions[counter] = Vec3d();
					info->solid_current_orientations[counter] = so->GetOrientation();
					info->solid_future_orientations[counter] = Quaterniond();
				}

				// �V�~�����[�V�����̌��ʂ�͊o�����_�����O�̃v���Z�X�ŏ��X�ɔ��f�����鏈���������t���O��������
				info->bLocalReflection1[counter] = true;
				info->bLocalReflection2[counter] = true;

				break;
			}
			// �������̂����łɒǉ��ς݂̏ꍇ�͒ǉ����Ȃ�
			// �Y�����ϊ��\�̂ݍ쐬
			else if(info->nearest_solids[counter] == so)
			{
				info->SolToCol[counter][info->num_col_per_sol[counter]] = i;
				info->num_col_per_sol[counter]++;

				info->ColToSol[i] = counter;

				break;
			}

			// ���̓o�^���ꂽ���̂ɐi��
			counter++;
		}

		info->constraint[i] = constraint;
		PHContactPoint* contact = DCAST(PHContactPoint, constraint);

		bool current_exists = false;

		// �����ڐG���O�񑶍݂������`�F�b�N
		for(int j = 0; j < prev_info->num_collisions; j++)
		{
			// ���������ꍇ�O��̖@���ƐڐG�_��ۑ���
			// ����̃f�[�^��ڕW�Ƃ��ď��X�ɍX�V���s��
			if(prev_info->constraint[j] == constraint && bLocalDynamics)
			{
				info->current_col_positions[i] = prev_info->current_col_positions[j];
				info->future_col_positions[i] = contact->pos;

				info->current_col_normals[i] = prev_info->current_col_normals[j];
				info->future_col_normals[i] = contact->shapePair->normal * sign;

				info->pointer_current_col_positions[i] = prev_info->pointer_current_col_positions[j];
				info->pointer_future_col_positions[i] = contact->pos;

				current_exists = true;
				break;
			}
		}

		// �O�񓯂��ڐG�����݂��Ȃ������̂�
		// �ڕW�̐ڐG�̖@���ƐڐG�_�͂Ȃ�
		if(!current_exists)
		{
			info->current_col_positions[i] = contact->pos;
			info->future_col_positions[i] = Vec3d();

			info->current_col_normals[i] = contact->shapePair->normal * sign;
			info->future_col_normals[i] = Vec3d();

			// �|�C���^�̐ڐG���_�͐ڐG�_�Ɠ���
			info->pointer_current_col_positions[i] = info->current_col_positions[i];
			info->pointer_future_col_positions[i] = Vec3d();
		}
	}

	info->num_collisions = consts_size + static_consts_size;

	// ���̃|�C���^�̏����i�[
	info->pointer_pos = soPointer->GetFramePosition();
	info->pointer_vel = soPointer->GetVelocity();
	info->pointer_massinv = soPointer->GetMassInv();
	info->pointer_angvel = soPointer->GetAngularVelocity();
	info->pointer_inertiainv = soPointer->GetInertiaInv();
	info->pointer_ori = soPointer->GetOrientation();
}

/**
 brief     	���̂̐ڐG�_���擾����֐�
 param		�Ȃ�
 return 	�ڐG�_�̏W��
*/
PHConstraints GetContactPoints(PHSceneIf* scene1)
{
	return scene1->GetConstraintEngine()->GetContactPoints();
}

// �ċA�I�ɐڂ��Ă��鍄�̂��擾����֐�
void RecursiveSolidRetrieval(map<PHConstraint *, bool>* csm, PHSolid* solid, set<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, int depth)
{
	for(map<PHConstraint *, bool>::iterator it = csm->begin(); it != csm->end(); it++)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if(it->second) continue;

		// �^����ꂽ���̂���A���̍��̂ɐڐG���鍄�̂��擾����
		PHSolid* col_solid = getAdjacentSolid(it->first, solid);

		// �������̍��̂����݂��A�|�C���^�ł͂Ȃ��ꍇ
		// �|�C���^�̏ꍇ��GetSolidsFromPointer�Ŏ擾����
		if(col_solid && col_solid != DCAST(PHSolid, soPointer))
		{
			// �����ς݂ɂ���
			it->second = true;

			// �ڐG���̂�ǉ�
			PHConstraint* c = it->first;
			pair<set<PHConstraint *>::iterator, bool> p = relative_consts->insert(c);

			// PHSolid��ۑ�
			// �d�����Ă���ꍇ��set�Ȃ̂Ŏ����I�ɔr�������
			relative_solids->insert(c->solid[0]);
			relative_solids->insert(c->solid[1]);

			// �����Ȃ����̂ł͂Ȃ��A���擾����܂ŒB���Ă��Ȃ������ꍇ�A���̍��̂ɂ�����
			if(col_solid->IsDynamical() && depth < LIMIT_DEPTH) 
				RecursiveSolidRetrieval(csm, col_solid, relative_consts, relative_solids, depth++);
		}
	}
}

// �|�C���^����X�^�[�g���ĕK�v�ȍ��̂����ׂĎ擾���Ă���֐�
void GetSolidsFromPointer(vector<pair<PHConstraint *, int> >* pointer_consts, set<PHSolid *>* pointer_relative_solids, set<PHSolid *>* relative_solids, set<PHConstraint *>* relative_consts, vector<pair<PHConstraint *, int> >* pointer_static_consts)
{
	// �Փ˓_���擾
	PHConstraints cs = GetContactPoints(scene);
	map<PHConstraint *, bool> csm;

	// �v�Z���Ԃ�Z�k���邽�߂Ƀ}�b�v�ɏ����σt���O�����ĕۑ�����
	// �ڐG�������Ȃ��ꍇ�͖��ʂ��������A
	// �ڐG���������Ȃ����Ƃ��̐L�т͂�����̂ق����f�R���Ȃ��͂�
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
	{
		csm.insert(pair<PHConstraint *, bool>(*it, false));
	}

	// ���ׂĂ̏Փ˓_�ɂ��Ē���
	for(map<PHConstraint *, bool>::iterator it = csm.begin(); it != csm.end(); it++)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if(it->second) continue;

		int sign = 1;

		// �|�C���^���܂ސڐG���擾
		PHSolid* col_solid = getAdjacentSolid(it->first, DCAST(PHSolid, soPointer), &sign);
		
		// �����ł���
		if(col_solid)
		{
			// �����ςɂ���
			it->second = true;

			// �|�C���^�Ɛڂ��Ă��鍄�̂�PHSolid��ۑ�
			if(sign > 0)
			{
				relative_solids->insert(it->first->solid[0]);
				pointer_relative_solids->insert(it->first->solid[0]);
			}
			else
			{
				relative_solids->insert(it->first->solid[1]);
				pointer_relative_solids->insert(it->first->solid[1]);
			}

			// ���������鍄�̂ł���΁A�ڐG���̂��i�[
			// �����Ȃ����͕̂ʂȃx�N�g���ɒǉ�
			// �����闝�R�͌�ōs���V�~�����[�V�����ŁA�����Ȃ����̂ƃ|�C���^�̐ڐG���܂߂�͈̂Ӗ����Ȃ�����
			if(col_solid->IsDynamical())pointer_consts->push_back(pair<PHConstraint *, int>(it->first, sign));
			else pointer_static_consts->push_back(pair<PHConstraint *, int>(it->first, sign));

// ���͂̉e�����v�Z���邽�߂̃f�[�^�쐬����
#if 1
			relative_consts->insert(it->first);

			// ���̍��̂Ɛ������ڐG�����̍��̂ɉe�����y�ڂ��\��������ꍇ�́A���̍��̂ɂ�����
			if(col_solid->IsDynamical())
				RecursiveSolidRetrieval(&csm, col_solid, relative_consts, relative_solids, 0);
#endif
		}
	}
}

map<PHConstraint *, SpatialVector> lambda;

// �����_�̒l��ۑ����Ă����֐�
void SaveLambda(set<PHConstraint *> relative_consts)
{
	// lambda��������
	lambda.clear();

	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); it++)
	{
		// �����_�̒l(fv, fw)��ڐG�ƃy�A�ŕۑ����Ă���
		lambda.insert(pair<PHConstraint *, SpatialVector>(*it, (*it)->f));
	}
}

// �摗��V�~�����[�V����������֐�
map<PHSolid *, SpatialVector> PredictSimulation(vector<pair<PHConstraint *, int> > pointer_consts, set<PHSolid *> pointer_relative_solids, set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index = -1)
{
	SpatialVector dv_save;
	PHSolid* si;

	// �|�C���^���ڂ��鍄�̂̂���index�Ԗڂ̐ڐG�ɒP�ʗ͂�������
	if(index != -1)
	{
		double dt = scene->GetTimeStep();

		PHConstraint *constraint = pointer_consts[index].first;
		PHContactPoint* contact = DCAST(PHContactPoint, constraint);
			
		if(pointer_consts[index].second > 0) si = DCAST(PHSolid, constraint->solid[0]);
		else si = DCAST(PHSolid, constraint->solid[1]);

		// ���O�Ɍv�Z���Ă������l���Q��
		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);

		// �P�ʗ͂�������O��dv��ۑ����Ă���
		dv_save = si->dv;

		// �����ŗ^����ꂽ�P�ʗ͂�������
		si->dv.v += si->GetMassInv() * force_vec * dt;
		si->dv.w += si->GetInertiaInv() * (((contact->pos - si->GetCenterPosition()) ^ force_vec) - cef->second) * dt;
	}

	// �V�~�����[�V�����ɕK�v�ȏ����Z�b�g�A�b�v
	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); it++)
	{
		(*it)->SetupLCP();
	}

	for(int i = 0; i < NUM_PREDICT_ITERATE; i++)
	{
		for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); it++)
		{
			// ���ׂĂ̐ڐG��iterate
			(*it)->IterateLCP();
		}
	}

	map<PHSolid *, SpatialVector> effects;

	// �e�����ϑ�
	// ���̂̑��x�ω����擾
	for(set<PHSolid *>::iterator it = pointer_relative_solids.begin(); it != pointer_relative_solids.end(); it++)
	{
		effects.insert(pair<PHSolid *, SpatialVector>(*it, (*it)->dv));
	}

	// dv�̒l�����ɖ߂�
	if(index != -1)si->dv = dv_save;

	// fv��fw�̒l�����ɂ��ǂ�
	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); it++)
	{
		(*it)->f = lambda.find(*it)->second;
	}

	// �e����return
	return effects;
}

// �\���V�~�����[�V�������Z�b�g�A�b�v����
void SetupPredictSimulation(set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, set<PHSolid *> pointer_relative_solids, map<PHSolid *, Vec3d> *coeff)
{
	double dt = scene->GetTimeStep();

	// ���ׂĂ̍��̂��Z�b�g�A�b�v
	for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); it++)
	{
		(*it)->UpdateCacheLCP(dt);
	}

	for(set<PHSolid *>::iterator it = pointer_relative_solids.begin(); it != pointer_relative_solids.end(); it++)
	{
		PHSolid* solid = *it;

		// �P�ʗ͂������鏈���̂Ƃ���Ŏg���l(=�������͂����[���h���W����S�����W�ɕϊ��������)�����̂����Ɍv�Z���Ă���
		Vec3d w = solid->GetOrientation().Conjugated() * solid->GetAngularVelocity();
		Vec3d calc = w % (solid->GetInertia() * w);
		coeff->insert(pair<PHSolid *, Vec3d>(*it, calc));
	}

	// ���ׂĂ̐ڐG�ɂ��ă��R�r�A�����v�Z
	// ���R�r�A���̌v�Z�͈�x�����ł悢
	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); it++)
	{
		(*it)->CompJacobian();
	}

	// �V�~�����[�V�����ŏ㏑�����Ă��܂��ϐ������̂����ɕۑ�����
	SaveLambda(relative_consts);
}


// ��Ď�@�̂��߂ɐ摗��V�~�����[�V�������v��I�ɍs���Ă����֐�
void PredictSimulations(vector<pair<PHConstraint *, int> > pointer_consts, set<PHSolid *> pointer_relative_solids, set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > *mat, map<PHSolid *, SpatialVector> *vec)
{
	map<PHSolid *, Vec3d> coeff;

	// �K�v�ȏ��̏���
	SetupPredictSimulation(relative_consts, relative_solids, pointer_relative_solids, &coeff);

	// ������͂��O�x�N�g���Ƃ��Ē萔�����擾
	map<PHSolid*, SpatialVector> b = PredictSimulation(pointer_consts, pointer_relative_solids, relative_consts, relative_solids, Vec3d(), coeff);

	// �K�؂Ƀf�[�^���i�[����map-vector�̏���
	// ���̐��������vector��錾����map�ɒǉ�����
	map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > > mm_map;
	for(map<PHSolid*, SpatialVector>::iterator it = b.begin(); it != b.end(); it++)
	{
		mm_map.insert(pair<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >((*it).first, vector<pair<Matrix3d, Matrix3d> >()));
	}

	// �ڐG�����񐔂����J��Ԃ�
	for(int i = 0; i < (int)pointer_consts.size(); i++)
	{
		// ����P�ʃx�N�g���������Ă��̌��ʂ�����킷�x�N�g�����擾
		map<PHSolid *, SpatialVector> vec_x = PredictSimulation(pointer_consts, pointer_relative_solids, relative_consts, relative_solids, Vec3d(1, 0, 0), coeff, i);
		map<PHSolid *, SpatialVector> vec_y = PredictSimulation(pointer_consts, pointer_relative_solids, relative_consts, relative_solids, Vec3d(1, 1, 0), coeff, i);
		map<PHSolid *, SpatialVector> vec_z = PredictSimulation(pointer_consts, pointer_relative_solids, relative_consts, relative_solids, Vec3d(1, 0, 1), coeff, i);

		// ���̃��[�v�ō���so�̐ڐGpointer_consts[i]�Ɋւ���s����v�Z���i�[
		// ���̐ڐG�̂��ׂĂ̍��̂ɑ΂���e�������̃��[�v�ŏ�������
		for(map<PHSolid *, SpatialVector>::iterator it = b.begin(); it != b.end(); it++)
		{
			PHSolid *so = it->first;
			SpatialVector sv = it->second;
			SpatialVector sv_x = vec_x.find(so)->second;
			SpatialVector sv_y = vec_y.find(so)->second;
			SpatialVector sv_z = vec_z.find(so)->second;

			// ��قǉ������P�ʗ͂���A�e�������߂čs����쐬����
			// �Ȃ��g���N�ɂ��Ă͔��a�Ɨ͂̊O�ς����������
			// ���a�̊O�ς��s�񉻂���Ă��łɔ��f����Ă���͂��ł���
			Matrix3d v = Matrix3d(sv_x.v - sv.v, sv_y.v - sv_x.v - sv.v, sv_z.v - sv_x.v - sv.v);
			Matrix3d w = Matrix3d(sv_x.w - sv.w, sv_y.w - sv_x.w - sv.w, sv_z.w - sv_x.w - sv.w);

			// �f�[�^�̊i�[�B			
			pair<Matrix3d, Matrix3d> p = pair<Matrix3d, Matrix3d>(v, w);

			// ���̂��ƂɐڐG�ƌW���s��̃}�b�v���쐬����
			// �ڐG�̏��Ԃ͂킩�邪���̂̏��Ԃ͂킩��Ȃ����߁B
			// ���̂��ƂɐڐG�̏��Ԃɍs�񂪊i�[����Ă���
			mm_map.find(so)->second.push_back(p);
		}
	}

	*mat = mm_map;
	*vec = b;
}


// ���͂̉e�����v�Z���A�����̕ϐ��Ɋi�[����֐�
void calculate_surround_effect(HapticInfo* info, HapticInfo* prev_info)
{
	vector<pair<PHConstraint *, int> > pointer_consts;
	vector<pair<PHConstraint *, int> > pointer_static_consts;
	set<PHSolid *> pointer_relative_solids;

	set<PHSolid *> relative_solids;
	set<PHConstraint *> relative_consts;

	// �|�C���^����X�^�[�g���ĕK�v�ȍ��̂����ׂĎ擾���Ă���֐�
	GetSolidsFromPointer(&pointer_consts, &pointer_relative_solids, &relative_solids, &relative_consts, &pointer_static_consts);

	map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices;
	map<PHSolid *, SpatialVector> vecs;

	// �͂������Ă݂ē������A�e�����ϑ�����֐�
	PredictSimulations(pointer_consts, pointer_relative_solids, relative_consts, relative_solids, &matrices, &vecs);

	// �ڐG�̏����v�Z����
	MakeHapticInfo(info, prev_info, pointer_consts, matrices, vecs, pointer_static_consts);
}

/**
 brief     	���̂̐ڐG�_�̏���\������֐�
 param		�Ȃ�
 return 	�Ȃ�
*/
void show_collision_info()
{
	// �Փ˓_���擾
	PHConstraints cp = GetContactPoints(scene);

	// �ڐG�_�����݂���Ώ�������
	if(cp.size() > 0)
	{
		// �Փ˂�\������Ƃ��̌��𐔂���J�E���^�[
		static double global_collision_counter = 0;
		int counter = 0;
		cout << "----------NEW COLLISION DATA No." << global_collision_counter++ << "-----------" << endl;
		
		// ���ׂĂ̏Փ˂ɑ΂��ď���
		for(PHConstraints::iterator it = cp.begin(); it != cp.end(); it++)
		{
			// ���[�v���̃J�E���^�[���X�V
			counter++;
			cout << "COLLISION" << counter << ": " << endl;

			// PHContactPoint��dynamic cast���������̃`�F�b�N
			PHContactPoint* contact = DCAST(PHContactPoint, (*it));
			cout << "collision pos = " << contact->pos << endl;
			cout << endl;

			// PHConstraint�̏��̕\��
			// solid1
			cout << "solid1.mass = " << (*it)->solid[0]->mass << endl;
			cout << "solid1.inertia = " << endl << (*it)->solid[0]->inertia;
			cout << "solid1.velocity = " << (*it)->solid[0]->GetVelocity()<< endl;
			cout << "solid1.angular_velocity = " << (*it)->solid[0]->GetAngularVelocity() << endl;
			cout << "solid1.position = " << (*it)->solid[0]->GetFramePosition() << endl;
			cout << "vector to working point from center = " << (contact->pos - (*it)->solid[0]->GetCenterPosition()).unit() << endl;

			if(current_valid_data)
			{
				for(int i = 0; i < info1.num_solids; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = (PHSolidIf *)&(*info1.nearest_solids[i]);
					if(nearest_address == (PHSolidIf *)((*it)->solid[0]))cout << " --this solid is nearest object!!" << endl;
				}
			}
			else 
			{
				for(int i = 0; i < info2.num_solids; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = (PHSolidIf *)&(*info2.nearest_solids[i]);
					if(nearest_address == (PHSolidIf *)((*it)->solid[0]))cout << " --this solid is nearest object!!" << endl;
				}

			}
			cout << endl;

			// solid2
			cout << "solid2.mass = " << (*it)->solid[1]->mass << endl;
			cout << "solid2.inertia = " << endl << (*it)->solid[1]->inertia;
			cout << "solid2.velocity = " << (*it)->solid[1]->GetVelocity()<< endl;
			cout << "solid2.angular_velocity = " << (*it)->solid[1]->GetAngularVelocity() << endl;
			cout << "solid2.position = " << (*it)->solid[1]->GetFramePosition() << endl;
			cout << "vector to working point from center = " << (contact->pos - (*it)->solid[1]->GetCenterPosition()).unit() << endl;

			if(current_valid_data)
			{
				for(int i = 0; i < info1.num_solids; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]))cout << " --this solid is nearest object!!" << endl;
				}
			}
			else
			{
				for(int i = 0; i < info2.num_solids; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info2.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]))cout << " --this solid is nearest object!!" << endl;
				}
			}
			cout << endl;
		}
	}
}



// �����ɂ����鎞�Ԃ��v���B�ꎞ�I�Ȃ���
static int start, end;

void t_start()
{
	start = timer.CountUS();
}

void t_end()
{
	// ����ɂ����鎞�Ԃ��`�F�b�N
	end = timer.CountUS();
	if(time > 0 && bOutput)ofs << "time = " << end - start << endl;
}
/**
 brief  	�񎦗͂̌v�Z��Haptic Device	�֔��f. multimedia timer�ɂ���ČĂяo�����
			�����I�ɂ̓|�C���^��SPIDAR��virtual coupling�łȂ��A�|�C���^���X�V���Ă���
 param	 	�Ȃ�
 return 	�Ȃ�
*/
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
//	t_start();
	
//	t_end();
//	t_start();

	// �V�~�����[�V�����Ɨ͊o�����_�����O�̍X�V�����̔䗦������킷�ϐ�
	static const double ratio = (double)SIMULATION_FREQ / HAPTIC_FREQ;

	// �V�~�����[�V�������Ǘ�����J�E���^�[���쐬
	static int step_counter = 0;
	if(step_counter == (int)ratio)
	{
		// �V�~�����[�V�������I����Ă�����V�����f�[�^�ɐ؂�ւ��鏈��������
		// �I����Ă��Ȃ�������d�����Ȃ��̂őO��̃f�[�^�����̂܂܎g��
		// (�����ɔ�΂��Ɨ͊o�Ɉ�a����������
		// �J�E���^�[���X�V����Ȃ��̂Ŏ��ł�����x�����ɂ���
		if(!bSimulation)
		{
			bSimulation = true;
			step_counter = 0;
			current_valid_data = !current_valid_data;
		}
		else if(bOutput)ofs << "error : simulation could not be finished in time" << endl;
	}
	else step_counter++;

	// SPIDAR�X�V��
	static const double dt = (double)1.0 / HAPTIC_FREQ;

	// �V�~�����[�V�������瓾��������i�[����ϐ�
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data) info = &info1;
	else info = &info2;


	spidarG6.Update(dt);//	USB�̒ʐM���s���D


	/////////////////////////////////////////////////////////////////////////////////////////
	// Virtual Coupling
	// ���i
	// SPIDAR�̈ʒu�̍X�V
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

	// SPIDAR�̏����i�[����ϐ�
	static Vec3d old_pos = spidar_pos;

	// SPIDAR�̑��x���v�Z
	// ���x�̒�`�ł� v = dx / dt ����
	// dt�Ŋ���Ȃ��ق�������݂����Ȃ̂�
	// �����������g�����Ƃɂ���
	Vec3d SPIDARVel = spidar_pos - old_pos;
	old_pos = spidar_pos;

	// SPIDAR�̈ʒu�܂ł̃x�N�g�����쐬
	Vec3d goal = spidar_pos - info->pointer_pos;

	// VR��Ԃ̃|�C���^��SPIDAR��virtual coupling�łȂ���
	Vec3d VCForce = Km * goal + Bm * (SPIDARVel - info->pointer_vel);


	/////////////////////////////////////////////////////////////////////////////////////////
	// Virtual Coupling
	// ��]
	// SPIDAR�̎p�����X�V
	Quaterniond qv;
	qv.FromMatrix(view_rot);

	static Quaterniond spidar_ori;
	spidar_ori = qv * spidarG6.GetOri();

	static Quaterniond old_ori = spidar_ori;

	// SPIDAR�̉�]���x���v�Z
	// �O�̎p�����猻�݂̎p���ɕϊ�����S�������v�Z
	Quaterniond SPIDARAngVel = spidar_ori * old_ori.Inv();
	old_ori = spidar_ori;

	// ���݂̃|�C���^�̎p������SPIDAR�̎p���܂ł̉�]��\���S�������v�Z
	Quaterniond ang_goal = spidar_ori * info->pointer_ori.Inv();

	// ��]�ɂ��Ẵo�[�`�����J�b�v�����O
	Vec3d VCTorque = Kr * ang_goal.Rotation() + Br * (SPIDARAngVel.Rotation() - info->pointer_angvel);


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// �o�[�`�����J�b�v�����O�ɏ]���ă|�C���^�̈ʒu�E�p���X�V
	info->pointer_vel += info->pointer_massinv * VCForce * dt;
	Vec3d pointer_dx = info->pointer_vel * dt;
	info->pointer_pos += pointer_dx;

	info->pointer_angvel += info->pointer_inertiainv * VCTorque * dt;
	Vec3d pointer_dth = info->pointer_angvel * dt;
	info->pointer_ori = Quaterniond::Rot(pointer_dth) * info->pointer_ori;
	info->pointer_ori = info->pointer_ori.unit();


	////////////////////////////////////////////////////////////////////////////////////////
	// �͊o�v�Z
	// ���[�U�ւ̃t�H�[�X�t�B�[�h�o�b�N�ƐڐG���̂̉����x�X�V

	// �o�^���ꂽ�ڐG�������Ă��A���ݖ{���ɐڐG���Ă��邩�͂킩��Ȃ��̂Ńt���O��p��
	bool feedback = false;

	if(bLocalDynamics)
	{
		for(int i = 0; i < info->num_collisions; i++)
		{
			// �ʂ̖@���ƁA�|�C���^��̓_���獄�̏�̓_�܂ł����񂾃x�N�g���̓��ς��v�Z
			// ���ꂪ�O�ȏ�Ȃ�i�[���x�N�g�����܂ށB���傤�ǐڂ��Ă���j�ڐG������
			if(dot(info->current_col_positions[i] - info->pointer_current_col_positions[i], info->current_col_normals[i]) >= 0)
			{
				// �͂������Ă悢�Ώۂ��`�F�b�N
				// �͂̌����ƍ��̂̐ڂ��鑤�̃`�F�b�N
				if(dot(info->current_col_normals[i], -VCForce) >= 0)
				{
					// ���[�U�ɗ͊o���
					feedback = true;

					// ���̂̓Y����
					int sol_index = info->ColToSol[i];
			
					// �O�͂œ������̂ł���Α��x���ω�����͂��Ȃ̂ŉ����x���v�Z���đ��x���X�V����
					if(info->nearest_solids[sol_index]->IsDynamical())
					{
						Vec3d accel;
						Vec3d ang_accel;

						// �Ǐ��I�ȓ��͊w�v�Z�̂�
						if(!bSurroundEffect)
						{
							accel = info->solid_massinvs[sol_index] * FORCE_COEFF * VCForce;
							ang_accel = info->solid_inertiainvs[sol_index] * ((info->current_col_positions[i] - info->solid_current_center_positions[sol_index]) ^ (VCForce));

							// ����ꂽ�����x�ő��x���X�V
							// ���W�͌�ōX�V����
							info->solid_velocity[sol_index] += accel * dt;
										
							// �p���x���X�V
							info->solid_angular_velocity[sol_index] += ang_accel * dt;
						}
						// �Ǐ��I�ȓ��͊w�v�Z�{���͂̉e�����l��
						else
						{
							// ���̂ɉ�����͂��s��ɂ����āA�����x���v�Z����
							Vec3d q_f = info->solid_massinvs[sol_index] * dt * VCForce;

							// ���̗͂��y�ڂ��e�������ׂĂ̍��̂ɂ��Čv�Z����
							for(int j = 0; j < info->num_solids; j++)
							{
								// �O�͂œ����Ȃ����͔̂�΂�
								if(!info->nearest_solids[j]->IsDynamical())continue;

								// �s���p���ĉ����x���v�Z���đ��x���X�V
								info->solid_velocity[j] += info->vel_effect[j][i] * q_f;
								info->solid_angular_velocity[j] += info->ang_effect[j][i] * q_f;
							}
						}
					}
				}
			}
			else
			{
				// �����ڂ��Ă��Ȃ��̂ŁA�V�~�����[�V�������瓾��ꂽ�ڕW�n�_���Ӗ����Ȃ��Ȃ�
				info->bLocalReflection1[i] = false;
				info->bLocalReflection2[i] = false;
			}
		}
	}
	// �����ȃo�[�`�����J�b�v�����O
	else
	{
		// �ڐG������ΐڐG���Ă��鍄�̂ɒ񎦗͂̋t��������
		// ���̂̋����͕����V�~�����[�V�����ɔC����
		if(info->num_collisions)
		{
			// �͊o�t�B�[�h�o�b�NON
			feedback = true;
			for(int i = 0; i < info->num_collisions; i++)
			{
				int sol_index = info->ColToSol[i];
				info->nearest_solids[sol_index]->AddForce(FORCE_COEFF * VCForce, info->current_col_positions[i]);
			}
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���[�U�ւ̗͂̒�
	if(feedback && bforce)
	{
		// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
		Vec3d f = - view_haptic * VCForce;
		Vec3d t = - view_haptic * VCTorque;

		spidarG6.SetForce(FORCE_COEFF * f, FORCE_COEFF * t);

		// debug�o�͏���
//		if(bOutput)	ofs << FORCE_COEFF * f << " " << FORCE_COEFF * t << endl;
	}
	else spidarG6.SetForce(Vec3d(), Vec3d());


	if(bLocalDynamics)
	{
		Vec3d correct_vector = Vec3d();
		Vec3d correct_torque = Vec3d();

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// �Ǐ��I�V�~�����[�V�����ɂ��ڐG���鍄�̂̈ʒu�E�p�����X�V
		for(int i = 0; i < info->num_solids; i++)
		{
			if(!bSurroundEffect)
			{
				if(info->nearest_solids[i]->IsDynamical()) info->solid_velocity[i] += scene->GetGravity() * dt;
			}
			else
			{
				// ���͂̉e���̂����ŁA�萔�������X�ɉ����Ă���
				info->solid_velocity[i] += info->vel_constant[i] * ratio;
				info->solid_angular_velocity[i] += info->ang_constant[i] * ratio;
			}

			// ���x�ɂ��ړ��E��]
			Vec3d dx_local = info->solid_velocity[i] * dt;
			info->solid_current_center_positions[i] += dx_local;

			Vec3d dth_local = info->solid_angular_velocity[i] * dt;
			info->solid_current_orientations[i] = Quaterniond::Rot(dth_local) * info->solid_current_orientations[i];

			if(bGradualReflection)
			{
				// ���i
				if(info->solid_future_center_positions[i] != Vec3d())
				{
					// future�ɋ߂Â��Ă���
					Vec3d bias_center_mov_vec = ratio * (fabs(dot(dx_local, info->solid_future_center_positions[i])) * info->solid_future_center_positions[i].unit() - dx_local);
					
					info->solid_current_center_positions[i] += bias_center_mov_vec;
					dx_local += bias_center_mov_vec;
				}
				// ��]
				if(info->solid_future_orientations[i] != Quaterniond())
				{
					// future�̎p���ɋ߂Â��Ă���
					Vec3d bias_center_ori_vec = ratio * (info->solid_future_orientations[i].Axis() - info->solid_current_orientations[i].Axis());
					double theta = ratio * (info->solid_future_orientations[i].Theta() - info->solid_current_orientations[i].Theta());

					info->solid_current_orientations[i] = Quaterniond::Rot(theta + info->solid_current_orientations[i].Theta(), (bias_center_ori_vec + info->solid_current_orientations[i].Axis()).unit());
 					if(bias_center_ori_vec != Vec3d())dth_local += Quaterniond::Rot(theta, bias_center_ori_vec).Rotation();
				}
			}
			
			// ���̏�ŋN�������ڐG����������		
			for(int j = 0; j < info->num_col_per_sol[i]; j++)
			{
				// ���̍��̏�ɂ���ڐG�����̂̈ړ��ɔ����čX�V����
				int col_index = info->SolToCol[i][j];
				info->current_col_positions[col_index] += dx_local;
				info->current_col_normals[col_index] += dth_local ^ (info->current_col_positions[col_index] - info->solid_current_center_positions[i]);
				info->current_col_normals[col_index] = info->current_col_normals[col_index].unit();

				// �|�C���^�̐ڐG�_�X�V
				// �|�C���^�̈ړ��ʂɏ]���ĕ��s�ړ�
				// ���̏��̓|�C���^�����̂ł͂Ȃ��ڐG�����ꂼ�ꎝ���Ă���
				info->pointer_current_col_positions[col_index] += pointer_dx;
				info->pointer_current_col_positions[col_index] += pointer_dth ^ (info->pointer_current_col_positions[col_index] - info->pointer_pos);

				if(bCorrectPenetration)
				{
					// �߂荞�݉�������
					// ���ׂĂ̐ڐG�̂߂荞�݂������A�|�C���^�̈ʒu�ɔ��f������
					Vec3d col_vector = info->current_col_positions[col_index] - info->pointer_current_col_positions[col_index];
					Vec3d col_normal = dot(col_vector, info->current_col_normals[col_index]) * info->current_col_normals[col_index].unit();
					correct_vector += col_normal;

//					correct_torque += (info->pointer_current_col_positions[col_index] - info->pointer_pos) ^ col_normal;
				}
			}
		}

		// �|�C���^�̈ʒu�␳�B�߂荞�݂�����
		if(bCorrectPenetration)
		{
			// �|�C���^���g�̈ʒu���ړ�
			info->pointer_pos += 0.2 * correct_vector;

//			Quaterniond correct_q = 0.2 * Quaterniond::Rot(correct_torque);
//			info->pointer_ori = info->pointer_ori * correct_q;

			// ���ׂĂ̐ڐG�����̕����Ɉړ�
			for(int i = 0; i < info->num_collisions; i++)
			{
//				info->pointer_current_col_positions[i] += correct_torque ^ (info->current_col_positions[i] - info->pointer_current_col_positions[i]);
				info->pointer_current_col_positions[i] += 0.2 * correct_vector;
			}
		}
	}
//	t_end();
}

/**
 brief		����̃\���b�h����d�͂��Ȃ���
 param		�d�͂��Ȃ����\���b�h
 return		�Ȃ�
*/
void RemoveGravity(PHSolidIf* solid)
{
	solid->SetGravity(false);
}

// ��Ď�@�ɂ���Đ��������̂̈ʒu�̌덷���C������֐�
void ErrorCorrection()
{
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data) info = &info1;
	else info = &info2;

	// �͊o�����_�����O����V�~�����[�V�����ւ̋Ǐ��I���͊w�v�Z�̌��ʔ��f����
	// ���ׂĂ̍��̂ɂ��čs��
	for(int i = 0; i < info->num_solids; i++)
	{
		// �덷�̕����s�ړ��E��]����
		Vec3d pos_error = info->solid_current_center_positions[i] - info->nearest_solids[i]->GetCenterPosition();
		Quaterniond ori_error = info->solid_current_orientations[i] * info->nearest_solids[i]->GetOrientation().Inv();

		info->nearest_solids[i]->SetFramePosition(info->nearest_solids[i]->GetFramePosition() + pos_error);
		info->nearest_solids[i]->SetOrientation(info->nearest_solids[i]->GetOrientation() * ori_error);
		info->nearest_solids[i]->SetCenterPosition(info->solid_current_center_positions[i]);
		info->nearest_solids[i]->SetVelocity(info->solid_velocity[i]);
		info->nearest_solids[i]->SetAngularVelocity(info->solid_angular_velocity[i]);
	}
}

/**
 brief		VR���̃v���L�V�̈ʒu���X�V����֐��B
			���̊֐��Ɨ͊o�����_�����O��g�ݍ��킹�邱�ƂŃ|�C���^�̍X�V��HAPTIC_FREQ�̎��g���ōX�V�ł���B
 param		�Ȃ�
 return		�Ȃ�
 */
void UpdatePointer()
{
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data) info = &info1;
	else info = &info2;

	soPointer->SetFramePosition(info->pointer_pos);
	soPointer->SetVelocity(info->pointer_vel);

	soPointer->SetOrientation(info->pointer_ori);
	soPointer->SetAngularVelocity(info->pointer_angvel);
}

void StepSimulation()
{
	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ����������C������
	ErrorCorrection();

	// �V�~�����[�V�������P�X�e�b�v�i�߂�
 	scene->Step();

	// �Փ˓_����\��
//	show_collision_info();

	// �Փ˓_������͂̉e�����v�Z
	// ��{�I�ɑO��̃f�[�^�������p��
	// �P���Q�ƒ��B�Q���X�V
	if(current_valid_data) calculate_surround_effect(&info2, &info1);
	// �Q���Q�ƒ��B�P���X�V
	else calculate_surround_effect(&info1, &info2);

	// �͊o�X���b�h�̃V�~�����[�V�����̕ϐ��̎Q�Ɛ��ύX
//	current_valid_data = !current_valid_data;
	bSimulation = false;
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	// �ĕ`��
	glutPostRedisplay();

	if(bSimulation)StepSimulation();
}

/**
 brief		��͖@�̉e�����󂯂Ȃ����̂�o�^����B���̍��̂����ސڐG�͑���ɐڐG�͂�^���Ȃ�
 param		�o�^���鍄��
 return		�Ȃ�
*/
void AddInactiveSolid(PHSceneIf* scene1, PHSolidIf* soSolid)
{
	scene1->GetConstraintEngine()->AddInactiveSolid(soSolid);
}

// SPIDAR�̈ʒu�ƌ��_������킷�_��`�悷��֐�
void displayPointer()
{
	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
	GLfloat orange[] = {1.0, 0.2, 0.0, 1.0};
	GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat green[] = {0.0, 1.0, 0.0, 1.0};
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

	// SPIDAR�̈ʒu��`��
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

	// ���_��`��
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
		timeEndPeriod(1);

		cout << "EXIT from Key Input" << endl;
		exit(0);
	}
	// �f�o�b�O�o�͂̋��E�s�̐؂�ւ�
	else if(key == 'o')
	{
		bOutput = !bOutput;
		cout << "File Output is ";
		if(bOutput)cout << "ENABLED" << endl;
		else cout << "DISABLED" << endl;
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
		cout << "Feedback Force ";
		if(bforce) cout << "ON" << endl;
		else cout << "OFF" << endl;
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
			soSphere[i]->SetFramePosition(Vec3f(0, 5+5*(i+1), 0));
			soSphere[i]->SetAngularVelocity(Vec3d());
			soSphere[i]->SetVelocity(Vec3d());
		}
	}
	// ���͂̉e����L���ɂ��邩�ǂ���
	else if(key == 's')
	{
		bSurroundEffect = !bSurroundEffect;

		// ���b�Z�[�W���o��
		cout << "Surrounding Effect ";
		if(bSurroundEffect)
		{
			cout << "ON" << endl;

			if(!bLocalDynamics)
			{
				bLocalDynamics = true;
				cout << "Local Dynamics ON" << endl;
			}
		}
		else cout << "OFF" << endl;
	}
	// ���݂̃o�[�`�����J�b�v�����O��K��B�̒l��\������
	else if(key == 'e')
	{
		cout << "Km = " << Km << " Bm = " << Bm << endl;
		cout << "Kr = " << Kr << " Br = " << Br << endl;
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'k')
	{
		if(bMode)Km += 1;
		else Kr += 1;
		keyboard('e', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'b')
	{
		if(bMode)Bm += 1;
		else Br += 1;
		keyboard('e', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'j')
	{
		if(bMode)Km -= 1;
		else Kr -= 1;
		keyboard('e', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'v')
	{
		if(bMode)Bm -= 1;
		else Br -= 1;
		keyboard('e', 0, 0);
	}
	// �񎦗͂𒲐߂���l��0.1��������
	else if(key == 'm')
	{
		FORCE_COEFF += 0.01;
		cout << "coeff = " << FORCE_COEFF << endl;
	}
	// �񎦗͂𒲐߂���l��0.1��������
	else if(key == 'n')
	{
		FORCE_COEFF -= 0.01;
		cout << "coeff = " << FORCE_COEFF << endl;
	}
	// SPIDAR�̈ʒu�ƌ��_������킷���̕\����ON/OFF����
	else if(key == 'd')
	{
		bDisplayPointer = !bDisplayPointer;
		cout << "Pointer Display ";
		if(!bDisplayPointer)cout << "OFF " << endl;
		else cout << "ON " << endl;
	}
	// �p�����[�^�ҏW�Ώۂ̐؂�ւ�����
	else if(key == 't')
	{
		bMode = !bMode;
		if(bMode)cout << "MOVEMENT ";
		else cout << "ROTATION ";
		cout << "Edit Mode" << endl;
	}
	// �߂荞�ݕ␳�����̃t���O�ύX
	else if(key == 'p')
	{
		bCorrectPenetration = !bCorrectPenetration;
		cout << "Correct Penetration ";
		if(bCorrectPenetration)
		{
			cout << "ON " << endl;

			if(!bLocalDynamics)
			{
				bLocalDynamics = true;
				cout << "Local Dynamics ON" << endl;
			}
		}
		else cout << "OFF " << endl;
	}
	// �������f�����̃t���O�ύX
	else if(key == 'g')
	{
		bGradualReflection = !bGradualReflection;
		cout << "Gradual Reflection ";
		if(bGradualReflection)
		{
			cout << "ON " << endl;
			
			if(!bLocalDynamics)
			{
				bLocalDynamics = true;
				cout << "Local Dynamics ON" << endl;
			}
		}
		else cout << "OFF " << endl;
	}
	// �Ǐ��I���͊w�v�Z�̃t���O�ύX
	else if(key == 'l')
	{
		bLocalDynamics = !bLocalDynamics;
		cout << "Local Dynamics ";
		if(bLocalDynamics)cout << "ON " << endl;
		else
		{
			// OFF�ɂȂ����Ƃ���
			// �Ǐ��I���͊w�v�Z�ɔ����ق��̋@�\��OFF�ɂ���
			cout << "OFF " << endl;

			bCorrectPenetration = false;
			bGradualReflection = false;
			bSurroundEffect = false;

			cout << "Correct Penetration OFF" << endl;
			cout << "Gradual Reflection OFF" << endl;
			cout << "Surround Effect OFF " << endl;
		}
	}
	// ���ׂẴt���O�̃f�o�b�O�\��
	else if(key == 'f')
	{
		cout << "--- Current Status ---" << endl;
		keyboard('e', 0, 0);
		cout << "coeff = " << FORCE_COEFF << endl;
		cout << "File Output ";
		if(bOutput)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "Feedback Force ";
		if(bforce)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "Surrounding Effect ";
		if(bSurroundEffect)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "Pointer Display ";
		if(bDisplayPointer)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "Penetration Correct ";
		if(bCorrectPenetration)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "Gradual Reflection ";
		if(bGradualReflection)cout << "ON" << endl;
		else cout << "OFF" << endl;
		cout << "----------------------" << endl;
	}
	// �ݒ�̃v���Z�b�g
	else if(key == '1')
	{
		bSurroundEffect = true;
		bCorrectPenetration = true;
		bGradualReflection = true;
		bLocalDynamics = true;

		cout << "PRESET 1: Local Dynamics With Surrounding Effect, Penetration Correction, and Gradual Reflection" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '2')
	{
		bSurroundEffect = true;
		bCorrectPenetration = false;
		bGradualReflection = false;
		bLocalDynamics = true;
		
		cout << "PRESET 2: Local Dynamics With Surrounding Effect" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '3')
	{
		bSurroundEffect = false;
		bCorrectPenetration = true;
		bGradualReflection = true;
		bLocalDynamics = true;

		cout << "PRESET 3: Local Dynamics With Penetration Correction and Gradual Reflection" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '4')
	{
		bSurroundEffect = false;
		bCorrectPenetration = false;
		bGradualReflection = false;
		bLocalDynamics = true;

		cout << "PRESET 4: Local Dynamics" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '5')
	{
		bSurroundEffect = false;
		bCorrectPenetration = false;
		bGradualReflection = false;
		bLocalDynamics = false;

		cout << "PRESET 5: Virtual Coupling" << endl;
		keyboard('f', 0, 0);
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

	ofs.open("log.txt");

	// object, solid�Ȃǂ̏�����
	InitScene();

	// rendering�֌W�̏�����
	InitRendering(&argc, argv);

	// device manager�̏�����
	InitDeviceManager();

	// device�̏�����
	spidarG6.Init(devMan, false);			//	SPIDAR�̏������C�L�����u���[�V����������D

	timeBeginPeriod(1);							//����\�̍ŏ��l

	DWORD time = 0;

	timer.CountUS();

	// haptic�X���b�h�̐����E�J�n
	FTimerId1 = timeSetEvent(1000 / HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    HapticRendering,//�R�[���o�b�N�֐�
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
	desc.mass = 0.3;

	// inertia�̌v�Z��
	// �����̂̏ꍇ I = mass * (r1^2 + r2^2) / 12
	// ���̏ꍇ I = mass * r~2 * 2/5
	desc.inertia = Matrix3d(0.45,0,0,0,0.45,0,0,0,0.45);

	// Solid�̍쐬
	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}

	desc.mass = 0.1;
	desc.inertia = Matrix3d(0.66, 0, 0, 0, 0.66, 0, 0, 0, 0.66);

	soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
	soFloor->SetDynamical(false);

	soPointer = scene->CreateSolid(desc);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;

	{
		CDSphereDesc sd;
		sd.radius = 1.25;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));

		// ���`�̃|�C���^
		/*
		soPointer->AddShape(sphere);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));
		*/

		sd.radius = 2.0;
		sphere = DCAST(CDSphereIf,phSdk->CreateShape(sd));

		CDBoxDesc bd;
		bd.boxsize = Vec3f (30.0, 1.0, 30.0);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soFloor->AddShape(floor);
		soFloor->SetFramePosition(Vec3f(0,-2.5,0));

		// �l�p�̃|�C���^
		bd.boxsize = Vec3f(2, 2, 2);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soPointer->AddShape(floor);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));

		bd.boxsize = Vec3f(3, 3, 3);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere[sphereCnt]->AddShape(floor);
		soSphere[sphereCnt]->SetFramePosition(Vec3f(0, 5*(sphereCnt+1), 0));
	}
	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	// pointer�̏d�͂𖳌��ɂ���
	RemoveGravity(soPointer);

	// pointer�ƐڐG���鍄�̂Ƃ̉�͖@�ɂ��v�Z����߁A
	// �͊o�ŏ�������悤�ɂ���
	AddInactiveSolid(scene, soPointer);
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
	cout << devMan;						//	�������̌��ʂ�\��	
}
