#include "PhysicsProcess.h"
#include "HapticProcess.h"
#include <GL/glut.h>
#include <iostream>
#include <time.h>
#include "Foundation/UTMMTimer.h"
#include <Physics/PHConstraintEngine.h>

#ifdef USE_HDRSTOP
#pragma hdrstop
#endif

#define ESC 27

using namespace std;

UTMMTimer timer;

void CallBackHapticProcess(void* arg);
void Synchronize();
void _cdecl Keyboard(unsigned char key, int x, int y);


void CallBackHapticProcess(void* arg){
	if(hprocess.stepcount > 300) return;
	hprocess.Step();
	Synchronize();
};

void StartTimer(){
	timer.Resolution(1);
	timer.Interval(1);
	timer.Set(CallBackHapticProcess, NULL);
	timer.Create();
};

void Synchronize(){
	if(pprocess.bsync){
		// �͊o�|�C���^�̓���
		pprocess.phpointer = hprocess.hpointer;
		pprocess.UpdateHapticPointer();

		// �͊o�v���Z�X->�����v���Z�X
		// �͊o�v���Z�X�ł̋ߖT���̂̃V�~�����[�V�������ʂ𕨗��v���Z�X�ɔ��f������
		for(unsigned i = 0; i < hprocess.expandedObjects.size(); i++){
		// blocal��true�Cblocal���ȏꍇ�͌��ʂ𔽉f������
			if(pprocess.expandedObjects[i].flag.blocal && !pprocess.expandedObjects[i].flag.bfirstlocal){
				SpatialVector b = (pprocess.expandedObjects[i].syncInfo.motionCoeff.b + 
					(pprocess.expandedObjects[i].syncInfo.motionCoeff.curb - pprocess.expandedObjects[i].syncInfo.motionCoeff.lastb)) *  pprocess.dt;
				Vec3d v = hprocess.expandedObjects[i].phSolid.GetVelocity() + b.v();
				hprocess.expandedObjects[i].phSolidIf->SetVelocity(v);
				Vec3d w = hprocess.expandedObjects[i].phSolid.GetAngularVelocity() + b.w();
				hprocess.expandedObjects[i].phSolidIf->SetAngularVelocity(w);
				hprocess.expandedObjects[i].phSolidIf->SetCenterPosition(hprocess.expandedObjects[i].phSolid.GetCenterPosition());
				hprocess.expandedObjects[i].phSolidIf->SetOrientation(hprocess.expandedObjects[i].phSolid.GetOrientation());
				pprocess.expandedObjects[i].syncInfo.neighborPoint.test_force_norm = hprocess.expandedObjects[i].syncInfo.neighborPoint.test_force_norm;
			}
		}

		// �����v���Z�X->�͊o�v���Z�X
		// �V�[���ŐV�����������ꂽ���̂��i�[
		for(size_t i = hprocess.expandedObjects.size(); i < pprocess.expandedObjects.size(); i++){
			hprocess.expandedObjects.resize(hprocess.expandedObjects.size() + 1);
			hprocess.expandedObjects.back() = pprocess.expandedObjects[i];
		}
		for(unsigned i = 0; i < hprocess.expandedObjects.size(); i++){
			// ���߂Ď擾�����ߖT���̂̂ݍs��
			if(pprocess.expandedObjects[i].flag.bfirstlocal){
				hprocess.expandedObjects[i].phSolid = pprocess.expandedObjects[i].phSolid;
				pprocess.expandedObjects[i].flag.bfirstlocal = false;
			}
			hprocess.expandedObjects[i].syncInfo = pprocess.expandedObjects[i].syncInfo;
		}

		// �����v���Z�X�Ŏg�p���鍏�ݎ���
		//pprocess.phscene->SetTimeStep(0.001f * (float)stepcount);
		//pprocess.dt = 0.001f * (float)stepcount;
		hprocess.stepcount = 0;

		// �����I���̃t���O
		pprocess.bsync = false;
	}
	if(hprocess.stepcount > hprocess.countmax) {
		DSTR << "too many step" << endl;
		hprocess.stepcount = 0;
		pprocess.bsync = false;
	}
	hprocess.stepcount++;
	pprocess.hapticcount++;
};

void Reset(){
	timer.Release();
	// �����ō����vector��������
	pprocess.sceneSolids.clear();
	pprocess.expandedObjects.clear();
	hprocess.expandedObjects.clear();
	pprocess.bsync = false;
	pprocess.calcPhys=true;
	pprocess.hapticcount = 1;
	hprocess.stepcount = 1;

	// scene�̃��[�h
	pprocess.GetSdk()->Clear();															// SDK�̍쐬
	pprocess.GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	PHSceneIf* phscene = pprocess.GetSdk()->GetScene()->GetPHScene(); 
	pprocess.states = ObjectStatesIf::Create();
	pprocess.states2 = ObjectStatesIf::Create();
	pprocess.DesignObject();																// ���̂��쐬
	phscene->SetGravity(pprocess.gravity);				
	phscene->SetTimeStep(pprocess.dt);
	phscene->SetNumIteration(pprocess.nIter);
	pprocess.GetCurrentWin()->SetScene(pprocess.GetSdk()->GetScene());
	// pprocess.GetCurrentWin()->SetRender(pprocess.GetSdk()->GetRender());
	pprocess.InitCameraView();
	timer.Create();

	DSTR << "Reset" << endl;
}

void _cdecl Keyboard(unsigned char key, int x, int y){
		switch (key) {
		case ESC:		
		case 'q':
			timer.Release();
			exit(0);
			break;
		case 'r':
			Reset();
			pprocess.Keyboard('x');
			break;
		default:
			pprocess.Keyboard(key);
			hprocess.Keyboard(key);
			break;
	}
};

int _cdecl main(int argc, char* argv[]){
	Sleep(100);
	hprocess.Init();
	pprocess.Init(argc, argv);
	StartTimer();
	glutKeyboardFunc(Keyboard);
	pprocess.Start();
	return 0;
}