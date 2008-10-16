#ifndef BOXSTACK_H
#define BOXSTACK_H

#include <Springhead.h>
#include <Framework/SprFWAppGLUT.h>
#include <Base/Combination.h>
#include<Collision/CDDetectorImp.h>
#include <Physics/PHSolid.h>

using namespace Spr;
using namespace PTM;
using namespace std;



class BoxStack : public FWAppGLUT, public UTRefCount{
public:
	FWWin* window;

	double dt;
	Vec3d gravity;
	double nIter;
	bool bGravity;
	// ���̂Ɏg���ϐ�
	PHSolidDesc desc;
	PHSolidIf* soFloor;
	PHSolidIf* soPointer;
	vector<PHSolidIf*> soBox;
	CDConvexMeshIf* meshFloor;
	CDConvexMeshIf* meshConvex;
	CDBoxIf* meshBox;
	CDSphereIf* meshSphere;
	CDCapsuleIf* meshCapsule;
	//�f�o�b�N�\���Ɏg���ϐ�
	bool bDebug;
	bool bStep;
	bool bOneStep;

	BoxStack();	
	void Init(int argc, char* argv[]);		
	void Reset();
	void InitCameraView();										
	void DesignObject();
	void Step();
	void Display();		
	void Keyboard(int key, int x, int y);
	void MakeContactList();
}; 
extern BoxStack bstack;

#endif