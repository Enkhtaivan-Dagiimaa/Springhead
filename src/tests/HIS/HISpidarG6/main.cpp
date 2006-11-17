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
#define NUM_PREDICT_ITERATE 1		// �\���V�~�����[�V�����̃C�e���[�g��

#define NUM_COLLISIONS 10			// �|�C���^�ւ̋��e�ڐG��

#ifdef _DEBUG
	#define SIMULATION_FREQ	80         // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		100			// �͊o�X���b�h�̎���Hz
	float Km = 400;						// virtual coupling�̌W��
	float Bm = 30;						// ���i

	float Kr = 40;						// ��]
	float Br = 13;

#elif _WINDOWS
	#define SIMULATION_FREQ	80         // �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000		// �͊o�X���b�h�̎���Hz
//	float Km = 2000;					// virtual coupling�̌W��
	float Km = 400;
	float Bm = 30;

	float Kr = 40;						// ��]
	float Br = 13;
#endif

// �񎦗͂ƍ��̂ɒ񎦂���͂𒼐ڕω�������萔
double FORCE_COEFF =		1.0;

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

bool bforce = false;

MMRESULT FTimerId1;
MMRESULT FTimerId2;


// �͊o�v�Z�ɕK�v�ȃf�[�^���W�߂��\����
// �͊o�v�Z�������ōs����悤��
// �V�~�����[�V�����Ńf�[�^���i�[���Ă���
typedef struct {
	// collision solid data
	PHSolid* nearest_solids[NUM_COLLISIONS];
	Vec3d solid_velocity[NUM_COLLISIONS];
	Vec3d solid_angular_velocity[NUM_COLLISIONS];
	Vec3d solid_center_position[NUM_COLLISIONS];
	double solid_massinv[NUM_COLLISIONS];
	Matrix3d solid_inertiainv[NUM_COLLISIONS];

	// collision data
	Vec3d col_positions[NUM_COLLISIONS];
	Vec3d col_normals[NUM_COLLISIONS];
	int num_collisions;
	
	Vec3d original_col_positions[NUM_COLLISIONS];
	Vec3d original_col_normals[NUM_COLLISIONS];

	// ���͂̉e���̌��ʂ��i�[����ϐ�
	Matrix3d vel_effect[NUM_COLLISIONS];
	Vec3d vel_constant[NUM_COLLISIONS];

	Matrix3d ang_effect[NUM_COLLISIONS];
	Vec3d ang_constant[NUM_COLLISIONS];

	// pointer data
	Vec3d pointer_pos;
	Vec3d pointer_vel;
	double pointer_massinv;

	Quaterniond pointer_ori;
	Vec3d pointer_angvel;
	Matrix3d pointer_inertiainv;

	bool bCollision;
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
void MakeHapticInfo(HapticInfo *info, vector<pair<PHConstraint *, int> > pointer_consts, vector<Matrix3d> matrices, vector<SpatialVector> vecs, bool bCollision)
{
	int num_dynamical = 0;

	// �͊o�����_�����O�ɕK�v�ȃf�[�^�̍쐬
	for(int i = 0; i < (int)pointer_consts.size(); i++)
	{
		PHConstraint* constraint = pointer_consts.at(num_dynamical).first;
		int sign = pointer_consts.at(num_dynamical).second;
		PHSolid* so;

		if(sign > 0)
		{
			so = pointer_consts.at(num_dynamical).first->solid[0];
		}
		else
		{
			so = pointer_consts.at(num_dynamical).first->solid[1];
		}

		PHContactPoint* contact = DCAST(PHContactPoint, constraint);
		info->col_positions[num_dynamical] = contact->pos;
		info->original_col_positions[num_dynamical] = info->col_positions[num_dynamical];

		info->solid_velocity[num_dynamical] = so->GetVelocity() + so->GetAngularVelocity() ^ (contact->pos - so->GetCenterPosition());
		info->solid_angular_velocity[num_dynamical] = so->GetAngularVelocity();
		info->solid_center_position[num_dynamical] = so->GetCenterPosition();
		info->solid_massinv[num_dynamical] = so->GetMassInv();
		info->solid_inertiainv[num_dynamical] = so->GetInertiaInv();

		info->nearest_solids[num_dynamical] = so;

		info->col_normals[num_dynamical] = contact->shapePair->normal * sign;
		info->original_col_normals[num_dynamical] = info->col_normals[num_dynamical];

		/*
		// �v�Z�œ���ꂽ���͂̉e����\���ϐ����R�s�[����
		info->vel_constant[num_dynamical] = C;
		info->ang_constant[num_dynamical] = D;
		info->vel_effect[num_dynamical] = M;
	//	info->ang_effect[num_dynamical] = N;
		*/

		num_dynamical++;
	}

	info->num_collisions = num_dynamical > NUM_COLLISIONS ? NUM_COLLISIONS : num_dynamical;
	info->bCollision = bCollision;

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
void RecursiveSolidRetrieval(PHSolid* solid, set<PHConstraint *>* relative_solid_consts, set<PHSolid *>* relative_solids, int depth)
{
	PHConstraints cs = GetContactPoints(scene);

	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
	{
		// �^����ꂽ���̂���A���̍��̂ɐڐG���鍄�̂��擾����
		PHSolid* col_solid = getAdjacentSolid(*it, solid);

		// �������̍��̂����݂��A�|�C���^�ł͂Ȃ��ꍇ
		// �|�C���^�̏ꍇ��GetSolidsFromPointer�Ŏ擾����
		if(col_solid != NULL && col_solid != soPointer)
		{
			// �ڐG���̂�ǉ�
			PHConstraint* c = *it;
			pair<set<PHConstraint *>::iterator, bool> p = relative_solid_consts->insert(c);

			// ���ǉ������f�[�^�͏d�����ĂȂ�����
			if(p.second != false)
			{
				// PHSolid��ۑ�
				// �d�����Ă���ꍇ��set�Ȃ̂Ŏ����I�ɔr�������
				relative_solids->insert(c->solid[0]);
				relative_solids->insert(c->solid[1]);

				// �����Ȃ����̂ł͂Ȃ��A���擾����܂ŒB���Ă��Ȃ�����
				if(col_solid->IsDynamical() == true && depth < LIMIT_DEPTH)
				{
					// ���̍��̂ɂ�����
					RecursiveSolidRetrieval(col_solid, relative_solid_consts, relative_solids, depth++);
				}
			}
		}
	}
}

// �|�C���^����X�^�[�g���ĕK�v�ȍ��̂����ׂĎ擾���Ă���֐�
void GetSolidsFromPointer(vector<pair<PHConstraint *, int> >* pointer_consts, set<PHSolid *>* relative_solids, set<PHConstraint *>* relative_solid_consts, bool* bCollision)
{
	// �Փ˓_���擾
	PHConstraints cs = GetContactPoints(scene);

	// ���ׂĂ̏Փ˓_�ɂ��Ē���
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); it++)
	{
		int sign = 1;

		// �|�C���^���܂ސڐG���擾
		PHSolid* col_solid = getAdjacentSolid(*it, (PHSolid*)soPointer, &sign);
		
		// �����ł���
		if(col_solid != NULL)
		{
			// �ڐG�����݂���
			*bCollision = true;

			// �|�C���^�Ɛڂ��Ă��鍄�̂�PHSolid��ۑ�
			if(sign > 0)
			{
				relative_solids->insert((*it)->solid[0]);
			}
			else
			{
				relative_solids->insert((*it)->solid[1]);
			}

			// ���������鍄�̂ł���΁A�ڐG���̂��i�[
			// �����Ȃ����̂͒ǉ������A���̑����bCollision�ł܂��Ȃ�
			// ��������ƃf�[�^���쐬���鍄�̂̐�������̂ō������ł���
			if(col_solid->IsDynamical() == true)pointer_consts->push_back(pair<PHConstraint *, int>(*it, sign));

// ���͂̉e�����v�Z���邽�߂̃f�[�^�쐬����
#if 1
			relative_solid_consts->insert(*it);

			// ���̍��̂̂Ɛ������ڐG�����̍��̂ɉe�����y�ڂ��\��������ꍇ�́A���̍��̂ɂ�����
			if(col_solid->IsDynamical() == true)RecursiveSolidRetrieval(col_solid, relative_solid_consts, relative_solids, 0);
#endif
		}
	}
}

