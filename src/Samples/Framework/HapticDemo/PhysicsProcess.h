#ifndef PHYSICS_PROCESS_H
#define PHYSICS_PROCESS_H

#include "Synchronization.h"
#include <Framework/SprFWAppGLUT.h>

using namespace Spr;
using namespace PTM;
using namespace std;

class PhysicsProcess : public FWAppGLUT, public UTRefCount{
public:
	//�@�v���Z�X�Ԃ̓����Ɏg���ϐ�
	volatile bool bsync;
	bool calcPhys;
	volatile int hapticcount;
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
	void PhysicsProcess::DisplayLocalSphere();
	void DisplayContactPlane();
	void DisplayLineToNearestPoint();
	void Keyboard(unsigned char key);
}; 
extern PhysicsProcess pprocess;

#endif