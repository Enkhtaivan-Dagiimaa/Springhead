/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWInteractAdaptee.h>
#include <Framework/FWInteractScene.h>
#include <Physics/PHConstraintEngine.h>
#include <iostream>

namespace Spr{;

/** FWHapticLoopBase�̎��� */
///////////////////////////////////////////////////////////////////////////////////

void FWHapticLoopBase::Clear(){
	GetIASolids()->clear();
	loopCount = 1;
}

/** FWInteractAdaptee�̎��� */
///////////////////////////////////////////////////////////////////////////////////
FWInteractAdaptee::FWInteractAdaptee(){}
/// IAScene�֌W
void FWInteractAdaptee::SetIAScene(FWInteractScene* iScene){ interactScene = iScene; }
FWInteractScene* FWInteractAdaptee::GetIAScene(){ return interactScene; }			
PHSceneIf* FWInteractAdaptee::GetPHScene(){ return GetIAScene()->GetScene()->GetPHScene(); }

/// HapticLoop�֌W
FWHapticLoopBase* FWInteractAdaptee::GetHapticLoop(){ return NULL; }
void FWInteractAdaptee::SetHMode(FWHapticMode hMode) {GetHapticLoop()->hmode = hMode;}

/// IAPointer�֌W(IAScene����Ď擾)
FWInteractPointer* FWInteractAdaptee::GetIAPointer(int i){ return GetIAScene()->GetIAPointer(i)->Cast(); }
FWInteractPointers* FWInteractAdaptee::GetIAPointers(){ return GetIAScene()->GetIAPointers(); }
int FWInteractAdaptee::NIAPointers(){ return GetIAScene()->NIAPointers(); }

/// IASolid�֌W(IAScene����Ď擾)
FWInteractSolid* FWInteractAdaptee::GetIASolid(int i){ return GetIAScene()->GetIASolid(i); }
FWInteractSolids* FWInteractAdaptee::GetIASolids(){ return GetIAScene()->GetIASolids(); }
int FWInteractAdaptee::NIASolids(){ return GetIAScene()->NIASolids(); }

void FWInteractAdaptee::UpdateSolidList(){
	PHSceneIf* phScene = GetPHScene(); 
	PHSolidIf** phSolids = phScene->GetSolids();
	for(int i = NIASolids(); i < phScene->NSolids(); i++){
		GetIASolids()->resize(i + 1);
		GetIASolids()->back().sceneSolid = phSolids[i]->Cast();
		for(int j = 0; j < NIAPointers(); j++){
			GetIAPointer(j)->interactInfo.resize(i + 1);
		}
	}
}

void FWInteractAdaptee::NeighborObjectFromPointer(){
	// GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	// ��������ׂĂ�shape�������̂ɂ��Ă��
	for(int i = 0; i < NIASolids(); i++){
		int lCount = 0;					///< flag.blocal�̐�
		int fCount = 0;					///< flag.bfirstlocal�̐�
		PHSolid* phSolid = GetIASolid(i)->sceneSolid;
		/// IAPointer�̐��������
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* iPointer = GetIAPointer(j);
			FWInteractInfo* iaInfo = &GetIAPointer(j)->interactInfo[i];
			PHSolid* soPointer = GetIAPointer(j)->pointerSolid->Cast();

			iaInfo->flag.bneighbor = false;				// ���[�J���\���̏�����
			/// Solid�����̃|�C���^�ł������ꍇ
			for(int k = 0; k < NIAPointers(); k++){
				if(phSolid == GetIAPointer(k)->pointerSolid->Cast()) phSolid = NULL;
			}
			if (soPointer != phSolid && phSolid){
				/* AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
				   �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
				*/
				/// 1. BBox���x���̏Փ˔���(Sweep & Prune)
				Vec3d range = Vec3d(1, 1, 1) * iPointer->GetLocalRange();
				Vec3d pMin = soPointer->GetPose() * soPointer->bbox.GetBBoxMin() - range;		// Pointer��BBox�̍ŏ��l(3��)
				Vec3d pMax = soPointer->GetPose() * soPointer->bbox.GetBBoxMax() + range;		// Pointer��BBox�̍ő�l(3��)
				Vec3d soMin = phSolid->GetPose().Pos() + phSolid->bbox.GetBBoxMin();		// Solid��BBox�̍ŏ��l(3��)
				Vec3d soMax = phSolid->GetPose().Pos() + phSolid->bbox.GetBBoxMax();		// Solid��BBox�̍ő�l(3��)
				/// 3���Ŕ���
				int isLocal = 3;		//< �����̎��Ō������Ă��邩�ǂ���
				for(int i = 0; i < 3; ++i){
					int in = 0;
					/// �|�C���^�̃G�b�W�ԂɃ\���b�h�̃G�b�W�������������
					if(pMin[i] <= soMin[i] && soMin[i] <= pMax[i]) in++; 
					if(pMin[i] <= soMax[i] && soMax[i] <= pMax[i]) in++; 
					/// �\���b�h�̃G�b�W�ԂɃ|�C���^�̍ő�l�������������
					if(soMin[i] <= pMin[i] && pMin[i] <= soMax[i]) in++;
					if(soMin[i] <= pMax[i] && pMax[i] <= soMax[i]) in++;
					/// in��1�ȏ�Ȃ炻�̎��Ō���
					if(in > 0) isLocal++;
#if 0
					DSTR << i << " pMin[i] = " << pMin[i] << "  soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << std::endl;
					DSTR << i << " pMin[i] = "  << pMin[i] << "  soMax[i] = " << soMax[i] << "  pMax[i] = " << pMax[i] << std::endl;
					DSTR << i << " soMin[i] = " << soMin[i] << "  pMin[i] = " << pMin[i] << "  soMax[i] = " << soMax[i] << std::endl;
					DSTR << i << " soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << "  soMax[i] = " << soMax[i] << std::endl;
#endif
				}
//				DSTR << "isLocal" << isLocal <<  std::endl;
				/// 2.�ߖT�̉\�������镨�̂͏ڍה���(GJK��)
				if(isLocal > 2){
					iaInfo->flag.bneighbor = true;
					UpdateInteractSolid(i, GetIAPointer(j));
				}
			}
			/// bneighbor����blocal�ł����lCount++
			if(iaInfo->flag.bneighbor){
				if(iaInfo->flag.blocal){
					lCount++;
					/// �����bfirstlocal�ł����fCount++
					if(iaInfo->flag.bfirstlocal){
						fCount++;
						iaInfo->flag.bfirstlocal = false;
					}
				}else{
					iaInfo->flag.bfirstlocal = false;							
					iaInfo->flag.blocal = false;													
				}
			}else{
				/// �ߖT���̂łȂ��̂�false�ɂ���
				iaInfo->flag.bfirstlocal = false;							
				iaInfo->flag.blocal = false;									
			}
		}	// end IAPointer�̐��������
		/// ���V�~�����[�V�����̏����t���O��true�ɂ���
		FWInteractSolid* inSolid = GetIASolid(i);
		if(fCount > 0){
			inSolid->bfirstSim = true;	
			inSolid->copiedSolid = *inSolid->sceneSolid;	// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
		}
		/// �V�~�����[�V�����̏����t���O��true�ɂ���
		if(lCount > 0){
			inSolid->bSim = true;					
		}else{
			/// ����ȊO�͑S��false�ɂ���
			inSolid->bSim = false;					
			inSolid->bfirstSim = false;
		}
	}
//	DSTR << "-------------------------------------------------" << std::endl;
}

