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
  DrawScene API �Őݒ肳��Ă���}�e���A���T���v����p���āA�J���t���ȋ��̂���ђ����̂������_�����O����B�@
  �܂��ASPIDAR�ƂȂ��A���́A�����̋y�ђn�ʂƗ͊o�C���^���N�V�������s���B
  
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
#define NUM_OBJECTS		3			// object��
#define SPIDAR_SCALE	70			// SPIDAR��VE���ł̓���X�P�[��

#define POINTER_RADIUS 0.5f			// �|�C���^�̔��a
#define EPSILON 0.15					// �|�C���^�ɐڐG�������ȍ��̂�\�����邽�߂Ƀ|�C���^��c��܂��ĐڐG���������Ƃ��̖c��܂����
									// �傫������قǂ�������̐ڐG��\�z�ł��邪�A���̕��̔��菈����������̂�
									// �c��܂�������̂��悭�Ȃ�

#define LIMIT_DEPTH 100				// �\���V�~�����[�V�������s�����̎擾�̐[�����
#define NUM_PREDICT_ITERATE 15		// �\���V�~�����[�V�����̃C�e���[�g��

#define NUM_COLLISIONS 100			// �|�C���^�ւ̋��e�ڐG��
#define NUM_COL_SOLIDS 100			// �|�C���^�ւ̋��e�ڐG���̐��@
									// NUM_COLLISIONS�Ƌ�ʂ���̂̓v���O������ǂ݂₷�����邽�߁B�����I�ȑ��݈Ӌ`�͂Ȃ�

#define COLLISION_DENSITY 0.05		// �͊o�̌v�Z�Ɏg�p����ڐG�_�̕��z�ʂ𒲐߂���l
									// �ڐG�_�͂��̒l���傫�������ŕ��z����

#ifdef _DEBUG
	#define SIMULATION_FREQ	60		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		500		// �͊o�X���b�h�̎���Hz
	float Km = 550;					// virtual coupling�̌W��
	float Bm = 40;					// ���i

	float Kr = 550;					// ��]
	float Br = 116;

#elif _WINDOWS
	#define SIMULATION_FREQ	60		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000	// �͊o�X���b�h�̎���Hz
	float Km = 4500;				// virtual coupling�̌W��
	float Bm = 150;					// ���i

	float Kr = 4500;				// ��]
	float Br = 160;

//	float Km = 1500;
//	float Bm = 80;

//	float Kr = 1500;
//	float Br = 100;
#endif

// �񎦗͂ƍ��̂ɒ񎦂���͂𒼐ڕω�������萔
double FORCE_COEFF =		1.0;

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
	#include <Device/DRUsb20Simple.h>
	#include <Device/DRUsb20Sh4.h>
	#include <conio.h>
#endif

// �O���t�B�b�N�p�̕ϐ�
UTRef<GRSdkIf> grSdk;
GRDebugRenderIf* render;
GRDeviceGLIf* grDevice;

// �����v�Z�p�̕ϐ�
UTRef<PHSdkIf> phSdk;
UTRef<PHSceneIf> scene;

// ���z���E�ň�������
vector<PHSolidIf*> soWalls;
vector<PHSolidIf*> soObject; 
PHSolidIf* soPointer;
PHSolidIf* soPointerCopy;

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

// �������f�����̃t���O
bool bGradualReflection = false;

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

// ���͂̉e����L���ɂ��邩�ǂ��������߂�t���O
bool bSurroundEffect = false;

// SPIDAR�̈ʒu��\�����邩�ǂ���
bool bDisplayPointer = true;

// K��B�̒l�ǂ���̕ύX��L���ɂ��邩
// true�̏ꍇ���i
// false�̏ꍇ��]
bool bMode = true;

// �͊o�v�Z�ɕK�v�ȃf�[�^���W�߂��\����
// �͊o�v�Z�������ōs����悤��
// �V�~�����[�V�����Ńf�[�^���i�[���A
// HapticRendering�ɓn��
typedef struct {
	// collision data
	PHConstraint* constraint[NUM_COLLISIONS];
	
	// �V���ȐڐG����邽�߂ɕK�v��shapePair
	Spr::PHShapePairForLCP* shapePair[NUM_COLLISIONS];

	// �O�񂩂�����p���ꂽ�ڐG���ǂ������肷��t���O���i�[����z��
	bool bPreviousCollide[NUM_COLLISIONS];

	int sign[NUM_COLLISIONS];

	// �����ō쐬�����ڐG�_�̏W�����i�[����ϐ�
	multimap<double, PHContactPoint *> points;

	// �ڐG�_����A���̌��ɂȂ����f�[�^�փA�N�Z�X���邽�߂̔z��̓Y�������i�[����ϐ�
	int NewcolToCol[NUM_COLLISIONS];
	int NewsolToSol[NUM_COL_SOLIDS];

	PHSolid* solid[NUM_COLLISIONS][2];

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

	// ���ۂɐڐG���Ă��邩����킷�t���O
	// MakeHapticInfo�őO��ڐG���Ă����ꍇ�A�̔��f�Ɏg��
	bool bCollide[NUM_COLLISIONS];
} HapticInfo;


// pointer data
Vec3d pointer_pos;
Vec3d pointer_vel;
double pointer_massinv;
Quaterniond pointer_ori;
Vec3d pointer_angvel;
Matrix3d pointer_inertiainv;


// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
HapticInfo info1;
HapticInfo info2;

