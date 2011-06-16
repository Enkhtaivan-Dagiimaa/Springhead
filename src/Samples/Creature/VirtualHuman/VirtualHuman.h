#ifndef VIRTUALHUMAN_H
#define VIRTUALHUMAN_H

#include <vector>

#include <Springhead.h>
#include <Framework/SprFWApp.h>
#include <Physics/SprPHIK.h>
#include <Creature/SprCRCreature.h>

#include "Creature/CRBodyGenerator/CRBallHumanBodyGen.h"

using namespace Spr;
using namespace PTM;
using namespace std;

class VirtualHuman : public FWApp{
public:
	FWWinIf* window;

	Vec3d gravity;

	//�f�o�b�N�\���Ɏg���ϐ�
	bool bGravity;
	bool bDebug;
	bool bGraphic;
	bool bStep;
	bool bOneStep;
	bool bIK;

	// ���[�U�̎w
	PHSolidIf* soCursor;
	double zP;

	// �r�̓��̂ɑ΂���Writheness
	double wrL, wrR, wrLG;
	Vec3d trunkUp, j1, j2, j3, g1;
	Vec3d via1L;

	// �N���[�`��
	CRBallHumanBodyGen            *bodyGen;

	CRSdkIf                       *crSdk;
	CRCreatureIf                  *creature;
	CRBodyIf                      *body;
	CRReachingControllerIf        *reachLH, *reachRH;

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
	bool LoadScene(UTString filename);
	void CalcWritheness();
	void RenderWritheness(GRRenderIf* curRender);
}; 

extern VirtualHuman app;

#endif
