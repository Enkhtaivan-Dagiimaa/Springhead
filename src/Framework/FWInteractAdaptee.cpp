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
#include <Collision/CDDetectorImp.h>
#include <iostream>

namespace Spr{;

/** FWHapticLoopBase�̎��� */
///////////////////////////////////////////////////////////////////////////////////

void FWHapticLoopBase::Clear(){
	GetIASolids()->clear();
	loopCount = 1;
}

void FWHapticLoopBase::UpdateInterface(){
	int N = NIAPointers();
	for(int i = 0; i < N; i++){
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		double s = iPointer->GetWorldScale() * iPointer->GetPosScale();
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			//6���R�x�C���^�t�F�[�X�̏ꍇ
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
			hif->Update((float)hdt);
			PHSolid* hiSolid = &iPointer->hiSolid;
			hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
			hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
			Posed hifPose;
			hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
			hifPose.Ori()=hif->GetOrientation();
			hiSolid->SetPose(GetIAPointer(i)->GetDefaultPosition() * hifPose);
		}else{
			//3���R�x�C���^�t�F�[�X�̏ꍇ
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
			hif->Update((float)hdt);
			PHSolid* hiSolid = &iPointer->hiSolid;
			hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
			Posed hifPose;
			hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
			hiSolid->SetPose(GetIAPointer(i)->GetDefaultPosition() * hifPose);
		}
	}
}

