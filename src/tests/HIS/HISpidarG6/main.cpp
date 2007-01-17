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
#include "Physics/PHForceField.h"
#include "Physics/PHConstraint.h"
#include "Physics/PHContactPoint.h"
#include <Physics/PHSpatial.h>

#include "Collision/CDDetectorImp.h"
#include "Collision/CDConvex.h"

#include <mmsystem.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

using namespace Spr;

typedef PTM::TMatrixCol<50, 6, double> MatrixN6d;
typedef PTM::TMatrixCol<6, 50, double> Matrix6Nd;
typedef PTM::TVector<50, double> VecNd;

#define ESC				27			// ESC key
//#define EXIT_TIMER		10000		// ���s�X�e�b�v��
#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define NUM_OBJECTS		2			// object��
#define SPIDAR_SCALE	70			// SPIDAR��VE���ł̓���X�P�[��

#define POINTER_RADIUS 0.5f			// �|�C���^�̔��a
#define EPSILON 3.0					// �|�C���^�ɐڐG�������ȍ��̂�\�����邽�߂Ƀ|�C���^��BBox��c��܂��ĐڐG���������Ƃ��̖c��܂���{��
									// ���Ƃ��Ă�d = (EPSILON - 1) x POINTER_RADIUS������̍��̂��������ĐڐG���_�����
									// �傫������قǂ�������̐ڐG��\�z�ł��邪�A���̕��ʂ�������̂Ōv�Z���d���Ȃ�
									// 1�ɂ���Ɨ\���Ȃ�

#define LIMIT_DEPTH 100				// �\���V�~�����[�V�������s�����̎擾�̐[�����
#define NUM_PREDICT_ITERATE 15		// �\���V�~�����[�V�����̃C�e���[�g��

#define NUM_COLLISIONS 50			// �|�C���^�ւ̋��e�ڐG��
#define NUM_COL_SOLIDS 50			// �|�C���^�ւ̋��e�ڐG���̐��@
									// NUM_COLLISIONS�Ƌ�ʂ���̂̓v���O������ǂ݂₷�����邽�߁B�����I�ȑ��݈Ӌ`�͂Ȃ�

#define COLLISION_DENSITY 0.5		// �͊o�̌v�Z�Ɏg�p����ڐG�_�̕��z�ʂ𒲐߂���l
									// �ڐG��O�񂩂�����p�����ꍇ�ڐG�_�͂��̒l���傫�������ŕ��z����

#define DISTANCE_LIMIT    1.0		// �ڐG�_�����ꂽ�Ƃ��ɐڐG���������邩�����肷�鋗��
#ifdef _DEBUG
	#define SIMULATION_FREQ	60		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		500		// �͊o�X���b�h�̎���Hz
	float Km = 13;					// virtual coupling�̌W��
	float Bm = 0;					// ���i

	float Kr = 13;					// ��]
	float Br = 0;

#elif _WINDOWS
	#define SIMULATION_FREQ	60		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000	// �͊o�X���b�h�̎���Hz
	float Km = 200;					// virtual coupling�̌W��
	float Bm = 0;					// ���i

	float Kr = 1;					// ��]
	float Br = 0;

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

// SPIDAR�p�̕ϐ�
DVDeviceManager devMan;						//	D/A��J�E���^�Ȃǂ̃f�o�C�X�̊Ǘ�
HISpidarG6X3 spidarG6;						//	SPIDAR�ɑΉ�����N���X

// ���̑��̕ϐ�
Vec3f spidar_pos = Vec3f();
Quaterniond spidar_ori = Quaterniond();
Matrix3f view_rot;
Matrix3f view_haptic;


// multimedia timer�̃n���h��
MMRESULT FTimerId1;


// �񎦗͂��o�����ǂ���
bool bforce = false;

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

// �ڐG�_��`�悷�邩�ǂ���
bool bDisplayCollision = true;

// �ڐG�\�z�_���g�����ǂ���
bool bPredictCollision = true;

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

	Vec3d solid_center_positions[NUM_COL_SOLIDS];
	Quaterniond solid_orientations[NUM_COL_SOLIDS];

	double solid_massinvs[NUM_COL_SOLIDS];
	Matrix3d solid_inertiainvs[NUM_COL_SOLIDS];

	// collision data
	int num_collisions;
	Vec3d col_positions[NUM_COLLISIONS];
	Vec3d col_normals[NUM_COLLISIONS];
	Vec3d pointer_col_positions[NUM_COLLISIONS];

	// ���͂̉e���̌��ʂ��i�[����ϐ�
	Matrix3d vel_effect[NUM_COL_SOLIDS][NUM_COLLISIONS];
	Vec3d vel_constant[NUM_COLLISIONS];

	Matrix3d ang_effect[NUM_COL_SOLIDS][NUM_COLLISIONS];
	Vec3d ang_constant[NUM_COLLISIONS];

	SpatialVector gravity_effect[NUM_COL_SOLIDS];

	// �d�͂��K������Ă��邩�ǂ���
	bool bGravity[NUM_COL_SOLIDS];

	// ���ۂɐڐG���Ă��邩����킷�t���O
	// MakeHapticInfo�őO��ڐG���Ă����ꍇ�A�̔��f�Ɏg��
	bool bCollide[NUM_COLLISIONS];

	// ���[���h���W�n���獄�̍��W�n�ɒ����S����
	// PHConstraint::Xj.Conjugated()�Ɠ���
	Quaterniond wldTosld[NUM_COLLISIONS];

	Vec3d pointer_pos[NUM_COLLISIONS];
	Quaterniond pointer_ori[NUM_COLLISIONS];
} HapticInfo;

// �ڐG���_�̏����i�[����\����
typedef struct{
	int pointer_index;
	PHSolid* solid[2];
	Vec3d closestPoint[2];
	Vec3d normal;
	Spr::PHShapePairForLCP* shapePair;
} CandidateInfo;

struct Edge{
		float edge;				///<	�[�̈ʒu(�O���[�o���n)
		int	index;				///<	���� solid�̈ʒu
		bool bMin;				///<	���[�Ȃ�true
		bool operator < (const Edge& s) const { return edge < s.edge; }
	};
typedef std::vector<Edge> Edges;

// pointer data
Vec3d pointer_pos;
Quaterniond pointer_ori;
Vec3d pointer_vel;
Vec3d pointer_angvel;

// ���͂̉e�����v�Z����Ƃ��Ɏg���ϐ�
// ���͂̉e�����v�Z����Ƃ���
// ���̑O��HapticRendering�ŉ������͂̕��ς��g���ċ��߂�
Vec3d VCForce_sum = Vec3d();
Vec3d TestForce = Vec3d(1, 1, 1);
Vec3d MoveVector = Vec3d(1, 1, 1);

// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
HapticInfo info1;
HapticInfo info2;

// true�̂Ƃ���info�P���Q��
// false�̂Ƃ���info�Q���Q��
bool current_valid_data = true; 

// �֐��̃v���g�^�C�v�錾

//////////////////////////////////////////////////////////////////////////////
// ���������쐬���鏈��
void CalcSurroundEffect(HapticInfo*, HapticInfo*);

// �e���̂��鍄�̂̏W�����擾���Ă��鏈��
void GetSolidsCollisionsOnPointer(vector<pair<PHConstraint *, int> >*, vector<pair<PHConstraint *, int> >*,  
						  multimap<double, PHContactPoint*> ,
						  vector<PHConstraint *>*, set<PHSolid *>*, set<PHSolid *>*);
void GetAllRelativeSolidsCollisions(vector<PHConstraint*>* relative_consts, set<PHSolid*>*, set<PHSolid*>);
void RecursiveSolidsCollisionsRetrieval(vector<pair<PHConstraint *, bool> >*, PHSolid*, vector<PHConstraint *>*, set<PHSolid *>*, int);

// �ڐG���_���쐬���鏈��
void CreateCandidateContactPoints(vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >,
								  vector<pair<PHConstraint*, int> >*, vector<pair<PHConstraint*, int> >*,
								  vector<PHConstraint *>*, set<PHSolid*>*, set<PHSolid*>*,
								  map<PHContactPoint*, Vec3d>*, HapticInfo*);
void GetCandidateSolidsByBBox(set<int> *, int *);
void GetWillCollidePoints(int, int, vector<CandidateInfo>*);
void CreateShapePairMMap(vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >, multimap<PHSolid*, Spr::PHShapePairForLCP*>*);

// �\���V�~�����[�V���������鏈��
void PredictSimulations(vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >,
						set<PHSolid*>,
						vector<PHConstraint*>, set<PHSolid*>,
						map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >*, map<PHSolid*, SpatialVector>*, map<PHSolid*, SpatialVector>*);
void SetupPredictSimulation(vector<PHConstraint*>, set<PHSolid*>, set<PHSolid*>, map<PHSolid*, Vec3d>*);
void AddForceToConstraint(int, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >, map<PHSolid *, Vec3d>, Vec3d);
vector<SpatialVector> PredictSimulation(vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >,
										set<PHSolid*>,
										vector<PHConstraint *>, set<PHSolid*>);
vector<SpatialVector> PredictSimulationToCollision(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
										set<PHSolid *> nearest_solids,
										vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, 
										Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index = -1);
SpatialVector PredictSimulationForGravity(vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >, 
								  set<PHSolid*>, vector<PHConstraint*>, set<PHSolid*>, PHSolid*, SpatialVector, int);

// �ڐG�_�������p������
void CreateConstraintFromCurrentInfo(HapticInfo*, vector<pair<PHConstraint *, int> >*, vector<pair<PHConstraint *, int> >*, map<PHConstraint*, int>*,
									 set<PHSolid*>*, vector<PHConstraint *>*, set<PHSolid *>*nearest_solids);

// ����ꂽ�����������ׂĂ܂Ƃ߂鏈��
void MakeHapticInfo(HapticInfo*, HapticInfo*, vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >,
					vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >, map<PHContactPoint*, Vec3d>,
					vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >,
					map<PHConstraint*, int>, map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >, map<PHSolid*, SpatialVector>, map<PHSolid*, SpatialVector>, bool);
PHConstraint* GetSpecificDataFromCollisions(int i, 
					 vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >, 
					 vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >, 
					 map<PHContactPoint*, Vec3d>,
					 vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >, 
					 int*, PHSolid**, Vec3d*, bool*, bool*);
void RegisterNewSolid(HapticInfo*, int, HapticInfo*, PHSolid*, map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >, map<PHSolid*, SpatialVector>, map<PHSolid*, SpatialVector>);
void RegisterNewCollision(HapticInfo*, int i, HapticInfo*, PHConstraint*, int, bool, bool, map<PHConstraint*, int>, Vec3d, bool);


////////////////////////////////////////////////////////////////////////////////////
// �͊o�v�Z�ł̏���
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

// �f�[�^��؂�ւ��鎞�ɍs������
void UpdateNewInfoFromCurrent();

// �߂荞�݂�SPIDAR�̈ʒu����|�C���^�̈ʒu���v�Z���鏈��
void CheckPenetrations(HapticInfo*, int*, vector<Vec3d>*, vector<double>*, vector<Vec3d>*);
void CalcTTheta(HapticInfo*, Vec3d*, Vec3d*);
void CorrectPenetration(HapticInfo*);
void GetParametersFromCollision(double*, double*, double*, double*, double*, double*, double*);
void MakePointerPosMatrix(int, vector<Vec3d>t, vector<double>, vector<Vec3d>, MatrixN6d*, VecNd*);

// �Ǐ��I�ȓ��͊w�v�Z���s������
void UpdateVelocityByCollision(HapticInfo*, Vec3d, bool*);
void UpdateSolidByCollision(HapticInfo *, Vec3d, bool*);
void UpdateSolids(HapticInfo*);

// �����ȃo�[�`�����J�b�v�����O�i�������j
void VirtualCoupling(HapticInfo*, Vec3d*, Vec3d*, bool*);


////////////////////////////////////////////////////////////////////////////////////
// �����I�ȏ���
void ErrorCorrection();
void UpdatePointer();
void StepSimulation();


////////////////////////////////////////////////////////////////////////////////////
// �{���I�łȂ�����
void idle();
void display();
void setLight();
void reshape(int w, int h);
void keyboard(unsigned char key, int x, int y);
void InitScene();
void InitRendering(int*, char**);
void InitPointer(PHSolidIf*);
void SaveCollisionF(vector<PHConstraint *>);
void SaveSolidDv(set<PHSolid*>);
void SaveSolidV(set<PHSolid*>);
void SaveSolidF(set<PHSolid*>);
void RestoreParameters(vector<PHConstraint*>*, set<PHSolid*>*);


////////////////////////////////////////////////////////////////////////////////////
// ���[�e�B���e�B
void ShowCollisionInfo();
inline void t_start();
inline void t_end();
void CalcPinv(MatrixN6d, Matrix6Nd*);
void UpdateSpidarData(Vec3d*, Quaterniond*);
void DisplayPointer();
void DisplayCollisions();
PHSolid* GetAdjacentSolid(PHConstraint*, PHSolid*, int* sign = NULL);
PHContactPoint* CreateContactPoint(Vec3d, Vec3d, PHSolid*, PHSolid*, Spr::PHShapePairForLCP*);
void ResetOriginalContactPoints(multimap<double, PHContactPoint*>*);
void AddInactiveSolid(PHSolidIf*);
void InitDeviceManager();
void DisplaySolidCenter();
inline double calcDistance(Vec3d, Vec3d);


///////////////////////////////////////////////////////////////////////////////////////////////
// ���͂̉e�����v�Z���邽�߂̏���
inline void SaveParameters(vector<PHConstraint*> relative_consts, set<PHSolid*> relative_solids)
{
	SaveCollisionF(relative_consts);
	SaveSolidDv(relative_solids);
	SaveSolidV(relative_solids);
	SaveSolidF(relative_solids);
}

