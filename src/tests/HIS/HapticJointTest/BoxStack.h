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
	bool naga;
	// xFile�̃��[�h�Ɏg���ϐ�
	const char* filePath;		//< xFile���u���Ă���t�H���_�ւ̑��΃p�X
	const char* fileName;		//< ���[�h����t�@�C����
	const char* hPointerName;	//< �͊o�|�C���^�̖��O

	//�@�v���Z�X�Ԃ̓����Ɏg���ϐ�
	volatile bool bsync;
	bool calcPhys;
	volatile int hapticcount;
	FWWin* window;
	PHSceneIf* phscene;		
	GRRenderIf* render;		
	double dt;
	Vec3d gravity;
	double nIter;
	bool bGravity;

	// ���̂Ɏg���ϐ�
	PHSolidIf* soPointer;
	vector<PHSolidIf*> soBox;
	PHSolid phpointer;
	/*CDBoxIf* meshBox;*/
	//Floor
	PHSolidIf*			soFloor;
	CDShapeIf*			shapeFloor;
	//Object
	PHSolidDesc			desc;
	PHSolidIf*			soBox2[3];
	PHJointIf*			jointBox;
	vector<PHSolidIf*>	soObject;
	CDBoxIf*			shapeBox;
	CDSphereIf*			shapeSphere;

	//�|�C���^�ɂ�����
	PHNeighborObject	grabObject;
	Vec3d				grabForce;
	bool				grabObjectFlag;
	double				grabForceRate;

	//�L�[�{�[�h�p�̃J�E���g�ϐ�
	double countKey;
	// �ߖT���̒T���Ɏg���ϐ�
	double range;
	vector<PHNeighborObject> neighborObjects;	///<�ߖT���̂��i�[����
	vector<PHSceneSolid> sceneSolids;
	// �\���V�~�����[�V�����Ɏg���ϐ�
	UTRef<ObjectStatesIf> states, states2;
	//�f�o�b�N�\���Ɏg���ϐ�
	bool bStep;
	bool bOneStep;
	bool bDebug;
	vector<PHSolid> hapticsolids;

	struct Edge{
		float edge;				///<	�[�̈ʒu(�O���[�o���n)
		int	index;				///<	���� solid�̈ʒu
		bool bMin;				///<	���[�Ȃ�true
		bool operator < (const Edge& s) const { return edge < s.edge; }
	};
	typedef std::vector<Edge> Edges;
	static BoxStack* GetInstance(){return (BoxStack*)instance;}
	BoxStack();	
	void DesignObject();
	void CreateObject();
	bool CopyObject(const char* phName, const char* grName);

	void Init(int argc, char* argv[]);		
	void SetHapticPointer();
	void InitCameraView();		
	void Start();
	void Idle();
	void PhysicsStep();
	void Display();		
	void UpdateHapticPointer();
	void FindNearestObject();
	void PredictSimulation();
	void FindNearestPoint();
	void DisplayContactPlane();
	void DisplayLineToNearestPoint();
	void DrawHapticSolids();
//	bool CopyObject(const char* phName, const char* grName);
	void Keyboard(unsigned char key);

	void GrabSpring(double dt);
}; 
extern BoxStack bstack;


#endif