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
  �܂��ASPIDAR�𑀍삵�ċ��́A�����̋y�ђn�ʂƗ͊o�C���^���N�V�������s���B
  
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

// �����ɂ��킹�Đݒ���s���t���O
// ���̂��������I�Ԃ��ƂŎ����p�̐ݒ�ɂł���
//#define EXP1
//#define EXP2
//#define EXP3
//#define EXP4
//#define Demo

#define ESC				27			// ESC key
//#define EXIT_TIMER		10000		// ���s�X�e�b�v��
#if !defined Demo
//	#define WINSIZE_WIDTH	480			// �E�B���h�E�T�C�Y(width)
//	#define WINSIZE_HEIGHT	360			// �E�B���h�E�T�C�Y(height)
#define WINSIZE_WIDTH 960
#define WINSIZE_HEIGHT 720
#else
	#define WINSIZE_WIDTH   1280
	#define WINSIZE_HEIGHT  960
#endif

#if defined EXP1
	#define NUM_OBJECTS		2			// object��
#elif defined EXP2
	#define NUM_OBJECTS		2
#elif defined EXP3
	#define NUM_OBJECTS		1
#elif defined EXP4
	#define NUM_OBJECTS		5
#elif defined Demo
	#define NUM_OBJECTS		2
#else
	#define NUM_OBJECTS		2
#endif

#define SPIDAR_SCALE	2			// SPIDAR��VE���ł̓���X�P�[��
#define MASS_SCALE		1000			// ���̂̎��ʂ̃X�P�[��
									// ���̓�̃X�P�[���͉��z���E�ƌ������E�̃X�P�[�������킹��̂Ɏg����

#define POINTER_RADIUS 0.1f			// �|�C���^�̔��a
#define EPSILON 2.0					// �|�C���^�ɐڐG�������ȍ��̂�\�����邽�߂Ƀ|�C���^��BBox��c��܂��ĐڐG���������Ƃ��̖c��܂���{��
									// ���Ƃ��Ă�d = (EPSILON - 1) x POINTER_RADIUS������̍��̂��������ĐڐG���_�����
									// �傫������قǂ�������̐ڐG��\�z�ł��邪�A���̕��ʂ�������̂Ōv�Z���d���Ȃ�
									// 1�ɂ���Ɨ\���Ȃ�

#define LIMIT_DEPTH 100				// �\���V�~�����[�V�������s�����̎擾�̐[�����
#define NUM_PREDICT_ITERATE 15		// �\���V�~�����[�V�����̃C�e���[�g��

#define NUM_COLLISIONS 20			// �|�C���^�ւ̋��e�ڐG��
#define NUM_COL_SOLIDS 20			// �|�C���^�ւ̋��e�ڐG���̐��@
									// NUM_COLLISIONS�Ƌ�ʂ���̂̓v���O������ǂ݂₷�����邽�߁B�����I�ȑ��݈Ӌ`�͂Ȃ�

#define COLLISION_DENSITY 0.4		// �͊o�̌v�Z�Ɏg�p����ڐG�_�̕��z�ʂ𒲐߂���l
									// �ڐG��O�񂩂�����p�����ꍇ�ڐG�_�͂��̒l���傫�������ŕ��z����

#define DISTANCE_LIMIT    0.3		// �ڐG�_�����ꂽ�Ƃ��ɐڐG���������邩�����肷�鋗��
									// �����苗�����傫���Ȃ����ꍇ�͂��̐ڐG�͂Ȃ��Ȃ������̂Ƃ݂Ȃ�
									// ������]�ɂ��딻���h���@�\

#define PCORRECTIONRATE    0.5		// �|�C���^�ɒ��ɐڂ��Ă��鍄�̂̎���Ŕ��������ڐG��\���V�~�����[�V��������Ƃ���
									// �ݒ肷��߂荞�ݕ␳�p�����[�^
									// �����傫������Ƃ߂荞�݂͂Ȃ��Ȃ邪�s����i���U�j���N����
									// ����������Ƃ߂荞�݂��������邪����ɂȂ�̂�
									// �|�C���^�ɒ��ɐڂ��鍄�̂Ɋւ��Ă͂��̌W���������邱�Ƃɂ���

#ifdef _DEBUG
	int SIMULATION_FREQ	= 60;		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		500		// �͊o�X���b�h�̎���Hz
	float Km = 20;//100;					// virtual coupling�̌W��
	float Bm = 0;					// ���i

	float Kr = 1;					// ��]
	float Br = 0;

#elif _WINDOWS
	int SIMULATION_FREQ = 60;		// �V�~�����[�V�����̍X�V����Hz
	#define HAPTIC_FREQ		1000	// �͊o�X���b�h�̎���Hz
	float Km = 20;//100;					// virtual coupling�̌W��
	float Bm = 0;					// ���i

	float Kr = 1;					// ��]
	float Br = 0;
#endif

// �V�~�����[�V�������g���̐؂�ւ��Ɏg���o�b�t�@
int SIMULATION_FREQ_BUFFER = SIMULATION_FREQ;

// �񎦗͂ƍ��̂ɒ񎦂���͂𒼐ڕω�������萔
double FORCE_COEFF =		1.0;

#ifdef _WIN32		//	Win32��(���ʂ͂�����)
	#include <Device/DRUsb20Simple.h>
	#include <Device/DRUsb20Sh4.h>
	#include <conio.h>
#endif


typedef PTM::TMatrixCol<NUM_COLLISIONS, 6, double> MatrixN6d;
typedef PTM::TMatrixCol<6, NUM_COLLISIONS, double> Matrix6Nd;
typedef PTM::TVector<NUM_COLLISIONS, double> VecNd;

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
bool bForce = false;

// �Ǐ��I���͊w�v�Z
bool bLocalDynamics = true;

// debug�o�͗pfstream
ofstream ofs;
bool bOutput = false;

// �V�~�����[�V���������邩�ǂ���������킷�t���O
// mmtimer�œ���HapticRendering�����Ԃ��J�E���g����
// �K�؂Ȏ�����ON�ɂ���
// idle function���V�~�����[�V�������s��
bool bSimulation = true;

// ���͂̉e����L���ɂ��邩�ǂ��������߂�t���O
bool bSurroundEffect = true;

// SPIDAR�̈ʒu��\�����邩�ǂ���
bool bDisplayPointer = false;

// �ڐG�_��`�悷�邩�ǂ���
bool bDisplayCollision = false;

// �ڐG�\�z���g�����ǂ���
bool bPredictCollision = false;

// K��B�̒l�ǂ���̕ύX��L���ɂ��邩
// true�̏ꍇ���i
// false�̏ꍇ��]
bool bMode = true;

// �ڐG���_�̏����i�[����\����
typedef struct{
	int pointer_index;
	PHSolid* solid[2];
	Vec3d closestPoint[2];
	Vec3d normal;
	Spr::PHShapePairForLCP* shapePair;
} CandidateInfo;

// �ڐG�\�z�̎��Ɏg���\����
struct Edge{
		float edge;				///<	�[�̈ʒu(�O���[�o���n)
		int	index;				///<	���� solid�̈ʒu
		bool bMin;				///<	���[�Ȃ�true
		bool operator < (const Edge& s) const { return edge < s.edge; }
	};
typedef std::vector<Edge> Edges;

// ���̍��̂Ɋւ�����͂̉e�����i�[����\����
typedef struct{
	SpatialVector constant_effect;
	vector<pair<Matrix3d, Matrix3d> > surround_effect;
} SEData;

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

	// constraint->solid�Ɠ���
	// constraint��integrate�ŏ����Ă��܂��̂�
	// solid�����łق��񂵂Ă����āA
	// �����ڐG�����Ƃ��Ɏg���悤�ɂ���
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

	// �ڐG�ɂ���đ��̐ڂ��鍄�̂��ǂ̂悤�ɓ����̂����i�[����ϐ�
	// �e�����ƁE�萔��ۑ�
	// ErrorCorrection�Ŏg��
	vector<SEData> relative_solids_velocity;
	set<PHSolid *> relative_solids;

	// ������ꂽ�͂̑��a���i�[����ϐ�
	Vec3d ForceSum[NUM_COLLISIONS];

	// �d�͂��K������Ă��邩�ǂ���
	bool bGravity[NUM_COL_SOLIDS];

	// ���ۂɐڐG���Ă��邩����킷�t���O
	// MakeHapticInfo�őO��ڐG���Ă����ꍇ�A�̔��f�Ɏg��
	bool bCollide[NUM_COLLISIONS];

	// ���[���h���W�n���獄�̍��W�n�ɒ����S����
	// PHConstraint::Xj.Conjugated()�Ɠ���
//	Quaterniond wldTosld[NUM_COLLISIONS];

	// �ڐG�ɗ͂�������Ƃ��̐N���ʂɔ�Ⴕ���d��
	double weight[NUM_COLLISIONS];

	Vec3d pointer_pos[NUM_COLLISIONS];
	Quaterniond pointer_ori[NUM_COLLISIONS];

	// �X�V���ԕ�����
	double HSratio;
	double SHratio;
	double Hdt;
	double Sdt;
} HapticInfo;

// pointer data
Vec3d pointer_pos;
Quaterniond pointer_ori;
Vec3d pointer_vel;
Vec3d pointer_angvel;

// ���͂̉e�����v�Z����Ƃ��Ɏg����
// ���͂̉e�����v�Z����Ƃ���
// ���̎��_��HapticRendering�ŉ������Ō�̗͂��g���ċ��߂�
Vec3d LastVCForce = Vec3d();
Vec3d TestForce = Vec3d(1, 1, 1);

// �P�ƂQ��p�ӂ���̂̓X���b�h�ŕK�v�Ȕr���A�N�Z�X������i�҂����������邽�߁j�A
// �e�v���Z�X�̍X�V���x��ۂ���
HapticInfo info1;
HapticInfo info2;

// �d�͂��K�p����Ă��邩�ǂ������i�[����}�b�v
// �{����PHSolid��bGravity������΂�����
// �Ƃ肠����set�ł��
// �o�^����Ă����,����͏d�͂�K�p���Ȃ�����
set<PHSolid*> bGravitySet;

// true�̂Ƃ���info�P���Q��
// false�̂Ƃ���info�Q���Q��
bool current_valid_data = true; 

#if defined EXP1
	// �o�͗p�o�b�t�@
	vector<Vec3d> Buffer;
	int num_data;
	bool bExperiment = false;
	Vec3d pos_array[] = {Vec3d(0.0, 0.0, 0.0), Vec3d(7.0, 0.0, 0.0)};
#elif defined EXP2
	double mass;
	int session;
	bool bTest = false;
#endif

// �֐��̃v���g�^�C�v�錾
//////////////////////////////////////////////////////////////////////////////
// ���������쐬���鏈��
void CalcSurroundEffect(HapticInfo*, HapticInfo*);

// �e���̂��鍄�̂̏W�����擾���Ă��鏈��
void GetSolidsCollisionsOnPointer(vector<pair<PHConstraint *, int> >*, multimap<double, PHContactPoint*> ,
						  set<PHConstraint *>*, set<PHSolid *>*);
void GetAllRelativeSolidsCollisions(set<PHConstraint*>*, set<PHSolid*>*, set<PHSolid*>, set<PHConstraint*>*);
void RecursiveSolidsCollisionsRetrieval(multimap<double, pair<PHConstraint *, bool> >*, PHSolid*, 
										set<PHConstraint *>*, set<PHSolid *>*, set<PHSolid*>, set<PHConstraint*>*, int);

// �ڐG���_���쐬���鏈��
void CreateCandidateContactPoints(vector<pair<PHConstraint*, int> >, 
								  vector<pair<PHConstraint*, int> >*, 
								  set<PHConstraint *>*, set<PHSolid*>*,
								  map<PHContactPoint*, Vec3d>*, HapticInfo*);
void GetCandidateSolidsByBBox(set<int> *, int *);
void GetWillCollidePoints(int, int, vector<CandidateInfo>*);
void CreateShapePairMMap(vector<pair<PHConstraint *, int> >, multimap<PHSolid*, Spr::PHShapePairForLCP*>*);

// �\���V�~�����[�V���������鏈��
void PredictSimulations(vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint *, int> >, vector<pair<PHConstraint *, int> >,
						set<PHSolid*>, set<PHConstraint*>, 
						set<PHSolid*>, set<PHConstraint*>,
						map<PHSolid*, SEData>*,
						vector<SEData>*);
void SetupPredictSimulation(set<PHConstraint*>, set<PHSolid*>, set<PHSolid*>, set<PHConstraint *>, map<PHSolid*, Vec3d>*);
void AddForceToConstraint(int, PHConstraint*, PHSolid*, map<PHSolid *, Vec3d>, Vec3d);
vector<SpatialVector> PredictSimulation(vector<SpatialVector>*,
										set<PHSolid*>, set<PHConstraint *>,
										set<PHConstraint *>, set<PHSolid*>);
vector<SpatialVector> PredictSimulationToCollision(vector<SpatialVector>*,
										PHConstraint*, PHSolid*,
										set<PHConstraint *>, set<PHSolid *>,
										set<PHConstraint *>, set<PHSolid *>,
										Vec3d, map<PHSolid *, Vec3d>, int index = -1);
// �ڐG�_�������p������
void CreateConstraintFromCurrentInfo(HapticInfo*, vector<pair<PHConstraint *, int> >*, map<PHConstraint*, int>*,
									 set<PHSolid*>*, set<PHConstraint *>*, bool);

// ����ꂽ�����������ׂĂ܂Ƃ߂鏈��
void MakeHapticInfo(HapticInfo*, HapticInfo*, vector<pair<PHConstraint*, int> >, 
					vector<pair<PHConstraint*, int> >, map<PHContactPoint*, Vec3d>,
					vector<pair<PHConstraint*, int> >, 
					map<PHConstraint*, int>, map<PHSolid*, SEData>,
					vector<SEData>, set<PHSolid *>, bool);
PHConstraint* GetSpecificDataFromCollisions(int i, 
					 vector<pair<PHConstraint*, int> >, vector<pair<PHConstraint*, int> >, 
					 map<PHContactPoint*, Vec3d>, vector<pair<PHConstraint*, int> >,  
					 int*, PHSolid**, Vec3d*, bool*, bool*);
void RegisterNewSolid(HapticInfo*, int, HapticInfo*, PHSolid*, map<PHSolid*, SEData>);
void RegisterNewCollision(HapticInfo*, int i, HapticInfo*, PHConstraint*, int, bool, bool, map<PHConstraint*, int>, Vec3d, bool);


