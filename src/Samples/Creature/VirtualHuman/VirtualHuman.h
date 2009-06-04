#ifndef VIRTUALHUMAN_H
#define VIRTUALHUMAN_H

#include <vector>

#include <Springhead.h>
#include <Framework/SprFWAppGLUT.h>
#include <Physics/PHIK.h>
#include <Creature/CRCreature.h>

using namespace Spr;
using namespace PTM;
using namespace std;

class VirtualHuman : public FWAppGLUT, public UTRefCount{
public:
	FWWin* window;

	Vec3d gravity;

	//�f�o�b�N�\���Ɏg���ϐ�
	bool bGravity;
	bool bDebug;
	bool bStep;
	bool bOneStep;
	bool bIK;

	// ���[�U�̎w
	PHSolidIf* soCursor;
	double zP;

	// �N���[�`��
	CRCreatureIf                  *creature, *userhand;
	CRBallHumanBodyDesc            descBody;
	CRBallHumanBodyIf             *body, *userhandModel;
	CRReachingControllersIf       *reaches;

	VirtualHuman();
	void Init(int argc, char* argv[]);
	void Reset(int sceneNum=0);
	void InitCameraView();
	void BuildScene(int sceneNum=0);
	void Step();
	void Display();		
	void Keyboard(int key, int x, int y);
	void MouseButton(int button, int state, int x, int y);
	void MouseMove(int x, int y);

	void OneStep();
	void UpdateCursor(int x, int y);
}; 

extern VirtualHuman app;

#endif