// true�̂Ƃ���info�P���Q��
// false�̂Ƃ���info�Q���Q��
bool current_valid_data = true; 


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
void MakeHapticInfo(HapticInfo *new_info, HapticInfo *current_info, 
					vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > pointer_static_consts, 
					vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > col_candidate_static_consts,
					vector<pair<PHConstraint *, int> > current_consts, vector<pair<PHConstraint *, int> >current_static_consts, 
					map<PHConstraint *, int> NewcolToCol, map<PHSolid *, int> NewsolToSol,
					map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices, map<PHSolid *, SpatialVector> vecs)
{
	// ����������
	// �O�X��̃f�[�^�͂����K�v�Ȃ��̂ŏ���������
	// ����ȊO�̃f�[�^�͏㏑�����A����Ɍ��ŃA�N�Z�X�𐧌�����̂�
	// �������̕K�v�͂Ȃ�
	for(int i = 0; i < new_info->num_solids; ++i) 
	{
		new_info->nearest_solids[i] = NULL;
		new_info->num_col_per_sol[i] = 0;
	}
	new_info->num_solids = 0;
	new_info->num_collisions = 0;

	int pointer_consts_size = (int)pointer_consts.size();
	int pointer_static_consts_size = (int)pointer_static_consts.size();

	int col_candidate_consts_size = (int)col_candidate_consts.size();
	int col_candidate_static_consts_size = (int)col_candidate_static_consts.size();

	int current_consts_size = (int)current_consts.size();
	int current_static_consts_size = (int)current_static_consts.size();

	// �͊o�����_�����O�ɕK�v�ȃf�[�^�̍쐬
	for(int i = 0; i < pointer_consts_size + pointer_static_consts_size + col_candidate_consts_size + col_candidate_static_consts_size + current_consts_size + current_static_consts_size; ++i)
	{
		// ���e���𒴂���ꍇ�͑ł��؂�
		if(i > NUM_COLLISIONS) break;

		PHConstraint* constraint;
		int sign;
		PHSolid* so;
		
		bool bCollide = true;
		bool bPreviousCollide = false;

		// ���ۂɐڐG���A�������̂��܂ސڐG
		if(pointer_consts_size > i)
		{
			constraint = pointer_consts[i].first;
			sign = pointer_consts[i].second;

			if(sign > 0) so = pointer_consts[i].first->solid[0];
			else so = pointer_consts[i].first->solid[1];
		}
		// �O�X�e�b�v�ŐڐG���Ă���
		else if(pointer_consts_size + current_consts_size > i)
		{
			int i_local = i - pointer_consts_size;

			constraint = current_consts[i_local].first;
			sign = current_consts[i_local].second;

			if(sign > 0) so = current_consts[i_local].first->solid[0];
			else so = current_consts[i_local].first->solid[1];

			bPreviousCollide = true;
		}
		// ���ۂɐڐG���Ă���A�������Ȃ����̂��܂ސڐG
		else if(pointer_consts_size + pointer_static_consts_size + current_consts_size > i)
		{
			int i_local = i - (pointer_consts_size + current_consts_size);
			constraint = pointer_static_consts[i_local].first;
			sign = pointer_static_consts[i_local].second;

			if(sign > 0) so = pointer_static_consts[i_local].first->solid[0];
			else so = pointer_static_consts[i_local].first->solid[1];
		}
		// �O�X�e�b�v�ŐڐG���Ă���A�������Ȃ����� pointer_static_consts_size + 
		else if(pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size > i)
		{
			int i_local = i - (pointer_consts_size + current_consts_size + pointer_static_consts_size);
			constraint = current_static_consts[i_local].first;
			sign = current_static_consts[i_local].second;

			if(sign > 0) so = current_static_consts[i_local].first->solid[0];
			else so = current_static_consts[i_local].first->solid[1];

			bPreviousCollide = true;
		}
		// �|�C���^�ɂ͐ڐG���Ă��Ȃ��A�������̂��܂ސڐG
		// �|�C���^�ɐڐG�������Ȃ��̂���
		else if(pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size + col_candidate_consts_size > i)
		{
			int i_local = i - (pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size);
			constraint = col_candidate_consts[i_local].first;
			sign = col_candidate_consts[i_local].second;

			if(sign > 0) so = col_candidate_consts[i_local].first->solid[0];
			else so = col_candidate_consts[i_local].first->solid[1];

			// ���̐ڐG�͎��ۂɂ͐ڐG���Ă��Ȃ��̂�false
			bCollide = false;
		}
		// �ڐG���Ă��炸�A�������Ȃ����̂��܂ސڐG
		// �|�C���^�ɐڐG�������ȐڐG
		else 
		{
			int i_local = i - (pointer_consts_size + col_candidate_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size);
			constraint = col_candidate_static_consts[i_local].first;
			sign = col_candidate_static_consts[i_local].second;

			if(sign > 0) so = col_candidate_static_consts[i_local].first->solid[0];
			else so = col_candidate_static_consts[i_local].first->solid[1];

			// ���̐ڐG�͎��ۂɂ͐ڐG���Ă��Ȃ��̂�false
			bCollide = false;
		}


		int counter = 0;
		// �z���PHSolid��ǉ�
		// ���̂ƐڐG���P�΂P�ł͂Ȃ����߁A�ڐG���獄�̂֓Y������ϊ�����}�b�v���쐬����
		while(1)
		{
			// �܂��ǉ�����Ă��Ȃ����̂͒ǉ�����
			if(!new_info->nearest_solids[counter])
			{
				new_info->nearest_solids[counter] = so;
				
				// ���̂���ڐG��
				// �܂��ڐG���獄�̂֎��Ԃ��������ɕϊ��ł���悤��
				// �z��ɓY�������i�[���Ă���
				new_info->SolToCol[counter][new_info->num_col_per_sol[counter]] = i;
				++new_info->num_col_per_sol[counter];

				new_info->ColToSol[i] = counter;


				// ����ƑO��̐ڐG�ŋ��ʂ̍��̂����邩���ׂ�
				map<PHSolid *, int>::iterator it = NewsolToSol.find(so);

				// �����\��������̂ňꉞ
				// ����ΓY������ǉ�����B�Ȃ����-1�ŏ�����
				if(it != NewsolToSol.end()) new_info->NewsolToSol[counter] = (*it).second;
				else new_info->NewsolToSol[counter] = -1;


				new_info->num_solids = counter+1;

				// ���x�������̂��Ƃɒǉ�
				new_info->solid_velocity[counter] = so->GetVelocity();
				new_info->solid_angular_velocity[counter] = so->GetAngularVelocity();
				new_info->solid_massinvs[counter] = so->GetMassInv();
				new_info->solid_inertiainvs[counter] = so->GetInertiaInv();

				// ���͂̉e��������킷�ϐ����R�s�[����
				// ���̕ϐ��͍��̌ŗL�̒l�Ȃ̂ŁA
				// ���̂��ǉ����ꂽ���_�Ŋi�[�������s��
				if(so->IsDynamical())
				{
					map<PHSolid *, SpatialVector>::iterator it1 = vecs.find(so);
					SpatialVector sv = (*it1).second;
					new_info->vel_constant[counter] = sv.v();
					new_info->ang_constant[counter] = sv.w();

					map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >::iterator it2 = matrices.find(so);
					vector<pair<Matrix3d, Matrix3d> > mm_vector = (*it2).second;
					for(int j = 0; j < (int)mm_vector.size(); ++j)
					{
						new_info->vel_effect[counter][j] = mm_vector[j].first;
						new_info->ang_effect[counter][j] = mm_vector[j].second;
					}
				}
				else
				{
					new_info->vel_constant[counter] = Vec3d();
					new_info->ang_constant[counter] = Vec3d();
				}


				bool current_exists = false;
				
				/*
				if(bLocalDynamics && so->IsDynamical())
				{
					// �O������̍��̂ƃ|�C���^�̐ڐG�����݂������`�F�b�N���A
					// ���݂����ꍇ�͏��X�ɖ߂��悤�Ƀf�[�^���i�[����
					for(int j = 0; j < current_info->num_solids; ++j)
					{
						// �����_�ŐڐG���Ă��邩�`�F�b�N���č�������}��
						if(!current_info->bCollide[j]) continue;

						if(current_info->nearest_solids[j] == so)
						{
							new_info->solid_current_center_positions[counter] = current_info->solid_current_center_positions[j];
							new_info->solid_future_center_positions[counter] = so->GetCenterPosition() - new_info->solid_current_center_positions[counter];
							new_info->solid_current_orientations[counter] = current_info->solid_current_orientations[j];
							new_info->solid_future_orientations[counter] = so->GetOrientation();

							current_exists = true;				
							break;
						}
					}
				}
				*/

				// �O��͑��݂��Ȃ�����
				if(!current_exists)
				{
					new_info->solid_current_center_positions[counter] = so->GetCenterPosition();
					new_info->solid_future_center_positions[counter] = Vec3d();
					new_info->solid_current_orientations[counter] = so->GetOrientation();
					new_info->solid_future_orientations[counter] = Quaterniond();
				}

				break;
			}
			// �������̂����łɒǉ��ς݂̏ꍇ�͒ǉ����Ȃ�
			// �Y�����ϊ��\�̂ݍ쐬
			else if(new_info->nearest_solids[counter] == so)
			{
				new_info->SolToCol[counter][new_info->num_col_per_sol[counter]] = i;
				++new_info->num_col_per_sol[counter];

				new_info->ColToSol[i] = counter;

				break;
			}

			// ���̓o�^���ꂽ���̂ɐi��
			++counter;
		}

		new_info->constraint[i] = constraint;
		new_info->sign[i] = sign;
		new_info->shapePair[i] = DCAST(PHContactPoint, constraint)->shapePair;
		new_info->bPreviousCollide[i] = bPreviousCollide;

		// ���ۂɐڐG���Ă��邩�ǂ����������t���O
		new_info->bCollide[i] = bCollide;

		PHContactPoint* contact = DCAST(PHContactPoint, constraint);
		new_info->solid[i][0] = contact->solid[0];
		new_info->solid[i][1] = contact->solid[1];

		bool current_exists = false;

		if(bPreviousCollide)
		{
			// ���̐ڐG���猻�݂̃f�[�^�ւ̈������擾����
			int index = NewcolToCol.find(constraint)->second;

			// ���݂̒l�����������p��
			new_info->current_col_positions[i] = current_info->current_col_positions[index];
			new_info->future_col_positions[i] = Vec3d();

			new_info->current_col_normals[i] = current_info->current_col_normals[index];
			new_info->future_col_normals[i] = Vec3d();

			new_info->pointer_current_col_positions[i] = current_info->pointer_current_col_positions[index];
			new_info->pointer_future_col_positions[i] = Vec3d();

			// �Y�����̕ϊ��\���쐬
			new_info->NewcolToCol[i] = index;

			current_exists = true;
		}

		// �O�񓯂��ڐG�����݂��Ȃ������̂�
		// �ڕW�̐ڐG�̖@���ƐڐG�_�͂Ȃ�
		if(!current_exists)
		{
			// �����_�ŐڐG���Ă���
			if(bCollide)
			{
				// �|�C���^�̐ڐG���_�͐ڐG�_�Ɠ���
				new_info->pointer_current_col_positions[i] = contact->pos;
			}
			// ���ۂɂ͐ڐG���Ă��Ȃ�
			// �ڐG���_
			// �|�C���^���̐ڐG�_�̈ʒu�𒲐߂���
			else
			{
				// �|�C���^�̒��S����ڐG�_�܂ł̃x�N�g�����v�Z���A
				// ���̃x�N�g�����|�C���^�̑傫���ɏk������i�c��܂����|�C���^�ƐڐG������s��������
				// ���̌��ʂ̓_��ڐG���_�Ƃ���
				// HapticRendering�ŐڐG�������ǂ����̔��f���s��
				Vec3d vec_to_contact = contact->pos - soPointer->GetFramePosition();				
				double j, k, l;

				if(vec_to_contact.x != 0.0) j = fabs(POINTER_RADIUS / vec_to_contact.x);
				else j = 0.0;

				if(vec_to_contact.y != 0.0) k = fabs(POINTER_RADIUS / vec_to_contact.y);
				else k = 0.0;

				if(vec_to_contact.z != 0.0) l = fabs(POINTER_RADIUS / vec_to_contact.z);
				else l = 0.0;

				double c = j;

				if(k != 0.0)
				{
					if(c != 0.0)c = std::min(c, k);
					else c = k;
				}
				if(l != 0.0)
				{
					if(c != 0.0) c = std::min(c, l);
					else c = k;
				}

				// c�{�ɏk�������x�N�g����ݒ�
				new_info->pointer_current_col_positions[i] = vec_to_contact * c + soPointer->GetFramePosition();
			}

			new_info->current_col_positions[i] = contact->pos;
			new_info->current_col_normals[i] = contact->shapePair->normal * sign;
			new_info->future_col_positions[i] = Vec3d();
			new_info->future_col_normals[i] = Vec3d();
			new_info->pointer_future_col_positions[i] = Vec3d();

			// �ꉞ-1�ŏ���������
			new_info->NewcolToCol[i] = -1;
		}
	}

	new_info->num_collisions = pointer_consts_size + pointer_static_consts_size + col_candidate_consts_size + col_candidate_static_consts_size + current_consts_size + current_static_consts_size;

	// debug info 
//	ofs << "num collision = " << new_info->num_collisions << endl;

	if(new_info->num_collisions >= NUM_COLLISIONS) new_info->num_collisions = NUM_COLLISIONS-1;
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
void RecursiveSolidRetrieval(vector<pair<PHConstraint *, bool> >* csm, PHSolid* solid, vector<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, int depth)
{
	for(vector<pair<PHConstraint *, bool> >::iterator it = csm->begin(); it != csm->end(); ++it)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if(it->second) continue;

		// �^����ꂽ���̂���A���̍��̂ɐڐG���鍄�̂��擾����
		PHSolid* col_solid = getAdjacentSolid(it->first, solid);

		// �������̍��̂����݂����ꍇ
		if(col_solid)
		{
			// �|�C���^�̏ꍇ��GetSolidsFromPointer�Ŏ擾����
			if(col_solid == DCAST(PHSolid, soPointer)) continue;
			
			// ����soPointerCopy�Ƃ̐ڐG��������
			// �{�����̐ڐG�͉e����^���Ȃ��͂��Ȃ̂�
			// �ǉ����Ȃ�
			else if(col_solid != DCAST(PHSolid, soPointerCopy))
			{
				// �����ς݂ɂ���
				it->second = true;

				// �ڐG���̂�ǉ�
				PHConstraint* c = it->first;
				relative_consts->push_back(c);

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
}

// �|�C���^����X�^�[�g���ĕK�v�ȍ��̂����ׂĎ擾���Ă���֐�
void GetCollisionsFromPointer(vector<pair<PHConstraint *, int> >* pointer_consts, vector<pair<PHConstraint *, int> >* pointer_static_consts,  
						  vector<pair<PHConstraint *, int> >* col_candidate_consts, vector<pair<PHConstraint *, int> >* col_candidate_static_consts, 
						  multimap<double, PHContactPoint*> points,
						  vector<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, set<PHSolid *>* nearest_solids)
{
	// �ڐG�_���擾
	PHConstraints cs = GetContactPoints(scene);
	vector<pair<PHConstraint *, bool> > csm;

	// �v�Z���Ԃ�Z�k���邽�߂ɐڐG�ɏ����σt���O�����ĕۑ�����
	// �ڐG�������Ȃ��ꍇ�͖��ʂ��������A
	// �ڐG���������Ȃ����Ƃ��̐L�т͂�����̂ق����f�R���Ȃ��͂�
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); ++it)
	{
		csm.push_back(pair<PHConstraint *, bool>(*it, false));
	}

	// ���ׂĂ̏Փ˓_�ɂ��Ē���
	for(vector<pair<PHConstraint *, bool> >::iterator it = csm.begin(); it != csm.end(); ++it)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if(it->second) continue;

		for(int i = 0; i < 2; ++i)
		{
			PHSolid* target1;
			PHSolid* target2;

			// ���[�v�J�E���^�ɂ���Č�����؂�ւ���
			// �ڐG����p�̕����|�C���^���傫���̂ŁA
			// �ڐG����p�̕����ڐG��������������Ă���\��������
			// �����Ń|�C���^���ɍs��
			switch(i)
			{
			case 0:
				target1 = DCAST(PHSolid, soPointer);
				target2 = DCAST(PHSolid, soPointerCopy);
				break;
			case 1:
				target1 = DCAST(PHSolid, soPointerCopy);
				target2 = DCAST(PHSolid, soPointer);
				break;
			}
	
			int sign = 1;

			// �|�C���^�������͐ڐG����p�̍��̂��܂ސڐG���擾
			PHSolid* col_solid = getAdjacentSolid(it->first, DCAST(PHSolid, target1), &sign);
			
			// �����ł��A���|�C���^�ƐڐG����p�̍��̂̃y�A�ł͂Ȃ�����
			// target1 && target2 => �|�C���^�ƐڐG����p���̂̃y�A
			if(col_solid && col_solid != DCAST(PHSolid, target2))
			{
				// �����ςɂ���
				it->second = true;

				PHSolid* so;

				// �|�C���^�Ɛڂ��Ă��鍄�̂�PHSolid��ۑ�
				if(sign > 0) so = it->first->solid[0];
				else so = it->first->solid[1];

				relative_solids->insert(so);
				nearest_solids->insert(so);

				// �|�C���^�Ƃ̐ڐG
				if(i == 0)
				{
					PHConstraint* consts = it->first;
					PHContactPoint* contact = DCAST(PHContactPoint, consts);
					bool bAdd = true;
				
					// �����p���ڐG�ƐV�������ڐG�̈ʒu���߂�����ꍇ��
					// �V�����ڐG�͒ǉ����Ȃ�
					// ���łɈ����p���ڐG�����̑���Ƃ��čl������̂ŁA
					// ���̓_�͍l����K�v�͂Ȃ�
					for(multimap<double, PHContactPoint*>::iterator it2 = points.lower_bound(contact->pos.z - COLLISION_DENSITY); it2 != points.upper_bound(contact->pos.z + COLLISION_DENSITY); it2++)
					{
						double norm = (contact->pos - (*it2).second->pos).norm();

						// ���p���̐ڐG�_�̈�ƁA�ǉ����悤�Ƃ����̐ڐG�_�̋����i�x�N�g���̃m�����j��
						// ����������ꍇ�͒ǉ����Ȃ�
						if(norm < COLLISION_DENSITY)
						{
//							bAdd = false;
							break;
						}
					}

					// ���������鍄�̂ł���΁A�ڐG���̂��i�[
					// �����Ȃ����͕̂ʂȃx�N�g���ɒǉ�
					// �����闝�R�͌�ōs���V�~�����[�V�����ŁA�����Ȃ����̂ƃ|�C���^�̐ڐG���܂߂�͈̂Ӗ����Ȃ�����
					if(bAdd)
					{
						if(col_solid->IsDynamical()) pointer_consts->push_back(pair<PHConstraint *, int>(consts, sign));
						else pointer_static_consts->push_back(pair<PHConstraint *, int>(consts, sign));
					}
				}
				// ����p�|�C���^�Ƃ̐ڐG
				else
				{
					if(col_solid->IsDynamical())col_candidate_consts->push_back(pair<PHConstraint *, int>(it->first, sign));
					else col_candidate_static_consts->push_back(pair<PHConstraint *, int>(it->first, sign));
				}

// ���͂̉e�����v�Z���邽�߂̃f�[�^�쐬����
// �֌W�̂��鍄�̂ƐڐG���ċA�I�ɂ��ׂĂƂ��Ă���
#if 1
				relative_consts->push_back(it->first);

				// ���̍��̂Ɛ������ڐG�����̍��̂ɉe�����y�ڂ��\��������ꍇ�́A���̍��̂ɂ�����
				if(col_solid->IsDynamical())
					RecursiveSolidRetrieval(&csm, col_solid, relative_consts, relative_solids, 0);
#endif
				break;
			}
		}
	}
}

