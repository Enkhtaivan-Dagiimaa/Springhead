#include <Framework/FWMultiRateHaptic.h>
#include <Framework/SprFWApp.h>
#include <Physics/PHConstraintEngine.h>
#include <sstream>

namespace Spr{;

FWMultiRateHaptic::FWMultiRateHaptic(){
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
}
void FWMultiRateHaptic::Sync(){
	FWHapticLoopBase* hLoop = GetHapticLoop();
	if(bSync){ 
		/// �͊o�|�C���^�̓���
		UpdatePointer();			
		/// �����̂��߂̏���
		FWInteractSolids* piSolids = GetInteractSolids();
		FWInteractSolids* hiSolids = GetHapticLoop()->GetInteractSolids();
		FWInteractPointers* piPointers = GetInteractScene()->GetInteractPointers();
		std::vector<FWInteractPointer>* hiPointers = GetHapticLoop()->GetInteractPointers();
		FWInteractSolid *piSolid, *hiSolid;
		FWInteractPointer* piPointer,*hiPointer;
		FWInteractInfo* piInfo, *hiInfo;

		/// HapticLoop--->PhysicsLoop ///
		for(unsigned i = 0; i < (int)hiSolids->size(); i++){
			piSolid = GetInteractSolid(i);
			hiSolid = GetHapticLoop()->GetInteractSolid(i);
			/// bSim = ture ���� bfirstSim = false�Ȃ猋�ʂ𔽉f������
			if(!hiSolid->bSim || hiSolid->bfirstSim) continue;
			double pdt = GetInteractScene()->GetScene()->GetPHScene()->GetTimeStep();			// physics�̍���
		
			/// �e�|�C���^���������擾
			for(int j = 0; j < (int)piPointers->size(); j++){
				piPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
				hiPointer = GetHapticLoop()->GetInteractPointer(j)->Cast();
				piInfo = &piPointer->interactInfo[i];
				hiInfo = &hiPointer->interactInfo[i];
				piInfo->neighborInfo.test_force_norm = hiInfo->neighborInfo.test_force_norm;
			}
			/// ���ʂ̔��f
			SpatialVector b;
			b += (piSolid->b + 
					(piSolid->curb - piSolid->lastb)) * pdt;	// ���r���e�B�萔��
			Vec3d v = hiSolid->copiedSolid.GetVelocity() + b.v();					// ���f���x
			Vec3d w = hiSolid->copiedSolid.GetAngularVelocity() + b.w();			// ���f�p���x
			hiSolid->sceneSolid->SetVelocity(v);
			hiSolid->sceneSolid->SetAngularVelocity(w);
			hiSolid->sceneSolid->SetCenterPosition(hiSolid->copiedSolid.GetCenterPosition());
			hiSolid->sceneSolid->SetOrientation(hiSolid->copiedSolid.GetOrientation());
		}
	

		/// PhysicsLoop--->HapticLoop ///
		/// �V�[���ŐV�����������ꂽ�����g��
		for(int i = (int)hiPointers->size(); i < (int)piPointers->size(); i++){
			hiPointers->resize(hiPointers->size() + 1);
			FWInteractPointer* ip = GetInteractScene()->GetInteractPointer(i)->Cast(); 
			hiPointers->back() = *ip;
		}
		for(int i = (int)hiSolids->size(); i < (int)piSolids->size(); i++){
			hiSolids->resize(i + 1);
			hiSolids->back() = *GetInteractSolid(i);
			/// �|�C���^�������ɂ��Ă��g��
			for(int j = 0; j < (int)piPointers->size(); j++){
				hiPointer = GetHapticLoop()->GetInteractPointer(j);
				hiPointer->interactInfo.resize(i + 1);
				piPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
				hiPointer->interactInfo.back() = piPointer->interactInfo[i];
			}
		}
		/// ���̓���
		for(unsigned i = 0; i < hiSolids->size(); i++){
			piSolid = GetInteractSolid(i);
			hiSolid = GetHapticLoop()->GetInteractSolid(i);
			hiSolid->bSim = piSolid->bSim;
			hiSolid->bfirstSim = piSolid->bfirstSim;
			/// ���߂ăV�~�����[�V�����ΏۂɂȂ�����
//			DSTR << piSolid->bfirstSim << std::endl;
			if(piSolid->bfirstSim){								
				hiSolid->copiedSolid = piSolid->copiedSolid;	// �ŐV�̏����R�s�[
				piSolid->bfirstSim = false;						// ���߂ċߖT�ɂȂ����ꍇ�̏���
			}
			/// �|�C���^���ƂɎ����̓���
			for(int j = 0; j < (int)piPointers->size(); j++){
				piPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
				hiPointer = GetHapticLoop()->GetInteractPointer(j)->Cast();
				hiPointer->interactInfo[i] = piPointer->interactInfo[i];
			}
		}
		/* �����I������ */
		hLoop->loopCount = 0;		// HapticLoop�̃J�E���g������
		bSync = false;				// �����I���t���O
	}
	#define COUNT_MAX 100
	if(hLoop->loopCount > COUNT_MAX) {
		DSTR << "Too Many Loops in HapticLoop!" << std::endl;
		DSTR << "Reset HapticLoop Count" << std::endl;
		hLoop->loopCount = 0;
		bSync = false;
	}
	hLoop->loopCount++;
	hapticcount++;
}

void FWMultiRateHaptic::Clear(){
	GetHapticLoop()->Clear();
	GetInteractSolids()->clear();
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
}


}