////////////////////////////////////////////////////////////////////////////////////
// �͊o�v�Z�ł̏���
#ifdef EXP1
void HapticRendering();
#else 
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
#endif
// �f�[�^��؂�ւ��鎞�ɍs������
void UpdateNewInfoFromCurrent();

// �߂荞�݂�SPIDAR�̈ʒu����|�C���^�̈ʒu���v�Z���鏈��
void CheckPenetrations(HapticInfo*, int*, vector<Vec3d>*, vector<double>*, vector<Vec3d>*);
void CalcTTheta(HapticInfo*, Vec3d*, Vec3d*);
void CorrectPenetration(HapticInfo*);
inline void GetParametersOfCollision(double*, double*, double*, double*, double*, double*, double*);
void MakePointerPosMatrix(int, vector<Vec3d>t, vector<double>, vector<Vec3d>, MatrixN6d*, VecNd*);

// �Ǐ��I�ȓ��͊w�v�Z���s������
void UpdateVelocityByCollision(HapticInfo*, Vec3d, bool*);
inline void UpdateSolidByCollision(HapticInfo *, Vec3d, bool*);
void UpdateSolids(HapticInfo*);

// �����ȃo�[�`�����J�b�v�����O
void VirtualCoupling(HapticInfo*, Vec3d*, Vec3d*, bool*);

// �o�l�_���p���f������͂��v�Z����֐�
inline void calcForceBySpringDumper(Vec3d*, Vec3d*);

// �ڐG�_�ɉ�����͂̏d�݂��v�Z���鏈��
void CalcForceWeight(HapticInfo*);

// �v��I�ɗ͊o�v�Z�ƕ����V�~�����[�V�������s������
inline void scheduleSimulationFromHaptic();

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
void SaveCollisionF(set<PHConstraint *>, set<PHConstraint *>);
void SaveSolidDv(set<PHSolid*>, set<PHSolid*>);
void SaveSolidV(set<PHSolid*>);
void SaveSolidF(set<PHSolid*>);
void RestoreParameters(set<PHConstraint*>*, set<PHSolid*>*, set<PHConstraint*>*, set<PHSolid*>*);


////////////////////////////////////////////////////////////////////////////////////
// ���[�e�B���e�B
void ShowCollisionInfo();
inline void t_start();
inline int t_end();
void CalcPinv(MatrixN6d, Matrix6Nd*);
void UpdateSpidarData(Vec3d*, Quaterniond*);
void DisplayPointer();
void DisplayCollisions();
inline PHSolid* GetAdjacentSolid(PHConstraint*, PHSolid*, int* sign = NULL);
PHContactPoint* CreateContactPoint(Vec3d, Vec3d, PHSolid*, PHSolid*, Spr::PHShapePairForLCP*);
void ResetOriginalContactPoints(multimap<double, PHContactPoint*>*);
void AddInactiveSolid(PHSolidIf*);
void InitDeviceManager();
void InitHapticInfo(HapticInfo*);
void updatePenetrations(HapticInfo* info);
inline double calcDistance(Vec3d, Vec3d);
inline void updateSimulationFrequency();
inline void calcTimeSpan(HapticInfo* info);

///////////////////////////////////////////////////////////////////////////////////////////////
// ���͂̉e�����v�Z���邽�߂̏���
inline void SaveParameters(set<PHConstraint*> relative_consts, set<PHSolid*> relative_solids, set<PHConstraint *> nearest_consts, set<PHSolid *> nearest_solids)
{
	SaveCollisionF(relative_consts, nearest_consts);
	SaveSolidDv(relative_solids, nearest_solids);
//	SaveSolidV(relative_solids);
//	SaveSolidF(relative_solids);
}

// ���͂̉e�����v�Z���A������HapticInfo�Ɋi�[����֐�
void CalcSurroundEffect(HapticInfo* new_info, HapticInfo* current_info)
{
	// �|�C���^�Ɋւ���ڐG���i�[����ϐ�
	vector<pair<PHConstraint *, int> > pointer_consts;

	// �ڐG�\���p�̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > col_candidate_consts;
	map<PHContactPoint*, Vec3d> col_candidate_pointer_pos;

	// �O��̐ڐG�������p�����߂̃f�[�^���i�[����ϐ�
	vector<pair<PHConstraint *, int> > current_consts;

	// �V����������ڐG�ƌ��݂̐ڐG���֘A�t����}�b�v
	map<PHConstraint *, int> NewcolToCol;

	// �|�C���^�ƒ��Ɋ֌W�̂��鍄�́E�ڐG
	set<PHSolid *> nearest_solids;
	set<PHConstraint *> nearest_consts;

	// ���ڂł͂Ȃ����֌W�̂��鍄��,�ڐG���i�[����ϐ�
	set<PHSolid *> relative_solids;
	set<PHConstraint *> relative_consts;

	// ���ڂƊԐڂŃ|�C���^�Ɋ֌W������͂̉e�����i�[����ϐ�
	map<PHSolid *, SEData> se_map;
	vector<SEData> relSE;

	// �f�[�^����̓r���Ńt���O���ύX�����ƍ���̂�
	// ���̎��_�ł̃t���O�̒l��ۑ����Ă�����
	// ���̊֐����ł͂����M���邱�Ƃɂ���
	bool bLocalDynamics_local = bLocalDynamics;
	bool bPredictCollision_local = bPredictCollision;

	// ����̐ڐG���폜����
	ResetOriginalContactPoints(&(current_info->points));

	// ���ݎg���Ă���ڐG�̂����ŁA�����g�������ȐڐG�����o���֐�
	CreateConstraintFromCurrentInfo(current_info, &current_consts, 
									&NewcolToCol, &nearest_solids,
									&nearest_consts,
									bLocalDynamics_local);

	// �|�C���^�ɐڐG���Ă���ڐG�ƍ��̂��擾���Ă���֐�
	GetSolidsCollisionsOnPointer(&pointer_consts, current_info->points,
								&nearest_consts, &nearest_solids);

	// ���͂̍��̂ɂ��e���̌v�Z
	if(bLocalDynamics_local)
	{
		// �ڐG���_��T���č쐬���鏈��
		if(bPredictCollision_local) 
			CreateCandidateContactPoints(pointer_consts, 
										&col_candidate_consts, 
										&nearest_consts, &nearest_solids, 
										&col_candidate_pointer_pos, current_info);

		// ���͂̉e�����v�Z���邽�߂̃f�[�^�쐬����
		// �֌W�̂��鍄�̂ƐڐG���ċA�I�ɂ��ׂĂƂ��Ă���
		GetAllRelativeSolidsCollisions(&relative_consts, &relative_solids, nearest_solids, &nearest_consts);

		// �͂������Ă݂ē������A�e�����ϑ�����֐�
		PredictSimulations(pointer_consts, col_candidate_consts, current_consts, 
							nearest_solids, nearest_consts, 
							relative_solids, relative_consts,
							&se_map, &relSE);
	}

	// �ڐG�̏����쐬����
	MakeHapticInfo(new_info, current_info, 
					pointer_consts,  
					col_candidate_consts, col_candidate_pointer_pos,
					current_consts,  
					NewcolToCol, 
					se_map, relSE,
					relative_solids, bLocalDynamics_local);
}

// ���݂̏�񂩂玟���g���ڐG���쐬���鏈��
void CreateConstraintFromCurrentInfo(HapticInfo* current_info, vector<pair<PHConstraint *, int> > *current_consts, map<PHConstraint *, int>* NewcolToCol,
									set<PHSolid *> *nearest_solids, set<PHConstraint *> *nearest_consts, bool bLocalDynamics_local)
{
	// ���ݎg�p���Ă���f�[�^����
	// ���ۂɐڐG���Ă������
	// �ڐG���Ă��Ȃ����̂𒲂�
	// ���̃f�[�^�����Ɏg�����ǂ������肷��
	for(int i = 0; i < current_info->num_collisions; ++i)
	{
		// ���ۂɂ͐ڐG���Ă��Ȃ��ڐG
 		if(!current_info->bCollide[i]) continue;

		int sol_index = current_info->ColToSol[i];

		// �o�[�`�����J�b�v�����O�̏ꍇ��
		// IsDynamical == false�̏ꍇ�̂ݐڐG�_���p������
//		if(!bLocalDynamics_local && current_info->nearest_solids[sol_index]->IsDynamical()) continue;

		// ���ݎg�p���Ă���Contact Point(=constraint)�̂قƂ�ǂ�step���Ă΂�邽�тɍ폜�����̂ŁA
		// ����V����Contact Point�����K�v������
		// �����ł͌��݂̈ʒu�ƌ�����������Contact Point��V���ɍ쐬����
		// �����ō��ꂽContact Point�� hapticinfo->points �Ɋi�[����A�蓮�ŏ���
		// sign�����������ǂ����đO�̃X�e�b�v��sign�̏�Z��ł�������contact point���쐬
//		PHContactPoint* point = CreateContactPoint(current_info->col_normals[i] * current_info->sign[i], (current_info->col_positions[i] + current_info->pointer_col_positions[i]) / 2, current_info->solid[i][0], current_info->solid[i][1], current_info->shapePair[i]);
		PHContactPoint* point = CreateContactPoint(current_info->col_normals[i] * current_info->sign[i], current_info->col_positions[i], current_info->solid[i][0], current_info->solid[i][1], current_info->shapePair[i]);

		// ���݂̃|�C���^���̐ڐG�_���i�[����
		NewcolToCol->insert(pair<PHConstraint *, int>(point, i));

		int sign;
		PHSolid* so = GetAdjacentSolid(point, DCAST(PHSolid, soPointer), &sign);

		// ��������ڐG�̓o�^
		current_consts->push_back(pair<PHConstraint *, int>(point, sign));

		nearest_consts->insert(point);
		nearest_solids->insert(so);

		// ���̉�ō폜�Ɏg�����߂Ƀ����N��ۑ����Ă���
		current_info->points.insert(pair<double, PHContactPoint*>(point->pos.z, point));
	}
}

// �|�C���^���܂ސڐG�Ƃ��̍��̂����ׂĎ擾���Ă���֐�
void GetSolidsCollisionsOnPointer(vector<pair<PHConstraint *, int> >* pointer_consts, multimap<double, PHContactPoint*> points,
						  set<PHConstraint *>* nearest_consts, set<PHSolid *>* nearest_solids)
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
			nearest_solids->insert(col_solid);

			PHContactPoint* contact = DCAST(PHContactPoint, *it);
			
			// �����p���ڐG�ƐV�������ڐG�̈ʒu���߂�����ꍇ��
			// �V�����ڐG�͒ǉ����Ȃ�
			// ���łɈ����p���ڐG�����̑���Ƃ��čl������̂ŁA
			// ���̏����̕����͍l����K�v�͂Ȃ�
			// BoundingBox�I�ȍl�����Ōv�Z�Ώۂ��i��
			bool bAdd = true;
			for(multimap<double, PHContactPoint*>::iterator it2 = points.lower_bound(contact->pos.z - COLLISION_DENSITY); it2 != points.upper_bound(contact->pos.z + COLLISION_DENSITY); ++it2)
			{
				// shapePair�������Ȃ瓯���ڐG�_�̃O���[�v���ƍl����
				if(contact->shapePair != (*it2).second->shapePair) continue;

				// �ǉ����悤�Ƃ���ڐG�_�̋����i�x�N�g���̃m�����j��
				// ����������ꍇ�͒ǉ����Ȃ�
				double norm = (contact->pos - (*it2).second->pos).norm();
				if(norm < COLLISION_DENSITY)
				{
					bAdd = false;
					break;
				}
			}

			if(bAdd)
			{
				pointer_consts->push_back(pair<PHConstraint *, int>(*it, sign));
				nearest_consts->insert(*it);
			}
		}
	}
}

// �|�C���^�Ɛڂ��Ă��鍄�̂���X�^�[�g���āA�֌W�̂���ڐG�ƍ��̂����ׂĂƂ��Ă���֐�
void GetAllRelativeSolidsCollisions(set<PHConstraint*> *relative_consts, set<PHSolid*>* relative_solids, 
									set<PHSolid*> nearest_solids, set<PHConstraint*>* nearest_consts)
{
	// �ڐG�_���擾
	PHConstraints cs = scene->GetConstraintEngine()->GetContactPoints();
	multimap<double, pair<PHConstraint *, bool> > csm;

	// �v�Z���Ԃ�Z�k���邽�߂ɐڐG�ɏ����σt���O�����ĕۑ�����
	// �ڐG�������Ȃ��ꍇ�͖��ʂ��������A
	// �ڐG���������Ȃ����Ƃ��̐L�т͂�����̂ق����f�R���Ȃ��͂�
	// �����z����bounding box�Ō����͈͂����߂�
	for(PHConstraints::iterator it = cs.begin(); it != cs.end(); ++it)
	{
		csm.insert(pair<double, pair<PHConstraint *, bool> >(DCAST(PHContactPoint, *it)->pos.z, pair<PHConstraint *, bool>(*it, false)));
	}

	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		// ���̍��̂Ɛ������ڐG�����̍��̂ɉe�����y�ڂ��\��������ꍇ�́A���̍��̂ɂ�����
		if((*it)->IsDynamical())
			RecursiveSolidsCollisionsRetrieval(&csm, *it, relative_consts, relative_solids, nearest_solids, nearest_consts, 0);
	}
}

// �^����ꂽ���̂ɒ��ɐڂ��Ă��鍄�̂ƁA�ڐG���擾����֐�
void RecursiveSolidsCollisionsRetrieval(multimap<double, pair<PHConstraint *, bool> >* csm, PHSolid* solid, 
										set<PHConstraint *>* relative_consts, set<PHSolid *>* relative_solids, 
										set<PHSolid*> nearest_solids, set<PHConstraint *>* nearest_consts, int depth)
{
	// z���Ɋւ���bounding box�Ō����͈͂����߂�
	for(multimap<double, pair<PHConstraint *, bool> >::iterator it = csm->lower_bound(solid->GetFramePosition().z - 1.5); it != csm->upper_bound(solid->GetFramePosition().z + 1.5); ++it)
	{
		// �����ς̏ꍇ�͎��ɂ�����
		if((*it).second.second) continue;

		// �^����ꂽ���̂���A���̍��̂ɐڐG���鍄�̂��擾����
		PHSolid* col_solid = GetAdjacentSolid(it->second.first, solid);

		// �������̍��̂����݂����ꍇ
		if(col_solid && col_solid != DCAST(PHSolid, soPointer))
		{
			// �����ς݂ɂ���
			it->second.second = true;

			// �ڐG���̂�ǉ�
			PHConstraint* c = it->second.first;
			
			// �|�C���^�ƒ��ɐڂ��Ă��鍄�̏�ɔ��������ڐG��
			// nearest_consts�Ɋi�[����
			// nearest_consts�͗\���V�~�����[�V�����̎���
			// correctionRate��Ⴂ�l�ɐݒ肵�čs���̂�
			// �|�C���^�ɒ��ɐڂ��鍄�̂�
			// �����̂߂荞�݂�����������ɓ���ł���
			if(depth)relative_consts->insert(c);
			else nearest_consts->insert(c);

			// PHSolid��ۑ�
			// �|�C���^�ɒ��ɐڂ��Ă��鍄�̂łȂ���Βǉ�����
			// �d�����Ă���ꍇ��set�Ȃ̂Ŏ����I�ɔr�������
			if(nearest_solids.find(c->solid[0]) == nearest_solids.end()) relative_solids->insert(c->solid[0]);
			if(nearest_solids.find(c->solid[1]) == nearest_solids.end()) relative_solids->insert(c->solid[1]);
			
			// �����Ȃ����̂ł͂Ȃ��A���擾����܂ŒB���Ă��Ȃ������ꍇ�A���̍��̂ɂ�����
			if(col_solid->IsDynamical() && depth < LIMIT_DEPTH) 
				RecursiveSolidsCollisionsRetrieval(csm, col_solid, relative_consts, relative_solids, nearest_solids, nearest_consts, depth+1);
		}
	}
}