vector<SpatialVector> lambda;

// �����_�̒l��ۑ����Ă����֐�
void SaveLambda(vector<PHConstraint *> relative_consts)
{
	// lambda��������
	lambda.clear();

	for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		// �����_�̒l(fv, fw)��ۑ����Ă���
		lambda.push_back((*it)->f);
	}
}

// �摗��V�~�����[�V����������֐�
vector<SpatialVector> PredictSimulation(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
										set<PHSolid *> nearest_solids,
										vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, 
										Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index = -1)
{
	SpatialVector dv_save;
	PHSolid* si;

	// �|�C���^���ڂ��鍄�̂̂���index�Ԗڂ̐ڐG�ɒP�ʗ͂�������
	if(index != -1)
	{
		double dt = scene->GetTimeStep();
		int size1 = (int)pointer_consts.size();
		int size2 = (int)col_candidate_consts.size();

		PHConstraint* constraint;
		int sign;

		if(size1 > index)
		{
			constraint = pointer_consts[index].first;
			sign = pointer_consts[index].second;
		}
		else if(size2 + size1 > index)
		{
			constraint = col_candidate_consts[index - size1].first;
			sign = col_candidate_consts[index - size1].second;
		}
		else
		{
			constraint = current_consts[index - size1 - size2].first;
			sign = current_consts[index - size1 - size2].second;
		}

		PHContactPoint* contact = DCAST(PHContactPoint, constraint);
			
		if(sign > 0) si = DCAST(PHSolid, constraint->solid[0]);
		else si = DCAST(PHSolid, constraint->solid[1]);

		// ���O�Ɍv�Z���Ă������l���Q��
		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);

		// �P�ʗ͂�������O��dv��ۑ����Ă���
		dv_save = si->dv;

		// �����ŗ^����ꂽ�P�ʗ͂�������
		si->dv.v() += si->GetMassInv() * force_vec * dt;
		si->dv.w() += si->GetInertiaInv() * (((contact->pos - si->GetCenterPosition()) ^ force_vec) - cef->second) * dt;
	}

	// �V�~�����[�V�����ɕK�v�ȏ����Z�b�g�A�b�v
	for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		(*it)->SetupLCP();
	}

	for(int i = 0; i < NUM_PREDICT_ITERATE; ++i)
	{
		for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
		{
			// ���ׂĂ̐ڐG��iterate
			(*it)->IterateLCP();
		}
	}

	vector<SpatialVector> effects;

	// �e�����ϑ�
	// ���̂̑��x�ω����擾
	for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		effects.push_back((*it)->dv);
	}

	// dv�̒l�����ɖ߂�
	if(index != -1)si->dv = dv_save;

	int local_counter = 0;
	// fv��fw�̒l�����ɂ��ǂ�
	// lambda�̕ۑ��̉ߒ���relative_consts��iterator��vector�Ɋi�[���Ă���͂��Ȃ̂�
	// ����iterator�Ń��[�v���܂킹�ΐڐG���킩��Ȃ��Ă����������ɎQ�Ƃł���͂�
	for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		(*it)->f = lambda[local_counter++];
	}

	// �e����return
	return effects;
}

