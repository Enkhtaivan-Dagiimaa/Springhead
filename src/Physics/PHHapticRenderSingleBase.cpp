#include <Physics/PHHapticRenderSingleBase.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticRenderSingleBase
PHHapticRenderSingleBase::PHHapticRenderSingleBase(){}

void PHHapticRenderSingleBase::Step(){
	engine->StartDetection();
	UpdateInterface();
	UpdateHapticPointer();
	HapticRendering();
}

void PHHapticRenderSingleBase::UpdateInterface(){
	for(int i = 0; i < NHapticPointers(); i++){
		GetHapticPointer(i)->UpdateInterface((float)GetPhysicsTimeStep());
	}
}

void PHHapticRenderSingleBase::UpdateHapticPointer(){
	for(int i = 0; i < NHapticPointers(); i++){
		GetHapticPointer(i)->UpdateHapticPointer();
	}
}
void PHHapticRenderSingleBase::HapticRendering(){
	for(int i = 0; i < NHapticPointers(); i++){
		PHSolidsForHaptic* hsolids = GetHapticSolids();
		PHSolidPairsForHaptic* sps = GetSolidPairsForHaptic();
		GetHapticPointer(i)->MultiPointRendering(hsolids, sps, 1.0, GetPhysicsTimeStep(), GetPhysicsTimeStep());
	}
}


// �f�o�b�N�p�R�[�h
// ���ŁAPHScene::Step()����������Ă���̂�
// �����ŌĂԂ킯�ɂ͂����Ȃ�
void PHHapticRenderSingleBase::StepSimulation(){
	/// �V�~�����[�V�����̎��s
	engine->GetScene()->Step();
}




}