// ���͂̉e�����v�Z���A�����̕ϐ��Ɋi�[����֐�
void CalcSurroundEffect(HapticInfo* new_info, HapticInfo* current_info)
{
	// �|�C���^�Ɋւ���ڐG���i�[����ϐ�
	vector<pair<PHConstraint *, int> > pointer_consts;
	vector<pair<PHConstraint *, int> > pointer_static_consts;

	// �ڐG�\���p�̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > col_candidate_consts;
	vector<pair<PHConstraint *, int> > col_candidate_static_consts;
	map<PHContactPoint*, Vec3d> col_candidate_pointer_pos;

	// �O��̐ڐG�������p�����߂̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > current_consts;
	vector<pair<PHConstraint *, int> > current_static_consts;

	// �V����������ڐG�ƌ��݂̐ڐG���֘A�t����}�b�v
	map<PHConstraint *, int> NewcolToCol;

	set<PHSolid *> relative_solids;
	vector<PHConstraint *> relative_consts;
	set<PHSolid *> nearest_solids;

	// ���͂̉e�����i�[����ϐ�
	map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices;
	map<PHSolid *, SpatialVector> vecs;
	map<PHSolid *, SpatialVector> g_map;

	// �f�[�^����̓r���Ńt���O���ύX�����ƍ���̂�
	// ���̎��_�ł̃t���O�̒l��ۑ����Ă�����
	// ���̊֐����ł͂����M���邱�Ƃɂ���
	bool bLocalDynamics_local = bLocalDynamics;
	bool bPredictCollision_local = bPredictCollision;

	// ����̐ڐG���폜����
	ResetOriginalContactPoints(&(current_info->points));

	// ���ݎg���Ă���͊o���̐ڐG�̂����ŁA�����g�������ȐڐG�����o���֐�
	CreateConstraintFromCurrentInfo(current_info, &current_consts, &current_static_consts, 
									&NewcolToCol,
									&relative_solids, &relative_consts, &nearest_solids);

	// �|�C���^�ɐڐG���Ă���ڐG�ƍ��̂��擾���Ă���֐�
	GetSolidsCollisionsOnPointer(&pointer_consts, &pointer_static_consts, 
						current_info->points,
						&relative_consts, &relative_solids, &nearest_solids);

	if(bLocalDynamics_local)
	{
		// �ڐG���_��T���č쐬���鏈��
		if(bPredictCollision_local) CreateCandidateContactPoints(pointer_consts, pointer_static_consts, 
																&col_candidate_consts, &col_candidate_static_consts,
																&relative_consts, &relative_solids, &nearest_solids, 
																&col_candidate_pointer_pos, current_info);

		// ���͂̉e�����v�Z���邽�߂̃f�[�^�쐬����
		// �֌W�̂��鍄�̂ƐڐG���ċA�I�ɂ��ׂĂƂ��Ă���
		GetAllRelativeSolidsCollisions(&relative_consts, &relative_solids, nearest_solids);

		// �͂������Ă݂ē������A�e�����ϑ�����֐�
		PredictSimulations(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
							relative_consts, relative_solids,
							&matrices, &vecs, &g_map);
	}

	// �ڐG�̏����쐬����
	MakeHapticInfo(new_info, current_info, 
					pointer_consts, pointer_static_consts, 
					col_candidate_consts, col_candidate_static_consts, col_candidate_pointer_pos,
					current_consts, current_static_consts, 
					NewcolToCol, matrices, vecs, g_map, bLocalDynamics_local);
}

// �|�C���^����X�^�[�g���ă|�C���^���܂ސڐG�Ƃ��̍��̂����ׂĎ擾���Ă���֐�
void GetSolidsCollisionsOnPointer(vector<pair<PHConstraint *, int> >* pointer_consts, vector<pair<PHConstraint *, int> >* pointer_static_consts,  
						  multimap<double, PHContactPoint*> points,
						  vector<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, set<PHSolid *>* nearest_solids)
{
	// �ڐG�_���擾
	PHConstraints cs = scene->GetConstraintEngine()->GetContactPoints();

	// ���ׂĂ̏Փ˓_�ɂ��Ē���
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); ++it)
	{
		int sign = 1;

		// �|�C���^���܂ސڐG���擾
		PHSolid* col_solid = GetAdjacentSolid(*it, DCAST(PHSolid, soPointer), &sign);
			
		if(col_solid)
		{
			relative_solids->insert(col_solid);
			nearest_solids->insert(col_solid);

			PHContactPoint* contact = DCAST(PHContactPoint, *it);
			bool bAdd = true;
				
			// �����p���ڐG�ƐV�������ڐG�̈ʒu���߂�����ꍇ��
			// �V�����ڐG�͒ǉ����Ȃ�
			// ���łɈ����p���ڐG�����̑���Ƃ��čl������̂ŁA
			// ���̏����̕����͍l����K�v�͂Ȃ�
			for(multimap<double, PHContactPoint*>::iterator it2 = points.lower_bound(contact->pos.z - COLLISION_DENSITY); it2 != points.upper_bound(contact->pos.z + COLLISION_DENSITY); it2++)
			{
				double norm = (contact->pos - (*it2).second->pos).norm();

				// ���Ȃ�shapePair�������p���̐ڐG�_�̈�ƁA�ǉ����悤�Ƃ����̐ڐG�_�̋����i�x�N�g���̃m�����j��
				// ����������ꍇ�͒ǉ����Ȃ�
				if(norm < COLLISION_DENSITY && contact->shapePair == (*it2).second->shapePair)
				{
					bAdd = false;
					break;
				}
			}

			// ���������鍄�̂ł���΁A�ڐG���̂��i�[
			// �����Ȃ����͕̂ʂȃx�N�g���ɒǉ�
			// �����闝�R�͌�ōs���V�~�����[�V�����ŁA�����Ȃ����̂ƃ|�C���^�̐ڐG���܂߂�͈̂Ӗ����Ȃ�����
			if(bAdd)
			{
				if(col_solid->IsDynamical()) pointer_consts->push_back(pair<PHConstraint *, int>(*it, sign));
				else pointer_static_consts->push_back(pair<PHConstraint *, int>(*it, sign));
				relative_consts->push_back(*it);
			}
		}
	}
}

void GetAllRelativeSolidsCollisions(vector<PHConstraint*> *relative_consts, set<PHSolid*>* relative_solids, set<PHSolid*> nearest_solids)
{
	// �ڐG�_���擾
	PHConstraints cs = scene->GetConstraintEngine()->GetContactPoints();
	vector<pair<PHConstraint *, bool> > csm;

	// �v�Z���Ԃ�Z�k���邽�߂ɐڐG�ɏ����σt���O�����ĕۑ�����
	// �ڐG�������Ȃ��ꍇ�͖��ʂ��������A
	// �ڐG���������Ȃ����Ƃ��̐L�т͂�����̂ق����f�R���Ȃ��͂�
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); ++it)
	{
		csm.push_back(pair<PHConstraint *, bool>(*it, false));
	}

	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		// ���̍��̂Ɛ������ڐG�����̍��̂ɉe�����y�ڂ��\��������ꍇ�́A���̍��̂ɂ�����
		if((*it)->IsDynamical())
			RecursiveSolidsCollisionsRetrieval(&csm, *it, relative_consts, relative_solids, 0);
	}
}


void RecursiveSolidsCollisionsRetrieval(vector<pair<PHConstraint *, bool> >* csm, PHSolid* solid, vector<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, int depth)
{
	for(vector<pair<PHConstraint *, bool> >::iterator it = csm->begin(); it != csm->end(); ++it)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if(it->second) continue;

		// �^����ꂽ���̂���A���̍��̂ɐڐG���鍄�̂��擾����
		PHSolid* col_solid = GetAdjacentSolid(it->first, solid);

		// �������̍��̂����݂����ꍇ
		if(col_solid && col_solid != DCAST(PHSolid, soPointer))
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
				RecursiveSolidsCollisionsRetrieval(csm, col_solid, relative_consts, relative_solids, depth++);
		}
	}
}

// �|�C���^�ɐڐG�������ȍ��̂�T���A�ڐG���Ă��Ȃ���ΐڐG���쐬����֐�
void CreateCandidateContactPoints(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > pointer_static_consts,
								  vector<pair<PHConstraint *, int> >* col_candidate_consts, vector<pair<PHConstraint *, int> >* col_candidate_static_consts,
								  vector<PHConstraint *> *relative_consts, set<PHSolid *> *relative_solids, set<PHSolid *>* nearest_solids, 
								  map<PHContactPoint*, Vec3d>* col_candidate_pointer_pos, HapticInfo* current_info) 
{
	// ���łɓ���ꂽ�ڐG�_����shapePair���쐬���鏈��
	// �ڐG���_�����łɑ��݂��邩�ǂ����̔���Ɏg��
	multimap<PHSolid*, Spr::PHShapePairForLCP*> shapePairMMap;
	CreateShapePairMMap(pointer_consts, pointer_static_consts, &shapePairMMap);

	// BBox���g���ă|�C���^�ƐڐG�������ȍ��̂��擾���Ă���
	int pointer_index;
	set<int> candidate_solid_indices;
	GetCandidateSolidsByBBox(&candidate_solid_indices, &pointer_index);

	// �ڐG�������ȍ��̂�shape���x���Ŗ{���ɐڐG���邩���ׂ�
	for(set<int>::iterator it = candidate_solid_indices.begin(); it != candidate_solid_indices.end(); ++it)
	{
		vector<CandidateInfo> candidates;
		
		// ���ۂɈʒu�֌W���v�Z���ĐڐG���������ǂ����𔻒�
		// �ڐG�������ȏ����擾���Ă���
		GetWillCollidePoints(*it, pointer_index, &candidates);

		// �ڐG�������ȏ��̂�����
		// ���łɐڐG�Ƃ��ď�������Ă��邩�ǂ����𔻒�
		// �܂���������Ă��Ȃ���ΐV�����ڐG���쐬����
		for(int i = 0; i < (int)candidates.size(); ++i)
		{
			bool bExist = false;
			CandidateInfo cinfo = candidates.at(i);

			pair<multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator, multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator> it = shapePairMMap.equal_range(cinfo.solid[1 - cinfo.pointer_index]);
			for(multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator it2 = it.first; it2 != it.second; ++it2)
			{
				// �ڐG�������ȍ��̂��L�[�Ƃ��Ă��łɓo�^���ꂽ�ڐG����������
				// ����������shapePair�����������
				// so && soPointer�@�ł����
				// shape && shape �ƂȂ�̂œ����ڐG�Ƃ݂Ȃ���
				if((*it2).second == cinfo.shapePair)
				{
					bExist = true;
					break;
				}
			}

			// �|�C���^�Ƃ̐ڐG���Ȃ����̂�
			// �ڐG�����鍄�̂�������shapePair�ł͂܂��ڐG������Ă��Ȃ�����
			if(!bExist)
			{
				// �ڐG�̍쐬
				PHContactPoint* point = CreateContactPoint(cinfo.normal, cinfo.closestPoint[1 - cinfo.pointer_index], cinfo.solid[0], cinfo.solid[1], cinfo.shapePair);
				
				// �֌W�̂���ڐG
				relative_consts->push_back(point);
				// �֌W�̂��鍄��
				relative_solids->insert(cinfo.solid[1 - cinfo.pointer_index]);
				// �߂�����
				nearest_solids->insert(cinfo.solid[1 - cinfo.pointer_index]);
				// �|�C���^���̐ڐG�_�ۑ�
				col_candidate_pointer_pos->insert(pair<PHContactPoint*, Vec3d>(point, cinfo.closestPoint[cinfo.pointer_index]));
				// �폜�p�Ƀ����N��ۑ�
				current_info->points.insert(pair<double, PHContactPoint*>(cinfo.closestPoint[1 - cinfo.pointer_index].z, point));

				int sign;
				GetAdjacentSolid(point, DCAST(PHSolid, soPointer), &sign);
				
				// �ڐG�̕ۑ�
				if(cinfo.solid[1 - cinfo.pointer_index]->IsDynamical())
				{
					col_candidate_consts->push_back(pair<PHConstraint*, int>(point, sign));
				}
				else
				{
					col_candidate_static_consts->push_back(pair<PHConstraint*, int>(point, sign));
				}
			}
		}
	}
}

// �|�C���^�ɐڐG�������ȍ��̂��擾����֐�
void GetCandidateSolidsByBBox(set<int> *candidate_solid_indices, int *pointer_index)
{
	// �����ȉ��̓|�C���^�ƍ��̂�BBox���g����
	// �����Ń\�[�g����
	// �ڐG���������
	// BBox���c��܂��Ă���̂�
	// �ʏ��葽�߂Ɏ擾�ł���͂�
	int N =	scene->NSolids();
	Vec3f dir(0, 0, 1);
	Edges edges;
	edges.resize(2 * N);

	PHSolids solids = scene->GetConstraintEngine()->solids;
	Edges::iterator eit = edges.begin();

	for(int i = 0; i < N; ++i){
		// �|�C���^�̓Y�������擾
		if(solids[i] == DCAST(PHSolid, soPointer)) *pointer_index = i;
		
		solids[i]->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = solids[i]->GetDeltaPosition();
		float dLen = (float) (dPos * dir);
		if (dLen < 0){
			eit[0].edge += dLen;
		}else{
			eit[1].edge += dLen;
		}
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	// �����W�Ń\�[�g
	std::sort(edges.begin(), edges.end());

	typedef std::set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool bCheck = false;
	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
			// �|�C���^�ȊO�̍��̂̎n�܂�
			if(it->index != *pointer_index) 
			{
				cur.insert(it->index);
				
				// �������ł�pointer���n�܂��Ă���΃`�F�b�N�ΏۂɂȂ�
				if(bCheck)
				{
					int f1 = *pointer_index;
					candidate_solid_indices->insert(it->index);
				}
			}
			// �|�C���^�̎n�܂�
			else
			{
				// ���������ɏo�Ă���Edge�͑S���`�F�b�N�Ώ�
				bCheck = true;
				int f1 = *pointer_index;

				// �o�^����Ă��鍄�̂̃y�A�����ׂă`�F�b�N
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					candidate_solid_indices->insert(*itf);
				}
			}
		}else{
			// �|�C���^�ȊO�̍��̂̏I���
			if(it->index != *pointer_index){
				cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
			}
			// �|�C���^��BBox�̏I���
			// �����܂łœ���ꂽ���̂�index���ڐG���
			else break;
		}
	}
}