map<PHConstraint *, SpatialVector> lambda;

// �����_�̒l��ۑ����Ă���
void SaveLambda(set<PHConstraint *> relative_solid_consts)
{
	for(set<PHConstraint *>::iterator it = relative_solid_consts.begin(); it != relative_solid_consts.end(); it++)
	{
		// �����_�̒l(fv, fw)��ڐG�ƃy�A�ŕۑ����Ă���
		lambda.insert(pair<PHConstraint *, SpatialVector>(*it, (*it)->f));
	}
}

// �摗��V�~�����[�V����������֐�
vector<SpatialVector> PredictSimulation(vector<pair<PHConstraint *, int> > pointer_consts, set<PHConstraint *> relative_solid_consts, set<PHSolid *> relative_solids, Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index = -1)
{
	double dt = scene->GetTimeStep();

	// �ꎞ�I�ȕۑ��p�ϐ�
	SpatialVector f_save;
	PHSolid* si;

	// �|�C���^���ڂ��鍄�̂̂���index�ԖڂɒP�ʗ͂�������
	if(index != -1)
	{
		PHConstraint *constraint = pointer_consts.at(index).first;
		PHContactPoint* contact = DCAST(PHContactPoint, constraint);
			
		if(pointer_consts.at(index).second > 0)
		{
			si = DCAST(PHSolid, constraint->solid[0]);
		}
		else
		{
			si = DCAST(PHSolid, constraint->solid[1]);
		}

		f_save = si->f;

		// ���O�Ɍv�Z���Ă������l���Q��
		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);

		// �����ŗ^����ꂽ�P�ʗ͂��S�����W�ɕϊ����ĉ�����
		si->dv.v += si->GetMassInv() * force_vec * dt;
		si->dv.w += si->GetInertiaInv() * (si->GetOrientation().Conjugated() * ((contact->pos - si->GetCenterPosition()) ^ force_vec) - contact->shapePair->normal * pointer_consts.at(index).second) * dt;
	}

	// �ڐG��SetupDynamics��compjacobian�ȊO�̕��������
	// �ʂɍs�����R�͌v�Z�ʂ̖��ƈˑ��֌W�ɂ��dv(dw)��dv0(dw0)�Ɉˑ�
	for(set<PHConstraint *>::iterator it = relative_solid_consts.begin(); it != relative_solid_consts.end(); it++)
	{
		(*it)->SetupDynamicsForPrediction();
	}

	for(int i = 0; i < NUM_PREDICT_ITERATE; i++)
	{
		// ���ׂĂ̐ڐG��iterate
		for(set<PHConstraint *>::iterator it = relative_solid_consts.begin(); it != relative_solid_consts.end(); it++)
		{
			(*it)->IterateDynamics();
		}
	}
	
	vector<SpatialVector> effects;

	// �e�����ϑ�
	for(vector<pair<PHConstraint *, int> >::iterator it = pointer_consts.begin(); it != pointer_consts.end(); it++)
	{
		PHConstraint* c = (*it).first;
		int sign = (*it).second;

		PHSolid* s;

		if(sign > 0)
		{
			s = c->solid[0];
		}
		else
		{
			s = c->solid[1];
		}

		effects.push_back(s->dv);
	}

	// �������P�ʗ͂̕��������ɂ��ǂ�
	if(index != -1)
	{		
		si->f = f_save;
	}

	// fv��fw�̒l�����ɂ��ǂ�
	for(set<PHConstraint *>::iterator it = relative_solid_consts.begin(); it != relative_solid_consts.end(); it++)
	{
		map<PHConstraint *, SpatialVector>::iterator p = lambda.find(*it);
		(*it)->f = p->second;
	}

	// �e����return
	return effects;
}