void FWHapticLoopBase::SetRenderedForce(HIBaseIf* hi, bool bForce, SpatialVector f){
	if(bForce){
		if(DCAST(HIForceInterface6DIf, hi)){
			HIForceInterface6DIf* hif = hi->Cast();
			hif->SetForce(f.v(), Vec3d());
			hif->SetForce(f.v(), f.w());	
		}else{
			HIForceInterface3DIf* hif = hi->Cast();
			hif->SetForce(f.v());
		}
	}else{
		if(DCAST(HIForceInterface6DIf, hi)){
			HIForceInterface6DIf* hif = hi->Cast();
			hif->SetForce(Vec3d(), Vec3d());
		}else{
			HIForceInterface3DIf* hif = hi->Cast();
			hif->SetForce(Vec3d());
		}		
	}
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
void FWInteractAdaptee::InitIAAdaptee(){
	GetHapticLoop()->SetIAAdaptee(this);
}
void FWInteractAdaptee::SetHMode(FWHapticMode hMode) { GetHapticLoop()->hmode = hMode; }

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
			// �ڐG�_�̏�����
			iaInfo->toHaptic.solid_section.clear();
			iaInfo->toHaptic.pointer_section.clear();

			/// Solid�����̃|�C���^�ł������ꍇ
			for(int k = 0; k < NIAPointers(); k++){
				if(phSolid == GetIAPointer(k)->pointerSolid->Cast()) phSolid = NULL;
			}
			if (soPointer != phSolid && phSolid){
				/* AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
				   �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
				*/
				/// 1. BBox���x���̏Փ˔���(Sweep & Prune)
				/// sceneSolid��shape���W�n�ɂ��낦�Ĕ�����s��
				Vec3d range = Vec3d(1, 1, 1) * iPointer->GetLocalRange();
				//Posed solidPose = phSolid->GetPose()*phSolid->GetShapePose(0) ;
				//Posed pointerPose = soPointer->GetPose() * soPointer->GetShapePose(0);
				Vec3f soMax = phSolid->GetShapePose(0).Inv() * phSolid->bbox.GetBBoxMax();		//sceneSolid��BBox�̍ő�l(3��) sceneSolid��shape���W�n
				Vec3f soMin = phSolid->GetShapePose(0).Inv() * phSolid->bbox.GetBBoxMin();		//sceneSolid��BBox�̍ŏ��l(3��) sceneSolid��shape���W�n
				Vec3d pMin = soPointer->GetShapePose(0) * soPointer->bbox.GetBBoxMin() ;		// Pointer��BBox�̍ŏ��l(3��) pointer��shape���W�n
				Vec3d pMax = soPointer->GetShapePose(0) * soPointer->bbox.GetBBoxMax() ;		// Pointer��BBox�̍ő�l(3��) pointer��shape���W�n

				//�|�C���^�[��BBox�̌`������o�Ȃ��ꍇ��BBox�̍ő�l�ŏ��l�����W�ϊ��ɂ��ς���Ă��܂�
				//�����ŁCBBox�̑傫����傫���Ƃ�
				double pmin = pMin[0],pmax = pMax[0];
				for(int i=1;i<3;i++){
					if(pmin>pMin[i]) pmin = pMin[i];
					if(pmax<pMax[i]) pmax = pMax[i];
				}
				pMin =Vec3d(pmin,pmin,pmin); // Pointer��BBox�̍ŏ��l(3��) pointer��shape���W�n(3���ň�ԍŏ��̒l)
				pMax =Vec3d(pmax,pmax,pmax); // Pointer��BBox�̍ő�l(3��) pointer��shape���W�n(3���ň�ԍő�̒l)

				pMin = phSolid->GetShapePose(0).Inv() * phSolid->GetPose().Inv() * soPointer->GetPose() * pMin - range;		// Pointer��BBox�̍ŏ��l(3��) sceneSolid��shape���W�n
				pMax = phSolid->GetShapePose(0).Inv() * phSolid->GetPose().Inv() * soPointer->GetPose() * pMax + range;		// Pointer��BBox�̍ő�l(3��) sceneSolid��shape���W�n
				/// 3���Ŕ���
				int isLocal = 0;		//< �����̎��Ō������Ă��邩�ǂ���
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
					DSTR << "isLocal" << isLocal <<  std::endl;
#endif
				}
				/// 2.�ߖT���̂Ɣ���D�ڐG��͂���я�ԍX�V
				isLocal = 3;
				if(isLocal > 2){
					UpdateInteractSolid(i, GetIAPointer(j));
					lCount++;		//< ���[�J���Ȃ̂�lCount++
					if(iaInfo->flag.bfirstlocal){
						fCount++;	//< firstLocal�Ȃ̂�fCount++;
						iaInfo->flag.bfirstlocal = false;
					}
				}else{
					iaInfo->flag.bfirstlocal = false;							
					iaInfo->flag.blocal = false;				
				}
			}
		}	// end IAPointer�̐��������

		/// ���V�~�����[�V�����̏����t���O��true�ɂ���
		FWInteractSolid* inSolid = GetIASolid(i);
		if(fCount > 0){
			inSolid->bfirstSim = true;	
			inSolid->copiedSolid = *inSolid->sceneSolid;	// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
			inSolid->b = inSolid->lastb = SpatialVector();
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
		Posed a2w = phSolid->GetPose() * phSolid->GetShapePose(0);				///< ���̂̎p��
		Posed b2w = soPointer->GetPose() * soPointer->GetShapePose(0);			///< �͊o�|�C���^�̎p��
		Posed shapePoseW0 = phSolid->GetShapePose(0);
		Posed shapePoseW1 = soPointer->GetShapePose(0);
		Vec3d dir = -1.0 * iaInfo->toHaptic.face_normal;
		Vec3d cp = phSolid->GetCenterPosition();								///< ���̂̏d�S
		Vec3d normal;															///< ���̂���͊o�|�C���^�ւ̖@��(���[���h���W)
		Vec3d pa, pb;															///< pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j

		/// GJK���g�����ߖT�_�T��
		int found = 0;
		found = FindNearestPoint(a, b, a2w, b2w, cp, dir, normal, pa, pb);		
		/// �����̌㏈��(���[�J���V�~�����[�V�������邩�ǂ����̃t���O�𗧂Ă�)
		if(found > 0){
			/// ���߂čŋߖT���̂ɂȂ����ꍇ
			if(iaInfo->flag.blocal == false){
				iaInfo->flag.bfirstlocal = true;	
				iaInfo->toHaptic.closest_point = pa;	//< ���߂ċߖT���̂ɂȂ����̂ŁC�O��̍��̂̐ڐG�_�ɍ���ł����ڐG�_���㏑��
				iaInfo->toHaptic.face_normal = normal;	//< ���߂ċߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D
				
				#ifdef _DEBUG
					if (iaInfo->toHaptic.face_normal * normal < 0.8){
						DSTR << "Too big change on normal = " << normal << std::endl;
					}
				#endif
			}
			/// ���߂ċߖT�܂��͌p�����ċߖT�������ꍇ
			iaInfo->flag.blocal = true;							//< �ߖT���̂Ȃ̂�blocal��true�ɂ���
			iaInfo->toHaptic.pose = phSolid->GetPose();
			iaInfo->toHaptic.closest_point = pa;				//< ���̋ߖT�_
			iaInfo->toHaptic.pointer_point = pb;				//< �͊o�|�C���^�ߖT�_
			iaInfo->toHaptic.common_point = (phSolid->GetPose() * pa + soPointer->GetPose() * pb)/2; 
			iaInfo->toHaptic.last_face_normal = iaInfo->toHaptic.face_normal;		//< �O��̖@��(�@���̕�ԂɎg��)�C���߂ċߖT�ɂȂ������͍���ł����@��
			iaInfo->toHaptic.face_normal = normal;				//< ���̂���͊o�|�C���^�ւ̖@��
		}

/// �ڐG���(susa������)
#define CONTACT_ANALYSIS
#ifdef CONTACT_ANALYSIS		
		iaInfo->toHaptic.intersection_vertices.clear();
		if(found == 1)	iaInfo->toHaptic.intersection_vertices.push_back(pb);
		if(found == 2)	AnalyzeContactResion(phSolid, soPointer, pa, pb, &iaInfo->toHaptic);
#endif
	}
}

