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

void FWHapticLoopBase::SetRenderedForce(HIBaseIf* hi, bool bForce, SpatialVector f){
	if(bForce){
		if(DCAST(HIForceInterface6DIf, hi)){
			HIForceInterface6DIf* hif = hi->Cast();
			hif->SetForce(f.v(), Vec3d());
			#ifdef TORQUE
				hif->SetForce(outForce.v(), outForce.w());
			#endif
		}else{
			HIForceInterface3DIf* hif = hi->Cast();
			hif->SetForce(f.v());
		}
	}else{
		if(DCAST(HIForceInterface6DIf, hi)){
			HIForceInterface6DIf* hif = hi->Cast();
			hif->SetForce(Vec3d(), Vec3d());
			#ifdef TORQUE
				hif->SetForce(Vec3d(), Vec3d());
			#endif
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

			/// Solid�����̃|�C���^�ł������ꍇ
			for(int k = 0; k < NIAPointers(); k++){
				if(phSolid == GetIAPointer(k)->pointerSolid->Cast()) phSolid = NULL;
			}
			if (soPointer != phSolid && phSolid){
				/* AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
				   �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
				*/
				/// 1. BBox���x���̏Փ˔���(Sweep & Prune)
				/// sceneSolid�̃��[�J�����W�n�ɂ��낦�Ĕ�����s��
				Vec3d range = Vec3d(1, 1, 1) * iPointer->GetLocalRange();
				Posed solidPose = phSolid->GetPose() * phSolid->GetShapePose(0);
				Posed pointerPose = soPointer->GetPose() * soPointer->GetShapePose(0);
				Vec3f soMax = phSolid->bbox.GetBBoxMax();												//sceneSolid��BBox�̍ő�l(3��) sceneSolid���W�n
				Vec3f soMin = phSolid->bbox.GetBBoxMin();												//sceneSolid��BBox�̍ŏ��l(3��) sceneSolid���W�n
				Vec3d pMin = solidPose.Inv() * pointerPose * soPointer->bbox.GetBBoxMin() - range;		// Pointer��BBox�̍ŏ��l(3��) sceneSolid���W�n
				Vec3d pMax = solidPose.Inv() * pointerPose * soPointer->bbox.GetBBoxMax() + range;		// Pointer��BBox�̍ő�l(3��) sceneSolid���W�n
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
#if 1
					DSTR << i << " pMin[i] = " << pMin[i] << "  soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << std::endl;
					DSTR << i << " pMin[i] = "  << pMin[i] << "  soMax[i] = " << soMax[i] << "  pMax[i] = " << pMax[i] << std::endl;
					DSTR << i << " soMin[i] = " << soMin[i] << "  pMin[i] = " << pMin[i] << "  soMax[i] = " << soMax[i] << std::endl;
					DSTR << i << " soMin[i] = " << soMin[i] << "  pMax[i] = " << pMax[i] << "  soMax[i] = " << soMax[i] << std::endl;
					DSTR << "isLocal" << isLocal <<  std::endl;
#endif
				}
				/// 2.�ߖT���̂Ɣ���D�ڐG��͂���я�ԍX�V
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
		Posed a2w = phSolid->GetPose() * phSolid->GetShapePose(0);											///< ���̂̎p��
		Posed b2w = soPointer->GetPose() * soPointer->GetShapePose(0);										///< �͊o�|�C���^�̎p��
		Vec3d dir = -1.0 * iaInfo->neighborInfo.face_normal;
		Vec3d cp = phSolid->GetCenterPosition();								///< ���̂̏d�S
		Vec3d normal;															///< ���̂���͊o�|�C���^�ւ̖@��(���[���h���W)
		Vec3d pa, pb;															///< pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j

		/// GJK���g�����ߖT�_�T��
		int found = 0;
		found = FindNearestPoint(a, b, a2w, b2w, cp, dir, normal, pa, pb);							
		
		/// �ڐG���(susa������)
		Vec3d a2b = b2w * pb - a2w * pa;
		std::vector<Vec3d> section;
		Vec3d commonPoint;
		if(found == 1){
			/// FindClosestPoint�ŏI������ꍇ
			/// �͊o�|�C���^�ƍ��̂�����Ă���̂ŁC�߂Â��ĐڐG���
			b2w.Pos() -= a2b;
			commonPoint = a2w * pa;
			FindSectionVertex(phSolid, soPointer, a2w, b2w, pa, pb, normal, commonPoint, section);
		}else if(found == 2){
			/// ContFindCommonPoint�ŏI������ꍇ
			///	���ɐڐG���Ă����ԂȂ̂ŁC���̂܂ܐڐG���
			commonPoint = a2w * pa + 0.5 * a2b;
			FindSectionVertex(phSolid, soPointer, a2w, b2w, pa, pb, normal, commonPoint, section);
		}
		iaInfo->neighborInfo.section = section;

		if(found > 0){
			/// ���߂čŋߖT���̂ɂȂ����ꍇ
			if(iaInfo->flag.blocal == false){																
				iaInfo->flag.bfirstlocal = true;	
				iaInfo->neighborInfo.face_normal = normal;	//< ���߂ċߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D										
				#ifdef _DEBUG
					if (iaInfo->neighborInfo.face_normal * normal < 0.8){
						DSTR << "Too big change on normal = " << normal << std::endl;
					}
				#endif
			}
			/// ���߂ċߖT�܂��͌p�����ċߖT�������ꍇ
			iaInfo->flag.blocal = true;								//< �ߖT���̂Ȃ̂�blocal��true�ɂ���
			iaInfo->neighborInfo.closest_point = pa;				//< ���̋ߖT�_�̃��[�J�����W
			iaInfo->neighborInfo.pointer_point = pb;				//< �͊o�|�C���^�ߖT�_�̃��[�J�����W
			iaInfo->neighborInfo.last_face_normal = iaInfo->neighborInfo.face_normal;		//< �O��̖@��(�@���̕�ԂɎg��)�C���߂ċߖT�ɂȂ������͍���ł����@��
			iaInfo->neighborInfo.face_normal = normal;				//< ���̂���͊o�|�C���^�ւ̖@��
		}else{
			/// �����ɂ͓���Ȃ��\����C�v����(susa)
			/// �ߖT���̂ł͂Ȃ��̂�false�ɂ���
			iaInfo->flag.bfirstlocal = false;						
			iaInfo->flag.blocal = false;																
		}
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

	if(a2b.norm() >= 0.01){
		return 1;	// FindClosestPoints�Ō����� 
	}else{
		/// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		/// dir���ׂ�Ă��܂��Ă���ꍇ�͍��̏d�S����ߖT�_�ւ̃x�N�g���Ƃ���
		if(dir == Vec3d(0.0, 0.0, 0.0) ){
			dir =-( wa - pc);
		}

		/// CCDGJK�̎��s	
extern bool bGJKDebug;
		bGJKDebug = false;

		double dist = 0.0;
		int cp = ContFindCommonPoint(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);

		bGJKDebug = false;
		/// CCDGJK�����s�����ꍇ�̏���
		if(cp != 1){
			ContFindCommonPointSaveParam(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
			DSTR << "ContFindCommonPoint do not find contact point" << std::endl;
		}
		return 2;	// CCDGJK�Ō�����
	}
}


void FWInteractAdaptee::FindSectionVertex(PHSolid* solid0, PHSolid* solid1, const Posed shapePoseW0, const Posed shapePoseW1,
										  Vec3d pa, Vec3d pb, const Vec3d normal,
										  const Vec3d commonPoint, std::vector<Vec3d>& section){
	/// �͊o�|�C���^�ƍ��̂Ƃ̐ڐG�������
	// PHConstraintEngine.h�ɂ��郁���o�֐�PHShapePairForLCP::EnumVertex()������
	// �{����CDDetectorImp.h�̃N���XCDContactAnalysisFace���g���ׂ��H

	// ���Α��x���݂�2D�̍��W�n�����߂�B
	FPCK_FINITE(solid0->pose);
	FPCK_FINITE(solid1->pose);
	Vec3d v0 = solid0->GetPointVelocity(solid0->pose * pa);
	Vec3d v1 = solid1->GetPointVelocity(solid1->pose * pb);
	Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
	local.Ex() = normal;
	local.Ey() = v1-v0;
	local.Ey() -= local.Ey() * normal * normal;
	if (local.Ey().square() > 1e-6){
		local.Ey().unitize(); 
	}else{
		if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
		else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
	}
	local.Ez() =  local.Ex() ^ local.Ey();

	//	�ʂƖʂ��G���ꍇ������̂ŁA�ڐG���ʑ��p�`��ʌ`��ɂȂ邱�Ƃ�����B
	//	�؂�������߂�B�܂��A���ꂼ��̌`��̐؂�����
	CDCutRing cutRing(commonPoint, local);	//	commonPoint�Ȃ�΁A������܂ޖʂŐ؂�΁A�K���؂���̒��ɂȂ�B
	//	�����ɐ؂��������ꍇ�D(���ȂǂȂ����̂�����)
	CDConvex* convex0 = solid0->GetShape(0)->Cast();
	bool found = convex0->FindCutRing(cutRing, shapePoseW0);
	int nLine0 = cutRing.lines.size();
	CDConvex* convex1 = solid1->GetShape(0)->Cast();
	if (found) found = convex1->FindCutRing(cutRing, shapePoseW1);
	int nLine1 = cutRing.lines.size() - nLine0;
	section.clear();
	if (found){
		//	2�̐؂���̃A���h���Ƃ��āA2���̂̐ڐG�ʂ̌`������߂�B
		cutRing.MakeRing();		
		if (cutRing.vtxs.begin != cutRing.vtxs.end && !(cutRing.vtxs.end-1)->deleted){
			CDQHLine<CDCutLine>* vtx = cutRing.vtxs.end-1;
			do{
				assert(finite(vtx->dist));
				Vec3d pos;
				pos.sub_vector(1, Vec2d()) = vtx->normal / vtx->dist;
				pos = cutRing.local * pos;
				section.push_back(pos);
				vtx = vtx->neighbor[0];
			} while (vtx!=cutRing.vtxs.end-1);
		}
	}
	if (section.size() == 0){	//	�ЂƂ��ǉ����Ă��Ȃ����؂�����Ȃ����� or �����Ă�ConvexHull�����Ȃ������D
		//	������1�_�ŐڐG���Ă���D
		section.push_back(commonPoint);
	}
#if 0
	DSTR << "commonPoint" << commonPoint << std::endl;
	DSTR << "section" << std::endl;
	for(int k = 0; k < section.size(); k++)		DSTR << section[k] << std::endl;
	DSTR << "--------------------------------" << std::endl;
#endif
}

Vec3d* FWInteractAdaptee::GetProxyPoint(){
	Vec3d DisplayProxy[2] = {Vec3d(5,0,0),Vec3d(5,0,0)};
	return DisplayProxy;
}

Vec3d* FWInteractAdaptee::GetForce(){
	Vec3d DisplayForce[2] = {Vec3d(0,0,0),Vec3d(0,0,0)};
	return DisplayForce;
}

bool FWInteractAdaptee::GetContactFlag(){
	return false;
}

}
