/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

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
		SyncHaptic2Phsyic();
		SyncPhsyic2Haptic();

		/// �����I������
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


//void FWMultiRateHaptic::Sync(){
//	if(bSync){ 
//		/// �͊o�|�C���^�̓���
//		UpdatePointer();			
//		/// �����̂��߂̏���
//		FWInteractSolids* hiSolids = GetHapticLoop()->GetIASolids();
//		std::vector<FWInteractPointer>* hiPointers = GetHapticLoop()->GetIAPointers();
//		FWInteractSolid *piSolid, *hiSolid;
//		FWInteractPointer* hiPointer;
//		FWInteractInfo* piInfo, *hiInfo;
//
//		/// HapticLoop--->PhysicsLoop ///
//		for(unsigned i = 0; i < (int)hiSolids->size(); i++){
//			hiSolid = GetHapticLoop()->GetIASolid(i);
//			/// bSim = ture ���� bfirstSim = false�Ȃ猋�ʂ𔽉f������
//			if(!hiSolid->bSim || hiSolid->bfirstSim) continue;
//			piSolid = GetIASolid(i);
//
//			double pdt = GetPHScene()->GetTimeStep();	// physics�̍���
//			SpatialVector b = (piSolid->b + (piSolid->curb - piSolid->lastb)) * pdt;	// ���r���e�B�萔��
//			Vec3d v = hiSolid->copiedSolid.GetVelocity() + b.v();			// ���f���x
//			Vec3d w = hiSolid->copiedSolid.GetAngularVelocity() + b.w();	// ���f�p���x
//			// ��Ԃ̍X�V�iPhysics���j
//			hiSolid->sceneSolid->SetVelocity(v);
//			hiSolid->sceneSolid->SetAngularVelocity(w);
//			hiSolid->sceneSolid->SetCenterPosition(hiSolid->copiedSolid.GetCenterPosition());
//			hiSolid->sceneSolid->SetOrientation(hiSolid->copiedSolid.GetOrientation());
//
//			/// �e�|�C���^�������𓯊�
//			for(int j = 0; j < NIAPointers(); j++){
//				piInfo = &GetIAPointer(j)->interactInfo[i];
//				hiPointer = GetHapticLoop()->GetIAPointer(j)->Cast();
//				hiInfo = &hiPointer->interactInfo[i];
//			}
//		}
//	
//		/// PhysicsLoop--->HapticLoop ///
//		/// �V�[���ŐV�����������ꂽ�����g��
//		for(int i = (int)hiPointers->size(); i < NIAPointers(); i++){
//			hiPointers->resize(i+1);
//			hiPointers->back() = *GetIAPointer(i);
//			hiPointers->back().Sync();
//		}
//		for(int i = (int)hiSolids->size(); i < (int)NIASolids(); i++){
//			hiSolids->resize(i + 1);
//			hiSolids->back() = *GetIASolid(i);
//			/// �|�C���^�������ɂ��Ă��g��
//			for(int j = 0; j < NIAPointers(); j++){
//				hiPointer = GetHapticLoop()->GetIAPointer(j);
//				hiPointer->interactInfo.resize(i + 1);
//				hiPointer->interactInfo.back() = GetIAPointer(j)->interactInfo[i];
//			}
//		}
//		/// ���̓���
//		for(unsigned i = 0; i < hiSolids->size(); i++){
//			piSolid = GetIASolid(i);
//			hiSolid = GetHapticLoop()->GetIASolid(i);
//
//			hiSolid->bSim = piSolid->bSim;
//			hiSolid->bfirstSim = piSolid->bfirstSim;
//			/// ���߂ăV�~�����[�V�����ΏۂɂȂ�����
//			if(piSolid->bfirstSim){								
//				hiSolid->copiedSolid = piSolid->copiedSolid;	// �ŐV�̏����R�s�[
//				piSolid->bfirstSim = false;				// ���߂ċߖT�ɂȂ����ꍇ�̏���
//			}
//			hiSolid->b = piSolid->b;
//			hiSolid->curb = piSolid->curb;
//			hiSolid->lastb = piSolid->lastb;
//
//			/// �|�C���^���ƂɎ����̓���
//			for(int j = 0; j < NIAPointers(); j++){
//				hiPointer = GetHapticLoop()->GetIAPointer(j)->Cast();
//				hiPointer->interactInfo[i] = GetIAPointer(j)->interactInfo[i];
//				hiPointer->bForce = GetIAPointer(j)->bForce;
//				hiPointer->bVibration = GetIAPointer(j)->bVibration;
//			}
//		}
//		/* �����I������ */
//		GetHapticLoop()->loopCount = 0;		// HapticLoop�̃J�E���g������
//		bSync = false;						// �����I���t���O
//	}
//	#define COUNT_MAX 100
//	if(GetHapticLoop()->loopCount > COUNT_MAX) {
//		DSTR << "Too Many Loops in HapticLoop!" << std::endl;
//		DSTR << "Reset HapticLoop Count" << std::endl;
//		GetHapticLoop()->loopCount = 0;
//		bSync = false;
//	}
//	GetHapticLoop()->loopCount++;
//	hapticcount++;
//}

void FWMultiRateHaptic::SyncPointer(){
	std::vector<FWInteractPointer>* hiPointers = GetHapticLoop()->GetIAPointers();
	for(int i = 0; i < NIAPointers(); i++){
		hiPointers->at(i)= *GetIAPointer(i);
	}
}

void FWMultiRateHaptic::Clear(){
	GetHapticLoop()->Clear();
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
}


}