// �\���V�~�����[�V�������Z�b�g�A�b�v����
void SetupPredictSimulation(set<PHConstraint *> relative_solid_consts, set<PHSolid *> relative_solids, map<PHSolid *, Vec3d> *coeff)
{
	double dt = scene->GetTimeStep();

	// ���ׂĂ̍��̂��Z�b�g�A�b�v
	for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); it++)
	{
		PHSolid* solid = *it;
		solid->SetupDynamics(dt);

		// �P�ʗ͂������鏈���̂Ƃ���Ŏg���l(=�������͂����[���h���W����S�����W�ɕϊ��������)�����̂����Ɍv�Z���Ă���
		Vec3d w = solid->GetOrientation().Conjugated() * solid->GetAngularVelocity();
		Vec3d calc = w % (solid->GetInertia() * w);
		coeff->insert(pair<PHSolid *, Vec3d>(*it, calc));
	}

	// ���ׂĂ̐ڐG�ɂ��ă��R�r�A�����v�Z
	for(set<PHConstraint *>::iterator it = relative_solid_consts.begin(); it != relative_solid_consts.end(); it++)
	{
		(*it)->CompResponseMatrix();
	}

	// lambda��������
	lambda.clear();

	// �����_��ۑ�����
	SaveLambda(relative_solid_consts);
}


