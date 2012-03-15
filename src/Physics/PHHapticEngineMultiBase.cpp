#include <Physics/PHHapticEngineMultiBase.h>

namespace Spr{;
//----------------------------------------------------------------------------
// PHHapticLoopImp

void PHHapticLoopImp::UpdateInterface(){
	for(int i = 0; i < NHapticPointers(); i++){
		GetHapticPointer(i)->UpdateInterface((float)GetHapticTimeStep());
		GetHapticPointer(i)->UpdateDirect();
	}
}
double PHHapticLoopImp::GetPhysicsTimeStep(){
	return engineImp->GetPhysicsTimeStep();
}
double PHHapticLoopImp::GetHapticTimeStep(){
	return engineImp->GetHapticTimeStep();
}
int PHHapticLoopImp::NHapticPointers(){
	return (int)hapticPointers.size();
}
int PHHapticLoopImp::NHapticSolids(){
	return (int)hapticSolids.size();
}
PHHapticPointer* PHHapticLoopImp::GetHapticPointer(int i){
	return hapticPointers[i];
}
PHSolidForHaptic* PHHapticLoopImp::GetHapticSolid(int i){
	return hapticSolids[i];
}
PHSolidPairForHaptic* PHHapticLoopImp::GetSolidPairForHaptic(int i, int j){
	return solidPairs.item(i, j);
}
PHHapticPointers* PHHapticLoopImp::GetHapticPointers(){
	return &hapticPointers;
}
PHSolidsForHaptic* PHHapticLoopImp::GetHapticSolids(){
	return &hapticSolids;
}
PHSolidPairsForHaptic* PHHapticLoopImp::GetSolidPairsForHaptic(){
	return &solidPairs;
}
PHHapticRender* PHHapticLoopImp::GetHapticRender(){
	return engineImp->GetHapticRender();
}

//----------------------------------------------------------------------------
// PHHapticEngineMultiBase
PHHapticEngineMultiBase::PHHapticEngineMultiBase(){
	bSync = false;
	bCalcPhys =  true;
	hapticCount = 1;
}

void PHHapticEngineMultiBase::StepHapticLoop(){
	hapticLoop->Step();
	SyncThreads();
}

void PHHapticEngineMultiBase::SyncThreads(){
	if(bSync){ 
		/// �����̎��s
		SyncHapticPointers();			
		SyncHaptic2Physic();
		SyncArrays();
		SyncPhysic2Haptic();
		/// �����I������
		hapticLoop->loopCount = 0;		// HapticLoop�̃J�E���g������
		bSync = false;					// �����I���t���O
	}
	/// �������s���̏���
	#define COUNT_MAX 100
	if(hapticLoop->loopCount > COUNT_MAX) {
		DSTR << "Too Many Loops in HapticLoop!" << std::endl;
		DSTR << "Reset HapticLoop Count" << std::endl;
		hapticLoop->loopCount = 0;
		bSync = false;
	}
	hapticLoop->loopCount++;
	hapticCount++;
}

void PHHapticEngineMultiBase::SyncHapticPointers(){
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		/* haptic���̃|�C���^�̏�Ԃ�physics���̃|�C���^�֔��f
		// physics <-------- haptic
		*/
		// 1.�|�C���^�̈ʒu�p���̓���
		if(ppointer->bDebugControl){
			// physics����̓��͂�haptic��
			ppointer->hiSolid = *DCAST(PHSolid, ppointer);
			hpointer->hiSolid = ppointer->hiSolid;
		}else{
			// haptic����̓��͂�physics��
			ppointer->hiSolid = hpointer->hiSolid;
		}
		ppointer->UpdateDirect();
		
		// 2.���̓���
		PHHapticPointerSt* pst = (PHHapticPointerSt*)ppointer;
		PHHapticPointerSt* hst = (PHHapticPointerSt*)hpointer;
		*pst = *hst;

		/* physics���̕ύX��haptic���֔��f
		// haptic <--------- physics
		*/
		*hpointer = *ppointer;
	}
}

void PHHapticEngineMultiBase::SyncArrays(){
	// haptic <------------- physics
	// Physics�ŐV�����ǉ����ꂽ�I�u�W�F�N�g��Haptic���ɃR�s�[
	// 1.�͊o�|�C���^�̑�����
	int hNpointers = hapticLoop->NHapticPointers();	// haptic���̃|�C���^��
	int pNpointers = NHapticPointers();				// physics���̃|�C���^��
	PHHapticPointers* hpointers = hapticLoop->GetHapticPointers();
	for(int i = hNpointers; i < pNpointers; i++){
		hpointers->push_back(DBG_NEW PHHapticPointer(*GetHapticPointer(i)));
	}
	//DSTR << "------------" << std::endl;
	//DSTR << pNpointers << hNpointers << std::endl;
	//DSTR << hNpointers << std::endl;

	// 2. Solid�̑�����
	const int hNsolids = hapticLoop->NHapticSolids();
	const int pNsolids = NHapticSolids();
	if(hNsolids == pNsolids) return;
	PHSolidsForHaptic* hsolids = hapticLoop->GetHapticSolids();
	for(int i = hNsolids; i < (int)pNsolids; i++){
		hsolids->push_back(DBG_NEW PHSolidForHaptic(*GetHapticSolid(i)));
	}

	// 3. solidPair, shapePair�̑�����
	// 3.1 �͊o�|�C���^�̑�����
	PHSolidPairsForHaptic* hsolidPairs = hapticLoop->GetSolidPairsForHaptic();
	hsolidPairs->resize(pNsolids, pNpointers);
	for(int i = 0; i < pNsolids; i++){
		for(int j = hNpointers; j < pNpointers; j++){
			PHSolidPairForHaptic* psolidPair = GetSolidPairForHaptic(i, j);
			hsolidPairs->item(i, j) = DBG_NEW PHSolidPairForHaptic(*psolidPair);
		}
	}
	// 3.2 solid�̑�����
	for(int i = hNsolids; i < pNsolids; i++){
		for(int j = 0; j < pNpointers; j++){
			PHSolidPairForHaptic* psolidPair = GetSolidPairForHaptic(i, j);
			hsolidPairs->item(i, j) = DBG_NEW PHSolidPairForHaptic(*psolidPair);
		}
	}
	//DSTR << "--------------" << std::endl;
	//DSTR << hapticLoop->NHapticPointers() << std::endl;
	//DSTR << hapticLoop->NHapticSolids() << std::endl;
	//DSTR << hapticLoop->GetHapticPointer(0)->neighborSolidIDs.size() << std::endl;
}


// �f�o�b�N�p�R�[�h
// ���ŁAPHScene::Step()����������Ă���̂�
void PHHapticEngineMultiBase::StepPhysicsSimulation(){
	if (bSync) return;
	if (bCalcPhys){
		/// �V�~�����[�V�����̎��s
		engine->GetScene()->Step();
		bCalcPhys = false;
	}
	double pdt = GetPhysicsTimeStep();
	double hdt = GetHapticTimeStep();
	if (hapticCount < pdt / hdt) return;
	hapticCount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;	
}


}