// �|�C���^�ƍ��̂��ڐG���������ǂ����𔻒肷�鏈��
// �ڐG�������ȍ��̂�����ꍇ�͏����쐬���ăx�N�g���ɂ߂�
void GetWillCollidePoints(int so_index, int pointer_index, vector<CandidateInfo>* candidates)
{
	Vec3d normal;
	Vec3d closestPoint[2];
	double distance;
	PHSolid* solid[2];

	// solidpair�����鏈��
	// index_small < index_large
	// �Ȃ̂ŁA�����Ŕ��肵��solid��p��
	if(so_index > pointer_index)
	{
		solid[0] = DCAST(PHSolid, soPointer);
		solid[1] = scene->GetConstraintEngine()->solids[so_index];
	}
	else 
	{
		solid[0] = scene->GetConstraintEngine()->solids[so_index];
		solid[1] = DCAST(PHSolid, soPointer);
	}
	
	// �K�v�ȏ��̏���
	std::vector<Vec3d> deltaPos[2];
	std::vector<Posed> shapePose[2];
	for(int i = 0; i < 2; i++){
		deltaPos[i].resize(solid[i]->NShape());
		shapePose[i].resize(solid[i]->NShape());
		for(int j = 0; j < solid[i]->NShape(); j++){
			CDConvex* convex = DCAST(CDConvex, solid[i]->GetShape(j));
			Posed lp = solid[i]->GetShapePose(j);
			Vec3d c = lp * convex->GetCenter();
			if(solid[i] == DCAST(PHSolid, soPointer)) deltaPos[i][j] = (pointer_vel * scene->GetTimeStep()) + (pointer_angvel ^ (c - solid[i]->GetCenterPosition())) * scene->GetTimeStep();
			else deltaPos[i][j] = solid[i]->GetDeltaPosition(c);
			shapePose[i][j] = solid[i]->GetPose() * lp;
			shapePose[i][j].Pos() -= deltaPos[i][j];
		}
	}

	for(int i = 0; i < solid[0]->NShape(); ++i)
	{
		for(int j = 0; j < solid[1]->NShape(); ++j)
		{
			// ���Έړ��ʒu
			Vec3d delta = deltaPos[0][i] - deltaPos[1][j];

			// �ڐG�\�z
			// �����𒲂ׂ�
			int res = ContFindCommonPoint(DCAST(CDConvex, solid[0]->GetShape(i)), DCAST(CDConvex, solid[1]->GetShape(j)), shapePose[0][i], shapePose[1][j], delta, normal, closestPoint[0], closestPoint[1], distance);

			// ���������ڐG����\��������
			if(res > 0)
			{
				// �����𒲂ׂ�
				double toi = distance / (delta * delta);
				if(toi >= 0 && toi <= POINTER_RADIUS * (EPSILON - 1))
				{
					// �ŋߖT�̓_�𒲂ׂ�
					FindClosestPoints(DCAST(CDConvex, solid[0]->GetShape(i)), DCAST(CDConvex, solid[1]->GetShape(j)),shapePose[0][i], shapePose[1][j], closestPoint[0], closestPoint[1]);

					// �ڐG���̏����쐬
					CandidateInfo cinfo;
					
					// �Y������ �i��,��j�ƂȂ��Ă���
					// ���̂��߂��̏ꍇ�͌�������
					if(so_index > pointer_index)
					{
						// pointer
						cinfo.closestPoint[0] = shapePose[1][j] * closestPoint[1];
						// so
						cinfo.closestPoint[1] = shapePose[0][i] * closestPoint[0];
						cinfo.shapePair = scene->GetConstraintEngine()->solidPairs.item(pointer_index, so_index)->shapePairs.item(i, j);
						cinfo.normal = cinfo.closestPoint[0] - cinfo.closestPoint[1]; // - normal;

						cinfo.solid[0] = DCAST(PHSolid, soPointer);
						cinfo.solid[1] = solid[1];
						cinfo.pointer_index = 0;
					}
					else
					{
						// so
						cinfo.closestPoint[0] = shapePose[0][i] * closestPoint[0];
						// pointer
						cinfo.closestPoint[1] = shapePose[1][j] * closestPoint[1];
						cinfo.shapePair = scene->GetConstraintEngine()->solidPairs.item(so_index, pointer_index)->shapePairs.item(i, j);
						cinfo.shapePair->depth = 0;

						// �@���̌v�Z
						cinfo.normal = cinfo.closestPoint[1] - cinfo.closestPoint[0]; // normal;
						cinfo.solid[0] = solid[0];
						cinfo.solid[1] = DCAST(PHSolid, soPointer);
						cinfo.pointer_index = 1;
					}
					// �ǉ�
					candidates->push_back(cinfo);
				}
			}
		}
	}
}

// �^����ꂽ��񂩂�shapePair�̃}�b�v���쐬����֐�
void CreateShapePairMMap(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > pointer_static_consts, multimap<PHSolid*, Spr::PHShapePairForLCP*>* shapePairMMap)
{
	// ���łɓ���ꂽ�|�C���^����̐ڐG����
	// �ڐG�Ώۂ̍��̂ƃ|�C���^�Ƃ�shapePair��������悤�ɂ���
	int size1 = (int)pointer_consts.size();
	int size2 = (int)pointer_static_consts.size();
	for(int i = 0; i < size1 + size2; ++i)
	{
		PHConstraint *consts;

		if(i >= size1)
		{
			consts = pointer_static_consts.at(i - size1).first;
		}
		else
		{
			consts = pointer_consts.at(i).first;			
		}
		shapePairMMap->insert(pair<PHSolid*, Spr::PHShapePairForLCP*>(GetAdjacentSolid(consts, DCAST(PHSolid, soPointer)), DCAST(PHContactPoint, consts)->shapePair));
	}
}

// ��Ď�@�̂��߂ɐ摗��V�~�����[�V�������v��I�ɍs���Ă����֐�
void PredictSimulations(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
						set<PHSolid *> nearest_solids,
						vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids,
						map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > *mat, map<PHSolid *, SpatialVector> *vec, map<PHSolid*, SpatialVector> *gvec)
{
	map<PHSolid *, Vec3d> coeff;
	
	// �K�v�ȏ��̏���
	SetupPredictSimulation(relative_consts, relative_solids, nearest_solids, &coeff);

	// ������͂��O�x�N�g���Ƃ��Ē萔�����擾
	vector<SpatialVector> b = PredictSimulationToCollision(pointer_consts, col_candidate_consts, current_consts,
												nearest_solids, 
												relative_consts, relative_solids, 
												Vec3d(), coeff);

	map<PHSolid*, SpatialVector> gravity_map;
	map<PHSolid*, SpatialVector> c;
	int local_counter = 0;

	// �K�؂Ƀf�[�^���i�[����map-vector�̏���
	// ���̐��������vector��錾����map�ɒǉ�����
	map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > > mm_map;
	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		mm_map.insert(pair<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >((*it), vector<pair<Matrix3d, Matrix3d> >()));

		// ���ł�vector����map�ւ̕ϊ��������s��
		c.insert(pair<PHSolid*, SpatialVector>((*it), SpatialVector((*it)->GetOrientation() * b[local_counter].v() , (*it)->GetOrientation() * b[local_counter].w())));

		// �d�͂̉e�����v�Z
		SpatialVector gravity_effect = PredictSimulationForGravity(pointer_consts, col_candidate_consts, current_consts, 
																					nearest_solids, relative_consts, relative_solids,
																					*it, b[local_counter], local_counter);
		gravity_map.insert(pair<PHSolid*, SpatialVector>((*it), gravity_effect));

		local_counter++;
	}

	int total_size = (int)pointer_consts.size() + (int)col_candidate_consts.size() + (int)current_consts.size();

	// �ڐG�����񐔂����J��Ԃ�
	for(int i = 0; i < total_size; ++i)
	{
		// ����P�ʃx�N�g���������Ă��̌��ʂ�����킷�x�N�g�����擾
		// �ȉ��̂R�̊֐���
		// ����ڐG�ɗ͂��������Ƃ���,���ׂĂ̍��̂̔������킩��
		vector<SpatialVector> vec_x = PredictSimulationToCollision(pointer_consts, col_candidate_consts, current_consts, nearest_solids,
														relative_consts, relative_solids, 
														Vec3d(TestForce.x, 0, 0), coeff, i);
		vector<SpatialVector> vec_y = PredictSimulationToCollision(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
														relative_consts, relative_solids, 
														Vec3d(TestForce.x, TestForce.y, 0), coeff, i);
		vector<SpatialVector> vec_z = PredictSimulationToCollision(pointer_consts, col_candidate_consts, current_consts, nearest_solids, 
														relative_consts, relative_solids, 
														Vec3d(TestForce.x, 0, TestForce.z), coeff, i);

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
			// �Ȃ��g���N�ɂ��Ă͋����Ɨ͂̊O�ς����������
			// ���a�̊O�ς��s�񉻂���Ă��łɔ��f����Ă���͂��ł���
			Matrix3d v = Matrix3d(so->GetOrientation() * ((sv_x.v() - sv.v()) / TestForce.x), so->GetOrientation() * ((sv_y.v() - sv_x.v()) / TestForce.y), so->GetOrientation() * ((sv_z.v() - sv_x.v()) / TestForce.z));
			Matrix3d w = Matrix3d(so->GetOrientation() * ((sv_x.w() - sv.w()) / TestForce.x), so->GetOrientation() * ((sv_y.w() - sv_x.w()) / TestForce.y), so->GetOrientation() * ((sv_z.w() - sv_x.w()) / TestForce.z));
			
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
	*gvec = gravity_map;
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

	// Setup���I��������_�ł̃p�����[�^��ۑ����Ă���
	// ���ɍs����PredictSimulation�ł�
	// RestoreParameters���ĂԂ��Ƃ�
	// ���̎��_�̃p�����[�^�ɃZ�b�g���Ȃ�����
	// �J��Ԃ��v�Z�ł���悤�ɂ���
	SaveParameters(relative_consts, relative_solids);

}

vector<SpatialVector> PredictSimulationToCollision(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
										set<PHSolid *> nearest_solids,
										vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, 
										Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index)
{
	// �|�C���^���ڂ��鍄�̂̂���index�Ԗڂ̐ڐG�ɒP�ʗ͂�������
	if(index != -1)
	{
		AddForceToConstraint(index, pointer_consts, col_candidate_consts, current_consts, coeff, force_vec);
	}
	return PredictSimulation(pointer_consts, col_candidate_consts, current_consts, nearest_solids, relative_consts, relative_solids);
}

SpatialVector PredictSimulationForGravity(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
								  set<PHSolid*> nearest_solids, vector<PHConstraint*> relative_consts, set<PHSolid*> relative_solids, PHSolid* solid, SpatialVector b, int index)
{
	PHSolids gravitySolids = scene->GetGravityEngine()->solids;

	// �d�͂��o�^����Ă��邩���ׂ�
	bool bGravity = false;
	for(int i = 0; i < (int)gravitySolids.size(); ++i)
	{
		if(gravitySolids[i] == solid)
		{
			bGravity = true;
			break;
		}
	}

	// �o�^����Ă���Ώd�͂̉e�����v�Z
	if(bGravity)
	{
		// �d�͂Ɋւ���\���V�~�����[�V����
		solid->dv.v() += solid->GetOrientation().Conjugated() * scene->GetGravity() * solid->GetMass() * scene->GetTimeStep();
		vector<SpatialVector> g = PredictSimulation(pointer_consts, col_candidate_consts, current_consts, nearest_solids, relative_consts, relative_solids);
		int local_counter2 = 0;
		// ���̃��[�v��set<PHSolid*>::iterator�̏��Ԃɂ܂��
		for(vector<SpatialVector>::iterator it2 = g.begin(); it2 != g.end(); ++it2)
		{
			// (*it)�̌��ʂ�����ꂽ��l���i�[����
			if(index == local_counter2)
			{
				return SpatialVector(solid->GetOrientation() * ((*it2) - b).v(), solid->GetOrientation() * ((*it2) - b).w());
			}
			local_counter2++;
		}
		return SpatialVector();
	}
	// �o�^����Ă��Ȃ���΋�̃x�N�g����ǉ�
	else 
	{
		return SpatialVector();
	}
}

// �摗��V�~�����[�V����������֐�
vector<SpatialVector> PredictSimulation(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
										set<PHSolid *> nearest_solids,
										vector<PHConstraint *> relative_consts, set<PHSolid *> relative_solids)
{
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

	// �㏑�����Ă��܂��ϐ��̉�
	RestoreParameters(&relative_consts, &relative_solids);

	// �e����return
	return effects;
}

