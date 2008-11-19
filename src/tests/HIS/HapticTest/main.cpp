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
bool bhaptic = true;

void AppStart();
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
		pprocess.hapticsolids.clear();
		for(unsigned i = 0; i < hprocess.neighborObjects.size(); i++){
		// blocal��true�Cblocal���ȏꍇ�͌��ʂ𔽉f������
			if(bhaptic){
				if(pprocess.neighborObjects[i].blocal && !pprocess.neighborObjects[i].bfirstlocal){
					pprocess.hapticsolids.push_back(hprocess.neighborObjects[i].phSolid);
					SpatialVector b = (pprocess.neighborObjects[i].b + (pprocess.neighborObjects[i].curb - pprocess.neighborObjects[i].lastb)) *  pprocess.dt;
					Vec3d v = hprocess.neighborObjects[i].phSolid.GetVelocity() + b.v();
					hprocess.neighborObjects[i].phSolidIf->SetVelocity(v);
					Vec3d w = hprocess.neighborObjects[i].phSolid.GetAngularVelocity() + b.w();
					hprocess.neighborObjects[i].phSolidIf->SetAngularVelocity(w);
					hprocess.neighborObjects[i].phSolidIf->SetCenterPosition(hprocess.neighborObjects[i].phSolid.GetCenterPosition());
					hprocess.neighborObjects[i].phSolidIf->SetOrientation(hprocess.neighborObjects[i].phSolid.GetOrientation());
					pprocess.neighborObjects[i].test_force_norm = hprocess.neighborObjects[i].test_force_norm;
				}
			}
		}

		// �����v���Z�X->�͊o�v���Z�X
		// �V�[���ŐV�����������ꂽ���̂��i�[
		for(size_t i = hprocess.neighborObjects.size(); i < pprocess.neighborObjects.size(); i++){
			hprocess.neighborObjects.resize(hprocess.neighborObjects.size() + 1);
			hprocess.neighborObjects.back() = pprocess.neighborObjects[i];
		}
		for(unsigned i = 0; i < hprocess.neighborObjects.size(); i++){
			// ���߂Ď擾�����ߖT���̂̂ݍs��
			if(pprocess.neighborObjects[i].bfirstlocal){
				hprocess.neighborObjects[i].phSolid = pprocess.neighborObjects[i].phSolid;
				pprocess.neighborObjects[i].bfirstlocal = false;
			}
			hprocess.neighborObjects[i].closestPoint = pprocess.neighborObjects[i].closestPoint;
			hprocess.neighborObjects[i].pointerPoint = pprocess.neighborObjects[i].pointerPoint;
			hprocess.neighborObjects[i].face_normal = pprocess.neighborObjects[i].face_normal;
			hprocess.neighborObjects[i].last_face_normal = pprocess.neighborObjects[i].last_face_normal;
			hprocess.neighborObjects[i].blocal = pprocess.neighborObjects[i].blocal;
			hprocess.neighborObjects[i].A = pprocess.neighborObjects[i].A;
			hprocess.neighborObjects[i].b = pprocess.neighborObjects[i].b;
			hprocess.neighborObjects[i].curb = pprocess.neighborObjects[i].curb;
			hprocess.neighborObjects[i].lastb = pprocess.neighborObjects[i].lastb;
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
	pprocess.soBox.clear();
	pprocess.sceneSolids.clear();
	pprocess.neighborObjects.clear();
	hprocess.neighborObjects.clear();
	pprocess.bsync = false;
	pprocess.calcPhys=true;
	pprocess.hapticcount = 1;
	hprocess.stepcount = 1;

	// scene�̃��[�h
	pprocess.GetSdk()->Clear();															// SDK�̍쐬
	pprocess.GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	pprocess.phscene = pprocess.GetSdk()->GetScene()->GetPHScene();
	pprocess.states = ObjectStatesIf::Create();
	pprocess.states2 = ObjectStatesIf::Create();
	pprocess.DesignObject();																// ���̂��쐬
	pprocess.phscene->SetGravity(pprocess.gravity);				
	pprocess.phscene->SetTimeStep(pprocess.dt);
	pprocess.phscene->SetNumIteration(pprocess.nIter);
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
		case 'h':
			if(bhaptic){
				bhaptic = false;
				DSTR << "Haptic Disconnect" << endl;
			}else{
				bhaptic = true;
				DSTR << "Haptic Connect" << endl;
			}
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