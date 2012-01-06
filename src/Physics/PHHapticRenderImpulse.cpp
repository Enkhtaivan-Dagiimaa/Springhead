#include <Physics/PHHapticRenderImpulse.h>

namespace Spr{;

//----------------------------------------------------------------------------
// PHHapticLoopImpulse

// 1/7�͂�������
void PHHapticLoopImpulse::Step(){}



//----------------------------------------------------------------------------
// PHHapticRenderImpulse
void PHHapticRenderImpulse::Step(){
	engine->StartDetection();

	// �f�o�b�N�R�[�h
	//PHHapticPointers* hp = GetHapticPointers();
	//int id = hp[0]->GetID();
	//int N = hp[0]->neighborSolidIDs.size();
	//DSTR << "shapepair" << std::endl;
	//for(int i = 0; i < N; i++){
	//	int a = id;
	//	int b = hp[0]->neighborSolidIDs[i];
	//	if(a > b) std::swap(a, b);
	//	PHSolidPairForHaptic* sh = GetSolidPairForHaptic(a, b);
	//	PHShapePairForHaptic* sp = sh->shapePairs.item(0, 0);
	//	//DSTR << sp->normal << std::endl;
	//	DSTR << hp[0]->neighborSolidIDs[i] <<std::endl;
	//	//DSTR << "------------" << std::endl;
	//}
	//DSTR << "solids" << std::endl;
	//PHSolidForHaptic** sh = GetHapticSolids();
	//for(int i = 0; i < NHapticSolids(); i++){
	//	if(sh[i]->doSim == 1 || sh[i]->doSim == 2) 
	//		DSTR << i << std::endl;
	//}
}

void PHHapticRenderImpulse::SyncHaptic2Physic(){
	// physics <-- haptic
	// �͊o�|�C���^�ƋߖT�̕��̂ɂ��Ċe����̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetID();
		int nNeighbors = hpointer->neighborSolidIDs.size();
		// �ߖT���̂̐�
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			int a = hpointerID;
			int b = solidID;
			int bSwapped = false;
			if(a > b){
				std::swap(a, b);
				bSwapped = true;
			}
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(a, b);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(a, b);

			// �͂̓����i��ToPhysic)
			// ��������ׂ������\���̂ɂ��Ă܂Ƃ߂�ׂ�
			ppair->test_force = hpair->test_force;
			ppair->test_torque = hpair->test_torque;
			ppair->impulse = hpair->impulse;
			ppair->impulsePoints = hpair->impulsePoints;

			// �ڐG�_�̐������͂����̂ɉ�����
			for(int k = 0; k < hpair->impulsePoints.size(); k++){
				ImpulsePoint ip = hpair->impulsePoints[k];
				if(bSwapped){
					ppair->solid[0]->AddForce(ip.impulse, ip.contactPointW);
				}else{
					ppair->solid[1]->AddForce(ip.impulse, ip.contactPointW);				
				}
			}
			hpair->impulsePoints.clear();
		}
	}
}

void PHHapticRenderImpulse::SyncPhysic2Haptic(){
	// haptic <------ physics
	// solid�̓���
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		*hsolid = *psolid; 
	}
	// hapticpointer�̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		const int ppointerID = ppointer->GetID();
		const int nNeighbors = ppointer->neighborSolidIDs.size();
		// solidpair, shapepair�̓���
		for(int j = 0; j < nNeighbors; j++){
			const int solidID = hpointer->neighborSolidIDs[j];
			int a = ppointerID;
			int b = solidID;
			int bSwapped = false;
			if(a > b){
				std::swap(a, b);
				bSwapped = true;
			}
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(a, b);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(a, b);
			*hpair = *ppair;
		}
	}
}

}