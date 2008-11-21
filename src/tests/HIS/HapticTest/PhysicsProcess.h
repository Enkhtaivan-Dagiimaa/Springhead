#ifndef PHYSICS_PROCESS_H
#define PHYSICS_PROCESS_H

#include "Synchronization.h"
#include <Framework/SprFWAppGLUT.h>
#include <Collision/CDDetectorImp.h>
#include <Physics/PHContactDetector.h>
#include <Physics/PHContactPoint.h>
#include <Base/Combination.h>
#include<Collision/CDDetectorImp.h>
#include <Physics/PHSolid.h>

using namespace Spr;
using namespace PTM;
using namespace std;

struct PHSceneSolid{
	PHSolidIf* phSolidIf;
	bool bneighbor;
	bool blocal;
	bool bfirstlocal;
};

struct Edge{
	float edge;				///<	�[�̈ʒu(�O���[�o���n)
	int	index;				///<	���� solid�̈ʒu
	bool bMin;				///<	���[�Ȃ�true
	bool operator < (const Edge& s) const { return edge < s.edge; }
};
typedef vector<Edge> Edges;

class PhysicsProcess : public FWAppGLUT, public UTRefCount{
public:
	//�@�v���Z�X�Ԃ̓����Ɏg���ϐ�
	volatile bool bsync;
	bool calcPhys;
	volatile int hapticcount;
	FWWin* window;	
	double dt;
	Vec3d gravity;
	double nIter;
	bool bGravity;
	// ���̂Ɏg���ϐ�
	PHSolidIf* soPointer;
	PHSolid phpointer;

	// �ߖT���̒T���Ɏg���ϐ�
	double range;
	vector<ExpandedObject> expandedObjects; 

//	vector<PHNeighborObject> expandedObjects;	///<�ߖT���̂��i�[����
//	vector<PHSceneSolid> sceneSolids;
	// �\���V�~�����[�V�����Ɏg���ϐ�
	UTRef<ObjectStatesIf> states, states2;
	//�f�o�b�N�\���Ɏg���ϐ�
	bool bDebug;
	bool bStep;
	bool bOneStep;

	PhysicsProcess();	
	void Init(int argc, char* argv[]);				
	void InitCameraView();										
	void DesignObject();
	void Idle();
	void Start();
	void PhysicsStep();
	void Display();		
	void UpdateHapticPointer();
	void ExpandSolidInfo();
	void FindNearestObject();
	void PredictSimulation();
	void DisplayContactPlane();
	void DisplayLineToNearestPoint();
	void Keyboard(unsigned char key);
}; 
extern PhysicsProcess pprocess;

#endif