// �|�C���^�ɐڐG�������ȍ��̂�T���A�ڐG���Ă��Ȃ���ΐڐG���쐬����֐�
void CreateCandidateContactPoints(vector<pair<PHConstraint *, int> > pointer_consts, 
								  vector<pair<PHConstraint *, int> >* col_candidate_consts, 
								  set<PHConstraint *> *nearest_consts, set<PHSolid *>* nearest_solids, 
								  map<PHContactPoint*, Vec3d>* col_candidate_pointer_pos, HapticInfo* current_info) 
{
	// ���łɓ���ꂽ�ڐG�_����shapePair���쐬���鏈��
	// �ڐG���_���o�^���ꂽ�ڐG�Ƃ��Ă��łɑ��݂��邩�ǂ����̔���Ɏg��
	multimap<PHSolid*, Spr::PHShapePairForLCP*> shapePairMMap;
	CreateShapePairMMap(pointer_consts, &shapePairMMap);

	// BBox���g���ă|�C���^�ƐڐG�������ȍ��̂��擾���Ă���
	int pointer_index;
	set<int> candidate_solid_indices;
	GetCandidateSolidsByBBox(&candidate_solid_indices, &pointer_index);

	// �ڐG�������ȍ��̂�shape���x���Ŗ{���ɐڐG���邩���ׂ�
	for(set<int>::iterator it = candidate_solid_indices.begin(); it != candidate_solid_indices.end(); ++it)
	{		
		// ���ۂɈʒu�֌W���v�Z���ĐڐG���������ǂ����𔻒�
		// �ڐG�������ȏ����擾���Ă���
		vector<CandidateInfo> candidates;
		GetWillCollidePoints(*it, pointer_index, &candidates);

		// �ڐG�������ȏ��̂�����
		// ���łɐڐG�Ƃ��ď�������Ă��邩�ǂ����𔻒�
		// �܂���������Ă��Ȃ���ΐV�����ڐG���쐬����
		for(int i = 0; i < (int)candidates.size(); ++i)
		{
			bool bExist = false;
			CandidateInfo cinfo = candidates[i];

			pair<multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator, multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator> it 
				= shapePairMMap.equal_range(cinfo.solid[1 - cinfo.pointer_index]);
			for(multimap<PHSolid*, Spr::PHShapePairForLCP*>::iterator it2 = it.first; it2 != it.second; ++it2)
			{
				// �ڐG�������ȍ��̂��L�[�Ƃ��Ă��łɓo�^���ꂽ�ڐG����������
				// ����������shapePair�����������
				// so && soPointer�@�ł����
				// so shape && pointer shape �ƂȂ�̂œ����ڐG�Ƃ݂Ȃ���
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
				nearest_consts->insert(point);
				// �߂�����
				nearest_solids->insert(cinfo.solid[1 - cinfo.pointer_index]);
				// �|�C���^���̐ڐG�_�ۑ�
				col_candidate_pointer_pos->insert(pair<PHContactPoint*, Vec3d>(point, cinfo.closestPoint[cinfo.pointer_index]));
				// �폜�p�Ƀ����N��ۑ�
				current_info->points.insert(pair<double, PHContactPoint*>(cinfo.closestPoint[1 - cinfo.pointer_index].z, point));

				int sign;
				GetAdjacentSolid(point, DCAST(PHSolid, soPointer), &sign);

				col_candidate_consts->push_back(pair<PHConstraint*, int>(point, sign));
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
	// BBox��c��܂��Ă���̂�
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

// bounding box�̔��������
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
	double dt = scene->GetTimeStep();
	for(int i = 0; i < 2; i++){
		deltaPos[i].resize(solid[i]->NShape());
		shapePose[i].resize(solid[i]->NShape());
		for(int j = 0; j < solid[i]->NShape(); j++){
			CDConvex* convex = DCAST(CDConvex, solid[i]->GetShape(j));
			Posed lp = solid[i]->GetShapePose(j);
			Vec3d c = lp * convex->GetCenter();
			if(solid[i] == DCAST(PHSolid, soPointer)) deltaPos[i][j] = (pointer_vel * dt) + (pointer_angvel ^ (c - solid[i]->GetCenterPosition())) * dt;
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
			int res = ContFindCommonPoint(DCAST(CDConvex, solid[0]->GetShape(i)), 
										DCAST(CDConvex, solid[1]->GetShape(j)), 
										shapePose[0][i], shapePose[1][j], 
										delta, normal, closestPoint[0], closestPoint[1], distance);

			// ���������ڐG����\��������
			if(res > 0)
			{
				// �����𒲂ׂ�
				double toi = distance / (delta * delta);
				if(toi >= 0 && toi <= POINTER_RADIUS * (EPSILON - 1))
				{
					// �ŋߖT�̓_�𒲂ׂ�
					FindClosestPoints(DCAST(CDConvex, solid[0]->GetShape(i)), 
									DCAST(CDConvex, solid[1]->GetShape(j)), 
									shapePose[0][i], shapePose[1][j], closestPoint[0], closestPoint[1]);

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
						cinfo.normal = cinfo.closestPoint[1] - cinfo.closestPoint[0]; // - normal;

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
void CreateShapePairMMap(vector<pair<PHConstraint *, int> > pointer_consts, multimap<PHSolid*, Spr::PHShapePairForLCP*>* shapePairMMap)
{
	// ���łɓ���ꂽ�|�C���^����̐ڐG����
	// �ڐG�Ώۂ̍��̂ƃ|�C���^�Ƃ�shapePair��������悤�ɂ���
	int size = (int)pointer_consts.size();
	for(int i = 0; i < size; ++i)
	{
		PHConstraint *consts;
		consts = pointer_consts.at(i).first;			
		shapePairMMap->insert(pair<PHSolid*, Spr::PHShapePairForLCP*>(GetAdjacentSolid(consts, DCAST(PHSolid, soPointer)), 
																	DCAST(PHContactPoint, consts)->shapePair));
	}
}

// ��Ď�@�̂��߂ɐ摗��V�~�����[�V�������v��I�ɍs���Ă����֐�
void PredictSimulations(vector<pair<PHConstraint *, int> > pointer_consts, vector<pair<PHConstraint *, int> > col_candidate_consts, vector<pair<PHConstraint *, int> > current_consts, 
						set<PHSolid *> nearest_solids, set<PHConstraint *> nearest_consts,
						set<PHSolid *> relative_solids, set<PHConstraint *> relative_consts,
						map<PHSolid *, SEData>* SEMap, vector<SEData>* relSE)
{
	double dt = scene->GetTimeStep();

	map<PHSolid *, Vec3d> coeff;
	// �\���V�~�����[�V�����̃Z�b�g�A�b�v
	// �K�v�ȏ��̏���
	SetupPredictSimulation(relative_consts, relative_solids, nearest_solids, nearest_consts, &coeff);

	// ���̎��͂̕��̂̓�����ۑ�����ϐ�
	vector<SpatialVector> relative_solid_velocity_const;
	// ������͂��O�x�N�g���Ƃ��Ē萔�����擾
	vector<SpatialVector> b = PredictSimulationToCollision(&relative_solid_velocity_const,
															NULL, NULL,
															relative_consts, relative_solids, 
															nearest_consts, nearest_solids,
															Vec3d(), coeff);

	// �|�C���^�ɒ��ɐڂ��鍄�̂́A�����͂������Ȃ������ꍇ�̓�����ۑ����鏈��
	// ���̐�����SEData��錾����map�ɒǉ�����
	int local_counter = 0;
	map<PHSolid*, SEData> se_map;
	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		SEData se;
		se.constant_effect = SpatialVector((*it)->GetOrientation() * b[local_counter].v(),
										   (*it)->GetOrientation() * b[local_counter].w());
		se_map.insert(pair<PHSolid*, SEData>((*it), se));
		++local_counter;
	}

	// �|�C���^�ɊԐړI�ɐڐG���鍄�̂̉����͂������Ȃ������Ƃ��̓�����ۑ����鏈��
	// �֌W���镨�̂̒萔���������[���h���W�n�ɒ����ĕۑ�
	vector<SEData> relative_se;
	local_counter = 0;
	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		SEData se;
		se.constant_effect = SpatialVector((*it)->GetOrientation() * relative_solid_velocity_const[local_counter].v(),
										   (*it)->GetOrientation() * relative_solid_velocity_const[local_counter].w());
		relative_se.push_back(se);
		++local_counter;
	}
	
	bool bCol, bPrevCol;
	PHSolid* solid;
	int sign;
	Vec3d vv;

	// �ڐG�����񐔂����A�͂������ē������ϑ����J��Ԃ�
	vector<SpatialVector> relative_solid_velocity_x, relative_solid_velocity_y, relative_solid_velocity_z;
	int total_size = (int)pointer_consts.size() + (int)col_candidate_consts.size() + (int)current_consts.size();
	for(int i = 0; i < total_size; ++i)
	{
		// i�Ԗڂ̐ڐG�����o��
		PHConstraint* constraint = GetSpecificDataFromCollisions(i, 
																pointer_consts, col_candidate_consts, map<PHContactPoint*, Vec3d>(), 
																current_consts, &sign, &solid, &vv, &bCol, &bPrevCol);

		// ���͂̉e�����v�Z���ׂ�����
		if(solid->IsDynamical())
		{
			// ����P�ʃx�N�g���������Ă��̌��ʂ�����킷�x�N�g�����擾
			// �ȉ��̂R�̊֐��ł���ڐG�ɗ͂��������Ƃ���,���ׂĂ̍��̂̔������킩��
			vector<SpatialVector> vec_x = PredictSimulationToCollision(&relative_solid_velocity_x,
															constraint, solid,
															relative_consts, relative_solids, 
															nearest_consts, nearest_solids,
															Vec3d(TestForce.x, 0, 0), coeff, i);
			vector<SpatialVector> vec_y = PredictSimulationToCollision(&relative_solid_velocity_y,
															constraint, solid,
															relative_consts, relative_solids, 
															nearest_consts, nearest_solids,
															Vec3d(TestForce.x, TestForce.y, 0), coeff, i);
			vector<SpatialVector> vec_z = PredictSimulationToCollision(&relative_solid_velocity_z,
															constraint, solid,
															relative_consts, relative_solids, 
															nearest_consts, nearest_solids,
															Vec3d(TestForce.x, 0, TestForce.z), coeff, i);

			// ���̃��[�v�ō���so�̐ڐGi�Ɋւ��铮��������킷�s����v�Z���i�[
			// ���̐ڐG�̃|�C���^�ɐڂ��邷�ׂĂ̍��̂ɑ΂���e�������̃��[�v�ŏ�������
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
				Matrix3d v = Matrix3d(so->GetOrientation() * ((sv_x.v() - sv.v())   / TestForce.x), 
									  so->GetOrientation() * ((sv_y.v() - sv_x.v()) / TestForce.y), 
									  so->GetOrientation() * ((sv_z.v() - sv_x.v()) / TestForce.z));
				Matrix3d w = Matrix3d(so->GetOrientation() * ((sv_x.w() - sv.w())   / TestForce.x), 
									  so->GetOrientation() * ((sv_y.w() - sv_x.w()) / TestForce.y), 
									  so->GetOrientation() * ((sv_z.w() - sv_x.w()) / TestForce.z));
				
				// �f�[�^�̊i�[
				pair<Matrix3d, Matrix3d> p = pair<Matrix3d, Matrix3d>(v, w);

				// ���̂��ƂɐڐG�ƌW���s��̃}�b�v���쐬����
				// �ڐG�̏��Ԃ͂킩�邪���̂̏��Ԃ͂킩��Ȃ����߁B
				// ���̂��ƂɐڐG�̏��Ԃɍs�񂪊i�[����Ă���
				se_map.find(so)->second.surround_effect.push_back(p);
			}

			// ���̐ڐG�ɗ͂��������Ƃ��́A�|�C���^�Ƃ͒��ɐڂ��Ă��Ȃ����͂̍��̒B�̓������ώ@���ĕۑ�����
			local_counter = 0;
			for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
			{
				PHSolid *so = *it;
				SpatialVector sv = relative_solid_velocity_const[local_counter];
				SpatialVector sv_x = relative_solid_velocity_x[local_counter];
				SpatialVector sv_y = relative_solid_velocity_y[local_counter];
				SpatialVector sv_z = relative_solid_velocity_z[local_counter];

				// ��قǉ������P�ʗ͂���e�������߂čs����쐬����
				Matrix3d v = Matrix3d(so->GetOrientation() * ((sv_x.v() - sv.v())   / TestForce.x), 
									  so->GetOrientation() * ((sv_y.v() - sv_x.v()) / TestForce.y), 
									  so->GetOrientation() * ((sv_z.v() - sv_x.v()) / TestForce.z));
				Matrix3d w = Matrix3d(so->GetOrientation() * ((sv_x.w() - sv.w())   / TestForce.x), 
									  so->GetOrientation() * ((sv_y.w() - sv_x.w()) / TestForce.y), 
									  so->GetOrientation() * ((sv_z.w() - sv_x.w()) / TestForce.z));
				
				// �f�[�^�̊i�[
				pair<Matrix3d, Matrix3d> p = pair<Matrix3d, Matrix3d>(v, w);
				relative_se[local_counter].surround_effect.push_back(p);
				++local_counter;
			}
			relative_solid_velocity_x.clear();
			relative_solid_velocity_y.clear();
			relative_solid_velocity_z.clear();
		}
		// �����Ȃ����̂Ƃ̐ڐG�̏ꍇ
		else
		{
			// �|�C���^�ƒ��ڐڂ��Ă��鍄�̂̓���
			for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
			{
				se_map.find((*it))->second.surround_effect.push_back(pair<Matrix3d, Matrix3d>(Matrix3d(), Matrix3d()));
			}

			// �|�C���^�ɂ͒��ڐڂ��Ă��Ȃ����̂̓���
			local_counter = 0;
			for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
			{
				relative_se[local_counter].surround_effect.push_back(pair<Matrix3d, Matrix3d>(Matrix3d(), Matrix3d()));
				++local_counter;
			}
		}
	}
	
	// ���ʂ̊i�[
	*SEMap = se_map;
	*relSE = relative_se;
}

// �\���V�~�����[�V�������Z�b�g�A�b�v����
void SetupPredictSimulation(set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids, set<PHSolid *> nearest_solids, set<PHConstraint *> nearest_consts, map<PHSolid *, Vec3d> *coeff)
{
	double dt = scene->GetTimeStep();
	Vec3d gravity = scene->GetGravity();

	// ���ׂĂ̍��̂��Z�b�g�A�b�v
	for(set<PHSolid *>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		// �d�͂�������
		if((*it)->IsDynamical() && bGravitySet.find((*it)) == bGravitySet.end()) (*it)->AddForce(gravity * (*it)->GetMass());
		(*it)->UpdateCacheLCP(dt);
	}
	for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		// �d�͂�������
		if((*it)->IsDynamical() && bGravitySet.find((*it)) == bGravitySet.end()) (*it)->AddForce(gravity * (*it)->GetMass());
		(*it)->UpdateCacheLCP(dt);
	}

/*
	for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		PHSolid* solid = *it;

		// �P�ʗ͂������鏈���̂Ƃ���Ŏg���l(=�������͂����[���h���W����S�����W�ɕϊ��������)�����̂����Ɍv�Z���Ă���
		Vec3d w = solid->GetOrientation().Conjugated() * solid->GetAngularVelocity();
		Vec3d calc = w % (solid->GetInertia() * w);
		coeff->insert(pair<PHSolid *, Vec3d>(solid, calc));
	}
*/

	// ���ׂĂ̐ڐG�ɂ��ă��R�r�A�����v�Z
	// ���R�r�A���̌v�Z�͈�x�����ł悢
	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		(*it)->CompJacobian();
	}
	for(set<PHConstraint *>::iterator it = nearest_consts.begin(); it != nearest_consts.end(); ++it)
	{
		(*it)->CompJacobian();
	}

	// Setup���I��������_�ł̃p�����[�^��ۑ����Ă���
	// ���ɍs����PredictSimulation�ł�
	// RestoreParameters���ĂԂ��Ƃ�
	// ���̎��_�̃p�����[�^�ɃZ�b�g���Ȃ�����
	// �J��Ԃ��v�Z�ł���悤�ɂ���
	SaveParameters(relative_consts, relative_solids, nearest_consts, nearest_solids);
}

// ����ڐG�ɗ͂������ė\���V�~�����[�V��������֐�
vector<SpatialVector> PredictSimulationToCollision(vector<SpatialVector>* relative_solid_velocity, 
													PHConstraint* constraint, PHSolid* solid,
													set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids,
													set<PHConstraint *> nearest_consts, set<PHSolid *> nearest_solids,
													Vec3d force_vec, map<PHSolid *, Vec3d> coeff, int index)
{
	// �|�C���^���ڂ��鍄�̂̂���index�Ԗڂ̐ڐG�ɒP�ʗ͂�������
	if(index != -1) AddForceToConstraint(index, constraint, solid, coeff, force_vec);
	return PredictSimulation(relative_solid_velocity, nearest_solids, nearest_consts, relative_consts, relative_solids);
}

// �\���V�~�����[�V����������֐�
vector<SpatialVector> PredictSimulation(vector<SpatialVector>* relative_solid_velocity, 
										set<PHSolid *> nearest_solids, set<PHConstraint *> nearest_consts,
										set<PHConstraint *> relative_consts, set<PHSolid *> relative_solids)
{
	// �|�C���^�Ƃ̐ڐG�Ɋւ��Ă�,correctionRate�������邱�ƂŔ��U��h��
	// ��������Ƃ߂肱�݂͋N���邪,����ɍ��̂��^������
	// ���̂ق��̐ڐG�Ɋւ��Ă�correctionRate��ʏ�̒l�ɂ���
	// �s����ɂȂ邪�߂荞�݂��N����Ȃ��悤�ɂ���(�J����)
	double pos_correctionRate = scene->GetConstraintEngine()->posCorrectionRate;

	// �V�~�����[�V�����ɕK�v�ȏ����Z�b�g�A�b�v
	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{ 
		(*it)->SetupLCP();
	}
	scene->GetConstraintEngine()->posCorrectionRate = PCORRECTIONRATE;
	for(set<PHConstraint *>::iterator it = nearest_consts.begin(); it != nearest_consts.end(); ++it)
	{ 
		(*it)->SetupLCP();
	}
	scene->GetConstraintEngine()->posCorrectionRate = pos_correctionRate;

	for(int i = 0; i < NUM_PREDICT_ITERATE; ++i)
	{
		for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
		{
			// ���ׂĂ̐ڐG��iterate
			(*it)->IterateLCP();
		}
		for(set<PHConstraint *>::iterator it = nearest_consts.begin(); it != nearest_consts.end(); ++it)
		{
			// ���ׂĂ̐ڐG��iterate
			(*it)->IterateLCP();
		}
	}

	// �e�����ϑ�
	// ���̂̑��x�ω����擾
	vector<SpatialVector> effects;
	for(set<PHSolid *>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		effects.push_back((*it)->dv);
	}

	// ���͂̕��̂̑��x�ω����i�[
	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		relative_solid_velocity->push_back((*it)->dv);
	}

	// �㏑�����Ă��܂��ϐ��̉�
	RestoreParameters(&relative_consts, &relative_solids, &nearest_consts, &nearest_solids);

	// �e����return
	return effects;
}

// ����̐ڐG�Ɋ܂܂�鍄�̂ɗ͂�������֐�
void AddForceToConstraint(int index, PHConstraint* constraint, PHSolid* solid, map<PHSolid *, Vec3d> coeff, Vec3d force_vec)
{
	double dt = scene->GetTimeStep();
	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	Vec3d m_accel;
	Vec3d r_accel;

//		force_vec = contact->Xj[0].q * force_vec;
//		map<PHSolid *, Vec3d>::iterator cef = coeff.find(si);

	// ���[���h���W�̗͂����̍��W�ɒ����ĉ�����
	force_vec = solid->GetOrientation().Conjugated() * force_vec;
	m_accel = solid->GetMassInv() * force_vec * dt;
	r_accel = solid->GetInertiaInv() * ((solid->GetOrientation().Conjugated() * (contact->pos - solid->GetCenterPosition()) ^ force_vec)) * dt;

	// �͂����������ʂɂ�鑬�x�ω��𑫂�
	solid->dv.v() += m_accel;
	solid->dv.w() += r_accel;
}

// �񎦗͂̌v�Z����ђ�Ď�@�ɕK�v�ȏ����W�߂č\���̂��쐬����
void MakeHapticInfo(HapticInfo *new_info, HapticInfo *current_info, 
					vector<pair<PHConstraint *, int> > pointer_consts,  
					vector<pair<PHConstraint *, int> > col_candidate_consts, map<PHContactPoint*, Vec3d> col_candidate_pointer_pos,
					vector<pair<PHConstraint *, int> > current_consts,  
					map<PHConstraint *, int> NewcolToCol, 
					map<PHSolid*, SEData> se_map,
					vector<SEData> relSE, set<PHSolid *> relative_solids, bool bLocalDynamics_local)
{
	// �͊o���̏�����
	InitHapticInfo(new_info);

	// �͊o�����_�����O�ɕK�v�ȃf�[�^�̍쐬
	int num_collisions = (int)pointer_consts.size() + (int)col_candidate_consts.size() + (int)current_consts.size();
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
									pointer_consts, col_candidate_consts, col_candidate_pointer_pos, 
									current_consts,	&sign, &so, &pointer_col_position, &bCollide, &bPreviousCollide);

		// ���̂̓o�^��,���̍��̂̎��͂���̉e���̓o�^�̏���
		RegisterNewSolid(new_info, i, current_info, so, se_map);

		// �ڐG�̓o�^��,���̑��t���O�̓o�^
		RegisterNewCollision(new_info, i, current_info, constraint, sign, 
			bPreviousCollide, bCollide, NewcolToCol, pointer_col_position, 
			bLocalDynamics_local);
	}

	// ���͂̕��̂ւ̉e�����i�[
	if(bLocalDynamics_local)
	{
		new_info->relative_solids_velocity = relSE;
		new_info->relative_solids = relative_solids;
	}

	// �ڐG���̓o�^
	new_info->num_collisions = num_collisions;

	// ���ԕ�����̌v�Z���s��
	calcTimeSpan(new_info);

	// �ڐG�_����������ꍇ�͑ł��؂�
	if(new_info->num_collisions >= NUM_COLLISIONS) 
	{
		cout << "number of collisions is overfull: " << num_collisions << endl << "you must change to acceptable numbers" << endl;
		new_info->num_collisions = NUM_COLLISIONS-1;
	}
}