// ��Ď�@�̂��߂ɐ摗��V�~�����[�V�������v��I�ɍs���Ă����֐�
void PredictSimulations(vector<pair<PHConstraint *, int> > pointer_consts, set<PHConstraint *> relative_solid_consts, set<PHSolid *> relative_solids, vector<Matrix3d> *mat, vector<SpatialVector> *vec)
{
	map<PHSolid *, Vec3d> coeff;

	// �K�v�ȏ��̏���
	SetupPredictSimulation(relative_solid_consts, relative_solids, &coeff);

	vector<Matrix3d> effects;

	// ������͂��O�x�N�g���Ƃ��Ē萔�����擾
	vector<SpatialVector> b = PredictSimulation(pointer_consts, relative_solid_consts, relative_solids, Vec3d(), coeff);

	// �ڐG�����񐔂����J��Ԃ�
	for(int i = 0; i < (int)pointer_consts.size(); i++)
	{
		// ����P�ʃx�N�g���������Ă��̌��ʂ�����킷�x�N�g�����擾
		vector<SpatialVector> vec_x = PredictSimulation(pointer_consts, relative_solid_consts, relative_solids, Vec3d(1, 0, 0), coeff, i);
		vector<SpatialVector> vec_y = PredictSimulation(pointer_consts, relative_solid_consts, relative_solids, Vec3d(1, 1, 0), coeff, i);
		vector<SpatialVector> vec_z = PredictSimulation(pointer_consts, relative_solid_consts, relative_solids, Vec3d(1, 0, 1), coeff, i);

		// ���̃��[�v�ŏc�x�N�g�����v�Z���i�[
		for(int j = 0; j < (int)b.size(); j++)
		{
			Matrix3d v = Matrix3d(vec_x[j].v - b[j].v, vec_y[j].v - vec_x[j].v - b[j].v, vec_z[j].v - vec_x[j].v - b[j].v);
			Matrix3d w = Matrix3d(vec_x[j].w - b[j].w, vec_y[j].w - vec_x[j].w - b[j].w, vec_z[j].w - vec_x[j].w - b[j].w);

			effects.push_back(v);
			effects.push_back(w);
		}
	}

	*mat = effects;
	*vec = b;
}


// ���͂̉e�����v�Z���A�����̕ϐ��Ɋi�[����֐�
void calculate_surround_effect(HapticInfo* info)
{
	vector<pair<PHConstraint *, int> > pointer_consts;

	set<PHSolid *> relative_solids;
	set<PHConstraint *> relative_solid_consts;

	bool bCollision = false;

	// �|�C���^����X�^�[�g���ĕK�v�ȍ��̂����ׂĎ擾���Ă���֐�
	GetSolidsFromPointer(&pointer_consts, &relative_solids, &relative_solid_consts, &bCollision);

	vector<Matrix3d> matrices;
	vector<SpatialVector> vecs;

	// �͂������Ă݂ē������A�e�����ϑ�����֐�
	if(bCollision == true)PredictSimulations(pointer_consts, relative_solid_consts, relative_solids, &matrices, &vecs);

	// �ڐG�̏����v�Z����
	MakeHapticInfo(info, pointer_consts, matrices, vecs, bCollision);
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
		DSTR << "----------NEW COLLISION DATA No." << global_collision_counter++ << "-----------" << endl;
		
		// ���ׂĂ̏Փ˂ɑ΂��ď���
		for(PHConstraints::iterator it = cp.begin(); it != cp.end(); it++)
		{
			// ���[�v���̃J�E���^�[���X�V
			counter++;
			DSTR << "COLLISION" << counter << ": " << endl;

			// PHContactPoint��dynamic cast���������̃`�F�b�N
			PHContactPoint* contact = DCAST(PHContactPoint, (*it));
			DSTR << "collision pos = " << contact->pos << endl;
			DSTR << endl;

			// PHConstraint�̏��̕\��
			// solid1
			DSTR << "solid1.mass = " << (*it)->solid[0]->mass << endl;
			DSTR << "solid1.inertia = " << endl << (*it)->solid[0]->inertia;
			DSTR << "solid1.velocity = " << (*it)->solid[0]->GetVelocity()<< endl;
			DSTR << "solid1.angular_velocity = " << (*it)->solid[0]->GetAngularVelocity() << endl;
			DSTR << "solid1.position = " << (*it)->solid[0]->GetFramePosition() << endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[0]->GetCenterPosition()).unit() << endl;

			if(current_valid_data == true)
			{
				for(int i = 0; i < info1.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[0]))DSTR << " --this solid is nearest object!!" << endl;
				}
			}
			else 
			{
				for(int i = 0; i < info2.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolidIf* nearest_address = &(*info2.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[0]))DSTR << " --this solid is nearest object!!" << endl;
				}

			}
			DSTR << endl;

			// solid2
			DSTR << "solid2.mass = " << (*it)->solid[1]->mass << endl;
			DSTR << "solid2.inertia = " << endl << (*it)->solid[1]->inertia;
			DSTR << "solid2.velocity = " << (*it)->solid[1]->GetVelocity()<< endl;
			DSTR << "solid2.angular_velocity = " << (*it)->solid[1]->GetAngularVelocity() << endl;
			DSTR << "solid2.position = " << (*it)->solid[1]->GetFramePosition() << endl;
			DSTR << "vector to working point from center = " << (contact->pos - (*it)->solid[1]->GetCenterPosition()).unit() << endl;

			if(current_valid_data == true)
			{
				for(int i = 0; i < info1.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info1.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]))DSTR << " --this solid is nearest object!!" << endl;
				}
			}
			else
			{
				for(int i = 0; i < info2.num_collisions; i++)
				{
					// �ŋߖT���̂̃A�h���X���擾
					PHSolid* nearest_address = &(*info2.nearest_solids[i]);
					if(nearest_address == ((*it)->solid[1]))DSTR << " --this solid is nearest object!!" << endl;
				}
			}
			DSTR << endl;
		}
	}
}