void FWInteractAdaptee::UpdateInteractSolid(int index, FWInteractPointer* iPointer){
	PHSolid* phSolid = GetIASolid(index)->sceneSolid;
	PHSolid* soPointer = iPointer->GetPointerSolid()->Cast();

	FWInteractInfo* iaInfo = &iPointer->interactInfo[index]; 
	if (!phSolid->NShape()==0){													///< �`��������Ȃ����̂̏ꍇ�͍s��Ȃ�
		CDConvexIf* a = DCAST(CDConvexIf, phSolid->GetShape(0));				///< ���̂����ʌ`��
		CDConvexIf* b = DCAST(CDConvexIf, soPointer->GetShape(0));				///< �͊o�|�C���^�̓ʌ`��
		Posed a2w = phSolid->GetPose();											///< ���̂̎p��
		Posed b2w = soPointer->GetPose();										///< �͊o�|�C���^�̎p��
		Vec3d dir = -1.0 * iaInfo->neighborInfo.face_normal;
		Vec3d cp = phSolid->GetCenterPosition();								///< ���̂̏d�S
		Vec3d normal;															///< ���̂���͊o�|�C���^�ւ̖@��(���[���h���W)
		Vec3d pa, pb;															///< pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j

		/// GJK���g�����ߖT�_�T��
		double r = FindNearestPoint(a, b, a2w, b2w, cp, dir, normal, pa, pb);	
		/// �ߖT�_�܂ł̒�������ߖT���̂��i��
		if(r < iPointer->GetLocalRange()){
			/// ���߂čŋߖT���̂ɂȂ����ꍇ
			if(iaInfo->flag.blocal == false){																
				iaInfo->flag.bfirstlocal = true;	
				iaInfo->neighborInfo.face_normal = normal;	// ���߂ċߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D										
				#ifdef _DEBUG
					if (iaInfo->neighborInfo.face_normal * normal < 0.8){
						DSTR << "Too big change on normal = " << normal << std::endl;
					}
				#endif
			}
			/// ���߂ċߖT�܂��͌p�����ċߖT�������ꍇ
			iaInfo->flag.blocal = true;								// �ߖT���̂Ȃ̂�blocal��true�ɂ���
			iaInfo->neighborInfo.closest_point = pa;				// ���̋ߖT�_�̃��[�J�����W
			iaInfo->neighborInfo.pointer_point = pb;				// �͊o�|�C���^�ߖT�_�̃��[�J�����W
			iaInfo->neighborInfo.last_face_normal = iaInfo->neighborInfo.face_normal;		// �O��̖@��(�@���̕�ԂɎg��)�C���߂ċߖT�ɂȂ������͍���ł����@��
			iaInfo->neighborInfo.face_normal = normal;				// ���̂���͊o�|�C���^�ւ̖@��
		}else{
			/// �ߖT���̂ł͂Ȃ��̂�false�ɂ���
			iaInfo->flag.bneighbor = false;
			iaInfo->flag.bfirstlocal = false;						
			iaInfo->flag.blocal = false;																
		}
	}
}

