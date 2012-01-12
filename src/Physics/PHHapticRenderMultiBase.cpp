#include <Physics/PHHapticRenderMultiBase.h>

namespace Spr{;
//----------------------------------------------------------------------------
// PHHapticLoopImp

void PHHapticLoopImp::UpdateInterface(){
	for(int i = 0; i < NHapticPointers(); i++){
		GetHapticPointer(i)->UpdateInterface((float)GetHapticTimeStep());
		GetHapticPointer(i)->UpdateHapticPointer();
	}
}
double PHHapticLoopImp::GetPhysicsTimeStep(){
	return renderImp->GetPhysicsTimeStep();
}
double PHHapticLoopImp::GetHapticTimeStep(){
	return renderImp->GetHapticTimeStep();
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

//PHHapticPointers* PHHapticLoopImp::GetHapticPointers(){
//	return hapticPointers.empty() ? NULL : (PHHapticPointers*)&*hapticPointers.begin();
//}
//
//PHSolidForHaptic** PHHapticLoopImp::GetHapticSolids(){
//	return hapticSolids.empty() ? NULL : (PHSolidForHaptic**)&*hapticSolids.begin();
//}

//----------------------------------------------------------------------------
// PHHapticRenderMultiBase
PHHapticRenderMultiBase::PHHapticRenderMultiBase(){
	bSync = false;
	bCalcPhys =  true;
	hapticCount = 1;
}

void PHHapticRenderMultiBase::StepHapticLoop(){
	hapticLoop->Step();
	Sync();
}

void PHHapticRenderMultiBase::Sync(){
	if(bSync){ 
		/// �����̎��s
		UpdateHapticPointer();			
		SyncHaptic2Physic();
		UpdateArrays();
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

void PHHapticRenderMultiBase::UpdateHapticPointer(){
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		// physics���̕ύX��haptic���֔��f
		// haptic <--------- physics
		// �ϐ��������Ă�����܂Ƃ߂�
		hpointer->bDebugControl = ppointer->bDebugControl;
		hpointer->bForce = ppointer->bForce;
		hpointer->bVibration = ppointer->bVibration;
		// haptic���̃|�C���^�̏�Ԃ�physics���̃|�C���^�֔��f
		// physics <-------- haptic
		if(ppointer->bDebugControl){
			// physics����̓��͂𔽉f
			*ppointer->hiSolid = *DCAST(PHSolid, ppointer);
			*hpointer->hiSolid = *ppointer->hiSolid;
		}else{
			// haptic����̓��͂�physics��
			*ppointer->hiSolid = *hpointer->hiSolid;
		}
		ppointer->UpdateHapticPointer();
	}
}

void PHHapticRenderMultiBase::UpdateArrays(){
	// haptic <------------- physics
	// Physics�ŐV�����ǉ����ꂽ�I�u�W�F�N�g��Haptic���ɃR�s�[
	// 1.�͊o�|�C���^�̑�����
	int hNpointers = hapticLoop->NHapticPointers();	// haptic���̃|�C���^��
	int pNpointers = NHapticPointers();				// physics���̃|�C���^��
	PHHapticPointers* hpointers = hapticLoop->GetHapticPointers();
	for(int i = hNpointers; i < pNpointers; i++){
		hpointers->push_back(DBG_NEW PHHapticPointer());
		*hpointers->back() = *GetHapticPointer(i);
	}

	// 2. Solid�̑�����
	const int hNsolids = hapticLoop->NHapticSolids();
	const int pNsolids = NHapticSolids();
	if(hNsolids == pNsolids) return;
	PHSolidsForHaptic* hsolids = hapticLoop->GetHapticSolids();
	for(int i = hNsolids; i < (int)pNsolids; i++){
		hsolids->push_back(DBG_NEW PHSolidForHaptic());
		*hsolids->back() = *GetHapticSolid(i);
	}

	// 3. solidPair�̑�����
	UTCombination< UTRef<PHSolidPairForHaptic> >* hsolidPairs = &hapticLoop->solidPairs;
	hsolidPairs->resize(pNpointers, pNsolids);
	// �͊o�|�C���^������
	for(int i = hNpointers; i < pNpointers; i++){
		for(int j = 0; j < pNsolids; j++){
			hsolidPairs->item(i, j) = DBG_NEW PHSolidPairForHaptic();
			*hsolidPairs->item(i, j) = *GetSolidPairForHaptic(i, j);	
		}
	}
	// solid�̑�����
	for(int i = 0; i < pNpointers; i++){
		for(int j = hNsolids; j < pNsolids; j++){
			hsolidPairs->item(i, j) = DBG_NEW PHSolidPairForHaptic();
			*hsolidPairs->item(i, j) = *GetSolidPairForHaptic(i, j);	
		}
	}
	//DSTR << "--------------" << std::endl;
	//DSTR << hapticLoop->NHapticPointers() << std::endl;
	//DSTR << hapticLoop->NHapticSolids() << std::endl;
	//DSTR << hapticLoop->GetHapticPointer(0)->neighborSolidIDs.size() << std::endl;
}

// �f�o�b�N�p�R�[�h
// ���ŁAPHScene::Step()����������Ă���̂�
// �����ŌĂԂ킯�ɂ͂����Ȃ�
void PHHapticRenderMultiBase::StepSimulation(){
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