// �\���V�~�����[�V�������Z�b�g�A�b�v����
void SetupPredictSimulation(vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, set<PHSolid *> nearest_solids, map<PHSolid *, Vec3d> *coeff)
{
	double dt = scene->GetTimeStep();

	// ���ׂĂ̍��̂��Z�b�g�A�b�v
	for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		(*it)->UpdateCacheLCP(dt);
	}

	// �|�C���^�ɐڂ��鍄�̂�
	// �|�C���^�ɐڂ���\���̂��鍄�̂�
	// �P�ʗ͂�������p�ӂ����āA�}�b�v�Ɋi�[����
	for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		PHSolid* solid = *it;

		// �P�ʗ͂������鏈���̂Ƃ���Ŏg���l(=�������͂����[���h���W����S�����W�ɕϊ��������)�����̂����Ɍv�Z���Ă���
		Vec3d w = solid->GetOrientation().Conjugated() * solid->GetAngularVelocity();
		Vec3d calc = w % (solid->GetInertia() * w);
		coeff->insert(pair<PHSolid *, Vec3d>(solid, calc));
	}

	// ���ׂĂ̐ڐG�ɂ��ă��R�r�A�����v�Z
	// ���R�r�A���̌v�Z�͈�x�����ł悢
	for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		(*it)->CompJacobian();
	}

	// �V�~�����[�V�����ŏ㏑�����Ă��܂��ϐ������̂����ɕۑ�����
	SaveLambda(relative_consts);
}


// ��Ď�@�̂��߂ɐ摗��V�~�����[�V�������v��I�ɍs���Ă����֐�
void PredictSimulations(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
						set<PHSolid *> nearest_solids,
						vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids,
						map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > *mat, map<PHSolid *, SpatialVector> *vec)
{
	map<PHSolid *, Vec3d> coeff;

	// �K�v�ȏ��̏���
	SetupPredictSimulation(relative_consts, relative_solids, nearest_solids, &coeff);

	// ������͂��O�x�N�g���Ƃ��Ē萔�����擾
	vector<SpatialVector> b = PredictSimulation(pointer_consts, col_candidate_consts, current_consts,
												nearest_solids, 
												relative_consts, relative_solids, 
												Vec3d(), coeff);

	map<PHSolid*, SpatialVector> c;
	int local_counter = 0;

	// �K�؂Ƀf�[�^���i�[����map-vector�̏���
	// ���̐��������vector��錾����map�ɒǉ�����
	map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > > mm_map;
	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		mm_map.insert(pair<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >((*it), vector<pair<Matrix3d, Matrix3d> >()));

		// ���ł�vector����map�ւ̕ϊ��������s��
		c.insert(pair<PHSolid*, SpatialVector>((*it), b[local_counter++]));
	}

	// �ڐG�����񐔂����J��Ԃ�
	for(int i = 0; i < (int)pointer_consts.size() + (int)col_candidate_consts.size() + (int)current_consts.size(); ++i)
	{
		// ����P�ʃx�N�g���������Ă��̌��ʂ�����킷�x�N�g�����擾
		vector<SpatialVector> vec_x = PredictSimulation(pointer_consts, col_candidate_consts, current_consts, nearest_solids,
														relative_consts, relative_solids, 
														Vec3d(10, 0, 0), coeff, i);
		vector<SpatialVector> vec_y = PredictSimulation(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
														relative_consts, relative_solids, 
														Vec3d(10, 10, 0), coeff, i);
		vector<SpatialVector> vec_z = PredictSimulation(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
														relative_consts, relative_solids, 
														Vec3d(10, 0, 10), coeff, i);

		// ���̃��[�v�ō���so�̐ڐGpointer_consts[i](��������col_candidate_consts[i])�Ɋւ���s����v�Z���i�[
		// ���̐ڐG�̂��ׂĂ̍��̂ɑ΂���e�������̃��[�v�ŏ�������
		// PredictSimulation���Ńx�N�g���ւ̗v�f�̒ǉ���nearest_solids��iterator�ł���Ă���̂ŁA
		// ��������̃��[�v�ł܂킹�ΑΉ�����PHSolid���킩��Ȃ��Ă�����������Ēǉ��ł���
		local_counter = 0;
		for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
		{
			PHSolid *so = *it;
			SpatialVector sv = b[local_counter];
			SpatialVector sv_x = vec_x[local_counter];
			SpatialVector sv_y = vec_y[local_counter];
			SpatialVector sv_z = vec_z[local_counter];
			++local_counter;

			// ��قǉ������P�ʗ͂���A�e�������߂čs����쐬����
			// �Ȃ��g���N�ɂ��Ă͔��a�Ɨ͂̊O�ς����������
			// ���a�̊O�ς��s�񉻂���Ă��łɔ��f����Ă���͂��ł���
			Matrix3d v = Matrix3d(sv_x.v() / 10.0 - sv.v(), (sv_y.v() - sv_x.v()) / 10.0 - sv.v(), (sv_z.v() - sv_x.v()) / 10.0 - sv.v());
			Matrix3d w = Matrix3d(sv_x.w() / 10.0 - sv.w(), (sv_y.w() - sv_x.w()) / 10.0 - sv.w(), (sv_z.w() - sv_x.w()) / 10.0 - sv.w());

			// �f�[�^�̊i�[�B			
			pair<Matrix3d, Matrix3d> p = pair<Matrix3d, Matrix3d>(v, w);

			// ���̂��ƂɐڐG�ƌW���s��̃}�b�v���쐬����
			// �ڐG�̏��Ԃ͂킩�邪���̂̏��Ԃ͂킩��Ȃ����߁B
			// ���̂��ƂɐڐG�̏��Ԃɍs�񂪊i�[����Ă���
			mm_map.find(so)->second.push_back(p);
		}
	}

	*mat = mm_map;
	*vec = c;
}

void CreateConstraintFromCurrentInfo(HapticInfo* current_info, vector<pair<PHConstraint *, int> > *current_consts, vector<pair<PHConstraint *, int> > *current_static_consts, map<PHConstraint *, int>* NewcolToCol, map<PHSolid *, int>* NewsolToSol,
									set<PHSolid *> *relative_solids, vector<PHConstraint *> *relative_consts, set<PHSolid *> *nearest_solids)
{

	// 2�X�e�b�v�O�ɍ�����ڐG�̍폜����
	// ���ŐV�����ڐG���쐬����
	for(multimap<double, PHContactPoint*>::iterator it = current_info->points.begin(); it != current_info->points.end(); ++it)
	{
		PHContactPoint* cp = (*it).second;
		delete cp;
	}
	current_info->points.clear();

	// ���ݎg�p���Ă���f�[�^����
	// ���ۂɐڐG���Ă������
	// �ڐG���Ă��Ȃ����̂𒲂�
	// ���̃f�[�^�����Ɏg�����ǂ������肷��
	for(int i = 0; i < current_info->num_collisions; i++)
	{
		// ���ۂɂ͐ڐG���Ă��Ȃ��ڐG
 		if(!current_info->bCollide[i]) continue;

		// ���ݎg�p���Ă���contactpoint(=constraint)�̂قƂ�ǂ�step���Ă΂�邽�тɍ폜�����̂ŁA
		// ����V����contactpoint�����K�v������
		// �����ł͌��݂̈ʒu�ƌ�����������contact point��V���ɍ쐬����
		// �����ō��ꂽcontactpoint��hapticinfo->points�Ɋi�[����A�蓮�ŏ����i��̕��ł���Ă�j

		Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
		Vec3d normal = current_info->current_col_normals[i];
		Vec3d center_pos = current_info->current_col_positions[i];

		Vec3d v0 = current_info->solid[i][0]->GetPointVelocity(center_pos);
		Vec3d v1 = current_info->solid[i][1]->GetPointVelocity(center_pos);

		local.Ex() = normal;
		local.Ey() = v1-v0;
		local.Ey() -= local.Ey() * normal * normal;
		if (local.Ey().square() > 1e-6){
			local.Ey().unitize(); 
		}else{
			if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
			else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
		}
		local.Ez() =  local.Ex() ^ local.Ey();

		PHContactPoint *point = DBG_NEW PHContactPoint(local, current_info->shapePair[i], current_info->current_col_positions[i], current_info->solid[i][0], current_info->solid[i][1]);
		point->scene = DCAST(PHScene, scene);
		point->engine = scene->GetConstraintEngine();

		// ���݂̃|�C���^���̐ڐG�_���i�[����
//		NewcolToCol->insert(pair<PHConstraint *, int>(point, i));

		PHSolid* so = getAdjacentSolid(point, DCAST(PHSolid, soPointer));
//		NewsolToSol->insert(pair<PHSolid *, int>(so, current_info->ColToSol[i]));

		if(scene->GetConstraintEngine()->IsInactiveSolid(current_info->solid[i][0]->Cast())) point->SetInactive(1, false);
		if(scene->GetConstraintEngine()->IsInactiveSolid(current_info->solid[i][1]->Cast())) point->SetInactive(0, false);

		if(current_info->solid[i][0]->IsDynamical() && current_info->solid[i][1]->IsDynamical()) 
		{
//			current_consts->push_back(pair<PHConstraint *, int>(point, current_info->sign[i]));
		}
		else
		{
//			current_static_consts->push_back(pair<PHConstraint *, int>(point, current_info->sign[i]));
		}

		// ��������ڐG�̓o�^
//		relative_consts->push_back(point);
//		relative_solids->insert(current_info->nearest_solids[current_info->ColToSol[i]]);
//		nearest_solids->insert(current_info->nearest_solids[current_info->ColToSol[i]]);

		// ���̉�ō폜�Ɏg�����߂Ƀ����N��ۑ����Ă���
//		current_info->points.insert(pair<double, PHContactPoint*>(point->pos.z, point));
	}
}

