#ifndef BOXSTACK_H
#define BOXSTACK_H

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

class BoxStack : public FWAppGLUT, public UTRefCount{
public:
	//�@�v���Z�X�Ԃ̓����Ɏg���ϐ�
	volatile bool bsync;
	bool calcPhys;
	volatile int hapticcount;
	FWWin* window;
	PHSceneIf* phscene;		
	GRDebugRenderIf* render;		
	double dt;
	Vec3d gravity;
	double nIter;
	bool bGravity;
	// ���̂Ɏg���ϐ�
	PHSolidDesc desc;
	PHSolidIf* soFloor;
	PHSolidIf* soPointer;
	vector<PHSolidIf*> soBox;
	PHSolid phpointer;
	CDConvexMeshIf* meshFloor;
	CDConvexMeshIf* meshConvex;
	CDBoxIf* meshBox;
	CDSphereIf* meshSphere;
	CDCapsuleIf* meshCapsule;
	// �ߖT���̒T���Ɏg���ϐ�
	double range;
	vector<PHNeighborObject> neighborObjects;	///<�ߖT���̂��i�[����
	vector<PHSceneSolid> sceneSolids;
	// �\���V�~�����[�V�����Ɏg���ϐ�
	UTRef<ObjectStatesIf> states, states2;
	//�f�o�b�N�\���Ɏg���ϐ�
	bool bDebug;
	bool bStep;
	bool bOneStep;
	vector<PHSolid> hapticsolids;

	struct Edge{
		float edge;				///<	�[�̈ʒu(�O���[�o���n)
		int	index;				///<	���� solid�̈ʒu
		bool bMin;				///<	���[�Ȃ�true
		bool operator < (const Edge& s) const { return edge < s.edge; }
	};
	typedef std::vector<Edge> Edges;

	BoxStack();	
	void Init(int argc, char* argv[]);				
	void InitCameraView();										
	void DesignObject();
	void Idle();
	void Start();
	void PhysicsStep();
	void Display();		
	void UpdateHapticPointer();
	void FindNearestObject();
	void PredictSimulation();
	void FindNearestPoint();
	void DisplayContactPlane();
	void DisplayLineToNearestPoint();
	void DrawHapticSolids();
	void Keyboard(unsigned char key);
}; 
extern BoxStack bstack;

#endif