// ����̐ڐG�Ɋ܂܂�鍄�̂ɗ͂�������֐�
void AddForceToConstraint(int index, vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, map<PHSolid *, Vec3d> coeff, Vec3d force_vec)
{
	PHSolid* si;

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
/*
	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
		
	if(sign > 0) si = DCAST(PHSolid, constraint->solid[0]);
	else si = DCAST(PHSolid, constraint->solid[1]);


	Vec3d m_accel = si->GetMassInv() * si->GetOrientation().Conjugated() * force_vec * dt;
	Vec3d r_accel = si->GetInertiaInv() * (si->GetOrientation().Conjugated() * ((contact->pos - si->GetCenterPosition()) ^ force_vec)) * dt;
*/

	// �S�����W�̗͂����̍��W�ɒ����ĉ�����
	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	Vec3d m_accel;
	Vec3d r_accel;

	if(sign > 0)
	{
		si = DCAST(PHSolid, constraint->solid[0]);
		// ���O�Ɍv�Z���Ă������l���Q��
		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);
		// force_vec �� (1, 0, 0)�Ȃ�
		// Xj�ōS�����W���獄�̍��W�ɕϊ�����
		m_accel = si->GetMassInv() * contact->Xj[0].q * force_vec * dt;
		r_accel = si->GetInertiaInv() * (contact->Xj[0].q * ((contact->pos - si->GetCenterPosition()) ^ force_vec)) * dt;
	}
	else
	{
		si = DCAST(PHSolid, constraint->solid[1]);
		// ���O�Ɍv�Z���Ă������l���Q��
		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);
		m_accel = si->GetMassInv() * contact->Xj[1].q * force_vec * dt;
		r_accel = si->GetInertiaInv() * (contact->Xj[1].q * ((contact->pos - si->GetCenterPosition()) ^ force_vec)) * dt;
	}

	// �����ŗ^����ꂽ�P�ʗ͂�������
	si->dv.v() += m_accel;
	si->dv.w() += r_accel;
}

void CreateConstraintFromCurrentInfo(HapticInfo* current_info, vector<pair<PHConstraint *, int> > *current_consts, vector<pair<PHConstraint *, int> > *current_static_consts, map<PHConstraint *, int>* NewcolToCol,
									set<PHSolid *> *relative_solids, vector<PHConstraint *> *relative_consts, set<PHSolid *> *nearest_solids)
{
	// ���ݎg�p���Ă���f�[�^����
	// ���ۂɐڐG���Ă������
	// �ڐG���Ă��Ȃ����̂𒲂�
	// ���̃f�[�^�����Ɏg�����ǂ������肷��
	for(int i = 0; i < current_info->num_collisions; i++)
	{
		// ���ۂɂ͐ڐG���Ă��Ȃ��ڐG
 		if(!current_info->bCollide[i]) continue;

		// ���ݎg�p���Ă���Contact Point(=constraint)�̂قƂ�ǂ�step���Ă΂�邽�тɍ폜�����̂ŁA
		// ����V����Contact Point�����K�v������
		// �����ł͌��݂̈ʒu�ƌ�����������Contact Point��V���ɍ쐬����
		// �����ō��ꂽContact Point�� hapticinfo->points �Ɋi�[����A�蓮�ŏ���
		PHContactPoint* point = CreateContactPoint(current_info->col_normals[i], (current_info->col_positions[i] + current_info->pointer_col_positions[i]) / 2, current_info->solid[i][0], current_info->solid[i][1], current_info->shapePair[i]);

		// ���݂̃|�C���^���̐ڐG�_���i�[����
		NewcolToCol->insert(pair<PHConstraint *, int>(point, i));

		PHSolid* so = GetAdjacentSolid(point, DCAST(PHSolid, soPointer));

		if(so->IsDynamical()) current_consts->push_back(pair<PHConstraint *, int>(point, current_info->sign[i]));
		else current_static_consts->push_back(pair<PHConstraint *, int>(point, current_info->sign[i]));

		// ��������ڐG�̓o�^
		relative_consts->push_back(point);
		relative_solids->insert(so);
		nearest_solids->insert(so);

		// ���̉�ō폜�Ɏg�����߂Ƀ����N��ۑ����Ă���
		current_info->points.insert(pair<double, PHContactPoint*>(point->pos.z, point));
	}
}

// �񎦗͂̌v�Z����ђ�Ď�@�ɕK�v�ȏ����W�߂č\���̂��쐬����
void MakeHapticInfo(HapticInfo *new_info, HapticInfo *current_info, 
					vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > pointer_static_consts, 
					vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > col_candidate_static_consts, map<PHContactPoint*, Vec3d> col_candidate_pointer_pos,
					vector<pair<PHConstraint *, int> > current_consts, vector<pair<PHConstraint *, int> >current_static_consts, 
					map<PHConstraint *, int> NewcolToCol, map<PHSolid *, vector<pair<Matrix3d, Matrix3d> > > matrices, map<PHSolid *, SpatialVector> vecs, map<PHSolid *, SpatialVector> g_map, bool bLocalDynamics_local)
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

	int num_collisions = (int)pointer_consts.size() + (int)pointer_static_consts.size() + (int)col_candidate_consts.size() + (int)col_candidate_static_consts.size() + (int)current_consts.size() + (int)current_static_consts.size();

	// �͊o�����_�����O�ɕK�v�ȃf�[�^�̍쐬
	for(int i = 0; i < num_collisions; ++i)
	{
		// ���e���𒴂���ꍇ�͑ł��؂�
		if(i > NUM_COLLISIONS) break;

		PHConstraint* constraint;
		int sign;
		PHSolid* so;
		
		// �|�C���^���̐ڐG�_�ƍ��̑��̐ڐG�_���������Ă���ꍇ��
		// �|�C���^���̐ڐG�_���i�[����ϐ�
		Vec3d pointer_col_position = Vec3d();

		bool bCollide = true;
		bool bPreviousCollide = false;

		// ���ׂĂ̐ڐG���獡��̏����ɕK�v�ȃf�[�^������Ă���֐�
		constraint = GetSpecificDataFromCollisions(i, 
									pointer_consts, pointer_static_consts, 
									col_candidate_consts, col_candidate_static_consts, col_candidate_pointer_pos, 
									current_consts, current_static_consts, 
									&sign, &so, &pointer_col_position, &bCollide, &bPreviousCollide);

		// ���̂̓o�^��,���̍��̂̎��͂���̉e���̓o�^�̏���
		RegisterNewSolid(new_info, i, current_info, so, matrices, vecs, g_map);
		
		// �ڐG�̓o�^��,���̑��t���O�̓o�^
		RegisterNewCollision(new_info, i, current_info, constraint, sign, bPreviousCollide, bCollide, NewcolToCol, pointer_col_position, bLocalDynamics_local);
	}

	new_info->num_collisions = num_collisions;

	// �ڐG�_����������ꍇ�͑ł��؂�
	if(new_info->num_collisions >= NUM_COLLISIONS) 
	{
		cout << "number of collisions is overfull" << endl << "you must change to acceptable numbers" << endl;
		new_info->num_collisions = NUM_COLLISIONS-1;
	}
}

// ���ׂĂ̐ڐG����i�Ԗڂ̃f�[�^������Ă���֐�
PHConstraint* GetSpecificDataFromCollisions(int i, 
					 vector<pair<PHConstraint*, int> > pointer_consts, vector<pair<PHConstraint*, int> > pointer_static_consts, 
					 vector<pair<PHConstraint*, int> > col_candidate_consts, vector<pair<PHConstraint*, int> > col_candidate_static_consts, 
					 map<PHContactPoint*, Vec3d> col_candidate_pointer_pos,
					 vector<pair<PHConstraint*, int> > current_consts, vector<pair<PHConstraint*, int> > current_static_consts, 
					 int* sign, PHSolid** so, Vec3d* pointer_col_position, bool* bCollide, bool* bPreviousCollide)
{
	PHConstraint* constraint;

	int pointer_consts_size = (int)pointer_consts.size();
	int pointer_static_consts_size = (int)pointer_static_consts.size();

	int col_candidate_consts_size = (int)col_candidate_consts.size();
	int col_candidate_static_consts_size = (int)col_candidate_static_consts.size();

	int current_consts_size = (int)current_consts.size();
	int current_static_consts_size = (int)current_static_consts.size();

	// ���ۂɐڐG���A�������̂��܂ސڐG
	if(pointer_consts_size > i)
	{
		constraint = pointer_consts[i].first;
		*sign = pointer_consts[i].second;

		if(*sign > 0) *so = pointer_consts[i].first->solid[0];
		else *so = pointer_consts[i].first->solid[1];
	}
	// �O�X�e�b�v�ŐڐG���Ă���
	else if(pointer_consts_size + current_consts_size > i)
	{
		int i_local = i - pointer_consts_size;

		constraint = current_consts[i_local].first;
		*sign = current_consts[i_local].second;

		if(*sign > 0) *so = current_consts[i_local].first->solid[0];
		else *so = current_consts[i_local].first->solid[1];

		*bPreviousCollide = true;
	}
	// ���ۂɐڐG���Ă���A�������Ȃ����̂��܂ސڐG
	else if(pointer_consts_size + pointer_static_consts_size + current_consts_size > i)
	{
		int i_local = i - (pointer_consts_size + current_consts_size);
		constraint = pointer_static_consts[i_local].first;
		*sign = pointer_static_consts[i_local].second;

		if(*sign > 0) *so = pointer_static_consts[i_local].first->solid[0];
		else *so = pointer_static_consts[i_local].first->solid[1];
	}
	// �O�X�e�b�v�ŐڐG���Ă���A�������Ȃ����� pointer_static_consts_size + 
	else if(pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size > i)
	{
		int i_local = i - (pointer_consts_size + current_consts_size + pointer_static_consts_size);
		constraint = current_static_consts[i_local].first;
		*sign = current_static_consts[i_local].second;

		if(*sign > 0) *so = current_static_consts[i_local].first->solid[0];
		else *so = current_static_consts[i_local].first->solid[1];

		*bPreviousCollide = true;
	}
	// �|�C���^�ɂ͐ڐG���Ă��Ȃ��A�������̂��܂ސڐG
	// �|�C���^�ɐڐG�������Ȃ��̂���
	else if(pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size + col_candidate_consts_size > i)
	{
		int i_local = i - (pointer_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size);
		constraint = col_candidate_consts[i_local].first;
		*sign = col_candidate_consts[i_local].second;

		*pointer_col_position = col_candidate_pointer_pos.find(DCAST(PHContactPoint, constraint))->second;

		if(*sign > 0) *so = col_candidate_consts[i_local].first->solid[0];
		else *so = col_candidate_consts[i_local].first->solid[1];

		// ���̐ڐG�͎��ۂɂ͐ڐG���Ă��Ȃ��̂�false
		*bCollide = false;
	}
	// �ڐG���Ă��炸�A�������Ȃ����̂��܂ސڐG
	// �|�C���^�ɐڐG�������ȐڐG
	else 
	{
		int i_local = i - (pointer_consts_size + col_candidate_consts_size + pointer_static_consts_size + current_consts_size + current_static_consts_size);
		constraint = col_candidate_static_consts[i_local].first;
		*sign = col_candidate_static_consts[i_local].second;

		*pointer_col_position = col_candidate_pointer_pos.find(DCAST(PHContactPoint, constraint))->second;

		if(*sign > 0) *so = col_candidate_static_consts[i_local].first->solid[0];
		else *so = col_candidate_static_consts[i_local].first->solid[1];

		// ���̐ڐG�͎��ۂɂ͐ڐG���Ă��Ȃ��̂�false
		*bCollide = false;
	}
	return constraint;
}