// ���͂̉e�����v�Z���A�����̕ϐ��Ɋi�[����֐�
void calculate_surround_effect(HapticInfo* new_info, HapticInfo* current_info)
{
	// �|�C���^�Ɋւ���ڐG���i�[����ϐ�
	vector<pair<PHConstraint *, int> > pointer_consts;
	vector<pair<PHConstraint *, int> > pointer_static_consts;

	// �ڐG�\���p�̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > col_candidate_consts;
	vector<pair<PHConstraint *, int> > col_candidate_static_consts;

	// �O��̐ڐG�������p�����߂̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > current_consts;
	vector<pair<PHConstraint *, int> > current_static_consts;

	// �V����������ڐG�ƌ��݂̐ڐG���֘A�t����}�b�v
	map<PHConstraint *, int> NewcolToCol;
	map<PHSolid *, int> NewsolToSol;

	set<PHSolid *> relative_solids;
	vector<PHConstraint *> relative_consts;
	set<PHSolid *> nearest_solids;

	// ���ݎg���Ă���͊o���̐ڐG�̂����ŁA�����g�������ȐڐG�����o���֐�
//	CreateConstraintFromCurrentInfo(current_info, &current_consts, &current_static_consts, &NewcolToCol,&NewsolToSol,
//									&relative_solids, &relative_consts, &nearest_solids);

	// �|�C���^����X�^�[�g���ă|�C���^�Ɋ֌W���鍄�̂ƐڐG�����ׂĎ擾���Ă���֐�
	GetCollisionsFromPointer(&pointer_consts, &pointer_static_consts, 
						&col_candidate_consts, &col_candidate_static_consts,
						current_info->points,
						&relative_consts, &relative_solids, &nearest_solids);

	// ���͂̉e�����i�[����ϐ�
	map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices;
	map<PHSolid *, SpatialVector> vecs;

	// �͂������Ă݂ē������A�e�����ϑ�����֐�
	PredictSimulations(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
						relative_consts, relative_solids,
						&matrices, &vecs);

	// �ڐG�̏����v�Z����
	MakeHapticInfo(new_info, current_info, 
					pointer_consts, pointer_static_consts, 
					col_candidate_consts, col_candidate_static_consts, 
					current_consts, current_static_consts, 
					NewcolToCol, NewsolToSol,
					matrices, vecs);

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
		for(PHConstraints::iterator it = cp.begin(); it != cp.end(); ++it)
		{
			// ���[�v���̃J�E���^�[���X�V
			++counter;
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
				for(int i = 0; i < info1.num_solids; ++i)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = (PHSolidIf *)&(*info1.nearest_solids[i]);
					if(nearest_address == (PHSolidIf *)((*it)->solid[0]))cout << " --this solid is nearest object!!" << endl;
				}
			}
			else 
			{
				for(int i = 0; i < info2.num_solids; ++i)
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
				for(int i = 0; i < info1.num_solids; ++i)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]))cout << " --this solid is nearest object!!" << endl;
				}
			}
			else
			{
				for(int i = 0; i < info2.num_solids; ++i)
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
	if(end - start > 0 && bOutput)ofs << "time = " << end - start << endl;
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

	// HAPTICREDERING�̍X�V��
	static const double dt = (double)1.0 / HAPTIC_FREQ;


	///////////////////////////////////////////////////////////////////////////////////////
	// �V�~�����[�V�����̊Ǘ��Ǝ��s

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

			// �V�������ɐ؂�ւ���O��
			// �V�������Ō��݂̏��������p���������X�V���Ȃ���
			// ����͐V������񂪍��ꂽ�Ƃ��ɂ͂��̏�񂪌Â����߂�
			// �͊o�Ɉ�a�����o���Ȃ����߂ɂ�
			// �ڐG�_�̈ʒu��@���͈�v������悤�ɂ���
			HapticInfo* info_next = NULL;
			HapticInfo* info_cur = NULL;

			if(current_valid_data)
			{
				info_cur = &info1;
				info_next = &info2;
			}
			else
			{
				info_cur = &info2;
				info_next = &info2;
			}

			for(int i = 0; i< info_next->num_solids; ++i)
			{
				// ����̍��̂��獡��̍��̂̓Y����������
				int prev_sol_index = info_next->NewsolToSol[i];

				// ������Ȃ���Ύ��ɂ�����
				// ����V�������̂���������ꍇ�͂��肦��
				if(prev_sol_index == -1)continue;

				// ����Ǝ���̃f�[�^�̊Ԃł̍��̂̌덷���v�Z����
				Vec3d pos_error = info_next->solid_current_center_positions[i] - info_cur->solid_current_center_positions[prev_sol_index];
				Quaterniond ori_error = info_next->solid_current_orientations[i] * info_cur->solid_current_orientations[prev_sol_index].Inv();

				// ����̃f�[�^�̂����ŁA���̌덷�ɂ���ĉe�����󂯂�����p�����ڐG�𒲂ׂ�
				for(int j = 0; j < info_next->num_col_per_sol[i]; j++)
				{
					if(info_next->bPreviousCollide[j])
					{
						// map���g���ĐV�����ڐG����Â����̓Y��������������
						int prev_col_index = info_next->NewcolToCol[info_next->SolToCol[i][j]];

						// �����͂��͖������ꉞ
						if(prev_col_index == -1) continue;

						// �ڐG�_�̏����R�s�[���鏈���ƍ��̂̌덷�𔽉f�����鏈��
						info_next->current_col_normals[j] = info_cur->current_col_normals[prev_col_index];
						info_next->current_col_normals[j] += ori_error * info_next->current_col_normals[j];
						info_next->current_col_normals[j] = info_next->current_col_normals[j].unit();

						info_next->current_col_positions[j] = info_cur->current_col_positions[prev_col_index];
						info_next->current_col_positions[j] += pos_error;
						info_next->current_col_positions[j] += ori_error * (info_next->current_col_positions[j] - info_next->solid_current_center_positions[j]);

						info_next->pointer_current_col_positions[j] = info_cur->pointer_current_col_positions[prev_col_index];						
					}
				}
			}
			
			bSimulation = true;
			step_counter = 0;
			current_valid_data = !current_valid_data;
		}