// ���ׂĂ̐ڐG����i�Ԗڂ̃f�[�^������Ă���֐�
PHConstraint* GetSpecificDataFromCollisions(int i, 
					 vector<pair<PHConstraint*, int> > pointer_consts, 
					 vector<pair<PHConstraint*, int> > col_candidate_consts,  
					 map<PHContactPoint*, Vec3d> col_candidate_pointer_pos,
					 vector<pair<PHConstraint*, int> > current_consts, 
					 int* sign, PHSolid** so, Vec3d* pointer_col_position, bool* bCollide, bool* bPreviousCollide)
{
	PHConstraint* constraint;
	int pointer_consts_size = (int)pointer_consts.size();
	int col_candidate_consts_size = (int)col_candidate_consts.size();
	int current_consts_size = (int)current_consts.size();

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
	// �|�C���^�ɂ͐ڐG���Ă��Ȃ��A�������̂��܂ސڐG
	// �|�C���^�ɐڐG�������Ȃ��̂���
	else if(pointer_consts_size + current_consts_size + col_candidate_consts_size > i)
	{
		int i_local = i - (pointer_consts_size + current_consts_size);
		constraint = col_candidate_consts[i_local].first;
		*sign = col_candidate_consts[i_local].second;

		*pointer_col_position = col_candidate_pointer_pos.find(DCAST(PHContactPoint, constraint))->second;

		if(*sign > 0) *so = col_candidate_consts[i_local].first->solid[0];
		else *so = col_candidate_consts[i_local].first->solid[1];

		// ���̐ڐG�͎��ۂɂ͐ڐG���Ă��Ȃ��̂�false
		*bCollide = false;
	}
	return constraint;
}

// ���̂̓o�^��,�ڐG�ƍ��̂̊֘A�t��
void RegisterNewSolid(HapticInfo* new_info, int i, HapticInfo* current_info, PHSolid* so, map<PHSolid*, SEData> se_map)
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

			// �O��̐ڐG�ƍ���̐ڐG�œ������̂��܂܂�Ă����index��ۑ�����
			new_info->NewsolToSol[counter] = -1;
			for(int j = 0; j < current_info->num_solids; ++j)
			{
				if(current_info->nearest_solids[j] == so)
				{
					new_info->NewsolToSol[counter] = j;
					break;
				}
			}			

			// ���̐����X�V
			new_info->num_solids = counter+1;

			// �����������̂��Ƃɒǉ�
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
				map<PHSolid *, SEData>::iterator it = se_map.find(so);

				new_info->vel_constant[counter] = (*it).second.constant_effect.v();
				new_info->ang_constant[counter] = (*it).second.constant_effect.w();
				
				int size = (int)(*it).second.surround_effect.size();
				for(int j = 0; j < size; ++j)
				{
					new_info->vel_effect[counter][j] = (*it).second.surround_effect[j].first;
					new_info->ang_effect[counter][j] = (*it).second.surround_effect[j].second;
				}
			}
			else
			{
				new_info->vel_constant[counter] = Vec3d();
				new_info->ang_constant[counter] = Vec3d();
				new_info->gravity_effect[counter] = SpatialVector();
			}

			// ���̕��̂ɏd�͂��K������Ă��邩���ׂ�			
			// �o�^����Ă����,���̍��̂�bGravity == false�Ȃ̂�false�ɂ���
			set<PHSolid*>::iterator it = bGravitySet.find(so);
			if(it != bGravitySet.end()) new_info->bGravity[counter] = false;
			else new_info->bGravity[counter] = true;

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