// ���̂̓o�^�ƐڐG�ƍ��̂̊֘A�t��
void RegisterNewSolid(HapticInfo* new_info, int i, HapticInfo* current_info, PHSolid* so, map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > > matrices, map<PHSolid*, SpatialVector> vecs, map<PHSolid *, SpatialVector> g_map)
{
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

			bool bExist = false;

			// �O��̐ڐG�ƍ���̐ڐG�œ������̂��܂܂�Ă����index��ۑ�����
			for(int j = 0; j < current_info->num_solids; ++j)
			{
				if(current_info->nearest_solids[j] == so)
				{
					bExist = true;
					new_info->NewsolToSol[counter] = j;
					break;
				}
			}			
			if(!bExist) new_info->NewsolToSol[counter] = -1;

			// ���̐����X�V
			new_info->num_solids = counter+1;

			// ���x�������̂��Ƃɒǉ�
			new_info->solid_velocity[counter] = so->GetVelocity();
			new_info->solid_angular_velocity[counter] = so->GetAngularVelocity();
			new_info->solid_massinvs[counter] = so->GetMassInv();
			new_info->solid_inertiainvs[counter] = so->GetInertiaInv();

			new_info->solid_center_positions[counter] = so->GetCenterPosition();
			new_info->solid_orientations[counter] = so->GetOrientation();

			// ���͂̉e��������킷�ϐ����R�s�[����
			// ���̕ϐ��͍��̌ŗL�̒l�Ȃ̂ŁA
			// ���̂��ǉ����ꂽ���_�Ŋi�[�������s��
			if(so->IsDynamical())
			{
				map<PHSolid *, SpatialVector>::iterator it1 = vecs.find(so);
				if(it1 != vecs.end())
				{
					SpatialVector sv = (*it1).second;
					new_info->vel_constant[counter] = sv.v();
					new_info->ang_constant[counter] = sv.w();
				}
				else
				{
					new_info->vel_constant[counter] = Vec3d();
					new_info->ang_constant[counter] = Vec3d();
				}

				// �d�͂̉e��
				map<PHSolid *, SpatialVector>::iterator it3 = g_map.find(so);
				if(it3 != g_map.end())
				{
					new_info->gravity_effect[counter] = (*it3).second;
				}
				else
				{
					new_info->gravity_effect[counter] = SpatialVector();
				}

				// ���͂̉e��
				map<PHSolid*, vector<pair<Matrix3d, Matrix3d> > >::iterator it2 = matrices.find(so);
				if(it2 != matrices.end())
				{
					vector<pair<Matrix3d, Matrix3d> > mm_vector = (*it2).second;
					for(int j = 0; j < (int)mm_vector.size(); ++j)
					{
						new_info->vel_effect[counter][j] = mm_vector[j].first;
						new_info->ang_effect[counter][j] = mm_vector[j].second;
					}
				}
				else
				{
					for(int j = 0; j < NUM_COLLISIONS; ++j)
					{
						if(counter == j)
						{
							double minv = so->GetMassInv();
							Matrix3d iinv = so->GetInertiaInv();

							new_info->vel_effect[counter][j] = Matrix3d(minv, 0, 0, 0, minv, 0, 0, 0, minv);
							new_info->ang_effect[counter][j] = iinv;
						}
						else
						{
							new_info->vel_effect[counter][j] = Matrix3d();
							new_info->ang_effect[counter][j] = Matrix3d();
						}
					}
				}
			}
			else
			{
				new_info->vel_constant[counter] = Vec3d();
				new_info->ang_constant[counter] = Vec3d();
				new_info->gravity_effect[counter] = SpatialVector();
			}

			// ���̕��̂ɏd�͂��K������Ă��邩���ׂ�
			new_info->bGravity[counter] = false;
			PHSolids solids = scene->GetGravityEngine()->solids;
			for(PHSolids::iterator it = solids.begin(); it != solids.end(); ++it)
			{
				if((*it) == so)
				{
					new_info->bGravity[counter] = true;
					break;
				}
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
}

// �������̍쐬
// �ڐG�̓o�^���s��
void RegisterNewCollision(HapticInfo* new_info, int i, HapticInfo* current_info, PHConstraint* constraint, int sign, bool bPreviousCollide, bool bCollide, map<PHConstraint*, int> NewcolToCol, Vec3d pointer_col_position, bool bLocalDynamics_local)
{
	new_info->constraint[i] = constraint;
	new_info->sign[i] = sign;
	new_info->shapePair[i] = DCAST(PHContactPoint, constraint)->shapePair;

	new_info->bPreviousCollide[i] = bPreviousCollide;
	new_info->bCollide[i] = bCollide;

	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	new_info->solid[i][0] = contact->solid[0];
	new_info->solid[i][1] = contact->solid[1];

	// ���[���h���W����S�����W�ɒ����S������o�^
	// ���͂̉e�����g���Ƃ���
	// ���[���h���W�̗͂��S�����W�Ɉڂ��ČW�����g��
	// ���͂̉e���͓��͂��S�����W�ŏo�͂����[���h���W
	if(bLocalDynamics_local)
	{
		if(sign > 0)
		{
			new_info->wldTosld[i] = contact->Xj[0].q.Conjugated();// * contact->solid[0]->GetOrientation().Conjugated();
		}
		else
		{
			new_info->wldTosld[i] = contact->Xj[1].q.Conjugated();// * contact->solid[1]->GetOrientation().Conjugated();
		}
	}
	// �Ǐ��I�ȓ��͊w�v�Z���s��Ȃ��ꍇ��
	// ���͂̉e�����v�Z����Ƃ���
	// ���[���h���W�̗͂����[���h���W�̂܂�
	// ���͂̉e���i���^���������j��������
	// �^������������邽�߂�
	// �͂ł̍S�����W�ւ̕ϊ���ł������悤�ɍ��̍��W�n�����[����
	else
	{
		if(sign > 0)
		{
			new_info->wldTosld[i] = contact->solid[0]->GetOrientation();
		}
		else
		{
			new_info->wldTosld[i] = contact->solid[1]->GetOrientation();
		}
	}

	bool current_exists = false;

	if(bPreviousCollide)
	{
		// ���̐ڐG���猻�݂̃f�[�^�ւ̈������擾����
		int index = NewcolToCol.find(constraint)->second;

		// ���݂̒l�����������p��
		new_info->col_positions[i] = current_info->col_positions[index];
		new_info->col_normals[i] = current_info->col_normals[index];
		new_info->pointer_col_positions[i] = current_info->pointer_col_positions[index];

		new_info->pointer_pos[i] = pointer_pos;
		new_info->pointer_ori[i] = pointer_ori;
		
		// �Y�����̕ϊ��\���쐬
		new_info->NewcolToCol[i] = index;

		current_exists = true;
	}

	// �O�񓯂��ڐG�����݂��Ȃ�����
	if(!current_exists)
	{
		// �����_�ŐڐG���Ă���
		if(bCollide)
		{
			// �|�C���^�̐ڐG�_�͐ڐG�̐ڐG�_�Ɠ���
			new_info->pointer_col_positions[i] = contact->pos;
			new_info->col_normals[i] = contact->shapePair->normal * sign;

//			if(bOutput)
			{
				/*
				ofs << "vector (-1, 0, 0) in constraint coordinate is represented in solid coordinate as ";
				if(sign > 0)
				{
					ofs << contact->Xj[0].q * Vec3d(-1, 0, 0) << endl;
				}
				else
				{
					ofs << contact->Xj[1].q * Vec3d(-1, 0, 0) << endl;
				}
				ofs << " and it is converted to wolrd coordinate like ";

				Vec3d f;

				if(sign > 0)
				{
					f =  contact->solid[0]->GetOrientation() * contact->Xj[0].q * Vec3d(-1, 0, 0);
					ofs << f << endl;
					ofs << " and its re-converted by multiplications with conjugeted quaternion ";
					ofs << contact->Xj[0].q.Conjugated() * contact->solid[0]->GetOrientation().Conjugated() * f << endl;
				}
				else 
				{
					f = contact->solid[1]->GetOrientation() * contact->Xj[1].q * Vec3d(-1, 0, 0);
					ofs << f << endl;
					ofs << " and its re-converted by multiplications with conjugeted quaternion ";
					ofs << contact->Xj[1].q.Conjugated() * contact->solid[1]->GetOrientation().Conjugated() * f << endl;
				}
				*/
			}
		}
		// ���ۂɂ͐ڐG���Ă��Ȃ�
		// �ڐG���_
		else
		{
			// �|�C���^�̐ڐG�_�͐ڐG�_�ƕ�
			new_info->pointer_col_positions[i] = pointer_col_position;
			new_info->col_normals[i] = (pointer_col_position - contact->pos).unit();
		}

		new_info->col_positions[i] = contact->pos;
		new_info->pointer_pos[i] = pointer_pos;
		new_info->pointer_ori[i] = pointer_ori;

		// �ꉞ-1�ŏ���������
		new_info->NewcolToCol[i] = -1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// �͊o�v�Z�֌W�̏���
int pos_mode = 0;
Vec3d pos_array[] = {Vec3d(0, 0.0, 0.0), Vec3d(0.0, -0.01, 0.0), Vec3d(0.0, -0.035, 0.0)};
// �߂荞�݂𒲂ׁA�K�v�ȏ����쐬����֐�
inline void calcTestForce(int step_counter)
{
	// �\���V�~�����[�V�����Ɏg���P�ʗ͂̃x�N�g���̗p��
	// ����HapticRendering��simulation��������Ă����̂ɗ͂������Ă��Ȃ������ꍇ
	// ����܂łɓ��������ʂ̕��ς���邱�Ƃɂ���
	if(VCForce_sum != Vec3d())TestForce = VCForce_sum / (double)step_counter;
	else TestForce = MoveVector / (double)step_counter;

	VCForce_sum = Vec3d();
	MoveVector = Vec3d();

	// �@���͊�{�I�Ƀ|�C���^�̕����������Ă���̂�
	// x-z���ʏ�łP�W�O�x��]������
	// �͂�����������ɂ���
	TestForce = Vec3d(-1, 1, -1);
}

inline void feedbackForce(bool feedback, Vec3d VCForce, Vec3d VCTorque)
{
	if(feedback && bforce)
	{
		// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
		// virtual coupling�̋t�����̗́E�g���N�𔭐�
		Vec3d f = - view_haptic * VCForce;
		Vec3d t = - view_haptic * VCTorque;

		spidarG6.SetForce(FORCE_COEFF * f, FORCE_COEFF * t);
	}
	else spidarG6.SetForce(Vec3d(), Vec3d());
}

inline HapticInfo* updateInfo()
{
	////////////////////////////////////////////////////////////////////////////////////////
	// �������̎擾
	// �V�~�����[�V�������瓾��������i�[����ϐ�
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data) info = &info1;
	else info = &info2;

	/////////////////////////////////////////////////////////////////////////////////////////
	// SPIDAR�����X�V����
	Vec3d SPIDARVel;
	Quaterniond SPIDARAngVel;
	UpdateSpidarData(&SPIDARVel, &SPIDARAngVel);

	return info;
}

inline void calcLocalDynamics(HapticInfo* info, Vec3d* VCForce, Vec3d* VCTorque, bool* feedback)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// �U���R�x�̂߂荞�݉�������
	// �����I��pointer_pos, pointer_ori��ύX���Ă���
	CorrectPenetration(info);

	// �ڐG���̂̐ڐG�͂ɂ�鑬�x�E�p���x�X�V
	*VCForce = Km * (spidar_pos - pointer_pos);// + Bm * (SPIDARVel - M_vec/dt);
	*VCTorque = (Kr * spidar_ori * pointer_ori.Inv()).Rotation();// + Br * (SPIDARAngVel.Rotation() - R_vec/dt);

	// �|�C���^�Ƃ̐ڐG�ɂ�鍄�̂̍X�V
	UpdateSolidByCollision(info, *VCForce, feedback);
}

/**
 brief  	�񎦗͂̌v�Z��Haptic Device	�֔��f. multimedia timer�ɂ���ČĂяo�����
			�����I�ɂ̓|�C���^��SPIDAR��virtual coupling�łȂ��A�|�C���^���X�V���Ă���
 param	 	�Ȃ�
 return 	�Ȃ�
*/
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	// �V�~�����[�V�����Ɨ͊o�����_�����O�̍X�V�����̔䗦������킷�ϐ�
	static const double ratio = (double)SIMULATION_FREQ / HAPTIC_FREQ;
	static const double ratioinv = (double)HAPTIC_FREQ / SIMULATION_FREQ;

	///////////////////////////////////////////////////////////////////////////////////////
	// �������̊Ǘ�,�V�~�����[�V�����̊Ǘ��Ǝ��s

	// �V�~�����[�V�������Ǘ�����J�E���^�[���쐬
	static int step_counter = 0;	
	// �J�E���^�[�̒l�������𖞂������`�F�b�N
	if(step_counter >= (int)ratioinv && !bSimulation)
	{
		// �ڐG�������p���ꍇ��,���Ɏg�p����f�[�^�̂����ŁA
		// �����p���ڐG�̏������݂̂��̂ŏ㏑������
		UpdateNewInfoFromCurrent();

		// ���͂̉e�����v�Z����Ƃ��ɂ����P�ʗ͂̌v�Z
		// �������͂̕��ς����߂�
		calcTestForce(step_counter);

		// �g�p����f�[�^��ύX���A�V�~�����[�V�������s��
		step_counter = 0;
		current_valid_data = !current_valid_data;
		bSimulation = true;

//		ofs << "use new data" << endl;
	}
	else ++step_counter;

	// �����ɕK�v�ȏ��𓾂�
	HapticInfo* info = updateInfo();

	// �o�^���ꂽ�ڐG�������Ă��A���ݖ{���ɐڐG���Ă��邩�͂킩��Ȃ��̂Ńt���O��p��
	bool feedback = false;

	Vec3d VCForce;
	Vec3d VCTorque;

	////////////////////////////////////////////////////////////////////////////////////////
	// �Ǐ��I�ȓ��͊w�v�Z
	if(bLocalDynamics)
	{
		calcLocalDynamics(info, &VCForce, &VCTorque, &feedback);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// �����ȃo�[�`�����J�b�v�����O
	// ���̂ɐڐG�͂�������
	else
	{
		VirtualCoupling(info, &VCForce, &VCTorque, &feedback);
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// ���[�U�ւ̒񎦗͂̌v�Z�Ɛݒ�
	feedbackForce(feedback, VCForce, VCTorque);
}

void CheckPenetrations(HapticInfo* info, int* num_cols, vector<Vec3d>* r_vectors, vector<double>* c_vectors, vector<Vec3d>* n_vectors)
{
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �߂荞�݉��������̏���
		Vec3d col_vector = info->col_positions[i] - info->pointer_col_positions[i];
		double vector_coeff = dot(col_vector, info->col_normals[i]);

		// �߂荞��ł�����␳�p�̃f�[�^��ǉ�
		if(vector_coeff > 0 && info->bCollide[i])
		{
			r_vectors->push_back(Vec3d(info->pointer_col_positions[i] - pointer_pos));
			c_vectors->push_back(vector_coeff);
			n_vectors->push_back(info->col_normals[i].unit());

			// �߂荞��ł��ڐG�̌�
			++(*num_cols);
		}
	}
}

// �߂荞�݂�␳����̂ɕK�v�ȂQ�̃x�N�g�����쐬����
void CalcTTheta(HapticInfo* info, Vec3d* T, Vec3d* Theta)
{
	int num_cols = 0;
	vector<Vec3d> r_vectors;
	vector<double> c_vectors;
	vector<Vec3d> n_vectors;

	// �߂荞�݂��`�F�b�N���čs��̍쐬�ɕK�v�ȃf�[�^���쐬����
	CheckPenetrations(info, &num_cols, &r_vectors, &c_vectors, &n_vectors);

	// �߂荞�݂̐��̍ő�l��ڐG���e��-2�ɂ���
	if(num_cols > NUM_COLLISIONS - 6) num_cols = NUM_COLLISIONS - 6;

	MatrixN6d M = MatrixN6d();
	VecNd C = VecNd();

	MakePointerPosMatrix(num_cols, r_vectors, c_vectors, n_vectors, &M, &C);

	// �[���t�s����v�Z
	Matrix6Nd Pinv;
	CalcPinv(M, &Pinv);

	// ���i�Ɖ�]�̃x�N�g��
	// ���`�O�v�f����]�x�N�g����xyz
	// ��l�`�Z�v�f�����i�x�N�g����xyz
	Vec6d Ans = Pinv * C;
	*Theta = Ans.sub_vector(TSubVectorDim<0,3>());
	*T = Ans.sub_vector(TSubVectorDim<3,3>());
	/*
	if((*Theta).norm() > 10) 
	{
		ofs << "------------------" << endl;
		ofs << "Theta = " << *Theta << endl;
		ofs << "num_cols = " << num_cols << endl;
		ofs << "pointer pos = " << pointer_pos << endl;

		for(int i = 0; i < info->num_collisions; ++i)
		{
			ofs << "penetration = " << info->col_positions[i] - info->pointer_col_positions[i] << endl;
		}

		for(int i = 0; i < info->num_solids; ++i)
		{
			if(info->nearest_solids[i]->IsDynamical())
			{
				ofs << "solid center position = " << info->solid_center_positions[i] << endl;
				ofs << "solid ang velocity = " << info->solid_angular_velocity[i] << endl;
			}
		}

		ofs << "Pinv * M = " << endl;
		ofs << Pinv * M << endl;
	}
	*/
}

double COEFFMX = 0.0015;
double COEFFMY = 0.0015;
double COEFFMZ = 0.0015;
double COEFFRX = 0.002;
double COEFFRY = 0.002;
double COEFFRZ = 0.002;

void MakePointerPosMatrix(int num_cols, vector<Vec3d> r_vectors, vector<double> c_vectors, vector<Vec3d> n_vectors, MatrixN6d* M, VecNd* C)
{
	// �߂荞�݂ɂ�����d��
	double P;

	// SPIDAR�̈ʒu�̕��ɂ�����d��
	double MX, MY, MZ;
	double RX, RY, RZ;

	// �ڐG�̏󋵂���p�����[�^�����߂�
	GetParametersFromCollision(&MX, &MY, &MZ, &RX, &RY, &RZ, &P);

	// �[���t�s��ō��W�v�Z�����˂�
	Vec3d t = spidar_pos - pointer_pos;
	Vec3d th = (spidar_ori * pointer_ori.Inv()).Rotation();

	for(int i = 0; i < 6; i++)
	{
		switch(i)
		{
		case 0:
			C->item(i) = RX * th.x;
			break;
		case 1:
			C->item(i) = RY * th.y;
			break;
		case 2:
			C->item(i) = RZ * th.z;
			break;
		case 3:
			C->item(i) = MX * t.x;
			break;
		case 4:
			C->item(i) = MY * t.y;
			break;
		case 5:
			C->item(i) = MZ * t.z;
			break;
		}

		// ��{�I��t��th�Ō��܂�悤�ɂ���
		// �ڐG������ꍇ�͂��̍s��ɂ���ɏ���ǉ����A
		// �[���t�s����������Ƃɂ���Ă߂荞�݂��l������
		// �v���L�V�̈ʒu�����߂�
		for(int j = 0; j < 6; j++)
		{
			if(i == j)
			{
				switch(i)
				{
				case 0:
					M->item(i, j) = RX;
					break;
				case 1:
					M->item(i, j) = RY;
					break;
				case 2:
					M->item(i, j) = RZ;
					break;
				case 3:
					M->item(i, j) = MX;
					break;
				case 4:
					M->item(i, j) = MY;
					break;
				case 5:
					M->item(i, j) = MZ;
					break;
				}
			}
			else M->item(i, j) = 0;
		}
	}

	// �[���t�s��̌v�Z�ɕK�v��
	// �s��ƃx�N�g��������Ă���
	for(int k = 6; k < NUM_COLLISIONS; ++k)
	{				
		if(k < num_cols + 6)
		{
			Vec3d n = P * n_vectors[k-6];
			double c = P * c_vectors[k-6];
			Vec3d r = P * r_vectors[k-6];

			C->item(k) = c;

			for(int j = 0; j < 6; ++j)
			{
				switch(j)
				{
				case 0:
					M->item(k, j) = r.y * n.z - r.z * n.y;
					break;
				case 1:
					M->item(k, j) = r.z * n.x - r.x * n.z;
					break;
				case 2:
					M->item(k, j) = r.z * n.y - r.y * n.x;
					break;
				case 3:
					M->item(k, j) = n.x;
					break;
				case 4:
					M->item(k, j) = n.y;
					break;
				case 5:
					M->item(k, j) = n.z;
					break;
				}
			}			
		}
		else
		{
			// �f�[�^���Ȃ������̏�����
			C->item(k) = 0;

			for(int j = 0; j < 6; ++j)
			{
				M->item(k, j) = 0;
			}
		}
	}
}

void GetParametersFromCollision(double* MX, double* MY, double* MZ, double* RX, double* RY, double* RZ, double *P)
{
	// �Ƃ肠�������̒l�ň���
//	*MX = *MY = *MZ = *RX = *RY = *RZ = *P = 1.0;
	*MX = *MY = *MZ = 0.8;
	*RX = *RY = *RZ = 1.0;
	*P = 1.0;
}

void CorrectPenetration(HapticInfo* info)
{
	// �|�C���^�̂߂荞�݂ɑ΂��ċ[���t�s����v�Z���A
	// �K�؂ȕ␳�ʂ����߂邱�ƂŁA�߂荞�݂�␳���Ă���
	// �|�C���^���g����i�E��]�����Ă߂荞�݂�����������悤�ɂ���
	Vec3d M_vec;
	Vec3d R_vec;
	CalcTTheta(info, &M_vec, &R_vec);

	// �|�C���^���g�̈ʒu���ړ�
	pointer_pos += M_vec;

//	if(R_vec != Vec3d()) ofs << "R_vec = " << R_vec << endl;
	MoveVector += M_vec;

	// �|�C���^���g�̎p����␳
	Quaterniond correct_q = Quaterniond::Rot(R_vec);
	pointer_ori= correct_q * pointer_ori;

	// ���ׂĂ̐ڐG�����̕����Ɉړ��E��]
	for(int i = 0; i < info->num_collisions; ++i)
	{
		info->pointer_col_positions[i] += M_vec;
		info->pointer_col_positions[i] += R_vec ^ (info->pointer_col_positions[i] - pointer_pos);
	}
}

void UpdateSolids(HapticInfo* info)
{
	const double dt = (double)1.0 / HAPTIC_FREQ; 
	const double ratio = (double)SIMULATION_FREQ / HAPTIC_FREQ;

	for(int i = 0; i < info->num_solids; ++i)
	{
		// �Ǐ��I�ȓ��͊w�v�Z�{���͂̉e���Ȃ��̏ꍇ
		// �d�͂��K������Ă��镨�̂ɂ͏d�͂�������
		if(info->nearest_solids[i]->IsDynamical() && !bSurroundEffect && info->bGravity[i]) info->solid_velocity[i] += scene->GetGravity() * dt / info->solid_massinvs[i];

		if(bSurroundEffect)
		{
			// ���͂̉e���̂����ŁA�萔�������X�ɉ����Ă���
			// IsDynamical == false�̏ꍇ�͂��ꂼ��Vec3d()�ŏ��������Ă���̂ŏ�������͕K�v�Ȃ�
			info->solid_velocity[i] += info->vel_constant[i] * ratio;
			info->solid_angular_velocity[i] += info->ang_constant[i] * ratio;

			// �d�͂̉e����������
			info->solid_velocity[i] += info->gravity_effect[i].v() * ratio;
			info->solid_angular_velocity[i] += info->gravity_effect[i].w() * ratio;
		}

		// ���̂̑��x�ɂ��ړ��E��]
		Vec3d dx_local = info->solid_velocity[i] * dt;
		info->solid_center_positions[i] += dx_local;
		
		Vec3d dth_local = info->solid_angular_velocity[i] * dt;
		info->solid_orientations[i] = Quaterniond::Rot(dth_local) * info->solid_orientations[i];

		// ���̏�ŋN�������ڐG����������		
		for(int j = 0; j < info->num_col_per_sol[i]; ++j)
		{
			// ���̍��̏�ɂ���ڐG�����̂̈ړ��ɔ����čX�V����
			// �ꗗ�\����ڐG�̓Y����������
			int col_index = info->SolToCol[i][j];

			info->col_positions[col_index] += dx_local;
			Vec3d dx_rotation = dth_local ^ (info->col_positions[col_index] - info->solid_center_positions[i]);
			info->col_positions[col_index] += dx_rotation;
			info->col_normals[col_index] += dth_local ^ info->col_normals[col_index];
			info->col_normals[col_index] = info->col_normals[col_index].unit();

//			info->wldTosld[col_index] = Quaterniond::Rot(dth_local) * info->wldTosld[col_index];
		}
	}
}

void UpdateVelocityByCollision(HapticInfo* info, Vec3d VCForce, bool* feedback)
{
	const double dt = (double)1.0 / HAPTIC_FREQ; 
	const double ratio = (double)SIMULATION_FREQ / HAPTIC_FREQ;

	for(int i = 0; i < info->num_collisions; i++)
	{
		// �Q�ڐG�_�̋����𒲂ׂ�
		double distance = calcDistance(info->col_positions[i], info->pointer_col_positions[i]);

		// �ʂ̖@���ƁA�|�C���^��̓_���獄�̏�̓_�܂ł����񂾃x�N�g���̓��ς��v�Z
		// ���ꂪ�O�ȏ�Ȃ�i�[���x�N�g�����܂ށB���傤�ǐڂ��Ă���j�ڐG������
		// �܂��A���̂܂܂��ƍ��̂������������Ƃ����͂𔭐������Ă��܂��̂ŁA
		// ���̂̏d�S����ڐG�_�܂ŁA�ƃ|�C���^�̏d�S����ڐG�_�܂ł̃x�N�g���̌����̊֌W���`�F�b�N����
		// ��̃x�N�g�����݂��Ɍ��������Ă�����͂��o��
		if(dot(info->col_positions[i] - info->pointer_col_positions[i], info->col_normals[i]) >= 0 && distance < DISTANCE_LIMIT && 
			((info->nearest_solids[info->ColToSol[i]]->IsDynamical() 
			&& dot(info->col_positions[i] - info->solid_center_positions[i], info->pointer_col_positions[i] - pointer_pos) <= 0)
			|| !info->nearest_solids[info->ColToSol[i]]->IsDynamical()))
			
		{
			// �Ƃɂ����ڐG���Ă���̂Ńt���O��ON�ɂ���
			info->bCollide[i] = true;

			// �͂������Ă悢�Ώۂ��`�F�b�N
			// �͂̌����ƍ��̂̐ڂ��鑤�̃`�F�b�N
			if(dot(info->col_normals[i], -VCForce) >= 0)
			{
				// ���[�U�ɗ͊o���
				*feedback = true;

				// ���̂̓Y����
				int sol_index = info->ColToSol[i];
		
				// �O�͂œ������̂ł���Α��x���ω�����͂��Ȃ̂ŉ����x���v�Z���đ��x���X�V����
				if(info->nearest_solids[sol_index]->IsDynamical())
				{
					// ���͂̉e�����v�Z����Ƃ��Ɏg���e�X�g�͂̂��߂̏���
					VCForce_sum += VCForce;

					// �Ǐ��I�ȓ��͊w�v�Z�̂�
					if(!bSurroundEffect)
					{
						// �^���������ɏ]���ĉ����x���v�Z����
						Vec3d accel = info->solid_massinvs[sol_index] * FORCE_COEFF * VCForce;
						Vec3d ang_accel = info->solid_inertiainvs[sol_index] * ((info->col_positions[i] - info->solid_center_positions[sol_index]) ^ (FORCE_COEFF * VCForce));

						// ����ꂽ�����x�ő��x���X�V
						info->solid_velocity[sol_index] += accel * dt;

						// �p���x���X�V
						info->solid_angular_velocity[sol_index] += ang_accel * dt;
//						if(bOutput) ofs << "-- collide --" << endl;
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
						// �܂��A���̂̍ŐV�̎p�����g���Ă���
						Vec3d q_f = info->wldTosld[i] * info->solid_orientations[sol_index].Conjugated() * FORCE_COEFF * VCForce * ratio;

						// ���̗͂��y�ڂ��e�������ׂĂ̍��̂ɂ��Čv�Z����
						for(int j = 0; j < info->num_solids; ++j)
						{
							// �O�͂œ����Ȃ����̂��ЂƂł��o����
							// ���̐�͂��ׂĊO�͂œ����Ȃ����̂Ȃ̂�break����
							if(!info->nearest_solids[j]->IsDynamical()) break;

							// �s���p���ĉ����x���v�Z���đ��x���X�V
							info->solid_velocity[j] += info->vel_effect[j][i] * q_f;
							info->solid_angular_velocity[j] += info->ang_effect[j][i] * q_f;
//							if(bOutput) ofs << "-- collide --" << endl;

							if(info->solid_angular_velocity[j].norm() > 10) 
							{
								ofs << "i = " << i << " j = " << j << endl;
								ofs << "ang accel = " << info->ang_effect[j][i] * q_f << endl;
								ofs << "ang velocity = " << info->solid_angular_velocity[j] << endl;
								ofs << "VCForce = " << VCForce << endl;
								ofs << "ang coeff = " << info->ang_effect[j][i] << endl;
							}
						}
					}
				}
			}
		}
		else info->bCollide[i] = false;
	}
}

void UpdateSolidByCollision(HapticInfo *info, Vec3d VCForce, bool* feedback)
{
	/////////////////////////////////////////////////////////////////////////////////////////////////////
	// �Ǐ��I�ȓ��͊w�v�Z
	// �ڐG�͂ɂ�鑬�x�̍X�V
	UpdateVelocityByCollision(info, VCForce, feedback);

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// �Ǐ��I���͊w�v�Z�ɂ�鍄�̂̈ʒu�E�p�����X�V
	// �|�C���^�ɐڂ��Ă��邷�ׂĂ̍��̂̏����X�V����
	UpdateSolids(info);
}

void UpdateNewInfoFromCurrent()
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
		info_next = &info1;
	}

	// ���̂̍X�V
	for(int i = 0; i< info_next->num_solids; ++i)
	{
		// ����̍��̂��獡��̍��̂̓Y����������
		int prev_sol_index = info_next->NewsolToSol[i];

		// ������Ȃ���Ύ��ɂ�����
		// ����V�������̂���������ꍇ�͂��肦��
		if(prev_sol_index == -1)continue;

		// �Ǐ��I�ȓ��͊w�v�Z���s���ꍇ��
		// ���݂̒l��M����
		if(bLocalDynamics)
		{
			// ����Ǝ���̃f�[�^�̊Ԃł̍��̂̌덷���v�Z����
			Vec3d pos_error = info_cur->solid_center_positions[prev_sol_index] - info_next->solid_center_positions[i];
			Quaterniond ori_error = info_cur->solid_orientations[prev_sol_index] * info_next->solid_orientations[i].Inv();

			info_next->solid_center_positions[i] += pos_error;
			info_next->solid_orientations[i] = ori_error * info_next->solid_orientations[i];
			info_next->solid_velocity[i] = info_cur->solid_velocity[prev_sol_index];
			info_next->solid_angular_velocity[i] = info_cur->solid_angular_velocity[prev_sol_index];
		}
		// �O�̐ڐG�������p�����ꍇ�͍ŐV�̏���M����
		else
		{
			Vec3d pos_error = info_next->solid_center_positions[i] - info_cur->solid_center_positions[prev_sol_index];
			Quaterniond ori_error = info_next->solid_orientations[i] * info_cur->solid_orientations[prev_sol_index].Inv();

			Vec3d ppos_error = pointer_pos - info_next->pointer_pos[i];
			Quaterniond pori_error = pointer_ori * info_next->pointer_ori[i].Inv();
			for(int j = 0; j < info_next->num_col_per_sol[i]; ++j)
			{
				int col_index = info_next->SolToCol[i][j];
				
				if(!info_next->bPreviousCollide[col_index]) continue;

				info_next->col_positions[col_index] += pos_error;
				info_next->col_positions[col_index] += ori_error.Rotation() ^ (info_next->col_positions[col_index] - info_next->solid_center_positions[i]);

				info_next->col_normals[col_index] += ori_error.Rotation() ^ info_next->col_normals[col_index];

//				info_next->pointer_col_positions[i] += ;
//				info_next->pointer_col_positions[i] += 
			}
		}
	}

	// �ڐG�̍X�V
	for(int i = 0; i < info_next->num_collisions; ++i)
	{
		int index = info_next->NewcolToCol[i];

		// �O�̐ڐG�������p��
		if(index != -1)
		{
			if(bLocalDynamics)
			{
				info_next->pointer_col_positions[i] = info_cur->pointer_col_positions[index];

				info_next->col_positions[i] = info_cur->col_positions[index];
				info_next->col_normals[i] = info_cur->col_normals[index];
			}

			double ddot = dot(info_next->pointer_col_positions[i] - info_next->col_positions[i], - info_next->col_normals[i]);			
			if(ddot > 0)info_next->bCollide[i] = true;
			else info_next->bCollide[i] = false;

//			if(ddot > 0 && info_next->bCollide[i])
			{
//				ofs << "dot1 = " << ddot << endl;
			}
		}
		// �V�����ڐG
		else
		{
			Vec3d spidar_pos_error = pointer_pos - info_next->pointer_pos[i];
			Quaterniond spidar_ori_error = pointer_ori * info_next->pointer_ori[i].Inv();

			info_next->pointer_col_positions[i] += spidar_pos_error;
			info_next->pointer_col_positions[i] += spidar_ori_error.Rotation() ^ (info_next->pointer_col_positions[i] - spidar_pos);


			// �V�����ڐG��ǉ�����Ƃ���
			// ���̐ڐG�����łɂ߂荞��ł����Ԃ���
			// �}�ɗ͂������ĕs����ɂȂ�̂�
			// ���̂߂荞�݂��`�F�b�N����
			// �������݂���ꍇ�͐ڐG�_�̈ʒu�����炵��
			// �ڐG���Ȃ���
			// ���������킩��Ȃ����ǌ���ł͂���Ă���
			double ddot = dot(info_next->pointer_col_positions[i] - info_next->col_positions[i], - info_next->col_normals[i]);

			if(ddot > 0) info_next->bCollide[i] = true;
			else info_next->bCollide[i] = false;

//			if(ddot >= 0)
			{
//				ofs << "dot2 = " << ddot << endl;
//				Vec3d ave = (info_next->pointer_col_positions[i] + info_next->col_positions[i]) / 2;
//				info_next->pointer_col_positions[i] = ave + 0.001 * info_next->col_normals[i];
//				info_next->col_positions[i] = ave - 0.001 * info_next->col_normals[i]; 
			}
		}
	}
}