static clock_t start, end;
static int temp_counter = 0;

void t_start()
{
	start = clock();
}

void t_end()
{
	// ����ɂ����鎞�Ԃ��`�F�b�N
	end = clock();

	double time = (double)(end - start) / CLOCKS_PER_SEC;
	if(time > 0)printf("%d: time =  %f\n", temp_counter++, time);
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

	// SPIDAR�X�V��
	static const double dt = (double)1.0 / HAPTIC_FREQ;

	// �V�~�����[�V�������瓾��������i�[����ϐ�
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data)
	{
		info = &info1;
	}
	else
	{
		info = &info2;
	}

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

	// soPointer�̑��x���X�V
	info->pointer_vel = info->pointer_vel + info->pointer_massinv * VCForce * dt;


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

	// �p���x���X�V
	info->pointer_angvel = info->pointer_angvel + info->pointer_inertiainv * VCTorque * dt;


	//////////////////////////////////////////////////////////////////////////////////////////
	// �ڐG�_�̍X�V
	if(info->bCollision)
	{
		// �ڐG�ɂ���ē������ʂ̑��a���i�[����ϐ�
		Vec3d pointer_dx = Vec3d();

		for(int i = 0; i < info->num_collisions; i++)
		{
			Vec3d accel;
			Vec3d ang_accel;

			// �Ǐ��I�ȓ��͊w�v�Z�̂�
			if(bSurroundEffect == false)
			{
				info->nearest_solids[i]->AddForce(VCForce, info->col_positions[i]);
				accel = info->solid_massinv[i] * VCForce;
				ang_accel = info->solid_inertiainv[i] * ((info->col_positions[i] - info->solid_center_position[i]) ^ (VCForce));
			}
			// �Ǐ��I�ȓ��͊w�v�Z�{���͂̉e�����l��
			else
			{
				// �����x���v�Z
				accel = info->vel_effect[i] * VCForce + info->vel_constant[i];
				ang_accel = info->solid_inertiainv[i] * VCForce + info->ang_constant[i];
			}

			// ���x���v�Z���A�Փ˓_���X�V����
			info->solid_velocity[i] = info->solid_velocity[i] + accel * dt;

			// �Փ˓_�̈ړ��������v�Z
			Vec3d dx = info->solid_velocity[i] * dt;

			// �Փ˓_���X�V
			info->col_positions[i] = info->col_positions[i] + dx;
			
			// ���̂̏d�S���X�V
			// ��]�v�Z��r�x�N�g���̂���
			info->solid_center_position[i] = info->solid_center_position[i] + dx;

			// �|�C���^�̈ړ��̂��߂ɉ��z�ǂ̈ړ��ʂ�ۑ����Ă���
			pointer_dx += dx;

			// �p���x���X�V
			info->solid_angular_velocity[i] = info->solid_angular_velocity[i] + ang_accel * dt;

			// �ړ��ʂ��v�Z theta = (w * dt) x r
			Vec3d dth = (info->solid_angular_velocity[i] * dt) ^ (info->col_positions[i] - info->solid_center_position[i]);

			// �x�N�g���̉��Z�ɂ��@���̍X�V
			// �Ǐ��I�ȓ��͊w�v�Z���U���R�x�ōs���̂ł���ΕK�v
			info->col_normals[i] = (info->col_normals[i] + dth).unit();
		}

		// spidar�ɗ͂�������
		if(bforce)
		{
			// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
			Vec3d f = - view_haptic * VCForce;
			Vec3d t = - view_haptic * VCTorque;

			spidarG6.SetForce(f, t);	
		}
		else spidarG6.SetForce(Vec3d(), Vec3d());

		if(pointer_dx != Vec3d())
		{
			// ���z�ǂ̈ړ��ɉ����ă|�C���^��������ɓ������Ƃ��āA�ʒu���X�V����
			// �{�����x�ɂ���Đi�ރx�N�g���̕����Ɏˉe��
			// ���̎ˉe�����x�N�g���Ƒ��x���瓾����x�N�g���̃m�����̏������ق����Ƃ�
			Vec3d temp = info->pointer_vel * dt;
			pointer_dx = dot(pointer_dx, temp) * temp / temp.norm();

			// ���z�ǂ̈ړ��̕����傫������
			if(pointer_dx.norm() >= temp.norm())
			{
				// ���x�œ������ʂ����X�V
				info->pointer_pos += temp;
			}
			// ���x�̈ړ��̕����傫������
			else
			{
				info->pointer_pos += pointer_dx;
			}

			info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
			info->pointer_ori = info->pointer_ori.unit();
		}
		// ���͓̂����Ȃ������̂ňʒu�͍X�V���Ȃ�
		else
		{
			// ���x�͉^���ʂƗ͐ς̊֌W���󂯂��͂̕������キ�Ȃ�
			// �����瓱�o�ł���
			info->pointer_vel += info->pointer_massinv * VCForce * dt;
			info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
			info->pointer_ori = info->pointer_ori.unit();
		}
	}
	else
	{
		// �Փ˂��Ȃ��̂Œ񎦗͂����Z�b�g����
		spidarG6.SetForce(Vec3d());

		// soPointer�̈ʒu���X�V
		Vec3d dx = info->pointer_vel * dt;
		info->pointer_pos = info->pointer_pos + dx;

		info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
		info->pointer_ori = info->pointer_ori.unit();
	}