int FWInteractAdaptee::FindNearestPoint(const CDConvexIf* a, const CDConvexIf* b,
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

	if(a2b.norm() > 0.001){
		return 1;	// FindClosestPoints�Ō�����
	}else{
		/// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		/// dir���ׂ�Ă��܂��Ă���ꍇ�͍��̏d�S����ߖT�_�ւ̃x�N�g���Ƃ���
		if(dir == Vec3d(0.0, 0.0, 0.0) ){
			dir =-( wa - pc);
			DSTR << "dir is Zero vecotor." << std::endl;
		}

		/// CCDGJK�̎��s	
extern bool bGJKDebug;
		bGJKDebug = false;

		double dist = 0.0;
		int cp = ContFindCommonPoint(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
		bGJKDebug = false;
		/// CCDGJK�����s�����ꍇ�̏���
		if(cp != 1){
			static int cont = 0;
			ContFindCommonPointSaveParam(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
			DSTR << cont << "ContFindCommonPoint do not find contact point" << std::endl;
			cont++;
		}
		return 2;	// CCDGJK�Ō�����
	}
}

// �͊o�|�C���^�ƍ��̂̐ڐG�`��(�{�����[���̒��_)���擾
extern bool bUseContactVolume;
void FWInteractAdaptee::AnalyzeContactResion(PHSolid* solida, PHSolid* solidb, Vec3d pa, Vec3d pb, ToHaptic* th){
	CDShapePair sp;
	sp.shape[0] = solida->GetShape(0)->Cast();
	sp.shape[1] = solidb->GetShape(0)->Cast();
	sp.commonPoint = th->common_point; 
	sp.shapePoseW[0] = solida->GetPose();
	sp.shapePoseW[1] = solidb->GetPose();

	static CDContactAnalysis analyzer;
	
	bUseContactVolume = true;
	analyzer.FindIntersection(&sp);
	bUseContactVolume = false;

	// �`��̒��_������Ă���
	for(CDQHPlane< CDContactAnalysisFace >* it = analyzer.planes.begin; it != analyzer.planes.end; ++it){
		if(it->deleted) continue;
		Vec3d point = it->normal/it->dist + sp.commonPoint;
	
/// ALL��SELECTION�̂ǂ��炩��I��
#define SELECTION	// ���ԕ\���̏�ɂɍڂ��Ă���_�͐ڐG�_�Ƃ��Ȃ�
//#define ALL		// �ڐG�`��̒��_��S�āC�ڐG�_�Ƃ���
#ifdef SELECTION
		double dot = (point - solida->GetPose() * pa) * th->face_normal;
		if(dot < -1e-5){
			th->intersection_vertices.push_back(solidb->GetPose().Inv() * point);
		}
#endif
#ifdef ALL
		th->intersection_vertices.push_back(solidb->GetPose().Inv() * point);
#endif
	}	
}


//Vec3d* FWInteractAdaptee::GetProxyPoint(){
//	Vec3d DisplayProxy[2] = {Vec3d(5,0,0),Vec3d(5,0,0)};
//	return DisplayProxy;
//}

//Vec3d* FWInteractAdaptee::GetForce(){
//	Vec3d DisplayForce[2] = {Vec3d(0,0,0),Vec3d(0,0,0)};
//	return DisplayForce;
//}

bool FWInteractAdaptee::GetContactFlag(){
	return false;
}

Vec3d FWInteractAdaptee::GetPicCenterPosition(){
	Vec3d centerPosition = Vec3d(0,0,0);
	return centerPosition;
}

}