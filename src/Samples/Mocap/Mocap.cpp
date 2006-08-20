// FWAppGLtest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <tchar.h>
#include <Springhead.h>
#include <SprFramework.h>
#include <Foundation/WBPreciseTimer.h>
#include <Framework/FWAppGL.h>
#include <Framework/FWScene.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <gl/glut.h>
#include "THuman.h"
#include "TDiffFile.h"


namespace Spr{
	void SPR_CDECL PHRegisterTypeDescs();
	void SPR_CDECL CDRegisterTypeDescs();
	void SPR_CDECL GRRegisterTypeDescs();
	void SPR_CDECL FIRegisterTypeDescs();

	void SPR_CDECL FWRegisterTypeDescs();
	void SPR_CDECL FWRegisterFactories();

class MyApp:public FWAppGL{
public:
	THuman human;						//	Virtual Human
	TDiffFile diff;						//	Diff �t�@�C��
	enum TAppState{						//	�A�v���P�[�V�����̏��
		INVALID,						//	����
		MAKE_ANGLE,						//	DIFF�t�@�C���̒��_��񂩂�p�x�t�@�C���𐶐�
		MAKE_FORCE,						//	�����͌v�Z
		NON_GRAVITY						//	���Œ�̃g���N����
	} appState;
	int stepCount;
	int waitCount;
	int stepTime;
	float time;
	WBPreciseTimer ptimer;

	MyApp(){
		appState = INVALID;
		stepCount = 0;
		waitCount = 0;
	}
	void Display(){
		DCAST(FWScene, fwScene)->Sync();

		grRender->ClearBuffer();
		grRender->BeginScene();
		if (debugMode == DM_DEBUG){
			GRCameraIf* cam = NULL;
			if (grScene) cam = grScene->GetCamera();
			if (cam) cam->Render(grRender);
			grRender->DrawScene(phScene);
			if (cam) cam->Rendered(grRender);
		}else{
			grScene->Render(grRender);
		}

		diff.Draw(DCAST(GRRender, grRender), human, time, true);

		if (stepTime==0) stepTime=1000;
		std::ostringstream os;
		os << std::setw(5) << 1000000/stepTime << "FPS";
		if (appState == MAKE_ANGLE){
			os << "    �}�[�J�ʒu�v���f�[�^ �� �֐ߊp�f�[�^  �f�[�^�ϊ���";
		}else{
			os << "    �֐ߊp�f�[�^�Đ� �{ �����̓V�~�����[�V������";
		}
		grRender->DrawFont(Vec2f(0,0), os.str().c_str());

		grRender->EndScene();	

	}
	void Step(){
		stepTime = ptimer.CountUS();
		float dt = phScene->GetTimeStep();
		//-------- Diff�t�@�C���Đ����e�֐ߊp�x��ۑ�------------//
		if (appState == MAKE_ANGLE){
			if ((waitCount++)*dt < 0) time = 0;
			bool rv = diff.AddSpringForce(human, time, dt);
			if (time > 0) human.SaveJointAngle();
			phScene->Step();
			stepCount ++;
			time += phScene->GetTimeStep();
			if (!rv){	//	�Ō�̃f�[�^�������ꍇ�CappState��i�߂�D
				appState = MAKE_FORCE;
				waitCount = 0;
			}
		}
	//-------- �ۑ����ꂽ�e�֐ߊp�x�̍Đ������̕␳���e�֐߂̃g���N�̕ۑ�------------//
		else if (appState == MAKE_FORCE){
			if (waitCount++ < 200){
				time = 0.0f;
				diff.AddSpringForce(human, time, (float)phScene->GetTimeStep());
				human.SetSpring((float)phScene->GetTimeStep());
				human.LoadJointAngle(time, (float)phScene->GetTimeStep());
			}else if (waitCount++ < 2000){
				PHSolid* sFloor = DCAST(PHSolid, phScene->FindObject("sofrFloor") );
				PHSolid* sRFoot = DCAST(PHSolid, phScene->FindObject("soRFoot") );
				PHSolid* sLFoot = DCAST(PHSolid, phScene->FindObject("soLFoot") );
				phScene->SetContactMode(sFloor, sRFoot);
				phScene->SetContactMode(sFloor, sLFoot);
				human.LoadJointAngle(time, (float)phScene->GetTimeStep());
			}
			phScene->Step();
			stepCount ++;
			time += phScene->GetTimeStep();
			if (time > 3){
				waitCount = 0;
			}
		}
	}
	void Load(){
		FWAppGL::Load();
		phScene->SetContactMode(PHSceneDesc::MODE_NONE);
		phScene->SetTimeStep(0.002);
		phScene->SetNumIteration(150);
		human.Connect(DCAST(PHScene, phScene));
		if (human.IsLoaded()){
			//	diff�̃��[�h
			if (!diff.IsLoaded()){
				std::ifstream file("Test.DA");
				if (file.good()) diff.Load(file);
			}
			//	VH�̑傫�������킹��D
			diff.SetHumanScale(human);
			//	�A�v���P�[�V�����̏��
			time = 0;
			appState = MAKE_ANGLE;
			human.angles.clear();
			waitCount = 0;
		}
	}
	void Timer(){
		int n = 0.01/phScene->GetTimeStep();
		if (n==0) n = 1;
		for(int i=0; i<n; ++i){
			Step();
		}
		glutPostRedisplay();
	}
};

}

using namespace Spr;

int _tmain(int argc, _TCHAR* argv[])
{
	PHRegisterTypeDescs();
	CDRegisterTypeDescs();
	GRRegisterTypeDescs();
	FIRegisterTypeDescs();
	FWRegisterTypeDescs();
	FWRegisterFactories();

	PHRegisterSdk();
	GRRegisterSdk();
	FWRegisterSdk();

	FWAppGLIf* fwApp = new Spr::MyApp;
//	fwApp->SetDebugMode(FWAppGLDesc::DM_DEBUG);
	fwApp->StartApp("vhOld.x");

	return 0;
}

