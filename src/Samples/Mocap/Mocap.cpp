// FWAppGLtest.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include <tchar.h>
#include <Springhead.h>
#include <SprFramework.h>
#include <Foundation/WBPreciseTimer.h>
#include <Framework/FWAppGL.h>
#include <fstream>
#include <sstream>
#include <iomanip>
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
	double time;
	WBPreciseTimer ptimer;

	MyApp(){
		appState = INVALID;
		stepCount = 0;
		waitCount = 0;
	}
	void Display(){
		FWAppGL::Display();

		if (stepTime==0) stepTime=1000;
		std::ostringstream os;
		os << std::setw(5) << 1000000/stepTime << "FPS";
		if (appState == MAKE_ANGLE){
			os << "    �}�[�J�ʒu�v���f�[�^ �� �֐ߊp�f�[�^  �f�[�^�ϊ���";
		}else{
			os << "    �֐ߊp�f�[�^�Đ� �{ �����̓V�~�����[�V������";
		}
		grRender->DrawFont(Vec2f(0,0), os.str().c_str());
	}
	void Step(){
		stepTime = ptimer.CountUS();
		//-------- Diff�t�@�C���Đ����e�֐ߊp�x��ۑ�------------//
		if (appState == MAKE_ANGLE){
			if (waitCount++ < 300) time = 0;
			bool rv = diff.AddSpringForce(human, time, (float)phScene->GetTimeStep());
			if (time > 0) human.SaveJointAngle();
			if (!rv){	//	�Ō�̃f�[�^�������ꍇ�C������x���[�h����appState��i�߂�D
				Load();
				waitCount = 0;
			}
		}
	//-------- �ۑ����ꂽ�e�֐ߊp�x�̍Đ������̕␳���e�֐߂̃g���N�̕ۑ�------------//
		else if (appState == MAKE_FORCE){
			if (waitCount++ < 500){
				time = 0.0f;
				diff.AddSpringForce(human, time, (float)phScene->GetTimeStep());
				human.SetSpring((float)phScene->GetTimeStep());
				human.LoadJointAngle(time, (float)phScene->GetTimeStep());
			}else if (waitCount++ < 2000){
				time = 0.0f;
				human.LoadJointAngle(time, (float)phScene->GetTimeStep());
			}else{
				//human.SaveTorque(time, scene->GetTimeStep());
				bool rv = human.LoadJointAngle(time, phScene->GetTimeStep());
				if (!rv){	//	�Ō�̃f�[�^�������ꍇ�C������x���[�h����appState��i�߂�D
					std::ofstream of("force.txt");
					for(float t=0.001f; t<time; t+= 0.001f){
						int c = t / phScene->GetTimeStep();
						TDiffRecord pos, vel;
						diff.frf.GetRecord(pos, vel, t);
						if (c < (int)human.angles.size()){
							of << t << "\t";
							of << pos.data[1].X() << "\t";
							of << pos.data[1].Y() << "\t";
							of << pos.data[1].Z() << "\t";
							of << human.angles[c].force.X() << "\t";
							of << human.angles[c].force.Y() << "\t";
							of << human.angles[c].force.Z() << "\t";
							for(int i=0; i<human.joints.size(); ++i){
								of << human.angles[c].jointTorque[i] << "\t";
							}
							of << std::endl;
						}
					}
					Load();
					waitCount = 0;
				}
			}
		}
		FWAppGL::Step();
	}
	void Load(){
		LoadFile(filename);
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

	FWAppGLIf* fwApp = CreateFWAppGL();
//	fwApp->SetDebugMode(FWAppGLDesc::DM_DEBUG);
	fwApp->StartApp("vhOld.x");

	return 0;
}

