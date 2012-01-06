#include <Physics/PHHapticRenderMultiBase.h>

namespace Spr{;
//----------------------------------------------------------------------------
// PHHapticLoopImp
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
		// haptic���̃|�C���^�̏�Ԃ�physics���̃|�C���^�֔��f
		// physics <-------- haptic
		*ppointer->hiSolid = *hpointer->hiSolid;
		ppointer->UpdateHapticPointer();
		// physics���̕ύX��haptic���֔��f
		// haptic <--------- physics
		// �ϐ��������Ă�����܂Ƃ߂�
		hpointer->bForce = ppointer->bForce;
		hpointer->bVibration = ppointer->bVibration;
	}
}

void PHHapticRenderMultiBase::UpdateArrays(){
	// haptic <------------- physics
	// Physics�ŐV�����ǉ����ꂽ�I�u�W�F�N�g��Haptic���ɃR�s�[
	// 1.�͊o�|�C���^�̑�����
	int hNhpointers = hapticLoop->NHapticPointers();	// haptic���̃|�C���^��
	int pNhpointers = NHapticPointers();				// physics���̃|�C���^��
	PHHapticPointers* hpointers = hapticLoop->GetHapticPointers();
	for(int i = hNhpointers; i < pNhpointers; i++){
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
	//DSTR << "solids size" << hNsolids << " " << pNsolids <<  std::endl;
	UTCombination< UTRef<PHSolidPairForHaptic> >* hsolidPairs = &hapticLoop->solidPairs;
	hsolidPairs->resize(pNsolids, pNsolids);
	for(int i = hNsolids - 1; i < pNsolids; i++){
		for(int j = 0; j < i; j++){
			hsolidPairs->item(j, i) = DBG_NEW PHSolidPairForHaptic();
			*hsolidPairs->item(j, i) = *GetSolidPairForHaptic(j, i);	
			//DSTR << j << " " << i << std::endl;
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