double FWInteractAdaptee::FindNearestPoint(const CDConvexIf* a, const CDConvexIf* b,
											const Posed& a2w, const Posed& b2w, const Vec3d pc, Vec3d& dir, 
											Vec3d& normal, Vec3d& pa, Vec3d& pb){
	/// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ŋߖT�̕��̂����肷��
	CDConvex* ca = DCAST(CDConvex, a);
	CDConvex* cb = DCAST(CDConvex, b);
	FindClosestPoints(ca, cb, a2w, b2w, pa, pb);	///< GJK�ŋߖT�_�̎Z�o
	Vec3d wa = a2w * pa;							///< ���̋ߖT�_�̃��[���h���W
	Vec3d wb = b2w * pb;							///< �͊o�|�C���^�ߖT�_�̃��[���h���W
	Vec3d a2b = wb - wa;							///< ���̂���͊o�|�C���^�ւ̃x�N�g��
	normal = a2b.unit();
	//DSTR << "dir" << dir<<std::endl;
	/// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�
	if(a2b.norm() < 0.01){									
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		/// dir���ׂ�Ă��܂��Ă���ꍇ�͍��̏d�S����ߖT�_�ւ̃x�N�g���Ƃ���
		if(dir == Vec3d(0.0, 0.0, 0.0) ){
			dir =-( wa - pc);
			//DSTR << "dir not find"<<std::endl;
		}
#if 0
		///Sphere�̏ꍇ�CGJK�̌��ʂ��g��Ȃ�
		if(DCAST(CDSphereIf,a)) dir =-( wa - pc);
#endif

		double dist = 0.0;
		/// CCDGJK�̎��s
			//DSTR << "dir="<< dir << std::endl;
		
extern bool bGJKDebug;
		bGJKDebug = false;
//		bCCDGJKDebug = true;
		int cp = ContFindCommonPoint(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
		bGJKDebug = false;
		/// CCDGJK�����s�����ꍇ�̏���
		if(cp != 1){
			ContFindCommonPointSaveParam(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
			DSTR << "ContFindCommonPoint do not find contact point" << std::endl;
		}
	}
	return a2b.norm();
}


}