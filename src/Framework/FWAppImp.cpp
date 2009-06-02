#include "FWAppImp.h"
#include "FWAppAdapter.h"

/*FWAppImpDesc�̎���*/
////////////////////////////////////////////////////////////////////////////////////
FWAppImpDesc::FWAppImpDesc(){
		sceneName = "";									// �V�[���̖��O
		pdt = 0.05f;
		nIter = 45;
		gravity =  Vec3d(0, -9.8f, 0);
		range = 0.7;
}

/*FWAppImp�̎���*/
////////////////////////////////////////////////////////////////////////////////////
FWAppImp::FWAppImp(){
	bStep = true;
	fwscene = NULL;
};

void FWAppImp::SetAdapter(FWAppAdapter* a){ adapter = a; }
FWSdkIf* FWAppImp::GetSdk(){ return adapter->GetSdk(); }
void FWAppImp::SetScene(FWSceneIf* s){ fwscene =s; }
FWSceneIf* FWAppImp::GetScene(){ return fwscene; } 
void FWAppImp::SetHapticInterface(HISpidarGIf* s){ spidarG6 = s; }
HISpidarGIf* FWAppImp::GetHapticInterface(){ return spidarG6; }
string FWAppImp::GetSceneName(){ return sceneName; }

void FWAppImp::Init(){
	SetScene(GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc()));		// Scene�̍쐬�C�ݒ�
	ResetScene();
}
void FWAppImp::ResetScene(){
	PHSceneIf* phscene;
	phscene = GetScene()->GetPHScene();
	phscene->Clear();
	phscene->SetGravity(gravity);				
	phscene->SetTimeStep(pdt);
	phscene->SetNumIteration(nIter);
	BuildScene();
}
void FWAppImp::BuildScene(){}
void FWAppImp::Idle(){}
void FWAppImp::Step(){}
void FWAppImp::CallBack(){}
void FWAppImp::DebugMode(bool bDebug, GRDebugRenderIf* render){}
void FWAppImp::Keyboard(int key, int x, int y){}
