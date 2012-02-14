#include <Physics/PHHapticEngineImpulse.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImpulse

// 1/7�͂�������
void PHHapticLoopImpulse::Step(){
	UpdateInterface();
	HapticRendering();

}

void PHHapticLoopImpulse::HapticRendering(){
	PHHapticRenderInfo hri;
	hri.pointers = GetHapticPointers();
	hri.hsolids = GetHapticSolids();
	hri.sps = GetSolidPairsForHaptic();
	hri.hdt = GetHapticTimeStep();
	hri.pdt = GetPhysicsTimeStep();
	hri.loopCount = loopCount;
	hri.bInterpolatePose = true;
	hri.bMultiPoints = true;
	//PenaltyBasedRendering(hri);
	ConstraintBasedRendering(hri);
}


//----------------------------------------------------------------------------
// PHHapticEngineImpulse
PHHapticEngineImpulse::PHHapticEngineImpulse(){ 
	hapticLoop = &hapticLoopImpulse;
	hapticLoop->engineImp = this;
}

void PHHapticEngineImpulse::Step1(){};
void PHHapticEngineImpulse::Step2(){
	engine->StartDetection();
}

void PHHapticEngineImpulse::SyncHaptic2Physic(){
	// physics <------ haptic
	// PHSolidForHaptic�̓���
	// PHSolidPairForHaptic(�͊o�|�C���^�ƋߖT�̕���)�̊e����̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetPointerID();
		int nNeighbors = hpointer->neighborSolidIDs.size();
		// �ߖT���̂ł���y�A��������
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHapticSt* hst = (PHSolidPairForHapticSt*)hpair;
			PHSolidPairForHapticSt* pst = (PHSolidPairForHapticSt*)ppair;
			*pst = *hst;	// haptic���ŕێ����Ă����ׂ����𓯊�
		}
	}
	hapticLoop->ReflectForce2Solid(hapticLoop->GetHapticSolids(), GetHapticTimeStep(), GetPhysicsTimeStep());
}

void PHHapticEngineImpulse::SyncPhysic2Haptic(){
	// haptic <------ physics
	// PHSolidForHaptic�̓���
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		*psolid->GetLocalSolid() = *psolid->sceneSolid;	//	impulse�̏ꍇ�͏펞scene�ŊǗ�����Ă���solid�Ɠ���				
		*hsolid = PHSolidForHaptic(*psolid);			// LocalDynamics�̏ꍇ��dosim�ɂ���ē�������������K�v������
	}
	// solidpair, shapepair�̓���
	// �ߖT���̂̂ݓ���������
	for(int i = 0; i < NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		const int ppointerID = ppointer->GetPointerID();
		const int nNeighbors = ppointer->neighborSolidIDs.size();
		for(int j = 0; j < nNeighbors; j++){
			const int solidID = ppointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, ppointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, ppointerID);
			*hpair = PHSolidPairForHaptic(*ppair);
		}
	}
}

}