#include <Physics/PHHapticRenderMultiBase.h>

namespace Spr{;

double PHHapticLoopImp::GetPhysicsTimeStep(){
	return renderImp->GetPhysicsTimeStep();
}

double PHHapticLoopImp::GetHapticTimeStep(){
	return renderImp->GetHapticTimeStep();
}

PHHapticRenderMultiBase::PHHapticRenderMultiBase(){
	bSync = false;
	bCalcPhys =  true;
	hapticCount = 1;
}


void PHHapticRenderMultiBase::Step(){
}


void PHHapticRenderMultiBase::StepHapticLoop(){
	hapticLoop->Step();
	Sync();
}

void PHHapticRenderMultiBase::Sync(){
	if(bSync){ 
		/// �����̎��s
		RunSync();
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