/*
	if(info->num_collisions > 0)
	{
		// �|�C���^�ɉ�����́E�g���N���i�[����ϐ�
		Vec3d pointer_force = Vec3d();
		Vec3d pointer_torque = Vec3d();

		// �ڐG�ɂ���ē������ʂ̑��a���i�[����ϐ�
		Vec3d pointer_dx = Vec3d();

		// �|�C���^�ɐ��������ׂĂ̐ڐG�ɂ��Čv�Z
		for(int i = 0; i < info->num_collisions; i++)
		{
			// �񎦗͂���ђ񎦃g���N�̌v�Z
			// �Փ˓_�ł̖@���̋t�����ɃJ�b�v�����O�͂��ˉe
			// normal�͒P�ʃx�N�g���O��
			Vec3d feedback_force = - FORCE_COEFF * dot(VCforce, info->col_normals[i]) * info->col_normals[i];
			Vec3d feedback_torque = (info->col_positions[i] - info->pointer_center) ^ feedback_force;

			// �����ŉ����x�̌v�Z���s��
			// �����x�̌v�Z�ɂǂ�����g�����͂��̂Ƃ��̃��[�h�ɂ���ĕς��
			// IsDynamical == false�ȍ��͈̂ړ����Ȃ��̂ŉ����x�͂Ȃ�
			if(info->nearest_solids[i]->IsDynamical() == true)
			{
				Vec3d accel;
				Vec3d ang_accel;

				// �ՓˑΏۂ̃g���N���v�Z
				Vec3d solid_torque_vector = (info->col_positions[i] - info->solid_center_position[i]) ^ (-feedback_force);

				// ���͂̉e�����l�����Ȃ��Ǐ��I���͊w�v�Z
				// �|�C���^�ɐڂ��鍄�̂̂݋Ǐ��I�ɍ����ōX�V
				if(bSurroundEffect == false)
				{
					// ����AddForce�������Correction����K�v���Ȃ�
					// �V�~�����[�V�����ł͎��͂̏󋵂��l�������X�V���s���邪�A
					// HapticRendering�ł̍����ȉ��z�ǂ̍X�V�ɂ͉����x�����f����Ȃ�
					// ����Ƀ��[�U�����ۂɗ͊o��������̂͂��̏󋵂ł��邽��
					// �d�v�ł���Ǝv��
					info->nearest_solids[i]->AddForce(-feedback_force, solid_torque_vector);
					accel = info->solid_massinv[i] * (-feedback_force);
					ang_accel = info->solid_inertiainv[i] * (solid_torque_vector);
				}
				// ���͂̉e�����l������
				// �|�C���^���ڐG���Ă��鉼�z�ǂ̉����x��
				// �V�~�����[�V�������瓾��ꂽ�l�����Ɍv�Z�����
				else
				{
					// �����x���v�Z
					accel = info->vel_effect[i] * (-feedback_force) + info->vel_constant[i];

					// �p�����x���v�Z ang_effect���g������solid_torque_vector���^����g���N���킩���Ă���̂ł�����g�����ق�����萳�m�Ȃ͂�
//					Vec3d ang_accel = info->ang_effect[i] * (-feedback_force) + info->ang_constant[i];
					ang_accel = info->solid_inertiainv[i] * (solid_torque_vector) + info->ang_constant[i];
				}

				// ���x���v�Z���A�Փ˓_���X�V����
				info->solid_velocity[i] = info->solid_velocity[i] + accel * dt;

				// �Փ˓_�̈ړ��������v�Z
				Vec3d dx = info->solid_velocity[i] * dt;

				// �Փ˓_���X�V
				info->col_positions[i] = info->col_positions[i] + dx;
				
				// ���̂̏d�S���X�V
				// ��]�v�Z��r�x�N�g���̂���
				info->solid_center_position[i] = info->solid_center_position[i] + dx;

				// �|�C���^�̈ړ��̂��߂ɉ��z�ǂ̈ړ��ʂ�ۑ����Ă���
				pointer_dx += dx;

				// �p���x���X�V
				info->solid_angular_velocity[i] = info->solid_angular_velocity[i] + ang_accel * dt;

				// �ړ��ʂ��v�Z theta = (w * dt) x r
				Vec3d dth = (info->solid_angular_velocity[i] * dt) ^ (info->col_positions[i] - info->solid_center_position[i]);

				// �x�N�g���̉��Z�ɂ��@���̍X�V
				info->col_normals[i] = (info->col_normals[i] + dth).unit();
			}
	
#if 0
			// �񎦗͂��傫������ꍇ�񎦗͂����炷
			if(fabs(feedback_force.x) > 300 || fabs(feedback_force.y) > 300 || fabs(feedback_force.z) > 300)
			{
				cout << "********" << endl;
				cout << i << ":force is " << feedback_force << endl;
				cout << "pointer_pos = " << info->pointer_pos << endl;
				cout << "dx = " << dx << endl;
				cout << "vel effect = " << info->vel_effect[i] << endl;
				cout << "vel const = " << info->vel_constant[i] << endl;
				cout << "accel = " << accel << endl;
				cout << "VCforce = " << VCforce << endl;
				cout << "goal = " << goal << endl;
				cout << "relative vel = " << PointerVel - info->pointer_vel << endl;
				cout << "normal = " << info->col_normals[i] << endl;
				cout << "dth = " << dth << endl;
				cout << "ang accel = " << ang_accel << endl;
				cout << endl;
				feedback_force /= 300;
			}
#endif
			
			// �񎦗͂�O�̒l�ɒǉ�
			pointer_force = pointer_force + feedback_force;
			pointer_torque = pointer_torque + feedback_torque;
		}

		// spidar�ɗ͂�������
		if(bforce)
		{
			// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
			Vec3d f = view_haptic * pointer_force;
			Vec3d t = view_haptic * pointer_torque;

#if 0
			// �񎦗͂��傫������ꍇ�͏���������
			if(fabs(f.x) > 300 || fabs(f.y) > 300 || fabs(f.z) > 300)
			{
				cout << "force  is " << f;
				f /= 300;
				cout << "; changed to " << f << endl;
				cout << "torque is " << t;
				t /= 300;
				cout << "; changed to " << t << endl;
			}
#endif
			spidarG6.SetForce(f,- VCTorque + t);	
		}
		else spidarG6.SetForce(Vec3d(), Vec3d());

		if(pointer_dx != Vec3d())
		{
			// ���z�ǂ̈ړ��ɉ����ă|�C���^��������ɓ������Ƃ��āA�ʒu���X�V����
			// �{�����x�ɂ���Đi�ރx�N�g���̕����Ɏˉe��
			// ���̎ˉe�����x�N�g���Ƒ��x���瓾����x�N�g���̃m�����̏������ق����Ƃ�
			Vec3d temp = info->pointer_vel * dt;
			pointer_dx = dot(pointer_dx, temp) * temp / temp.norm();

			// ���z�ǂ̈ړ��̕����傫������
			if(pointer_dx.norm() >= temp.norm())
			{
				// ���x�œ������ʂ����X�V
				info->pointer_pos += temp;
				info->pointer_center += temp;

				info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
				info->pointer_ori = info->pointer_ori.unit();
			}
		}
		// ���x�̈ړ��̕����傫������
		else
		{
			// ���z�ǂœ������������X�V
			info->pointer_pos += pointer_dx;
			info->pointer_center += pointer_dx;
		
			// ���x�͉^���ʂƗ͐ς̊֌W���󂯂��͂̕������キ�Ȃ�
			// �����瓱�o�ł���
			info->pointer_vel += info->pointer_massinv * pointer_force * dt;

			info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
			info->pointer_ori = info->pointer_ori.unit();
		}
	}
	else
	{
		// �Փ˂��Ȃ��̂Œ񎦗͂����Z�b�g����
		spidarG6.SetForce(Vec3d());

		// soPointer�̈ʒu���X�V
		Vec3d dx = info->pointer_vel * dt;
		info->pointer_pos = info->pointer_pos + dx;
		info->pointer_center += info->pointer_pos + dx;

		info->pointer_ori = Quaterniond::Rot(info->pointer_angvel * dt) * info->pointer_ori;
		info->pointer_ori = info->pointer_ori.unit();
	}

#if 0
	static int sec_counter = 0;
	// ��b���Ƃ�SPIDAR�̍��W��\������
	if(sec_counter % HAPTIC_FREQ == 0)
	{	
//		cout << "pointer = " << info->pointer_pos << endl;
		cout << "num collisions = " << info->num_collisions << endl;
		sec_counter = 0;	
	}
	sec_counter++;									// �J�E���^�[�̍X�V
#endif
*/
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
		Quaterniond ori_error = Quaterniond::Rot(info->col_normals[i]) * Quaterniond::Rot(info->original_col_normals[i]).Inv();

		// �덷�̕����s�ړ�����
		info->nearest_solids[i]->SetFramePosition(info->nearest_solids[i]->GetFramePosition() + pos_error);
		info->nearest_solids[i]->SetOrientation(info->nearest_solids[i]->GetOrientation() * ori_error);

		info->nearest_solids[i]->SetCenterPosition(info->solid_center_position[i]);

		info->nearest_solids[i]->SetVelocity(info->solid_velocity[i]);
		info->nearest_solids[i]->SetAngularVelocity(info->solid_angular_velocity[i]);
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
	HapticInfo* info = NULL;

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

	soPointer->SetOrientation(info->pointer_ori);
	soPointer->SetAngularVelocity(info->pointer_angvel);
}

