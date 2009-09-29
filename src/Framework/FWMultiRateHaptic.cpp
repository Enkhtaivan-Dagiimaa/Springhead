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
	if(bSync){ 
		/// �͊o�|�C���^�̓���
		UpdatePointer();			
		/// �����̂��߂̏���
		FWInteractSolids* hiSolids = GetHapticLoop()->GetINSolids();
		std::vector<FWInteractPointer>* hiPointers = GetHapticLoop()->GetINPointers();
		FWInteractSolid *piSolid, *hiSolid;
		FWInteractPointer* hiPointer;
		FWInteractInfo* piInfo, *hiInfo;

		/// HapticLoop--->PhysicsLoop ///
		for(unsigned i = 0; i < (int)hiSolids->size(); i++){
			hiSolid = GetHapticLoop()->GetINSolid(i);
			/// bSim = ture ���� bfirstSim = false�Ȃ猋�ʂ𔽉f������
			if(!hiSolid->bSim || hiSolid->bfirstSim) continue;
			piSolid = GetINSolid(i);
			double pdt = GetPHScene()->GetTimeStep();			// physics�̍���
			SpatialVector b;
			b += (piSolid->b + 
					(piSolid->curb - piSolid->lastb)) * pdt;	// ���r���e�B�萔��
			Vec3d v = hiSolid->copiedSolid.GetVelocity() + b.v();					// ���f���x
			Vec3d w = hiSolid->copiedSolid.GetAngularVelocity() + b.w();			// ���f�p���x
			
			hiSolid->sceneSolid->SetVelocity(v);
			hiSolid->sceneSolid->SetAngularVelocity(w);
			hiSolid->sceneSolid->SetCenterPosition(hiSolid->copiedSolid.GetCenterPosition());
			hiSolid->sceneSolid->SetOrientation(hiSolid->copiedSolid.GetOrientation());
			/// �e�|�C���^�������𓯊�
			for(int j = 0; j < NINPointers(); j++){
				piInfo = &GetINPointer(j)->interactInfo[i];
				hiPointer = GetHapticLoop()->GetINPointer(j)->Cast();
				hiInfo = &hiPointer->interactInfo[i];
				piInfo->neighborInfo.test_force_norm = hiInfo->neighborInfo.test_force_norm;
			}
		}
	
		/// PhysicsLoop--->HapticLoop ///
		/// �V�[���ŐV�����������ꂽ�����g��
		for(int i = (int)hiPointers->size(); i < NINPointers(); i++){
			hiPointers->resize(i + 1);
			hiPointers->back() = *GetINPointer(i);
		}
		for(int i = (int)hiSolids->size(); i < (int)NINSolids(); i++){
			hiSolids->resize(i + 1);
			hiSolids->back() = *GetINSolid(i);
			/// �|�C���^�������ɂ��Ă��g��
			for(int j = 0; j < NINPointers(); j++){
				hiPointer = GetHapticLoop()->GetINPointer(j);
				hiPointer->interactInfo.resize(i + 1);
				hiPointer->interactInfo.back() = GetINPointer(j)->interactInfo[i];
			}
		}
		/// ���̓���
		for(unsigned i = 0; i < hiSolids->size(); i++){
			piSolid = GetINSolid(i);
			hiSolid = GetHapticLoop()->GetINSolid(i);
			hiSolid->bSim = piSolid->bSim;
			hiSolid->bfirstSim = piSolid->bfirstSim;
			/// ���߂ăV�~�����[�V�����ΏۂɂȂ�����
			if(piSolid->bfirstSim){								
				hiSolid->copiedSolid = piSolid->copiedSolid;	// �ŐV�̏����R�s�[
				piSolid->bfirstSim = false;						// ���߂ċߖT�ɂȂ����ꍇ�̏���
			}
			//DSTR<<"sync:"<<piSolid->b<<std::endl; //naga
			//hiSolid->b = piSolid->b;
			hiSolid->curb = piSolid->curb;
			hiSolid->lastb = piSolid->lastb;
			/// �|�C���^���ƂɎ����̓���
			for(int j = 0; j < NINPointers(); j++){
				hiPointer = GetHapticLoop()->GetINPointer(j)->Cast();
				hiPointer->interactInfo[i] = GetINPointer(j)->interactInfo[i];
				hiPointer->bForce = GetINPointer(j)->bForce;
				hiPointer->bVibration = GetINPointer(j)->bVibration;
			}
		}
		/* �����I������ */
		GetHapticLoop()->loopCount = 0;		// HapticLoop�̃J�E���g������
		bSync = false;						// �����I���t���O
	}
	#define COUNT_MAX 100
	if(GetHapticLoop()->loopCount > COUNT_MAX) {
		DSTR << "Too Many Loops in HapticLoop!" << std::endl;
		DSTR << "Reset HapticLoop Count" << std::endl;
		GetHapticLoop()->loopCount = 0;
		bSync = false;
	}
	GetHapticLoop()->loopCount++;
	hapticcount++;
}

void FWMultiRateHaptic::SyncPointer(){
	std::vector<FWInteractPointer>* hiPointers = GetHapticLoop()->GetINPointers();
	for(int i = 0; i < NINPointers(); i++){
		hiPointers->at(i)= *GetINPointer(i);
	}
}

void FWMultiRateHaptic::Clear(){
	GetHapticLoop()->Clear();
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
}


}