// �ڐG�̓o�^���s��
void RegisterNewCollision(HapticInfo* new_info, int i, HapticInfo* current_info, PHConstraint* constraint, int sign, 
						  bool bPreviousCollide, bool bCollide, map<PHConstraint*, int> NewcolToCol, Vec3d pointer_col_position, 
						  bool bLocalDynamics_local)
{
	new_info->constraint[i] = constraint;
	new_info->sign[i] = sign;

	PHContactPoint* contact = DCAST(PHContactPoint, constraint);
	new_info->solid[i][0] = contact->solid[0];
	new_info->solid[i][1] = contact->solid[1];

	new_info->shapePair[i] = contact->shapePair;

	new_info->bPreviousCollide[i] = bPreviousCollide;
	new_info->bCollide[i] = bCollide;

	new_info->pointer_pos[i] = pointer_pos;
	new_info->pointer_ori[i] = pointer_ori;

/*
	// ���[���h���W����S�����W�ɒ����S������o�^
	// ���͂̉e�����g���Ƃ���
	// ���[���h���W�̗͂��S�����W�Ɉڂ��ČW�����g��
	// ���͂̉e���͓��͂��S�����W�ŏo�͂����[���h���W
	if(bLocalDynamics_local)
	{
		if(sign > 0) new_info->wldTosld[i] = contact->Xj[0].q.Conjugated();
		else new_info->wldTosld[i] = contact->Xj[1].q.Conjugated();
	}
	// �Ǐ��I�ȓ��͊w�v�Z���s��Ȃ��ꍇ��
	// ���͂̉e�����v�Z����Ƃ���
	// ���[���h���W�̗͂����[���h���W�̂܂�
	// ���͂̉e���i���^���������j��������
	// �^������������邽�߂�
	// �͂ł̍S�����W�ւ̕ϊ���ł������悤�ɍ��̍��W�n�����[����
	else
	{
		if(sign > 0) new_info->wldTosld[i] = contact->solid[0]->GetOrientation();
		else new_info->wldTosld[i] = contact->solid[1]->GetOrientation();
	}
*/

	// �����p�����ڐG
	if(bPreviousCollide)
	{
		// ���̐ڐG���猻�݂̃f�[�^�ւ̈������擾����
		int index = NewcolToCol.find(constraint)->second;

		// ���݂̒l�����������p��
		new_info->col_positions[i] = current_info->col_positions[index];
		new_info->col_normals[i] = current_info->col_normals[index];
		new_info->pointer_col_positions[i] = current_info->pointer_col_positions[index];
		
		// �Y�����̕ϊ��\���쐬
		new_info->NewcolToCol[i] = index;
	}
	else
	{
		// �����_�ŐڐG���Ă���
		if(bCollide)
		{
			// �|�C���^�̐ڐG�_�͐ڐG�̐ڐG�_�Ɠ���
			new_info->pointer_col_positions[i] = contact->pos;
			new_info->col_normals[i] = contact->shapePair->normal * sign;
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

		// �ꉞ-1�ŏ���������
		new_info->NewcolToCol[i] = -1;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// �͊o�v�Z�֌W�̏���

// �\���V�~�����[�V�����Ɏg���P�ʗ͂��쐬����֐�
inline void calcTestForce()
{
	// �\���V�~�����[�V�����Ɏg���P�ʗ͂̃x�N�g���̗p��
	// ��ԍŌ�ɉ������͂��g��
	// �͂������Ȃ������ꍇ�́A�ړ��ʂ���K���ɗ͂��v�Z
	if(LastVCForce != Vec3d()) TestForce = LastVCForce;
	else TestForce = 100 * (spidar_pos - pointer_pos);

	// �[������h�����߂�����x�ȏ㏬�����Ȃ�����0.0001�ɓ��ꂷ��
	if(fabs(TestForce.x) == 0.0)
	{
		TestForce.x = 0.0000001;
	}
	if(fabs(TestForce.y) == 0.0)
	{
		TestForce.y = 0.0000001;
	}
	if(fabs(TestForce.z) == 0.0)
	{
		TestForce.z = 0.0000001;
	}

	if(fabs(TestForce.x) < 0.0001) TestForce.x = 0.0001 * fabs(TestForce.x) / TestForce.x;
	if(fabs(TestForce.y) < 0.0001) TestForce.y = 0.0001 * fabs(TestForce.y) / TestForce.y;
	if(fabs(TestForce.z) < 0.0001) TestForce.z = 0.0001 * fabs(TestForce.z) / TestForce.z;

//	TestForce = Vec3d(1, 1, 1);
}

// �����̗͂�haptic device�ɒ񎦂��鏈��
inline void feedbackForce(bool feedback, Vec3d VCForce, Vec3d VCTorque)
{
	if(feedback && bForce)
	{
		// SPIDAR�̋�Ԃƌ��Ă����Ԃ��Ⴄ�̂ōs����|���Ďˉe����
		// virtual coupling�̋t�����̗́E�g���N�𔭐�
		Vec3d f = - view_haptic * VCForce / SPIDAR_SCALE;
		Vec3d t = - view_haptic * VCTorque / SPIDAR_SCALE;

		spidarG6.SetForce(FORCE_COEFF * MASS_SCALE * f, FORCE_COEFF * MASS_SCALE * t);
	}
	else spidarG6.SetForce(Vec3d(), Vec3d());
}

// ��������SPIDAR�̏����X�V����֐�
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

// �o�l�_���p����񎦗͂��v�Z����֐�
inline void calcForceBySpringDumper(Vec3d* VCForce, Vec3d* VCTorque)
{
	*VCForce = Km * (spidar_pos - pointer_pos);// + Bm * (SPIDARVel - M_vec/dt);
	*VCTorque = (Kr * spidar_ori * pointer_ori.Inv()).Rotation();// + Br * (SPIDARAngVel.Rotation() - R_vec/dt);
}

// �Ǐ��I�ȓ��͊w�v�Z
inline void calcLocalDynamics(HapticInfo* info, Vec3d* VCForce, Vec3d* VCTorque, bool* feedback)
{
	////////////////////////////////////////////////////////////////////////////////////////////////
	// �U���R�x�̂߂荞�݉�������

	// �����I��pointer_pos, pointer_ori��ύX���Ă���
	CorrectPenetration(info);

	// �ڐG���̂̐ڐG�͂ɂ�鑬�x�E�p���x�X�V
	calcForceBySpringDumper(VCForce, VCTorque);

	// �ڐG�_�̂߂荞�ݗʂ���
	// �e�ڐG�_�ł̗͂̏d�݂��v�Z����֐�
	CalcForceWeight(info);

	// �|�C���^�Ƃ̐ڐG�ɂ�鍄�̂̍X�V
	UpdateSolidByCollision(info, *VCForce, feedback);
}

// ���̏�̕����̐ڐG�_�̂߂荞�ݗʂ���
// �e�ڐG�_�ł̗͂������銄�����v�Z����֐�
void CalcForceWeight(HapticInfo* info)
{
	for(int i = 0; i < info->num_solids; ++i)
	{
		double sum = 0.0;
		
		for(int j = 0; j < info->num_col_per_sol[i]; ++j)
		{
			int col_index = info->SolToCol[i][j];

			// �߂荞�ݗʂ̑��ʂ��v�Z
			sum += info->weight[col_index];
		}
			
		if(sum == 0.0) continue;

		for(int j = 0; j < info->num_col_per_sol[i]; ++j)
		{
			int col_index = info->SolToCol[i][j];

#if defined EXP1
			// �����P�̏ꍇ��VC�Ə��������킹�邽�߂ɂ������ɂ���
			info->weight[col_index] = 1.0 / info->num_col_per_sol[i];
#else
			// ���a�Ŋ����Ċ������v�Z����
			// �G���������������̂ق�������݂���
			// �͂̔z���͐[�x�ɔ��
			info->weight[col_index] /= sum;
#endif
		}
	}
}

/**
 brief  	�񎦗͂̌v�Z��Haptic Device	�֔��f. multimedia timer�ɂ���ČĂяo�����
			�����I�ɂ̓|�C���^��SPIDAR��virtual coupling�łȂ��A�|�C���^���X�V���Ă���
 param	 	�Ȃ�
 return 	�Ȃ�
*/
#ifdef EXP1
void HapticRendering()
#else
void CALLBACK HapticRendering(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2)
#endif
{
	///////////////////////////////////////////////////////////////////////////////////////
	// �������̊Ǘ�,�V�~�����[�V�����̊Ǘ��Ǝ��s
	scheduleSimulationFromHaptic();

	// �����ɕK�v�ȏ��𓾂�
	HapticInfo* info = updateInfo();

	// �o�^���ꂽ�ڐG�������Ă��A���ݖ{���ɐڐG���Ă��邩�͂킩��Ȃ��̂Ńt���O��p��
	bool feedback = false;

	Vec3d VCForce, VCTorque;

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
	// ���[�U�ւ̗͂̒�
	feedbackForce(feedback, VCForce, VCTorque);
}

// �ڐG�i�߂荞�݁j�̃`�F�b�N������
void CheckPenetrations(HapticInfo* info, int* num_cols, vector<Vec3d>* r_vectors, vector<double>* c_vectors, vector<Vec3d>* n_vectors)
{
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �߂荞�݉��������̏���
		Vec3d col_vector = info->col_positions[i] - info->pointer_col_positions[i];
		double vector_coeff = dot(col_vector, info->col_normals[i]);

		// �߂荞��ł�����␳�v�Z�Ɏg���f�[�^��ǉ�
		if(vector_coeff > 0 && info->bCollide[i])
		{
			r_vectors->push_back(Vec3d(info->pointer_col_positions[i] - pointer_pos));
			c_vectors->push_back(vector_coeff);
			n_vectors->push_back(info->col_normals[i].unit());

			// �d�݂��i�[
			info->weight[i] = vector_coeff;

			// �߂荞��ł��ڐG�̌�
			++(*num_cols);
		}
		else info->weight[i] = 0;
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

	// �߂荞�݂̐��̍ő�l��ڐG���e��-6�ɂ���
	// �����6�s����SPIDAR�̈ʒu�𔽉f������̂Ɏg���邽��
	if(num_cols > NUM_COLLISIONS - 6) num_cols = NUM_COLLISIONS - 6;

	MatrixN6d M = MatrixN6d();
	VecNd C = VecNd();

	// ������A�������s����쐬����
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
}

// �߂荞�݂�A���������ɂ��čs����쐬����֐�
void MakePointerPosMatrix(int num_cols, vector<Vec3d> r_vectors, vector<double> c_vectors, vector<Vec3d> n_vectors, MatrixN6d* M, VecNd* C)
{
	// �߂荞�݂ɂ�����d��
	double P;

	// SPIDAR�̈ʒu�E�p���ɂ�����d��
	double MX, MY, MZ;
	double RX, RY, RZ;

	// �ڐG�̏󋵂���p�����[�^�����߂�
	GetParametersOfCollision(&MX, &MY, &MZ, &RX, &RY, &RZ, &P);

	// �[���t�s��ō��W�v�Z�����˂�
	Vec3d t = spidar_pos - pointer_pos;
	Vec3d th = (spidar_ori * pointer_ori.Inv()).Rotation();

	for(int i = 0; i < 6; ++i)
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

		// ��{�I��t��th(SPIDAR�̈ʒu�E�p��)�Ō��܂�悤�ɂ���
		// �ڐG������ꍇ�͂��̍s��ɂ���ɏ���ǉ����A
		// �[���t�s����������Ƃɂ���Ă߂荞�݂��l�������ʒu�����߂�
		for(int j = 0; j < 6; ++j)
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

// �|�C���^�̈ʒu�v�Z�ɂ����d�݂�Ԃ��֐�
inline void GetParametersOfCollision(double* MX, double* MY, double* MZ, double* RX, double* RY, double* RZ, double *P)
{
	// �Ƃ肠�������̒l�ň���
	// MX, MY, MZ����������������ƃ_���p�[�������Ȃ��Ă��܂�
	// �傫����������Əd�������̂ɂ߂荞��ł����Ă��܂�
#ifdef EXP2
	*MX = *MY = *MZ = 0.0045;
#else
	*MX = *MY = *MZ = 0.008;
#endif
	*RX = *RY = *RZ = 1.0;
	*P = 1.0;
}

// �߂荞�݂�␳����֐�
void CorrectPenetration(HapticInfo* info)
{
	// �|�C���^�̂߂荞�݂ɑ΂��ċ[���t�s����v�Z���A
	// �K�؂ȕ␳�ʂ����߂邱�ƂŁA�߂荞�݂�␳���Ă���
	// �|�C���^���g����i�E��]�����Ă߂荞�݂�����������悤�ɂ���
	Vec3d M_vec, R_vec;
	CalcTTheta(info, &M_vec, &R_vec);

	// �|�C���^���g�̈ʒu�E�p�����ړ�
	pointer_pos += M_vec;
	pointer_ori = Quaterniond::Rot(R_vec) * pointer_ori;

	// ���ׂĂ̐ڐG�����̕����Ɉړ��E��]
	for(int i = 0; i < info->num_collisions; ++i)
	{
		info->pointer_col_positions[i] += M_vec;
		info->pointer_col_positions[i] += R_vec ^ (info->pointer_col_positions[i] - pointer_pos);
	}
}

// ���̂̑��x����ړ��������v�Z���Ĉʒu���X�V����֐�
void UpdateSolids(HapticInfo* info)
{
#ifdef EXP1
	bool bWriteData = false;
#endif

	for(int i = 0; i < info->num_solids; ++i)
	{
		if(bSurroundEffect)
		{
			// ���͂̉e���̂����ŁA�萔�������X�ɉ����Ă���
			// IsDynamical == false�̏ꍇ�͂��ꂼ�ꏉ�������Ă���̂ŏ�������͕K�v�Ȃ�
			info->solid_velocity[i] += info->vel_constant[i] * info->SHratio;
			info->solid_angular_velocity[i] += info->ang_constant[i] * info->SHratio;
		}
		// �Ǐ��I�ȓ��͊w�v�Z�{���͂̉e���Ȃ��̏ꍇ
		// �d�͂��K������Ă��镨�̂ɂ͏d�͂�������
		else if(info->bGravity[i])
		{
			info->solid_velocity[i] += scene->GetGravity() * info->Hdt;
		}

#ifdef EXP1
		// ���x���v�Z���o��
		if(bOutput) 
		{
			if(info->nearest_solids[i] == DCAST(PHSolid, soObject[0]))
			{
				Buffer[num_data++] = info->solid_velocity[i];
				bWriteData = true;
			}
		}
#endif

		// ���̂̑��x�ɂ��ړ��E��]
		Vec3d dx_local = info->solid_velocity[i] * info->Hdt;
		info->solid_center_positions[i] += dx_local;
		
		Vec3d dth_local = info->solid_angular_velocity[i] * info->Hdt;
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

#ifdef EXP1
	if(!bWriteData && bOutput && !bSimulation) 
	{
		Buffer[num_data++] = soObject[0]->GetVelocity();
	}
#endif
}

// �͐ς��瑬�x���X�V����֐�
void UpdateVelocityByCollision(HapticInfo* info, Vec3d VCForce, bool* feedback)
{
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �Q�ڐG�_�̋����𒲂ׂ�
		double distance = calcDistance(info->col_positions[i], info->pointer_col_positions[i]);
		// ���̂̓Y����
		int sol_index = info->ColToSol[i];

		// �ʂ̖@���ƁA�|�C���^��̓_���獄�̏�̓_�܂ł����񂾃x�N�g���̓��ς��v�Z
		// ���ꂪ�O�ȏ�Ȃ�i�[���x�N�g�����܂ށB���傤�ǐڂ��Ă���j�ڐG������
		// �܂��A���̂܂܂��Ɖ��z�ǂ������������Ƃ����͂𔭐������Ă��܂��̂ŁA
		// ����ɂQ�̐ڐG�_�Ԃ̋�������������
		// ������������x�ȏ�傫���Ȃ�ƐڐG���Ă��Ȃ����Ƃɂ���
		if(dot(info->col_positions[i] - info->pointer_col_positions[i], info->col_normals[i]) >= 0 && 
			((info->nearest_solids[sol_index]->IsDynamical() && distance < DISTANCE_LIMIT)
			|| !info->nearest_solids[sol_index]->IsDynamical()))
		{
			// �Ƃɂ����ڐG���Ă���̂Ńt���O��ON�ɂ���
			info->bCollide[i] = true;

			// �͂������Ă悢�Ώۂ��`�F�b�N
			// �͂̌����ƍ��̂̐ڂ��鑤�̃`�F�b�N
			if(dot(info->col_normals[i], -VCForce) >= 0)
			{
				// ���[�U�ɗ͊o���
				*feedback = true;
		
				// �O�͂œ������̂ł���Α��x���ω�����͂��Ȃ̂ŉ����x���v�Z���đ��x���X�V����
				if(info->nearest_solids[sol_index]->IsDynamical())
				{
					// ���͂̉e�����v�Z����Ƃ��Ɏg���e�X�g�͂��X�V
					LastVCForce = VCForce;

					// �Ǐ��I�ȓ��͊w�v�Z�̂�
					if(!bSurroundEffect)
					{
						// �^���������ɏ]���ĉ����x���v�Z����
						Vec3d addF = FORCE_COEFF * info->weight[i] * VCForce;
						Vec3d accel = info->solid_massinvs[sol_index] * addF;
						Vec3d ang_accel = info->solid_inertiainvs[sol_index] * ((info->col_positions[i] - info->solid_center_positions[sol_index]) ^ addF);

						// ����ꂽ�����x�ő��x���X�V
						info->solid_velocity[sol_index] += accel * info->Hdt;

						// �p���x���X�V
						info->solid_angular_velocity[sol_index] += ang_accel * info->Hdt;
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
//						Vec3d q_f = info->wldTosld[i] * info->solid_orientations[sol_index].Conjugated() * /*info->weight[i] */ FORCE_COEFF * VCForce * ratio;
						Vec3d q_f = info->weight[i] * VCForce * info->SHratio;
				
						// �ڐG�_�ɉ������͂̑��a���X�V
						info->ForceSum[i] += q_f;

						// ���̗͂��y�ڂ��e�������ׂĂ̍��̂ɂ��Čv�Z����
						for(int j = 0; j < info->num_solids; ++j)
						{
							if(!info->nearest_solids[j]->IsDynamical()) continue;

							// �s���p���ĉ����x���v�Z���đ��x���X�V
							info->solid_velocity[j] += info->vel_effect[j][i] * q_f;
							info->solid_angular_velocity[j] += info->ang_effect[j][i] * q_f;
						}
					}
				}
			}
		}
		else{ info->bCollide[i] = false; }
	}
}

// �ڐG�ɂ�鍄�̂̍X�V
inline void UpdateSolidByCollision(HapticInfo *info, Vec3d VCForce, bool* feedback)
{
	// �ڐG�͂ɂ�鑬�x�̍X�V
	UpdateVelocityByCollision(info, VCForce, feedback);

	// �|�C���^�ɐڂ��Ă��邷�ׂĂ̍��̂̏����X�V����
	UpdateSolids(info);
}

// �͊o�����_�����O�ŕ�������؂�ւ���Ƃ���
// ��a���𐶂������Ȃ��悤�Ɍ덷���C�����鏈��������֐�
void UpdateNewInfoFromCurrent()
{
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

	// �Ǐ��I�ȓ��͊w�v�Z���s���ꍇ
	// ���݂̒l(info_cur)��M����
	if(bLocalDynamics)
	{
		// ���̂̍X�V
		for(int i = 0; i< info_next->num_solids; ++i)
		{
			// ����̍��̂��獡��̍��̂̓Y����������
			int prev_sol_index = info_next->NewsolToSol[i];

			// ������Ȃ���Ύ��ɂ�����
			// ����V�������̂���������ꍇ�͂��肦��
			if(prev_sol_index == -1) continue;

			info_next->solid_center_positions[i] = info_cur->solid_center_positions[prev_sol_index];
			info_next->solid_orientations[i] = info_cur->solid_orientations[prev_sol_index];
			info_next->solid_velocity[i] = info_cur->solid_velocity[prev_sol_index];
			info_next->solid_angular_velocity[i] = info_cur->solid_angular_velocity[prev_sol_index];
		}

		// �ڐG�̍X�V
		for(int i = 0; i < info_next->num_collisions; ++i)
		{
			int index = info_next->NewcolToCol[i];

			// �O�̐ڐG�������p��
			// �O�̏���M����
			if(index != -1)
			{
				info_next->pointer_col_positions[i] = info_cur->pointer_col_positions[index];
				info_next->col_positions[i] = info_cur->col_positions[index];
				info_next->col_normals[i] = info_cur->col_normals[index].unit();
			}
			// �V�����ڐG
			// �|�C���^�̈ʒu�̂ݍX�V����
			else
			{
				Vec3d spidar_pos_error = pointer_pos - info_next->pointer_pos[i];
				Quaterniond spidar_ori_error = pointer_ori * info_next->pointer_ori[i].Inv();

				info_next->pointer_col_positions[i] += spidar_pos_error;
				info_next->pointer_col_positions[i] += spidar_ori_error.Rotation() ^ (info_next->pointer_col_positions[i] - spidar_pos);
			}
		}
	}
	// �o�[�`�����J�b�v�����O�̏ꍇ
	// �ŐV�̏���M���邱�Ƃɂ���
	else
	{
		// �ڐG�̍X�V
		for(int i = 0; i < info_next->num_collisions; ++i)
		{
			int index = info_next->NewcolToCol[i];

			// �|�C���^�͍ŐV�̏����g��
			Vec3d ppos_error = pointer_pos - info_next->pointer_pos[i];
			Quaterniond pori_error = pointer_ori * info_next->pointer_ori[i].Inv();

			info_next->pointer_col_positions[i] += ppos_error;
			info_next->pointer_col_positions[i] += pori_error.Rotation() ^ (info_next->pointer_col_positions[i] - spidar_pos);

			// �V�����ڐG�̏ꍇ�͂���ȍ~�X�V����K�v�Ȃ�
			if(!info_next->bPreviousCollide[i]) continue;

			// �ڐG�̏����ŐV�̒l�ɏ���������
			int sol_index = info_next->ColToSol[i];
			int prev_sol_index = info_cur->ColToSol[index];

			Vec3d pos_error = info_next->solid_center_positions[sol_index] - info_cur->solid_center_positions[prev_sol_index];
			Quaterniond ori_error = info_next->solid_orientations[sol_index] * info_cur->solid_orientations[prev_sol_index].Inv();

			info_next->col_positions[i] += pos_error;
			info_next->col_positions[i] += ori_error.Rotation() ^ (info_next->col_positions[i] - info_next->solid_center_positions[sol_index]);

			info_next->col_normals[i] += ori_error.Rotation() ^ info_next->col_normals[i];
			info_next->col_normals[i] = info_next->col_normals[i].unit();
		}
	}

	// �߂荞�ݏ󋵂̍X�V
	updatePenetrations(info_next);
}

// ���ʂ̃o�[�`�����J�b�v�����O
void VirtualCoupling(HapticInfo* info, Vec3d *VCForce, Vec3d *VCTorque, bool* feedback)
{
	// �߂荞�݂̕␳�������s��
	CorrectPenetration(info);

	// �ڐG���̂̐ڐG�͂ɂ�鑬�x�E�p���x�X�V
	calcForceBySpringDumper(VCForce, VCTorque);

	Vec3d addF = FORCE_COEFF * (*VCForce) * info->SHratio;
	// �ڐG������ΐڐG���Ă��鍄�̂ɒ񎦗͂̋t�����̃x�N�g����������
	// ���̂̋����͕����V�~�����[�V�����ɔC����
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �͂�����������Ɩʂ̖@����90�x�ȏ�J���ĂȂ��ꍇ�i���������Ă��Ȃ��ꍇ�j
		// �͂������Ȃ��B�������Ȃ��ƐڐG�𗣂������ɓ��������Ƃ��ɔS������������
		if(!(dot(*VCForce, info->col_normals[i]) < 0)) continue;

		// �͊o�t�B�[�h�o�b�NON
		*feedback = true;

		// ratio�������闝�R�́A
		// �V�~�����[�V�������������͂̑��ʂ𕽋ς��ăV�~�����[�V�����̎����ŗ͐ςɂ��邽��
		// �܂��A��̍��̂ɑ��݂���ڐG�_�̐��Ŋ����āA�ϓ��ɗ͂�������悤�ɂ���
		int sol_index = info->ColToSol[i];
		info->nearest_solids[sol_index]->AddForce(addF / info->num_col_per_sol[sol_index], info->col_positions[i]);
	}

#ifdef EXP1
	// ���x���v�Z���o��
	if(bOutput) 
	{
		bool bWriteData = false;
		for(int i = 0; i < info->num_solids; ++i)
		{
			if(info->nearest_solids[i] == DCAST(PHSolid, soObject[0]))
			{
				Buffer[num_data++] = info->solid_velocity[i];
				bWriteData = true;
			}
		}
		if(!bWriteData)
		{
			if(!bSimulation)Buffer[num_data++] = soObject[0]->GetVelocity();
		}
	}
#endif
}

// �͊o�����_�����O�ƃV�~�����[�V�������v��I�Ɏ��s����֐�
inline void scheduleSimulationFromHaptic()
{
	// �V�~�����[�V�������瓾��������i�[����ϐ�
	HapticInfo* info = NULL;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	if(current_valid_data) info = &info1;
	else info = &info2;

	// �V�~�����[�V�������Ǘ�����J�E���^�[���쐬
	static int step_counter = 0;	
	// �J�E���^�[�̒l�������𖞂������`�F�b�N
	if(step_counter+1 >= (int)info->HSratio && !bSimulation)
	{
		// ���ݎg�p���Ă���f�[�^���玟�Ɏg�p����f�[�^�ɐ؂�ւ���ۂ�
		// ��a���������Ȃ��悤�ɒl�����낦�鏈��
		UpdateNewInfoFromCurrent();

		// ���͂̉e�����v�Z����Ƃ��ɂ����P�ʗ͂̌v�Z
		// �������͂̕��ς����߂�
		calcTestForce();

		// �g�p����f�[�^��ύX���A�V�~�����[�V�������s��
		step_counter = 0;
		current_valid_data = !current_valid_data;
		bSimulation = true;

#ifdef EXP1
		StepSimulation();
#endif
	}
	else ++step_counter;
}

////////////////////////////////////////////////////////////////////////////////////////////////
//�@�֘A�����蒆�����ȏ���

// ��Ď�@�ɂ���Đ��������̂̈ʒu�̌덷���C������֐�
void ErrorCorrection()
{
	// �Ǐ��I�ȓ��͊w���s��Ȃ��ꍇ��
	// �덷�������Ȃ��͂��Ȃ̂ł��̂܂܃��^�[��
	if(!bLocalDynamics) return;

	// ��Ԃɂ���ĎQ�Ɛ��؂�ւ���
	HapticInfo* info = NULL;
	if(!current_valid_data) info = &info1;
	else info = &info2;

//	Vec3d gravity = scene->GetGravity() * info->Sdt;

	// �͊o�����_�����O����V�~�����[�V�����ւ̋Ǐ��I���͊w�v�Z�̌��ʔ��f����
	// �|�C���^�ɐڂ��邷�ׂĂ̍��̂ɂ��čs��
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
		// HapticRendering���ŏd�͌v�Z���s���Ă���
		// ����ɃV�~�����[�V�����ŏd�͌v�Z���s���Ɠ�d�ŏd�͂�ǉ����Ă��܂��̂�
		// GravityEngine���d�͂������邱�Ƃ����z����
		// ���̉����镪���Ɉ����đ��E������
		// ���ڏd�͂�ON/OFF�����Ă��悢���A���̑��삾���ŃR�X�g���������Ă��܂��̂�
		// �Ƃ肠�������̂悤�ɂ���
		/*
		if(info->bGravity[i])
		{
			if(bSurroundEffect)
			{
				info->nearest_solids[i]->SetVelocity(info->nearest_solids[i]->GetVelocity()
													- info->gravity_effect[i].v() * scene->GetTimeStep());
				info->nearest_solids[i]->SetAngularVelocity(info->nearest_solids[i]->GetAngularVelocity()
													- info->gravity_effect[i].w() * scene->GetTimeStep());
			}
			else
			{
				info->nearest_solids[i]->SetVelocity(info->nearest_solids[i]->GetVelocity() 
													- gravity * info->nearest_solids[i]->GetMass());
			}
		}
		*/
	}

	// ���͂̐�����g�p���Ă���ꍇ��
	// ���͂̍��̂��X�V����
	if(bSurroundEffect)
	{
		// ���ڃ|�C���^�ɐڂ��Ă��Ȃ����e�����󂯂鍄�̂̍X�V
		int local_counter = -1;
		for(set<PHSolid *>::iterator it = info->relative_solids.begin(); it != info->relative_solids.end(); ++it)
		{			
			// ���̍��̂ɂ�����
			// �����l-1�Ȃ̂ōŏ���0�ɂȂ�
			// ������increment���Ȃ���continue�����Ƃ�increment����Ȃ��Ȃ�
			++local_counter;

			// �����Ȃ����͓̂����Ȃ�
			if(!(*it)->IsDynamical()) continue;

/*
			bool bNearest = false;
			for(int i = 0; i < info->num_solids; ++i)
			{
				if((*it) == info->nearest_solids[i])
				{
					bNearest = true;
					break;
				}
			}
			// ���̍��̂̓|�C���^�ɒ��ɐڂ��Ă���̂ŁA
			// �����ł͍X�V���Ȃ�
			if(bNearest) {ofs << "error " << endl; continue;}
*/

			// �܂��͑��x���X�V
			// �萔�������Z
			SpatialVector d_accel = info->relative_solids_velocity[local_counter].constant_effect;

			// �|�C���^�ƍ��̂̐ڐG����A�ԐړI�Ɏ󂯂�e�����v�Z
			for(int i = 0; i < info->num_collisions; ++i)
			{
				// �������̂��������Ƃ��̂�,���̍��̂���Ď��͂̍��̂�����
				if(info->nearest_solids[info->ColToSol[i]]->IsDynamical())
				{
					d_accel += SpatialVector(info->relative_solids_velocity[local_counter].surround_effect[i].first * info->ForceSum[i],
											 info->relative_solids_velocity[local_counter].surround_effect[i].second * info->ForceSum[i]);
				}
			}

			// ���XForceSum��ratio�ɂ��Ă���Ƃ����Haptic dt�ɂ��邽�߂�
			// ratio = haptic dt / sim dt ��� ratio * sim dt ������
			// �܂� d_accel * sim dt
			d_accel = d_accel * info->Sdt;

			// ���̉����x�ɂ��ړ��ʂ̌v�Z
			Vec3d d_pos = (*it)->GetVelocity()        * info->Sdt + d_accel.v() * info->Sdt / 2.0;
			Vec3d d_ori = (*it)->GetAngularVelocity() * info->Sdt + d_accel.w() * info->Sdt / 2.0;

			// ���x�̍X�V
			(*it)->SetVelocity((*it)->GetVelocity() + d_accel.v());
			(*it)->SetAngularVelocity((*it)->GetAngularVelocity() + d_accel.w());

			// �ʒu�̍X�V
			(*it)->SetCenterPosition((*it)->GetCenterPosition() + d_pos);
			(*it)->SetOrientation(Quaterniond::Rot(d_ori) * (*it)->GetOrientation());

			// �X�V����
			(*it)->SetUpdated(true);
		}
	}
}

// �����V�~�����[�V��������������߂�֐�
void StepSimulation()
{
#ifdef EXP3
	t_start();
#endif

	scene->ClearForce();

	// �|�C���^�̈ʒu���X�V
	UpdatePointer();

	// �Ǐ��I���͊w�v�Z�ɂ���Đ����������C������
	ErrorCorrection();

	// �V�~�����[�V�������g���ɕύX������΍X�V���s���֐�
	updateSimulationFrequency();

	// �V�~�����[�V�����̋�̓I�ȏ���
	scene->GenerateForce();
	scene->Integrate();

	// �Փ˓_����\��
//	show_collision_info();

	// �Փ˓_������͂̉e�����v�Z
	// �P���Q�ƒ��B�Q���X�V 
	if(current_valid_data) CalcSurroundEffect(&info2, &info1);
	// �Q���Q�ƒ��B�P���X�V
	else CalcSurroundEffect(&info1, &info2);

	// �V�~�����[�V�������I������̂Ńt���O��������
	bSimulation = false;

#ifdef EXP3
	int time = t_end();
	if(time > 1000000 / (double)SIMULATION_FREQ)
	{
		cout << "collapsed time = " << (double)time / 1000000 << " num object = " << (int)soObject.size() << endl;
//		ofs << "collapsed time = " << (double)time / 1000000 << " num object = " << (int)soObject.size() << endl;
	}
#endif
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
	ofs.precision(15);

#if defined EXP1
	Buffer.resize(10000, Vec3d());
	num_data = 0;
#endif

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

#ifdef EXP1
	StepSimulation();

	keyboard('2', 0, 0);
	keyboard('u', 0, 0);
	
	while(1)
	{
		HapticRendering();
	}
#else
	// haptic�X���b�h�̐����E�J�n
	FTimerId1 = timeSetEvent(1000 / HAPTIC_FREQ,    // �^�C�}�[�Ԋu[ms]
	                        1,   // ���ԕ���\
		                    HapticRendering,//�R�[���o�b�N�֐�
		                    (DWORD)&time,//���[�U�[��`�̒l
		                    TIME_PERIODIC //�P��(TIME_ONESHOT) or �J��Ԃ�(TIME_PERIODIC)
							);

	// openGL main loop
	glutMainLoop();
#endif

}
/**
 brief		VR���̃v���L�V�̈ʒu���X�V����֐��B
			���̊֐��Ɨ͊o�����_�����O��g�ݍ��킹�邱�ƂŃ|�C���^�̍X�V��HAPTIC_FREQ�̎��g���ōX�V�ł���B
 param		�Ȃ�
 return		�Ȃ�
 */
void UpdatePointer()
{
	double dt = scene->GetTimeStep();
	pointer_vel = (soPointer->GetFramePosition() - pointer_pos) / dt;
	pointer_angvel = (soPointer->GetOrientation() * pointer_ori.Inv()).Rotation() / dt;

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

		// ����̐ڐG�_���폜
		info1.points.clear();
		info2.points.clear();

		cout << "EXIT from Key Input" << endl;
		exit(0);
	}
	// �|�C���^�̃L�����u���[�V����
	else if(key == 'c')
	{
		spidarG6.Calib();
	}
	// toggle force output
	else if(key == ' ')
	{
		bForce = !bForce;

		// ���b�Z�[�W���o��
		cout << "Feedback Force ";
		if(bForce) cout << "ON" << endl;
		else cout << "OFF" << endl;
	}
	// object�̏ꏊ�����Z�b�g
	else if(key == 'r')
	{
		for(int i = 0; i < (int)soObject.size(); ++i)
		{
			if(!soObject[i]->IsDynamical()) continue;

			soObject[i]->SetPose(Posed());

#if defined EXP1
			soObject[i]->SetFramePosition(Vec3f(1.55 * (i + 1), 0.0, 0.0));
#elif defined EXP2
			if(!bTest)
			{
				if(i < 2) soObject[i]->SetFramePosition(Vec3f(1.55 * (i + 1), 0.0, 0.0));
				else soObject[i]->SetFramePosition(Vec3f(1.55 * (i - 1), 0.0, 2.0));
			}
			else
			{
				if(i < 2) soObject[i]->SetFramePosition(Vec3f(1.55 * (i + 1), 0.0, 2.0));
				else soObject[i]->SetFramePosition(Vec3f(1.55 * (i - 1), 0.0, 0.0));
			}
#elif defined EXP4
			soObject[i]->SetFramePosition(Vec3f(1.6 * (i+1) - 3.0, 0.0, 0.0));
#elif defined Demo
			if(i < 15) soObject[i]->SetFramePosition(Vec3f(-2.0 + (i % 3) * 2.0, 3.0, (i / 3) * 2.0));
			else if(i < 30)soObject[i]->SetFramePosition(Vec3f(-2.0 + (i % 3) * 2.0, 5.0, (i / 3) * 2.0 - 10));
			else soObject[i]->SetFramePosition(Vec3f(-2.0 + (i % 3) * 2.0, 7.0, (i / 3) * 2.0 - 20));
#else 
			soObject[i]->SetFramePosition(Vec3f(0, 0.5f*(i+1), 0));
#endif

			soObject[i]->SetAngularVelocity(Vec3d());
			soObject[i]->SetVelocity(Vec3d());
		}
	}
	// SPIDAR�̈ʒu�ƌ��_������킷���̕\����ON/OFF����
	else if(key == 'd')
	{
		bDisplayPointer = !bDisplayPointer;
		cout << "Pointer Display ";
		if(!bDisplayPointer)cout << "OFF " << endl;
		else cout << "ON " << endl;
	}
	// �V�������̂��쐬����
	else if(key == 'g')
	{
		PHSolidDesc desc;
#ifdef EXP4
		desc.mass = 0.2f;
#elif defined Demo
		desc.mass = 0.3f;
#else
		desc.mass = 1.0f;
#endif
		desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
		soObject.push_back(scene->CreateSolid(desc));
			
		CDBoxIf* box = NULL;
		CDBoxDesc bd;
		bd.boxsize = Vec3f(1.5f, 1.5f, 1.5f);
		box = DCAST(CDBoxIf, phSdk->CreateShape(bd));
		soObject[soObject.size() - 1]->AddShape(box);
		soObject[soObject.size() - 1]->SetFramePosition(Vec3d(0, soObject.size() * 1.1, 0));

		cout << "#object = " << (int)soObject.size() << endl;
	}
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
		// �V�~�����[�V�����̎��g����60Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 60;

		bSurroundEffect = true;
		bLocalDynamics = true;
		bPredictCollision = true;

#if !defined EXP2
		cout << "PRESET 1: Local Dynamics With Surrounding Effect and Predict Collision" << endl;
		keyboard('f', 0, 0);
#else
		cout << "PRESET 1" << endl;
		ofs << "PRESET 1" << endl;
#endif
	}
	else if(key == '2')
	{
		// �V�~�����[�V�����̎��g����60Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 60;

		bSurroundEffect = true;
		bLocalDynamics = true;
		bPredictCollision = false;
#if !defined EXP2
		cout << "PRESET 2: Local Dynamics With Surrounding Effect" << endl;
		keyboard('f', 0, 0);
#else
		cout << "PRESET 2" << endl;
		ofs << "PRESET 2" << endl;
#endif
	}
	else if(key == '3')
	{
		// �V�~�����[�V�����̎��g����60Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 60;

		bLocalDynamics = true;
		bPredictCollision = true;
		bSurroundEffect = false;
#if !defined EXP2
		cout << "PRESET 3: Local Dynamics With Predict Collision" << endl;
		keyboard('f', 0, 0);
#else 
		cout << "PRESET 3" << endl;
		ofs << "PRESET 3" << endl;
#endif
	}
	else if(key == '4')
	{
		// �V�~�����[�V�����̎��g����60Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 60;

		bSurroundEffect = false;
		bLocalDynamics = true;
		bPredictCollision = false;
#if !defined EXP2
		cout << "PRESET 4: Local Dynamics" << endl;
		keyboard('f', 0, 0);
#else
		cout << "PRESET 4" << endl;
		ofs << "PRESET 4" << endl;
#endif
	}
	else if(key == '5')
	{
		// �V�~�����[�V�����̎��g����60Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 60;

		bSurroundEffect = false;
		bLocalDynamics = false;
		bPredictCollision = false;
#if !defined EXP2
		cout << "PRESET 5: Virtual Coupling" << endl;
		keyboard('f', 0, 0);
#else
		cout << "PRESET 5" << endl;
		ofs << "PRESET 5" << endl;
#endif
	}
	else if(key == '6')
	{
		// �V�~�����[�V�����̎��g����500Hz�ɂ���
		SIMULATION_FREQ_BUFFER = 500;

		// �����g���o�[�`�����J�b�v�����O
		bSurroundEffect = false;
		bLocalDynamics = false;
		bPredictCollision = false;
#if !defined EXP2
		cout << "PRESET 6: High Definition Virtual Coupling" << endl;
		keyboard('f', 0, 0);
#else
		cout << "PRESET 6" << endl;
		ofs << "PRESET 6" << endl;
#endif
	}
#if !defined Demo
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
	// �f�o�b�O�o�͂̋��E�s�̐؂�ւ�
	else if(key == 'o')
	{
		bOutput = !bOutput;
		cout << "File Output is ";
		if(bOutput)cout << "ENABLED" << endl;
		else cout << "DISABLED" << endl;
	}
	// �V�~�����[�V�����̍X�V���g����ύX����
	else if(key == 'i')
	{
		if(SIMULATION_FREQ_BUFFER == 60)
		{
			SIMULATION_FREQ_BUFFER = 500;
			cout << "SIMULATION FREQUENCY is changed to 500 Hz" << endl;
		}
		else
		{
			SIMULATION_FREQ_BUFFER = 60;
			cout << "SIMULATION FREQUENCY is changed to 60 Hz" << endl;
		}
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
			cout << "Predict Collision OFF" << endl;
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
		
		cout << "Haptic Frequency : 1000 Hz" << endl;
		cout << "Simulation Frequency : " << SIMULATION_FREQ_BUFFER << " Hz" << endl;

		keyboard('e', 0, 0);
		cout << "coeff = " << FORCE_COEFF << endl;

		cout << "Feedback Force ";
		if(bForce)cout << "ON" << endl;
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
#endif
// �������Ƃ̋@�\
#ifdef EXP1
	else if(key == 'u')
	{
		if(!bExperiment) 
		{
			bOutput = true;
			cout << "Experiment start " << endl;
		}
		else
		{
			bOutput = false;
			cout << "suspend" << endl;
		}
		bExperiment = !bExperiment;
	}
#elif defined EXP2
	// ���̃Z�b�V�����ɐi��
	else if(key == 'y')
	{
		ofs << "session : " << session+1 << " finished!!" << endl;
		ofs << "--- mass = ---" << mass << endl;

		session++;

		if(session == 3)
		{
			cout << "finish experiment " << endl;
			keyboard(ESC, 0, 0);
		}
		else cout << "go to next session" << endl;

		PHSolidDesc desc;

		for(int i = 0; i < 4; ++i)
		{
			switch(i)
			{
			case 0:
				desc.mass = 1.0f;
				break;
			case 1:
				switch(session)
				{
				case 1:
					desc.mass = 2.0f;
					break;
				case 2:
					desc.mass = 3.0f;
					break;
				}
				break;
			case 2:
				desc.mass = 1.0f;
				break;
			case 3:
				desc.mass = 1.0f;
				mass = 1.0f;
				break;
			}

			ofs << "mass of solid " << i << " = " << desc.mass << endl;

			desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
				
			soObject[i]->SetMass(desc.mass);
			soObject[i]->SetInertia(desc.inertia);
		}

		ofs << "mass = " << mass << endl;
		bTest = false;
		keyboard('r', 0, 0);
	}
	else if(key == 'q')
	{
		mass = mass + 0.1;
		ofs << "mass + 0.1 :" << mass << endl;

		PHSolidDesc desc;
		desc.mass = mass;
		desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
		soObject[3]->SetMass(desc.mass);
		soObject[3]->SetInertia(desc.inertia);
		cout << "make box heavier" << endl;
	}
	else if(key == 'z')
	{
		if(mass - 0.1 <= 0.0f)
		{
			cout << "weight is less than zero" << endl;
		}
		else
		{
			mass = mass - 0.1;
			ofs << "mass - 0.1 :" << mass << endl;
			PHSolidDesc desc;
			desc.mass = mass;
			desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
			soObject[3]->SetMass(desc.mass);
			soObject[3]->SetInertia(desc.inertia);
			cout << "make box lighter" << endl;
		}
	}
	/*
	else if(key == 'w')
	{
		mass = mass + 0.5;
		ofs << "mass + 0.5 :" << mass << endl;
		PHSolidDesc desc;
		desc.mass = mass;
		desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
		soObject[3]->SetMass(desc.mass);
		soObject[3]->SetInertia(desc.inertia);
		cout << "make box much heavier" << endl;
	}
	else if(key == 'x')
	{
		if(mass - 0.5 <= 0.0f)
		{
			cout << "weight is less than zero" << endl;
		}
		else
		{
			mass = mass - 0.5;
			ofs << "mass - 0.5 :" << mass << endl;
			PHSolidDesc desc;
			desc.mass = mass;
			desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);
			soObject[3]->SetMass(desc.mass);
			soObject[3]->SetInertia(desc.inertia);
			cout << "make box much lighter" << endl;
		}
	}
	*/
	else if(key == 'p')
	{
		bTest = !bTest;
		if(bTest) cout << "Weight Adjust Mode" << endl;
		else cout << "Weight Check Mode" << endl;

		keyboard('r', 0, 0);
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// �{���ł͂Ȃ������̊֐�
vector<SpatialVector> CollisionF;
void SaveCollisionF(set<PHConstraint *> relative_consts, set<PHConstraint *> nearest_consts)
{
	// lambda��������
	CollisionF.clear();

	for(set<PHConstraint *>::iterator it = relative_consts.begin(); it != relative_consts.end(); ++it)
	{
		// �����_�̒l(fv, fw)��ۑ����Ă���
		CollisionF.push_back((*it)->f);
	}
	for(set<PHConstraint *>::iterator it = nearest_consts.begin(); it != nearest_consts.end(); ++it)
	{
		// �����_�̒l(fv, fw)��ۑ����Ă���
		CollisionF.push_back((*it)->f);
	}	
}

vector<SpatialVector> SolidDv;
void SaveSolidDv(set<PHSolid*> relative_solids, set<PHSolid*> nearest_solids)
{
	SolidDv.clear();

	for(set<PHSolid*>::iterator it = relative_solids.begin(); it != relative_solids.end(); ++it)
	{
		SolidDv.push_back((*it)->dv);
	}
	for(set<PHSolid*>::iterator it = nearest_solids.begin(); it != nearest_solids.end(); ++it)
	{
		SolidDv.push_back((*it)->dv);
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

// �\���V�~�����[�V�����ŏ㏑�����Ă��܂����ϐ��̒l�����ɖ߂��֐�
void RestoreParameters(set<PHConstraint*>* relative_consts, set<PHSolid*>* relative_solids, set<PHConstraint*> *nearest_consts, set<PHSolid*> *nearest_solids)
{
	int local_counter = 0;
	// fv��fw�̒l�����ɂ��ǂ�
	// lambda�̕ۑ��̉ߒ���relative_consts��iterator��vector�Ɋi�[���Ă���͂��Ȃ̂�
	// ����iterator�Ń��[�v���܂킹�ΐڐG���킩��Ȃ��Ă����������ɎQ�Ƃł���͂�
	for(set<PHConstraint *>::iterator it = relative_consts->begin(); it != relative_consts->end(); ++it)
	{
		(*it)->f = CollisionF[local_counter++];
	}
	for(set<PHConstraint *>::iterator it = nearest_consts->begin(); it != nearest_consts->end(); ++it)
	{
		(*it)->f = CollisionF[local_counter++];
	}

	local_counter = 0;
	// ���ׂĂ̍��̂�dv�����Ƃɖ߂�
	for(set<PHSolid*>::iterator it = relative_solids->begin(); it != relative_solids->end(); ++it)
	{
		(*it)->dv = SolidDv[local_counter];
/*
		it2 = SolidV.find(*it);
		(*it)->v = (*it2).second;

		it2 = SolidF.find(*it);
		(*it)->f = (*it2).second;
*/
		++local_counter;
	}
	for(set<PHSolid*>::iterator it = nearest_solids->begin(); it != nearest_solids->end(); ++it)
	{
		(*it)->dv = SolidDv[local_counter];
/*
		it2 = SolidV.find(*it);
		(*it)->v = (*it2).second;

		it2 = SolidF.find(*it);
		(*it)->f = (*it2).second;
*/
		++local_counter;
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

#ifndef EXP1
	// simulation���P�X�e�b�v�i�߂�
	if(bSimulation)StepSimulation();
#endif
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

	render->EndScene();
	glutSwapBuffers();
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
	sd.timeStep = (double)1.0 / (double)SIMULATION_FREQ;
	scene = phSdk->CreateScene(sd);				// �V�[���̍쐬
	PHSolidDesc desc;

#ifdef EXP2
	mass = 1.0;
	session = 0;
#endif

	// Solid�̍쐬
	for (unsigned int objectCnt=0; objectCnt<NUM_OBJECTS; ++objectCnt){
		// ���ʂ̐ݒ�
		// �X�P�[�����O�ɂ����MASS_SCALE�Ŋ���
		desc.mass = 1.0f / MASS_SCALE;// * (objectCnt + 1);

#ifdef EXP2
//		if(objectCnt == 1) desc.mass = 1.0f;
		if(objectCnt == 3)
		{
//			desc.mass = 1.0f;
			mass = desc.mass;
		}
		ofs << "mass of solid " << objectCnt << " = " << desc.mass << endl;
#elif defined Demo
		desc.mass = 0.3f;
#endif

		// inertia�̌v�Z��
		// �����̂̏ꍇ I = mass * (r1^2 + r2^2) / 12
		// ���̏ꍇ I = mass * r~2 * 2/5
		desc.inertia = Matrix3d(desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0, 0, 0, 0, desc.mass * 1.5 * 1.5 / 6.0);

		soObject.push_back(scene->CreateSolid(desc));		// ���̂�desc�Ɋ�Â��č쐬
//		ofs << "solid" << objectCnt << " = " << DCAST(PHSolid, soObject[objectCnt]) << endl;
#if defined EXP1 
		soObject[objectCnt]->SetGravity(false);
		bGravitySet.insert(DCAST(PHSolid, soObject[objectCnt]));
#elif defined EXP2
//		soObject[objectCnt]->SetGravity(false);
//		bGravitySet.insert(DCAST(PHSolid, soObject[objectCnt]));
#endif
	}

	desc.mass = 0.1f;
	desc.inertia = Matrix3d(0.16, 0, 0, 0, 0.16, 0, 0, 0, 0.16);

	for(int i = 0; i < 1; ++i)
	{
		PHSolidIf *soWall = scene->CreateSolid(desc);
		soWall->SetDynamical(false);
		soWalls.push_back(soWall);
		bGravitySet.insert(DCAST(PHSolid, soWalls[i]));
//		ofs << "static solid = " << DCAST(PHSolid, soWall) << endl;
	}
	soPointer = scene->CreateSolid(desc);

	//	�`��̍쐬
	CDBoxIf* floor=NULL;
	CDSphereIf* sphere=NULL;

	{
		/*
		CDSphereDesc sd;
		sd.radius = 1.25f;
		sphere = DCAST(CDSphereIf, phSdk->CreateShape(sd));

		// ���`�̃|�C���^
		soPointer->AddShape(sphere);
		soPointer->SetFramePosition(Vec3f(0, 0, 0));

		sd.radius = 2.0;
		sphere = DCAST(CDSphereIf,phSdk->CreateShape(sd));
		*/

		CDBoxDesc bd;
		for(int i = 0; i < 1; ++i)
		{
			Vec3f position;

			switch(i)
			{
			case 0:
				bd.boxsize = Vec3f(4.0f, 10.0f, 4.0f);
				position = Vec3f(0, -5.2f, 0);
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

		// �ʏ�̍��̗p��shape
		bd.boxsize = Vec3f(0.3f, 0.3f, 0.3f);
		floor = DCAST(CDBoxIf, phSdk->CreateShape(bd));
	}	

	for (unsigned int sphereCnt=0; sphereCnt<NUM_OBJECTS; ++sphereCnt){
		soObject[sphereCnt]->AddShape(floor);
#if defined EXP1
		soObject[sphereCnt]->SetFramePosition(Vec3f(1.55 * (sphereCnt + 1), 0.0, 0.0));
#elif defined EXP2
		if(sphereCnt < 2)soObject[sphereCnt]->SetFramePosition(Vec3f(1.55 * (sphereCnt + 1), 0.0, 0.0));
		else soObject[sphereCnt]->SetFramePosition(Vec3f(1.55 * (sphereCnt - 1), 0.0, 2.0));
#elif defined EXP4
		soObject[sphereCnt]->SetFramePosition(Vec3f(1.501 * (sphereCnt + 1) - 3.0, 0.0, 0.0));
#else
		soObject[sphereCnt]->SetFramePosition(Vec3f(0, 0.5f*(sphereCnt+1), 2));
#endif
	}
	scene->SetGravity(Vec3f(0,-9.8f * SPIDAR_SCALE, 0));	// �d�͂�ݒ�

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
	grDevice = grSdk->CreateDeviceGL();

	// �����ݒ�
	grDevice->Init();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	
	render->SetDevice(grDevice);	// �f�o�C�X�̐ݒ�

	// ���_��ݒ肷��
	Affinef view;
	view.Pos() = Vec3f(0.0, 0.75, -3.0);
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
inline PHSolid* GetAdjacentSolid(PHConstraint* constraint, PHSolid* solid, int* sign)
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

	if(scene->GetConstraintEngine()->IsInactiveSolid(solid0->Cast())) point->SetInactive(1, false);
	if(scene->GetConstraintEngine()->IsInactiveSolid(solid1->Cast())) point->SetInactive(0, false);

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

// debug�p���Ԍv���^�C�}�[
UTPreciseTimer timer;
static int start;
static int end;

// �����ɂ����鎞�Ԃ��v���B�ꎞ�I�Ȃ���
inline void t_start()
{
	start = timer.CountUS();
}

inline int t_end()
{
	// ����ɂ����鎞�Ԃ��`�F�b�N
	end = timer.CountUS();
	if(end > 0)
	{
		if(bOutput) ofs << "time = " << end << endl;
		return end;
	}
	return 0;
}

// �^����ꂽ�s��̋[���t�s����v�Z����
void CalcPinv(MatrixN6d M, Matrix6Nd* Pinv)
{
	Matrix6Nd Mt = M.trans();
	Matrix6d MTM = Mt * M;
	Matrix6d m = Matrix6d();

	// �Ίp�v�f�ɒl�𑫂��ă����N������h��
	// ���̐��x�͗����邪���܂�Ȃ��Ȃ邱�Ƃ͂Ȃ��Ȃ�
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

	spidarG6.Update(dt);//	USB�̒ʐM���s���D

	// SPIDAR�̈ʒu�̍X�V
	spidar_pos = view_rot * spidarG6.GetPos();		// SPIDAR���W���烏�[���h���W�ɂ���

#ifdef EXP1
	if(bExperiment)
	{
		if(num_data > 1500)
		{
			bExperiment = false;
			cout << "finish" << endl;
			
			// �t�@�C���ւ̃o�b�t�@�̏����o��
			for(int i = 0; i < num_data; ++i)
			{
				ofs << Buffer[i].x << endl;
			}
			ofs.close();

//			system("notepad C:/home/ohuchi/springhead2/src/tests/HIS/HISpidarG6/Release/log.txt");
			keyboard(ESC, 0, 0);

		}
		spidar_pos = pos_array[0];
		pos_array[0].x += 0.001;
	}
#endif

	// SPIDAR�̏����i�[����ϐ�
	static Vec3d old_pos = spidar_pos;

	// SPIDAR�̑��x���v�Z
	// ���x�̒�`�ł� v = dx / dt ����
	// dt�Ŋ���Ȃ��ق�������݂����Ȃ̂�
	// �����������g�����Ƃɂ���
	*SPIDARVel = spidar_pos - old_pos;
	old_pos = spidar_pos;

	// SPIDAR�̎p�����X�V
	Quaterniond qv;
	qv.FromMatrix(view_rot);

	spidar_ori = qv * spidarG6.GetOri();	
	
	static Quaterniond old_ori = spidar_ori;

	// SPIDAR�̉�]���x���v�Z
	// �O�̎p�����猻�݂̎p���ɕϊ�����S�������v�Z
	*SPIDARAngVel = spidar_ori * old_ori.Inv();
	old_ori = spidar_ori;

#if defined EXP1
	spidar_ori = Quaterniond::Rot(45, 'y');
	*SPIDARAngVel = Quaterniond();
#elif defined EXP2
	spidar_ori = Quaterniond::Rot(45, 'y');
	*SPIDARAngVel = Quaterniond();
#endif
}

/**
 brief		��͖@�̉e�����󂯂Ȃ����̂�o�^����B���̍��̂����ސڐG�͑���ɐڐG�͂�^���Ȃ�
 param		�o�^���鍄��
 return		�Ȃ�
*/
inline void AddInactiveSolid(PHSolidIf* soSolid)
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
	GLfloat yellow[] = {0.5, 0.5, 0.0, 1.0};
	GLfloat *color;

	// ��Ԃɂ����SPIDAR�̋��̐F��ς���
	if(bForce)
	{
		if(bSurroundEffect)color = orange;
		else if(!bLocalDynamics) color = green;
		else color = red;
	}
	else 
	{
		if(bSurroundEffect)color = blue;
		else if(!bLocalDynamics) color = yellow;
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
void DisplayCollisions()
{
	HapticInfo* info;

	if(current_valid_data) info = &info1;
	else info = &info2;

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

// �͊o��������������֐�
void InitHapticInfo(HapticInfo* info)
{
	// ����������
	// ����ȊO�̃f�[�^�͏㏑�����A����Ɍ��ŃA�N�Z�X�𐧌�����̂�
	// �������̕K�v�͂Ȃ�
	for(int i = 0; i < info->num_solids; ++i) 
	{
		info->nearest_solids[i] = NULL;
		info->num_col_per_sol[i] = 0;
	}
	for(int i = 0; i < info->num_collisions; ++i)
	{
		info->ForceSum[i] = Vec3d();
	}
	info->num_solids = 0;
	info->num_collisions = 0;
}

// �߂荞�݂̏󋵂��`�F�b�N���ăt���O���X�V����֐�
void updatePenetrations(HapticInfo* info)
{
	for(int i = 0; i < info->num_collisions; ++i)
	{
		// �ڐG���X�V�����u�Ԃ̐ڐG�󋵂��`�F�b�N����
		// ����ɂ������t���O��ݒ肷��
		double ddot = dot(info->pointer_col_positions[i] - info->col_positions[i], - info->col_normals[i]);			
		double distance = calcDistance(info->pointer_col_positions[i], info->col_positions[i]);

		// �ڐG���Ă��ē����Ȃ����̂������͋���������Ă���ڐG�̏ꍇ�͐ڐG����Ƃ���
		// �����Ȃ����̂�ʈ����ɂ��Ȃ���,�����Ȃ����̂Ƀ|�C���^���߂荞��ł����Ă��܂�
		if(ddot >= 0 && (!info->nearest_solids[info->ColToSol[i]]->IsDynamical() || distance <= DISTANCE_LIMIT))info->bCollide[i] = true;
		else info->bCollide[i] = false;
	}
}

inline void updateSimulationFrequency()
{
	SIMULATION_FREQ = SIMULATION_FREQ_BUFFER;
	scene->SetTimeStep((double)1.0 / SIMULATION_FREQ);
}

inline void calcTimeSpan(HapticInfo* info)
{
	info->Hdt = (double)1.0 / (double)HAPTIC_FREQ;
	info->Sdt = (double)1.0 / (double)SIMULATION_FREQ;
	info->HSratio = (double)HAPTIC_FREQ * info->Sdt;
	info->SHratio = (double)SIMULATION_FREQ * info->Hdt;
}

/*
			if(bOutput)
			{
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
			}
*/