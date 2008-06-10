#include "BoxStack.h"
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
	if(bstack.bsync){
		// �͊o�|�C���^�̓���
		bstack.phpointer = hprocess.hpointer;
		bstack.UpdateHapticPointer();

		// �͊o�v���Z�X->�����v���Z�X
		// �͊o�v���Z�X�ł̋ߖT���̂̃V�~�����[�V�������ʂ𕨗��v���Z�X�ɔ��f������
		bstack.hapticsolids.clear();
		for(unsigned i = 0; i < hprocess.neighborObjects.size(); i++){
		// blocal��true�Cblocal���ȏꍇ�͌��ʂ𔽉f������
			if(bhaptic){
				if(bstack.neighborObjects[i].blocal && !bstack.neighborObjects[i].bfirstlocal){
					bstack.hapticsolids.push_back(hprocess.neighborObjects[i].phSolid);
					SpatialVector b = (bstack.neighborObjects[i].b + (bstack.neighborObjects[i].curb - bstack.neighborObjects[i].lastb)) *  bstack.dt;
					Vec3d v = hprocess.neighborObjects[i].phSolid.GetVelocity() + b.v();
					hprocess.neighborObjects[i].phSolidIf->SetVelocity(v);
					Vec3d w = hprocess.neighborObjects[i].phSolid.GetAngularVelocity() + b.w();
					hprocess.neighborObjects[i].phSolidIf->SetAngularVelocity(w);
					hprocess.neighborObjects[i].phSolidIf->SetCenterPosition(hprocess.neighborObjects[i].phSolid.GetCenterPosition());
					hprocess.neighborObjects[i].phSolidIf->SetOrientation(hprocess.neighborObjects[i].phSolid.GetOrientation());
					bstack.neighborObjects[i].test_force_norm = hprocess.neighborObjects[i].test_force_norm;
				}
			}
		}

		// �����v���Z�X->�͊o�v���Z�X
		// �V�[���ŐV�����������ꂽ���̂��i�[
		for(size_t i = hprocess.neighborObjects.size(); i < bstack.neighborObjects.size(); i++){
			hprocess.neighborObjects.resize(hprocess.neighborObjects.size() + 1);
			hprocess.neighborObjects.back() = bstack.neighborObjects[i];
		}
		for(unsigned i = 0; i < hprocess.neighborObjects.size(); i++){
			// ���߂Ď擾�����ߖT���̂̂ݍs��
			if(bstack.neighborObjects[i].bfirstlocal){
				hprocess.neighborObjects[i].phSolid = bstack.neighborObjects[i].phSolid;
				bstack.neighborObjects[i].bfirstlocal = false;
			}
			hprocess.neighborObjects[i].closestPoint = bstack.neighborObjects[i].closestPoint;
			hprocess.neighborObjects[i].pointerPoint = bstack.neighborObjects[i].pointerPoint;
			hprocess.neighborObjects[i].face_normal = bstack.neighborObjects[i].face_normal;
			hprocess.neighborObjects[i].last_face_normal = bstack.neighborObjects[i].last_face_normal;
			hprocess.neighborObjects[i].blocal = bstack.neighborObjects[i].blocal;
			hprocess.neighborObjects[i].A = bstack.neighborObjects[i].A;
			hprocess.neighborObjects[i].b = bstack.neighborObjects[i].b;
			hprocess.neighborObjects[i].curb = bstack.neighborObjects[i].curb;
			hprocess.neighborObjects[i].lastb = bstack.neighborObjects[i].lastb;
		}

		// �����v���Z�X�Ŏg�p���鍏�ݎ���
		//bstack.phscene->SetTimeStep(0.001f * (float)stepcount);
		//bstack.dt = 0.001f * (float)stepcount;
		hprocess.stepcount = 0;

		// �����I���̃t���O
		bstack.bsync = false;
	}
	if(hprocess.stepcount > hprocess.countmax) {
		DSTR << "too many step" << endl;
		hprocess.stepcount = 0;
		bstack.bsync = false;
	}
	hprocess.stepcount++;
	bstack.hapticcount++;
};

void _cdecl Keyboard(unsigned char key, int x, int y){
		switch (key) {
		case ESC:		
		case 'q':
			timer.Release();
			exit(0);
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
			bstack.Keyboard(key);
			hprocess.Keyboard(key);
			break;
	}
};

int _cdecl main(int argc, char* argv[]){
	Sleep(100);
	hprocess.Init();
	bstack.Init(argc, argv);
	StartTimer();
	glutKeyboardFunc(Keyboard);
	bstack.Start();
	return 0;
}