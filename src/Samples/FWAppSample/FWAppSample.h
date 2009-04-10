#ifndef FWAPPSAMPLE_H
#define FWAPPSAMPLE_H

#include <Springhead.h>
#include <Framework/SprFWAppGLUT.h>

using namespace Spr;
using namespace PTM;
using namespace std;

class FWAppSample : public FWAppGLUT, public UTRefCount{
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

	FWAppSample();	
	void Init(int argc, char* argv[]);		
	void Reset();
	void InitCameraView();										
	void DesignObject();
	void Step();
	void Display();		
	void Keyboard(int key, int x, int y);
	void MakeContactList();
}; 
extern FWAppSample bstack;

#endif