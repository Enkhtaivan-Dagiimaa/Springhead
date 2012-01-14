#include <Physics/PHHapticRenderImpulse.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImpulse

// 1/7�͂�������
void PHHapticLoopImpulse::Step(){
	UpdateInterface();
	HapticRendering();
}

void PHHapticLoopImpulse::HapticRendering(){
	for(int i = 0; i < NHapticPointers(); i++){
		//GetHapticPointer(i)->HapticRendering(GetHapticSolids(), GetSolidPairsForHaptic(), loopCount);
		GetHapticPointer(i)->MultiPointRendering(GetHapticSolids(), GetSolidPairsForHaptic(), loopCount);
	}
}

//----------------------------------------------------------------------------
// PHHapticRenderImpulse
PHHapticRenderImpulse::PHHapticRenderImpulse(){ 
	hapticLoop = &hapticLoopImpulse;
	hapticLoop->renderImp = this;
}

void PHHapticRenderImpulse::Step(){
	//// �f�o�b�N�R�[�h
	//PHHapticPointer* hp = GetHapticPointer(0);
	//int pointerID = hp->GetPointerID();
	//int Nsolids = NHapticSolids();
	//DSTR << pointerID << std::endl;
	//DSTR << "solidpairs" << " " << Nsolids << std::endl;
	//for(int i = 0; i < Nsolids; i++){
	//	PHSolidPairForHaptic* sh = GetSolidPairForHaptic(pointerID, i);
	//	DSTR << sh->solid[0] << " " << sh->solid[1] << std::endl;
	//	DSTR << "shapeparis" << std::endl;
	//	for(int j = 0; j < sh->solid[0]->NShape(); j++)for(int k = 0; k < sh->solid[1]->NShape(); k++){
	//		DSTR << j << " " << k << std::endl;
	//		PHShapePairForHaptic* sp = 	sh->shapePairs.item(j, k);
	//		DSTR << sp << std::endl;
	//	}
	//	//DSTR << hp->neighborSolidIDs[i] <<std::endl;
	//	//DSTR << "------------" << std::endl;
	//}
	//DSTR << "solids" << " " << NHapticSolids() << std::endl;
	//for(int i = 0; i < NHapticSolids(); i++){
	//	PHSolidForHaptic* sh = GetHapticSolid(i);
	//	if(sh->doSim == 1 || sh->doSim == 2) 
	//		DSTR << i << std::endl;
	//}

	engine->StartDetection();
}

void PHHapticRenderImpulse::SyncHaptic2Physic(){
	// physics <-- haptic
	// PHSolidPairForHaptic(�͊o�|�C���^�ƋߖT�̕���)�̊e����̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetPointerID();
		int nNeighbors = hpointer->neighborSolidIDs.size();
		// �ߖT���̂̐�
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(hpointerID, solidID);

			// �͂̓����i��ToPhysic)
			// ��������ׂ������\���̂ɂ��Ă܂Ƃ߂�ׂ�
			ppair->test_force = hpair->test_force;
			ppair->test_torque = hpair->test_torque;
			ppair->impulse = hpair->impulse;
			ppair->impulsePoints = hpair->impulsePoints;

			// �ڐG�_�̐������͂����̂ɉ�����
			for(int k = 0; k < (int)hpair->impulsePoints.size(); k++){
				ImpulsePoint ip = hpair->impulsePoints[k];
					ppair->solid[1]->AddForce(ip.impulse, ip.contactPointW);				
			}
			hpair->impulsePoints.clear();
		}
	}
}

void PHHapticRenderImpulse::SyncPhysic2Haptic(){
	// haptic <------ physics
	// PHSolidForHaptic�̓���
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		*hsolid = *psolid;	// LocalDynamics�̏ꍇ��dosim�ɂ���ē�������������K�v������
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
			*hpair = *ppair;
		}
	}
}

}