void CALLBACK StepSimulation(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ������덷���C������
//	ErrorCorrection();

	// �V�~�����[�V�������P�X�e�b�v�i�߂�
	scene->Step();

	// �Փ˓_����\��
//	show_collision_info();

	// �Փ˓_������͂̉e�����v�Z
	// �P���Q�ƒ��B�Q���X�V
	if(current_valid_data == true)
	{
		calculate_surround_effect(&info2);
	}
	// �Q���Q�ƒ��B�P���X�V
	else
	{
		calculate_surround_effect(&info1);
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
		timeKillEvent(FTimerId1);
		timeEndPeriod(1);
		cout << "exit operation from key input" << endl;
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
		cout << "force ";
		if(bforce) cout << "on";
		else cout << "off";
		cout << endl;
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
	/*
	else if(key == 's')
	{
		bSurroundEffect = !bSurroundEffect;

		// ���b�Z�[�W���o��
		cout << "surrounding effect ";
		if(bSurroundEffect)cout << "on";
		else cout << "off";
		cout << endl;
	}
	*/
	else if(key == 'p')
	{
		soPointer->SetAngularVelocity(Vec3d());
	}
	// ���݂̃o�[�`�����J�b�v�����O��K��B�̒l��\������
	else if(key == 'l')
	{
		cout << "Km = " << Km << " Bm = " << Bm << endl;
		cout << "Kr = " << Kr << " Br = " << Br << endl;
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'k')
	{
		if(bMode)Km += 1;
		else Kr += 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'b')
	{
		if(bMode)Bm += 1;
		else Br += 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����K��1�������Č��݂̏�Ԃ�\������
	else if(key == 'j')
	{
		if(bMode)Km -= 1;
		else Kr -= 1;
		keyboard('l', 0, 0);
	}
	// �o�[�`�����J�b�v�����O�̌W����B��1�������Č��݂̏�Ԃ�\������
	else if(key == 'v')
	{
		if(bMode)Bm -= 1;
		else Br -= 1;
		keyboard('l', 0, 0);
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
		cout << "pointer is ";
		if(!bDisplayPointer)cout << "not ";
		cout << "displayed" << endl;
	}
	else if(key == 'e')
	{
		bMode = !bMode;
		if(bMode)cout << "movement ";
		else cout << "rotation ";
		cout << "edit mode" << endl;
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

	DWORD time = 0;

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
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_SPHERES; ++sphereCnt){
		soSphere.at(sphereCnt)->AddShape(floor);
		soSphere.at(sphereCnt)->SetFramePosition(Vec3f(0, 15+5*sphereCnt, 0));
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