//		else if(bOutput)ofs << "error : simulation could not be finished in time" << endl;
	}
	else ++step_counter;


	////////////////////////////////////////////////////////////////////////////////////////
	// �������̎擾

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
	Vec3d goal = spidar_pos - pointer_pos;

	// VR��Ԃ̃|�C���^��SPIDAR��virtual coupling�łȂ���
	Vec3d VCForce = Km * goal + Bm * (SPIDARVel - pointer_vel);


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
	Quaterniond ang_goal = spidar_ori * pointer_ori.Inv();

	// ��]�ɂ��Ẵo�[�`�����J�b�v�����O
	Vec3d VCTorque = Kr * ang_goal.Rotation() + Br * (SPIDARAngVel.Rotation() - pointer_angvel);


	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// �o�[�`�����J�b�v�����O�ɏ]���ă|�C���^�̈ʒu�E�p���X�V
	pointer_vel += pointer_massinv * VCForce * dt;
	Vec3d pointer_dx = pointer_vel * dt;
	pointer_pos += pointer_dx;

	pointer_angvel += pointer_inertiainv * VCTorque * dt;
	Vec3d pointer_dth = pointer_angvel * dt;
	pointer_ori = Quaterniond::Rot(pointer_dth) * pointer_ori;
	pointer_ori = pointer_ori.unit();


	// �o�^���ꂽ�ڐG�������Ă��A���ݖ{���ɐڐG���Ă��邩�͂킩��Ȃ��̂Ńt���O��p��
	bool feedback = false;

	// �Ǐ��I�ȓ��͊w�v�Z���s���ꍇ
	if(bLocalDynamics)
	{
		
		////////////////////////////////////////////////////////////////////////////////////////
		// �Ǐ��I�ȓ��͊w�v�Z
		// �ڐG���̂̐ڐG�͂ɂ�鑬�x�E�p���x�X�V

		for(int i = 0; i < info->num_collisions; ++i)
		{
			// �ʂ̖@���ƁA�|�C���^��̓_���獄�̏�̓_�܂ł����񂾃x�N�g���̓��ς��v�Z
			// ���ꂪ�O�ȏ�Ȃ�i�[���x�N�g�����܂ށB���傤�ǐڂ��Ă���j�ڐG������
			if(dot(info->current_col_positions[i] - info->pointer_current_col_positions[i], info->current_col_normals[i]) >= 0)
			{
				// �Ƃɂ����ڐG���Ă���̂Ńt���O��ON�ɂ���
				info->bCollide[i] = true;

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
							// �^���������ɏ]���ĉ����x���v�Z����
							accel = info->solid_massinvs[sol_index] * FORCE_COEFF * VCForce;
							ang_accel = info->solid_inertiainvs[sol_index] * ((info->current_col_positions[i] - info->solid_current_center_positions[sol_index]) ^ (FORCE_COEFF * VCForce));

							// ����ꂽ�����x�ő��x���X�V
							// ���W�͌�ōX�V����
							info->solid_velocity[sol_index] += accel * dt;
										
							// �p���x���X�V
							info->solid_angular_velocity[sol_index] += ang_accel * dt;
						}
						// �Ǐ��I�ȓ��͊w�v�Z�{���͂̉e�����l��
						else
						{
							// ���̂ɉ�����͂��v�Z����
							// ratio��������̂́A
							// �s�񎩑̂����ł�SIMULATION_FREQ��dt���������Ă���
							// �����HAPTIC_FREQ��dt�ɒ�������
							// �܂��A���̍s��ɂ͂��ł�massinv, inertiainv���������Ă���̂�
							// �͂�������΂��ׂČv�Z�����
							// ��]�Ɋւ��ẮA
							// �i���߂̎d���Ƃ��āj�g���N�̌v�Z��r x F�̂�����
							// �O�ς��s��ƃx�N�g���̐ςƂ��Ă���킳��A
							// ���̍s�񂪂��łɌW���s����ɓW�J����Ă���̂ŁA
							// �݂͂̂�������΂悢���ƂɂȂ�
							Vec3d q_f = FORCE_COEFF * VCForce * ratio;

							// ���̗͂��y�ڂ��e�������ׂĂ̍��̂ɂ��Čv�Z����
							for(int j = 0; j < info->num_solids; ++j)
							{
								// �O�͂œ����Ȃ����̂��ЂƂł��o����
								// ���̐�͂��ׂĊO�͂œ����Ȃ����̂Ȃ̂�break����
								if(!info->nearest_solids[j]->IsDynamical()) break;

								// �s���p���ĉ����x���v�Z���đ��x���X�V
								info->solid_velocity[j] += info->vel_effect[j][i] * q_f;
								info->solid_angular_velocity[j] += info->ang_effect[j][i] * q_f;
							}
						}
					}
				}
			}
			else info->bCollide[i] = false;
		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// �Ǐ��I���͊w�v�Z�ɂ�鍄�̂̈ʒu�E�p�����X�V
		Vec3d correct_vector = Vec3d();
		Vec3d correct_torque = Vec3d();
		int num_cols = 0;

		vector<Vec3d> r_vectors;
		vector<Vec3d> c_vectors;

		for(int i = 0; i < info->num_solids; ++i)
		{
			// �Ǐ��I�ȓ��͊w�v�Z�ł͏d�͂��l������Ă��Ȃ��̂�
			// �d�͂̕������x���X�V����
			if(info->nearest_solids[i]->IsDynamical()) info->solid_velocity[i] += scene->GetGravity() * dt;

			if(bSurroundEffect)
			{
				// ���͂̉e���̂����ŁA�萔�������X�ɉ����Ă���
				// IsDynamical == false�̏ꍇ�͂��ꂼ��Vec3d()�ŏ��������Ă���̂�
				// ��������͕K�v�Ȃ�
				info->solid_velocity[i] += info->vel_constant[i] * ratio;
				info->solid_angular_velocity[i] += info->ang_constant[i] * ratio;
			}

			// ���x�ɂ��ړ��E��]
			Vec3d dx_local = info->solid_velocity[i] * dt;
			info->solid_current_center_positions[i] += dx_local;

			Vec3d dth_local = info->solid_angular_velocity[i] * dt;
			info->solid_current_orientations[i] = Quaterniond::Rot(dth_local) * info->solid_current_orientations[i];


			//////////////////////////////////////////////////////////////////////////////////////////
			// �������f����
			// �V�~�����[�V�������瓾��ꂽ�e���̗̂��z�̈ʒu��
			// ���X�ɓ������Ă����������s��
			// ����������̃|�C���^�Ƃ̐ڐG�͂ɂ���ē����x�N�g����
			// ���z�̈ʒu�܂œ������x�N�g���ɐ����Ȑ����͉����Ȃ�
			// ��]�͏��X�ɉ�]���𓮂����A�܂��p�x�����X�ɋ߂Â��Ă���
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
			for(int j = 0; j < info->num_col_per_sol[i]; ++j)
			{
				// ���̍��̏�ɂ���ڐG�����̂̈ړ��ɔ����čX�V����
				// �ꗗ�\����ڐG�̓Y����������
				int col_index = info->SolToCol[i][j];

				info->current_col_positions[col_index] += dx_local;
				Vec3d dx_rotation = dth_local ^ (info->current_col_positions[col_index] - info->solid_current_center_positions[i]);
				info->current_col_positions[col_index] += dx_rotation;
				info->current_col_normals[col_index] += dx_rotation;
				info->current_col_normals[col_index] = info->current_col_normals[col_index].unit();

				// �|�C���^�̐ڐG�_�X�V
				// �|�C���^�̈ړ��ʂɏ]���ĕ��s�ړ�
				// ���̏��̓|�C���^�����̂ł͂Ȃ��ڐG�����ꂼ�ꎝ���Ă���
				info->pointer_current_col_positions[col_index] += pointer_dx;
				info->pointer_current_col_positions[col_index] += pointer_dth ^ (info->pointer_current_col_positions[col_index] - pointer_pos);

				// ���ɍs���߂荞�݉��������̏���
				Vec3d col_vector = info->current_col_positions[col_index] - info->pointer_current_col_positions[col_index];
				double vector_coeff = dot(col_vector, info->current_col_normals[col_index]);

//				ofs << "all vectors " << vector_coeff << endl;

				// �߂荞��ł�����␳�p�̃f�[�^��ǉ�
				if(vector_coeff > 0)
				{
					r_vectors.push_back(Vec3d(info->pointer_current_col_positions[col_index] - pointer_pos));
					c_vectors.push_back(Vec3d(vector_coeff * info->current_col_normals[col_index].unit()));

					// �߂荞��ł��ڐG�̌�
					++num_cols;
				}
			}
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// �U���R�x�̂߂荞�݉�������

		// �v�Z�Ɏg���ϐ��̌^���`
		typedef PTM::TMatrixCol<99, 6, double> MatrixN6d;
		typedef PTM::TMatrixCol<6, 99, double> Matrix6Nd;
		typedef PTM::TVector<99, double> VecNd;

		// �|�C���^�̂߂荞�݂ɑ΂��ċ[���t�s����v�Z���A
		// �K�؂ȕ␳�ʂ����߂邱�ƂŁA�߂荞�݂�␳���Ă���
		// �|�C���^���g����i�E��]�����Ă߂荞�݂�����������悤�ɂ���
		if(num_cols)
		{
			Vec3d M_vec = Vec3d();
			Vec3d R_vec = Vec3d();

			if(num_cols > 33) num_cols = 33;

			// �f�[�^�Ƃ��Ďg�p�����x�N�g�����i�[����ϐ�
			vector<Vec3d> used_r_vecs;
			vector<Vec3d> used_c_vecs;

			MatrixN6d M;
			VecNd C;

			// �m�������傫�����ɂR�̐ڐG�����o���A
			// �����̏�񂩂�[���t�s��̌v�Z�ɕK�v��
			// �s��ƃx�N�g��������Ă���
			for(int k = 0; k < num_cols; ++k)
			{				
				Vec3d c = c_vectors[k];
				Vec3d r = r_vectors[k];

				// �Rx�U�s��̍쐬�Ƒ傫���R�̃x�N�g���̍쐬���R��s��
				for(int i = 0; i < 3; ++i)
				{						
					switch(i)
					{
					// 1, 4, 7�s��
					case 0:
						C[k * 3 + i] = c[0];

						for(int j = 0; j < 6; ++j)
						{
							switch(j)
							{
							case 0:
							case 4:
							case 5:
								M[k * 3 + i][j] = 0;
								break;
							case 1:
								M[k * 3 + i][j] = r[2];
								break;
							case 2:
								M[k * 3 + i][j] = - r[1];
								break;
							case 3:
								M[k * 3 + i][j] = 1;
								break;
							}
						}
						break;
					// 2, 5, 8�s��
					case 1:
						C[k * 3 + i] = c[1];
						for(int j = 0; j < 6; ++j)
						{
							switch(j)
							{
							case 1:
							case 3:
							case 5:
								M[k * 3 + i][j] = 0;
								break;
							case 0:
								M[k * 3 + i][j] = - r[2];
								break;
							case 2:
								M[k * 3 + i][j] = r[0];
								break;
							case 4:
								M[k * 3 + i][j] = 1;
								break;
							}
						}
						break;
					// 3, 6, 9�s��
					case 2:
						C[k * 3 + i] = c[2];
						for(int j = 0; j < 6; ++j)
						{
							switch(j)
							{
							case 2:
							case 3:
							case 4:
								M[k * 3 + i][j] = 0;
								break;
							case 0:
								M[k * 3 + i][j] = r[1];
								break;
							case 1:
								M[k * 3 + i][j] = - r[0];
								break;
							case 5:
								M[k * 3 + i][j] = 1;
								break;
							}
						}
						break;
					}
				}
			}

			Matrix6Nd Mt = M.trans();
			Matrix6d MTM = Mt * M;
			Matrix6d m = Matrix6d();

			// �Ίp�v�f�ɒl�𑫂��e�X�g
			for(int i = 0; i < 6; ++i)
			{
				for(int j = 0; j < 6; ++j)
				{
					if(i == j) m[i][j] = 0.00001;
					else m[i][j] = 0;
				}
			}
			MTM += m;

			Matrix6d MTMinv = MTM.inv();

			for(int i = 0; i < 9; ++i)
			{
				for(int j = 0; j < 6; ++j)
				{
					// QNaN���`�F�b�N����
					// ����ϐ�x = QNaN�̂Ƃ�
					// x == x�͎��s����炵��

					// QNaN���������錴����
					// �Q�_�̐ڐG�̂Ƃ���determinant�̃I�[�_���Ⴍ(10e-300�Ƃ��j�Ȃ�
					// inv�̌v�Z�Ŕ��U�ɋ߂Â��Ă��܂�����
					if(MTMinv[i][j] != MTMinv[i][j]) 
					{
						MTMinv[i][j] = 1e-300;
					}
				}
			}

			// �[���t�s��
			Matrix6Nd Pinv = MTMinv * Mt;

			// ���i�Ɖ�]�̃x�N�g��
			// ���`�O�v�f����]�x�N�g����xyz
			// ��l�`�Z�v�f�����i�x�N�g����xyz
			Vec6d Ans = Pinv * C;
			R_vec = Ans.sub_vector(TSubVectorDim<0,3>());
			M_vec = Ans.sub_vector(TSubVectorDim<3,3>());

			/*
			pointer_vel += P_CORRECTION_COEFF * M_vec / dt;
			pointer_angvel += P_CORRECTION_COEFF * R_vec / dt;
			*/

			// �|�C���^���g�̈ʒu���ړ�
			pointer_pos += M_vec;

			// �|�C���^���g�̎p����␳
			Quaterniond correct_q = Quaterniond::Rot(R_vec);
			pointer_ori = pointer_ori * correct_q;

			bool bDebugOutput = false;
			vector<double> norm_array;
			vector<double> col_pos_y_array;
			vector<double> pcol_pos_array;

			// ���ׂĂ̐ڐG�����̕����Ɉړ��E��]
			for(int i = 0; i < info->num_collisions; ++i)
			{
				info->pointer_current_col_positions[i] += M_vec;
				info->pointer_current_col_positions[i] += R_vec ^ (info->pointer_current_col_positions[i] - pointer_pos);

				double norm_val = dot(info->current_col_positions[i] - info->pointer_current_col_positions[i], info->current_col_normals[i]);
				if(norm_val > 0.00001)
				{
					bDebugOutput = true;
					norm_array.push_back(norm_val);
					col_pos_y_array.push_back(info->current_col_positions[i].y);
					pcol_pos_array.push_back(info->pointer_current_col_positions[i].y);
				}
			}

			if(bDebugOutput)
			{
				ofs << "debug information!!! -----------------------" << endl;
				for(int i = 0; i < (int)norm_array.size(); i++)
				{
//					ofs << "norm val = " << norm_array[i] << " y coord = " << col_pos_y_array[i] << " pointer y coord = " << pcol_pos_array[i] << endl;
				}
				ofs << "num_cols = " << num_cols << endl;
//				ofs << "M = " << endl << M << endl;
//				ofs << "C = " << C << endl;
//				ofs << "MTM = " << endl << MTM << endl;
//				ofs << "MTMinv = " << endl << MTMinv << endl;
//				ofs << "Pinv = " << endl << Pinv << endl;
				ofs << "Pinv * M = " << endl << Pinv * M << endl;
				ofs << "M_vec = " << M_vec << " R_vec = " << R_vec << endl;
			}
		}
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// �����ȃo�[�`�����J�b�v�����O
	// ���̂ɐڐG�͂�������
	else
	{
		// �ڐG������ΐڐG���Ă��鍄�̂ɒ񎦗͂̋t�����̃x�N�g����������
		// ���̂̋����͕����V�~�����[�V�����ɔC����
		for(int i = 0; i < info->num_collisions; ++i)
		{
			// �͊o�t�B�[�h�o�b�NON
			feedback = true;

			int sol_index = info->ColToSol[i];

			// ratio�������闝�R�́A
			// �V�~�����[�V�������������͂̑��ʂ��V�~�����[�V�����̎����ŗ͐ςɂ��邽��
			info->nearest_solids[sol_index]->AddForce(FORCE_COEFF * VCForce * ratio, info->current_col_positions[i]);
		}
	}


	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���[�U�ւ̒񎦗͂̌v�Z�Ɛݒ�
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

	Vec3d gravity = scene->GetGravity() * scene->GetTimeStep();

	// �͊o�����_�����O����V�~�����[�V�����ւ̋Ǐ��I���͊w�v�Z�̌��ʔ��f����
	// ���ׂĂ̍��̂ɂ��čs��
	for(int i = 0; i < info->num_solids; ++i)
	{
		// �덷�̕����s�ړ��E��]����
		Vec3d pos_error = info->solid_current_center_positions[i] - info->nearest_solids[i]->GetCenterPosition();
		Quaterniond ori_error = info->solid_current_orientations[i] * info->nearest_solids[i]->GetOrientation().Inv();

		info->nearest_solids[i]->SetFramePosition(info->nearest_solids[i]->GetFramePosition() + pos_error);
		info->nearest_solids[i]->SetOrientation(info->nearest_solids[i]->GetOrientation() * ori_error);
		info->nearest_solids[i]->SetCenterPosition(info->solid_current_center_positions[i]);
		info->nearest_solids[i]->SetVelocity(info->solid_velocity[i]);

		// ���̏C���ɂ���ăV�~�����[�V�������ꂽ�̂�update��true�ɂ���
		info->nearest_solids[i]->SetUpdated(true);

		// �����Ǐ��I�ȓ��͊w�v�Z���s���Ă����ꍇ
		// HapticRendering���ŏd�͌v�Z���s��
		// ����ɃV�~�����[�V�����ŏd�͌v�Z���s���Ɠ�d�ŏd�͂�ǉ����Ă��܂��̂�
		// GravityEngine���d�͂������邱�Ƃ����z����
		// ���̉����镪���Ɉ����đ��E������
		// ���ڏd�͂�ON/OFF�����Ă��悢���A���̑��삾���ŃR�X�g���������Ă��܂��̂�
		// �Ƃ肠�������̂悤�ɂ���
		if(info->nearest_solids[i]->IsDynamical() && bLocalDynamics)
			info->nearest_solids[i]->SetVelocity(info->nearest_solids[i]->GetVelocity() 
				- gravity * info->nearest_solids[i]->GetMass());

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

	soPointer->SetFramePosition(pointer_pos);
	soPointer->SetVelocity(pointer_vel);

	soPointer->SetOrientation(pointer_ori);
	soPointer->SetAngularVelocity(pointer_angvel);
}

void SetupCollisionPrediction()
{
	// soPointerCopy�̍��W��soPointer�ɂ��킹��
	// ���̓��inactive���m�Ȃ̂ł߂肱�݁A
	// soPointerCopy��soPointer������傫���̂ŁA
	// ���̐ڐG�_���݂��soPointer�̐ڐG�������ȓ_���킩��
	soPointerCopy->SetFramePosition(pointer_pos);
	soPointerCopy->SetVelocity(pointer_vel);
	soPointerCopy->SetOrientation(pointer_ori);
	soPointerCopy->SetAngularVelocity(pointer_angvel);
}

void StepSimulation()
{
//	t_end();
//	t_start();

//	if(bOutput) ofs << "start simulation" << endl;

	scene->ClearForce();

	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ����������C������
	ErrorCorrection();

	// �|�C���^�ɐڐG�������ȓ_��T�����߂̏���
	SetupCollisionPrediction();

	scene->GenerateForce();
	scene->Integrate();

	// �Փ˓_����\��
//	show_collision_info();

	// �Փ˓_������͂̉e�����v�Z
	// ��{�I�ɑO��̃f�[�^�������p��
	// �P���Q�ƒ��B�Q���X�V
	if(current_valid_data) calculate_surround_effect(&info2, &info1);
	// �Q���Q�ƒ��B�P���X�V
	else calculate_surround_effect(&info1, &info2);

//	if(bOutput) ofs << "finish simulation " << endl;

	// �V�~�����[�V�������I������̂Ńt���O��������
	bSimulation = false;

	//	t_end();
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

		glutSolidSphere(0.5, 12, 9);
	glPopMatrix();

	// ���_��`��
	glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		Vec3f te = Vec3f();
		Affinef af = Affinef();
		Posef::Unit(te).ToAffine(af);
		glMultMatrixf(af);
		glutSolidSphere(0.5, 12, 9);
	glPopMatrix();
}

// �ڐG�_��`�悷��֐�
void displayCollisions()
{
	HapticInfo* info;

	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}

	GLfloat purple[] = {1.0, 0.0, 1.0, 1.0};
	GLfloat color2[] = {0.0, 1.0, 1.0, 1.0};
	GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};
	GLfloat red[] = {1.0, 0.0, 0.0, 1.0};

	for(int i = 0; i < info->num_collisions; ++i)
	{
		glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			Affinef col_affine;
			Posed::Unit(info->current_col_positions[i]).ToAffine(col_affine);
			glMultMatrixf(col_affine);
			
			if(!info->bPreviousCollide[i])
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, purple);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, purple);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, purple);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, purple);
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, red);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, red);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, red);
			}
			glutSolidSphere(0.1, 12, 9);
		glPopMatrix();
	}

	for(int i = 0; i < info->num_collisions; ++i)
	{
		glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			Affinef col_affine;
			Posed::Unit(info->pointer_current_col_positions[i]).ToAffine(col_affine);
			glMultMatrixf(col_affine);
	
			if(!info->bPreviousCollide[i])
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, color2);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, color2);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, color2);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, color2);
			}
			else
			{
				glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, blue);
				glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, blue);
				glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, blue);
			}
			glutSolidSphere(0.1, 12, 9);
		glPopMatrix();
	}

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

	displayCollisions();

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
		// HapticRendering�X���b�h���폜
		timeKillEvent(FTimerId1);
		timeEndPeriod(1);

		info1.points.clear();
		info2.points.clear();

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
	// �|�C���^�̃L�����u���[�V����
	else if(key == 'c')
	{
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
		soPointer->SetFramePosition(Vec3d());
		spidarG6.Calib();
	}
	// sphere�̏ꏊ�����Z�b�g
	else if(key == 'r')
	{
		for(int i = 0; i < NUM_OBJECTS; ++i)
		{
			soObject[i]->SetPose(Posed());
			soObject[i]->SetFramePosition(Vec3f(0, 5+5*(i+1), 0));
			soObject[i]->SetAngularVelocity(Vec3d());
			soObject[i]->SetVelocity(Vec3d());
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

		cout << "Feedback Force ";
		if(bforce)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Surrounding Effect ";
		if(bSurroundEffect)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Pointer Display ";
		if(bDisplayPointer)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Gradual Reflection ";
		if(bGradualReflection)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "File Output ";
		if(bOutput)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "----------------------" << endl;
	}
	// �ݒ�̃v���Z�b�g
	else if(key == '1')
	{
		bSurroundEffect = true;
		bGradualReflection = true;
		bLocalDynamics = true;

		cout << "PRESET 1: Local Dynamics With Surrounding Effect, Penetration Correction, and Gradual Reflection" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '2')
	{
		bSurroundEffect = true;
		bGradualReflection = false;
		bLocalDynamics = true;
		
		cout << "PRESET 2: Local Dynamics With Surrounding Effect" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '3')
	{
		bSurroundEffect = false;
		bGradualReflection = true;
		bLocalDynamics = true;

		cout << "PRESET 3: Local Dynamics With Penetration Correction and Gradual Reflection" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '4')
	{
		bSurroundEffect = false;
		bGradualReflection = false;
		bLocalDynamics = true;

		cout << "PRESET 4: Local Dynamics" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '5')
	{
		bSurroundEffect = false;
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
	ofs.precision(12);

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

	info1.num_collisions = 0;
	info2.num_collisions = 0;

//	timer.CountUS();

	// haptic�X���b�h�̐����E�J�n
	FTimerId1 = timeSetEvent(1000 / HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    HapticRendering,//�R�[���o�b�N�֐�
		                    (DWORD)&time,//���[�U�[��`�̒l
		                    TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
		                    );

	// openGL main loop
	glutMainLoop();
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
	desc.mass = 0.3f;

	// inertia�̌v�Z��
	// �����̂̏ꍇ I = mass * (r1^2 + r2^2) / 12
	// ���̏ꍇ I = mass * r~2 * 2/5
	desc.inertia = Matrix3d(0.2,0,0,0,0.2,0,0,0,0.2);
/*
	soObject.push_back(scene->CreateSolid(desc));

	desc.mass = 1.0f;
	desc.inertia = Matrix3d(0.375, 0.0, 0.0, 0.0, 0.375, 0.0, 0.0, 0.0, 0.375);
	soObject.push_back(scene->CreateSolid(desc));
*/

	// Solid�̍쐬
	for (unsigned int objectCnt=0; objectCnt<NUM_OBJECTS; ++objectCnt){
		soObject.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
	}


	desc.mass = 0.1f;
	desc.inertia = Matrix3d(0.16, 0, 0, 0, 0.16, 0, 0, 0, 0.16);

	for(int i = 0; i < 1; ++i)
	{
		PHSolidIf *soWall = scene->CreateSolid(desc);
		soWall->SetDynamical(false);
		soWalls.push_back(soWall);
//		soFloor = scene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
//		soFloor->SetDynamical(false);
	}
	soPointer = scene->CreateSolid(desc);
	soPointerCopy = scene->CreateSolid(desc);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;

	{
		CDSphereDesc sd;
		sd.radius = 1.25f;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));

		// ���`�̃|�C���^
		/*
		soPointer->AddShape(sphere);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));
		*/

		sd.radius = 2.0;
		sphere = DCAST(CDSphereIf,phSdk->CreateShape(sd));

		CDBoxDesc bd;
		for(int i = 0; i < 1; ++i)
		{
			Vec3f position;

			switch(i)
			{
			case 0:
				bd.boxsize = Vec3f(30.0f, 10.0f, 30.0f);
				position = Vec3f(0, -6.0f, 0);
				break;
			case 1:
				bd.boxsize = Vec3f(1.0f, 1.0f, 1.0f);
				position = Vec3f(1.0f, 0.0f, 0.0f);
				break;
			case 2:
				bd.boxsize = Vec3f(1.0f, 1.0f, 1.0f);
				position = Vec3f(-1.0f, 0.0f, 0.0f);
				break;
			case 3:
				bd.boxsize = Vec3f(1.0f, 1.0f, 1.0f);
				position = Vec3f(0.0f, 0.0f, 1.0f);
				break;
			}

			floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
			soWalls[i]->AddShape(floor);
			soWalls[i]->SetFramePosition(position);
//			soFloor->AddShape(floor);
//			soFloor->SetFramePosition(Vec3f(0,-4.0f,0));
		}

		// �l�p�̃|�C���^
		bd.boxsize = Vec3f(2 * POINTER_RADIUS, 2 * POINTER_RADIUS, 2 * POINTER_RADIUS);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soPointer->AddShape(floor);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));

		// �|�C���^��������傫�ȃ|�C���^���쐬���A
		// �ڐG�\�z�p�Ɏg��
		bd.boxsize += Vec3f(EPSILON, EPSILON, EPSILON);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soPointerCopy->AddShape(floor);
		soPointerCopy->SetFramePosition(soPointer->GetFramePosition() + Vec3f(1, 1, 1));

		bd.boxsize = Vec3f(1.5f, 1.5f, 1.5f);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_OBJECTS; ++sphereCnt){
		soObject[sphereCnt]->AddShape(floor);
		soObject[sphereCnt]->SetFramePosition(Vec3f(0, 5.0f*(sphereCnt+1), 2));
	}
	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	// pointer�̏d�͂𖳌��ɂ���
	RemoveGravity(soPointer);
	RemoveGravity(soPointerCopy);

	// pointer�ƐڐG���鍄�̂Ƃ̉�͖@�ɂ��v�Z����߁A
	// �͊o�ŏ�������悤�ɂ���
	AddInactiveSolid(scene, soPointer);

	// soPointerCopy��inactive�ɂ���
	// ���������soPointer�ɂ߂肱�ނ悤�ɂȂ�͂�
	AddInactiveSolid(scene, soPointerCopy);

	pointer_pos = soPointer->GetFramePosition();
	pointer_vel = soPointer->GetVelocity();
	pointer_massinv = soPointer->GetMassInv();
	pointer_angvel = soPointer->GetAngularVelocity();
	pointer_inertiainv = soPointer->GetInertiaInv();
	pointer_ori = soPointer->GetOrientation();

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
	view.Pos() = Vec3f(0.0, 2.0, -8.0);
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