void VirtualCoupling(HapticInfo* info, Vec3d *VCForce, Vec3d *VCTorque, bool* feedback)
{
	const double ratio = (double)SIMULATION_FREQ / HAPTIC_FREQ;

	CorrectPenetration(info);

	// �ڐG���̂̐ڐG�͂ɂ�鑬�x�E�p���x�X�V
	*VCForce = Km * (spidar_pos - pointer_pos);// + Bm * (SPIDARVel - M_vec/dt);
	*VCTorque = (Kr * spidar_ori * pointer_ori.Inv()).Rotation();// + Br * (SPIDARAngVel.Rotation() - R_vec/dt);

	// �ڐG������ΐڐG���Ă��鍄�̂ɒ񎦗͂̋t�����̃x�N�g����������
	// ���̂̋����͕����V�~�����[�V�����ɔC����
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �͊o�t�B�[�h�o�b�NON
		*feedback = true;

		int sol_index = info->ColToSol[i];

		// ratio�������闝�R�́A
		// �V�~�����[�V�������������͂̑��ʂ��V�~�����[�V�����̎����ŗ͐ςɂ��邽��
		info->nearest_solids[sol_index]->AddForce(FORCE_COEFF * (*VCForce) * ratio, info->col_positions[i]);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////
//�@�֘A�����蒆�����ȏ���

// ��Ď�@�ɂ���Đ��������̂̈ʒu�̌덷���C������֐�
void ErrorCorrection()
{
	// �Ǐ��I�ȓ��͊w���s��Ȃ��ꍇ��
	// �덷�������Ȃ��͂��Ȃ̂ł��̂܂܃��^�[��
	if(!bLocalDynamics) return;

	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(!current_valid_data) info = &info1;
	else info = &info2;

	Vec3d gravity = scene->GetGravity() * scene->GetTimeStep();

	// �͊o�����_�����O����V�~�����[�V�����ւ̋Ǐ��I���͊w�v�Z�̌��ʔ��f����
	// ���ׂĂ̍��̂ɂ��čs��
	for(int i = 0; i < info->num_solids; ++i)
	{
		// �덷�̕����s�ړ��E��]����
		Vec3d pos_error = info->solid_center_positions[i] - info->nearest_solids[i]->GetCenterPosition();
		Quaterniond ori_error = info->solid_orientations[i] * info->nearest_solids[i]->GetOrientation().Inv();

		info->nearest_solids[i]->SetFramePosition(info->nearest_solids[i]->GetFramePosition() + pos_error);
		info->nearest_solids[i]->SetOrientation(ori_error * info->nearest_solids[i]->GetOrientation());
		info->nearest_solids[i]->SetCenterPosition(info->solid_center_positions[i]);
		info->nearest_solids[i]->SetVelocity(info->solid_velocity[i]);
		info->nearest_solids[i]->SetAngularVelocity(info->solid_angular_velocity[i]);

		// ���̏C���ɂ���ăV�~�����[�V�������ꂽ�̂�update��true�ɂ���
		info->nearest_solids[i]->SetUpdated(true);

		// �����Ǐ��I�ȓ��͊w�v�Z���s���Ă����ꍇ
		// HapticRendering���ŏd�͌v�Z���s��
		// ����ɃV�~�����[�V�����ŏd�͌v�Z���s���Ɠ�d�ŏd�͂�ǉ����Ă��܂��̂�
		// GravityEngine���d�͂������邱�Ƃ����z����
		// ���̉����镪���Ɉ����đ��E������
		// ���ڏd�͂�ON/OFF�����Ă��悢���A���̑��삾���ŃR�X�g���������Ă��܂��̂�
		// �Ƃ肠�������̂悤�ɂ���
		if(info->nearest_solids[i]->IsDynamical() && !bSurroundEffect && info->bCollide[i])
		{
			info->nearest_solids[i]->SetVelocity(info->nearest_solids[i]->GetVelocity() 
				- gravity * info->nearest_solids[i]->GetMass());
		}
		else if(info->nearest_solids[i]->IsDynamical() && bSurroundEffect)
		{
			info->nearest_solids[i]->SetVelocity(info->nearest_solids[i]->GetVelocity()
			- info->gravity_effect[i].v() * scene->GetTimeStep());
			info->nearest_solids[i]->SetAngularVelocity(info->nearest_solids[i]->GetAngularVelocity()
				- info->gravity_effect[i].w() * scene->GetTimeStep());
		}
		
	}
}

void StepSimulation()
{
	scene->ClearForce();

	HapticInfo* info;

	if(!current_valid_data) info = &info1;
	else info = &info2;

	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ����������C������
	ErrorCorrection();

	scene->GenerateForce();
	scene->Integrate();

	// �Փ˓_����\��
//	show_collision_info();

	// �Փ˓_������͂̉e�����v�Z
	// ��{�I�ɑO��̃f�[�^�������p��
	// �P���Q�ƒ��B�Q���X�V
	if(current_valid_data) CalcSurroundEffect(&info2, &info1);
	// �Q���Q�ƒ��B�P���X�V
	else CalcSurroundEffect(&info1, &info2);

	// �V�~�����[�V�������I������̂Ńt���O��������
	bSimulation = false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// �֘A�͂��邪�{���ł͂Ȃ������̊֐�
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
 brief		VR���̃v���L�V�̈ʒu���X�V����֐��B
			���̊֐��Ɨ͊o�����_�����O��g�ݍ��킹�邱�ƂŃ|�C���^�̍X�V��HAPTIC_FREQ�̎��g���ōX�V�ł���B
 param		�Ȃ�
 return		�Ȃ�
 */
void UpdatePointer()
{
	pointer_vel = (soPointer->GetFramePosition() - pointer_pos) / scene->GetTimeStep();
	pointer_angvel = (soPointer->GetOrientation() * pointer_ori.Inv()).Rotation() / scene->GetTimeStep();

	soPointer->SetFramePosition(pointer_pos);
	soPointer->SetVelocity(Vec3d());
	soPointer->SetOrientation(pointer_ori);
	soPointer->SetAngularVelocity(Vec3d());
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
	/*
	else if(key == 'u')
	{
		pos_mode++;
		if(pos_mode == 3)pos_mode = 0;

		cout << "pos = " << pos_array[pos_mode] << endl;
	}
	*/
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
	/*
	else if(key == 'i')
	{
		soPointer->SetVelocity(Vec3d());
		soPointer->SetAngularVelocity(Vec3d());
		soPointer->SetPose(Posed());
		soPointer->SetFramePosition(Vec3d());
		spidarG6.Calib();
	}
	*/
	// object�̏ꏊ�����Z�b�g
	else if(key == 'r')
	{
		for(int i = 0; i < NUM_OBJECTS; ++i)
		{
			soObject[i]->SetPose(Posed());
			soObject[i]->SetFramePosition(Vec3f(0, 5*(i+1), 0));
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

			bSurroundEffect = false;
			cout << "Surround Effect OFF " << endl;

			bPredictCollision = false;
			cout << "Predict Collision" << endl;
		}
	}
	// �ڐG�\�z�̃t���O�ύX
	else if(key == 'h')
	{
		bPredictCollision = !bPredictCollision;
	
		cout << "Predict Collision ";
		if(bPredictCollision) cout << "ON" << endl;
		else cout << "OFF" << endl;
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

		cout << "LocalDynamics ";
		if(bLocalDynamics) cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Surrounding Effect ";
		if(bSurroundEffect)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Predict Collision ";
		if(bPredictCollision) cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Pointer Display ";
		if(bDisplayPointer)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "Collision Display ";
		if(bDisplayCollision)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "File Output ";
		if(bOutput)cout << "ON" << endl;
		else cout << "OFF" << endl;

		cout << "----------------------" << endl;
	}
/*
	else if(key == 'x')
	{
		if(bMode)
		{
			COEFFMX += 0.001;
		}
		else
		{
			COEFFRX += 0.001;
		}
		keyboard('g', 0, 0);
	}
	else if(key == 'z')
	{
		if(bMode)
		{
			COEFFMX -= 0.001;
		}
		else
		{
			COEFFRX -= 0.001;
		}
		keyboard('g', 0, 0);
	}
	else if(key == 'y')
	{
		if(bMode)
		{
			COEFFMY += 0.001;
		}
		else
		{
			COEFFRY += 0.001;
		}
		keyboard('g', 0, 0);
	}
	else if(key == 'u')
	{
		if(bMode)
		{
			COEFFMY -= 0.001;
		}
		else 
		{
			COEFFRY -= 0.001;
		}
		keyboard('g', 0, 0);
	}
	else if(key == 'w')
	{
		if(bMode)
		{
			COEFFMZ += 0.001;
		}
		else
		{
			COEFFRZ += 0.001;
		}
		keyboard('g', 0, 0);
	}
	else if(key == 'q')
	{
		if(bMode)
		{
			COEFFMZ -= 0.001;
		}
		else
		{
			COEFFRZ -= 0.001;
		}
		keyboard('g', 0, 0);
	}
	*/
	/*
	else if(key == 'g')
	{
		cout << "COEFFMX = " << COEFFMX << endl;
		cout << "COEFFMY = " << COEFFMY << endl;
		cout << "COEFFMZ = " << COEFFMZ << endl;

		cout << "COEFFRX = " << COEFFRX << endl;
		cout << "COEFFRY = " << COEFFRY << endl;
		cout << "COEFFRZ = " << COEFFRZ << endl;
		cout << endl;
	}
	*/
	else if(key == 'a')
	{
		bDisplayCollision = !bDisplayCollision;

		cout << "Collision Display ";
		if(bDisplayCollision) cout << "ON" << endl;
		else cout << "OFF" << endl;
	}
	// �ݒ�̃v���Z�b�g
	else if(key == '1')
	{
		bSurroundEffect = true;
		bLocalDynamics = true;

		cout << "PRESET 1: Local Dynamics With Surrounding Effect and Penetration Correction" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '2')
	{
		bSurroundEffect = true;
		bLocalDynamics = true;
		
		cout << "PRESET 2: Local Dynamics With Surrounding Effect" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '3')
	{
		bSurroundEffect = false;
		bLocalDynamics = true;

		cout << "PRESET 3: Local Dynamics With Penetration Correction" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '4')
	{
		bSurroundEffect = false;
		bLocalDynamics = true;

		cout << "PRESET 4: Local Dynamics" << endl;
		keyboard('f', 0, 0);
	}
	else if(key == '5')
	{
		bSurroundEffect = false;
		bLocalDynamics = false;

		cout << "PRESET 5: Virtual Coupling" << endl;
		keyboard('f', 0, 0);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// �{���ł͂Ȃ������̊֐�
vector<SpatialVector> CollisionF;
void SaveCollisionF(vector<PHConstraint *> relative_consts)
{
	// lambda��������
	CollisionF.clear();

	for(vector<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		// �����_�̒l(fv, fw)��ۑ����Ă���
		CollisionF.push_back((*it)->f);
	}
}

map<PHSolid*, SpatialVector> SolidDv;
void SaveSolidDv(set<PHSolid*> relative_solids)
{
	SolidDv.clear();

	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		SolidDv.insert(pair<PHSolid*, SpatialVector>(*it, (*it)->dv));
	}
}

map<PHSolid*, SpatialVector> SolidV;
void SaveSolidV(set<PHSolid*> relative_solids)
{
	SolidV.clear();

	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		SolidV.insert(pair<PHSolid*, SpatialVector>(*it, (*it)->v));
	}
}

map<PHSolid*, SpatialVector> SolidF;
void SaveSolidF(set<PHSolid*> relative_solids)
{
	SolidF.clear();

	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		SolidF.insert(pair<PHSolid*, SpatialVector>(*it, (*it)->f));
	}
}

void RestoreParameters(vector<PHConstraint*>* relative_consts, set<PHSolid*>* relative_solids)
{
	int local_counter = 0;
	// fv��fw�̒l�����ɂ��ǂ�
	// lambda�̕ۑ��̉ߒ���relative_consts��iterator��vector�Ɋi�[���Ă���͂��Ȃ̂�
	// ����iterator�Ń��[�v���܂킹�ΐڐG���킩��Ȃ��Ă����������ɎQ�Ƃł���͂�
	for(vector<PHConstraint *>::iterator it = relative_consts->begin(); it != relative_consts->end(); ++it)
	{
		(*it)->f = CollisionF[local_counter++];
	}

	// ���ׂĂ̍��̂�dv�����Ƃɖ߂�
	for(set<PHSolid*>::iterator it = relative_solids->begin(); it != relative_solids->end(); ++it)
	{
		map<PHSolid*,SpatialVector>::iterator it2 = SolidDv.find(*it);
		(*it)->dv = (*it2).second;

		it2 = SolidV.find(*it);
		(*it)->v = (*it2).second;

		it2 = SolidF.find(*it);
		(*it)->f = (*it2).second;
	}
}

/**
 brief  	glutIdleFunc�Ŏw�肵���R�[���o�b�N�֐�
 param	 	�Ȃ�
 return 	�Ȃ�
 */
void idle(){
	// �ĕ`��
	glutPostRedisplay();

	// simulation���P�X�e�b�v�i�߂�
	if(bSimulation)StepSimulation();
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
	if(bDisplayPointer) DisplayPointer();

	// �ڐG�_��`��
	if(bDisplayCollision) DisplayCollisions();

	// ���̂̈ʒu��`��
//	DisplaySolidCenter();

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
// �|�C���^�ŗL�̏������������s��
void InitPointer(PHSolidIf* pointer)
{
	// pointer�̏d�͂𖳌��ɂ���
	pointer->SetGravity(false);

	// pointer�ƐڐG���鍄�̂Ƃ̉�͖@�ɂ��v�Z����߁A
	// �͊o�ŏ�������悤�ɂ���
	AddInactiveSolid(pointer);

	pointer_pos = pointer->GetFramePosition();
	pointer_ori = pointer->GetOrientation();
	pointer_vel = Vec3d();
	pointer_angvel = Vec3d();

	// soPointer��BBox��c��܂���
	PHBBox* bbox = &(DCAST(PHSolid, pointer)->bbox);
	bbox->SetBBoxCenterExtent(bbox->GetBBoxCenter(), bbox->GetBBoxExtent() * EPSILON);	
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
	desc.mass = 1.0f;

	// inertia�̌v�Z��
	// �����̂̏ꍇ I = mass * (r1^2 + r2^2) / 12
	// ���̏ꍇ I = mass * r~2 * 2/5
	desc.inertia = Matrix3d(0.375,0,0,0,0.375,0,0,0,0.375);

	/*
	soObject.push_back(scene->CreateSolid(desc));

	desc.mass = 1.0f;
	desc.inertia = Matrix3d(0.375, 0.0, 0.0, 0.0, 0.375, 0.0, 0.0, 0.0, 0.375);
	soObject.push_back(scene->CreateSolid(desc));
	*/

	// Solid�̍쐬
	for (unsigned int objectCnt=0; objectCnt<NUM_OBJECTS; ++objectCnt){
		soObject.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
		ofs << "solid" << objectCnt << " = " << DCAST(PHSolid, soObject[objectCnt]) << endl;
	}

	desc.mass = 0.1f;
	desc.inertia = Matrix3d(0.16, 0, 0, 0, 0.16, 0, 0, 0, 0.16);

	for(int i = 0; i < 1; ++i)
	{
		PHSolidIf *soWall = scene->CreateSolid(desc);
		soWall->SetDynamical(false);
		soWalls.push_back(soWall);
		ofs << "static solid = " << DCAST(PHSolid, soWall) << endl;
	}
	soPointer = scene->CreateSolid(desc);

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

		bd.boxsize = Vec3f(1.5f, 1.5f, 1.5f);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_OBJECTS; ++sphereCnt){
		soObject[sphereCnt]->AddShape(floor);
		soObject[sphereCnt]->SetFramePosition(Vec3f(0, 5.0f*(sphereCnt+1), 2));
	}
	scene->SetGravity(Vec3f(0,-9.8f, 0));	// �d�͂�ݒ�

	// �|�C���^�̏�����
	InitPointer(soPointer);
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

/////////////////////////////////////////////////////////////////////////////////////////////////
/// ���[�e�B���e�B�֐�

// �^����ꂽ�ڐG�ŁA������solid�ɐڂ��Ă��鍄�̂�����ׁA����ΕԂ�
// �܂��A�@���̌��������ׂāA�P����O�Ɍ������悤�ɕ�����Ԃ�
PHSolid* GetAdjacentSolid(PHConstraint* constraint, PHSolid* solid, int* sign)
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

// �^����ꂽ��񂩂�ڐG�����֐�
PHContactPoint* CreateContactPoint(Vec3d col_normal, Vec3d col_position, PHSolid* solid0, PHSolid* solid1, Spr::PHShapePairForLCP* shapePair)
{
	// Contact Point���쐬���鏈��
	Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
	Vec3d normal = col_normal;
	Vec3d center_pos = col_position;

	Vec3d v0 = solid0->GetPointVelocity(center_pos);
	Vec3d v1 = solid1->GetPointVelocity(center_pos);

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

	PHContactPoint *point = DBG_NEW PHContactPoint(local, shapePair, col_position, solid0, solid1);
	point->scene = DCAST(PHScene, scene);
	point->engine = scene->GetConstraintEngine();

	if(!scene->GetConstraintEngine()->IsInactiveSolid(solid0->Cast())) point->SetInactive(0, false);
	if(!scene->GetConstraintEngine()->IsInactiveSolid(solid1->Cast())) point->SetInactive(1, false);

	return point;
}
/**
 brief     	���̂̐ڐG�_�̏���\������֐�
 param		�Ȃ�
 return 	�Ȃ�
*/
void show_collision_info()
{
	// �Փ˓_���擾
	PHConstraints cp = scene->GetConstraintEngine()->GetContactPoints();//GetContactPoints(scene);

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

// ����̐ڐG������ꍇ�ɂ������폜����֐�
void ResetOriginalContactPoints(multimap<double, PHContactPoint*>* points)
{
	// 2�X�e�b�v�O�ɍ�����ڐG�̍폜����
	for(multimap<double, PHContactPoint*>::iterator it = points->begin(); it != points->end(); ++it)
	{
		PHContactPoint* cp = (*it).second;
		delete cp;
	}
	points->clear();
}

static int start;
static int end;

// �����ɂ����鎞�Ԃ��v���B�ꎞ�I�Ȃ���
inline void t_start()
{
	start = timer.CountUS();
}

inline void t_end()
{
	// ����ɂ����鎞�Ԃ��`�F�b�N
	end = timer.CountUS();
	if(end - start > 0 && bOutput) ofs << "time = " << end - start << endl;
}
// �^����ꂽ�s��̋[���t�s����v�Z����
void CalcPinv(MatrixN6d M, Matrix6Nd* Pinv)
{
	Matrix6Nd Mt = M.trans();
	Matrix6d MTM = Mt * M;
	Matrix6d m = Matrix6d();

	// �Ίp�v�f�ɒl�𑫂��e�X�g
	// �����N������h��
	for(int i = 0; i < 6; ++i)
	{
		for(int j = 0; j < 6; ++j)
		{
			// �l�͓K��
			if(i == j) m[i][j] = 0.001;
			else m[i][j] = 0;
		}
	}
	MTM += m;

	Matrix6d MTMinv = MTM.inv();

	// �[���t�s��
	*Pinv = MTMinv * Mt;
}

inline void UpdateSpidarData(Vec3d* SPIDARVel, Quaterniond* SPIDARAngVel)
{
	const double dt = (double)1.0 / HAPTIC_FREQ;

	// �ڐG���N�������Ƃ��ɂ������x���Ȃ��Ă��܂�
	spidarG6.Update(dt);//	USB�̒ʐM���s���D

	/////////////////////////////////////////////////////////////////////////////////////////
	// Virtual Coupling
	// ���i
	// SPIDAR�̈ʒu�̍X�V
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

//	spidar_pos = view_rot * pos_array[pos_mode];
		
	// SPIDAR�̏����i�[����ϐ�
	static Vec3d old_pos = spidar_pos;

	// SPIDAR�̑��x���v�Z
	// ���x�̒�`�ł� v = dx / dt ����
	// dt�Ŋ���Ȃ��ق�������݂����Ȃ̂�
	// �����������g�����Ƃɂ���
	*SPIDARVel = spidar_pos - old_pos;
	old_pos = spidar_pos;


	/////////////////////////////////////////////////////////////////////////////////////////
	// Virtual Coupling
	// ��]
	// SPIDAR�̎p�����X�V
	Quaterniond qv;
	qv.FromMatrix(view_rot);

	spidar_ori = qv * spidarG6.GetOri();	
	
	static Quaterniond old_ori = spidar_ori;

	// SPIDAR�̉�]���x���v�Z
	// �O�̎p�����猻�݂̎p���ɕϊ�����S�������v�Z
	*SPIDARAngVel = spidar_ori * old_ori.Inv();
	old_ori = spidar_ori;
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
void DisplayPointer()
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
void DisplaySolidCenter()
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

	GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

	for(int i = 0; i < info->num_solids; ++i)
	{
		glPushMatrix();
			glMatrixMode(GL_MODELVIEW);
			Affinef sol_affine;
			Posed::Unit(info->solid_center_positions[i]).ToAffine(sol_affine);
			glMultMatrixf(sol_affine);

			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, white);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, white);
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, white);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, white);

			glutSolidSphere(1.0, 12, 9);
		glPopMatrix();
	}
}

// �ڐG�_��`�悷��֐�
void DisplayCollisions()
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
			Posed::Unit(info->col_positions[i]).ToAffine(col_affine);
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
			Posed::Unit(info->pointer_col_positions[i]).ToAffine(col_affine);
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

// ��_�̋������v�Z����֐�
inline double calcDistance(Vec3d a, Vec3d b)
{
	double x_sq = (a.x - b.x) * (a.x - b.x);
	double y_sq = (a.y - b.y) * (a.y - b.y);
	double z_sq = (a.z - b.z) * (a.z - b.z);

	return sqrt(x_sq + y_sq + z_sq);
}
