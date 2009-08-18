#include <Framework/FWInteractAdaptee.h>
#include <Framework/FWInteractScene.h>
#include <Physics/PHConstraintEngine.h>
#include <iostream>

namespace Spr{;

/** FWHapticLoopBase�̎��� */
///////////////////////////////////////////////////////////////////////////////////

void FWHapticLoopBase::Clear(){
	GetInteractSolids()->clear();
	loopCount = 1;
}

/** FWInteractAdaptee�̎��� */
///////////////////////////////////////////////////////////////////////////////////
FWInteractAdaptee::FWInteractAdaptee(){}
FWInteractScene* FWInteractAdaptee::GetInteractScene(){
	return interactScene; 
}										
FWHapticLoopBase* GetHapticLoop(){
	return NULL; 
}
FWInteractSolid* FWInteractAdaptee::GetInteractSolid(int i){
	if(0 <= i && i < NInteractSolids()) return &interactSolids[i];
	return NULL;
}
FWInteractSolids* FWInteractAdaptee::GetInteractSolids(){
	return &interactSolids;
}
int FWInteractAdaptee::NInteractSolids(){
	return (int)interactSolids.size();
}
void FWInteractAdaptee::UpdateSolidList(){
	PHSceneIf* phScene = GetInteractScene()->GetScene()->GetPHScene(); 
	PHSolidIf** solids = phScene->GetSolids();
	FWInteractPointers* iPointers = GetInteractScene()->GetInteractPointers();
	for(int i = (int)GetInteractSolids()->size(); i < phScene->NSolids(); i++){
		GetInteractSolids()->resize(i + 1);
		GetInteractSolids()->back().sceneSolid = solids[i]->Cast();
		DSTR << GetInteractSolids()->size() << std::endl;
		for(int j = 0; j < (int)iPointers->size(); j++){
			FWInteractPointer* ip = iPointers->at(j)->Cast();
			FWInteractInfo info;
			ip->interactInfo.resize(i + 1);
//			DSTR << ip->interactInfo.size() << std::endl;
		}
	}
}

void FWInteractAdaptee::NeighborObjectFromPointer(){
	// GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	// ��������ׂĂ�shape�������̂ɂ��Ă��
	// AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
	// �����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
	// Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O		
	//1. BBox���x���̏Փ˔���
	FWInteractSolid* iSolid;
	FWInteractPointer* iPointer;
	FWInteractInfo* iInfo;

	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * NInteractSolids());
	Edges::iterator eit = edges.begin();
	int pN = GetInteractScene()->NInteractPointers();
	for(int i = 0; i < NInteractSolids(); ++i){
		for(int j = 0; j < pN; j++){
			iPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
			iInfo = &iPointer->interactInfo[i];
			iInfo->flag.bneighbor = false;
		}
		iSolid = GetInteractSolid(i);
		iSolid->sceneSolid->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = iSolid->sceneSolid->GetDeltaPosition();
		float dLen = (float) (dPos * dir);
		if (dLen < 0){
			eit[0].edge += dLen;
		}else{
			eit[1].edge += dLen;
		}
		eit[0].index = i; eit[0].bMin = true;
		eit[1].index = i; eit[1].bMin = false;
		eit += 2;
	}
	sort(edges.begin(), edges.end());

	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef std::set<int> SolidSet;
	SolidSet cur;							//	���݂�Solid�̃Z�b�g
	bool found = false;
	int nP = GetInteractScene()->NInteractPointers();
	for(int i = 0; i < nP; i++){
		iPointer = GetInteractScene()->GetInteractPointer(i)->Cast();
		PHSolid* soPointer = DCAST(PHSolid, iPointer->GetPointerSolid());
		for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
			///	���[��������C���X�g���̕��̂Ɣ���
			if (it->bMin){						
				for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
					int f1 = it->index;
					int f2 = *itf;
					if (f1 > f2) std::swap(f1, f2);
					/// �ߖT���̂̉\������C�ڍה����
					if(GetInteractSolid(f1)->sceneSolid == soPointer){
						iPointer->interactInfo[f2].flag.bneighbor = true;
						UpdateInteractSolid(f2, iPointer);
					}else if(GetInteractSolid(f2)->sceneSolid == soPointer){
						iPointer->interactInfo[f1].flag.bneighbor = true;
						UpdateInteractSolid(f1, iPointer);
					}
				}
				cur.insert(it->index);
			}else{
				///	�I�[�Ȃ̂ō폜�D
				cur.erase(it->index);	
			}
		}
	}
	
	for(int i = 0; i < NInteractSolids(); i++){
		int lCount = 0;					///< flag.blocal�̐�
		int fCount = 0;					///< flag.bfirstlocal�̐�									
		iSolid = GetInteractSolid(i);
		for(int j = 0; j < nP; j++){
			iPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			/// bneighbor����blocal�ł����lCount++
			if(iInfo->flag.bneighbor && iInfo->flag.blocal){				
				lCount++;
				/// �����bfirstlocal�ł����fCount++
				if(iInfo->flag.bfirstlocal){
					fCount++;
					DSTR << fCount << std::endl;
				}
			}else{
				/// �ߖT���̂łȂ��̂�false�ɂ���
				iInfo->flag.bfirstlocal = false;							
				iInfo->flag.blocal = false;									
			}
		}
		/// ���V�~�����[�V�����̏����t���O��true�ɂ���
		if(fCount > 0){
			iSolid->bfirstSim = true;	
			iSolid->copiedSolid = *iSolid->sceneSolid;	// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
		}
		/// �V�~�����[�V�����̏����t���O��true�ɂ���
		if(lCount > 0){
			iSolid->bSim = true;					
		}else{
			/// ����ȊO�͑S��false�ɂ���
			iSolid->bSim = false;					
			iSolid->bfirstSim = false;
		}
	}
}

void FWInteractAdaptee::UpdateInteractSolid(int index, FWInteractPointer* iPointer){
	FWInteractInfo* iInfo = &iPointer->interactInfo[index]; 
	PHSolidIf* phSolid = GetInteractSolid(index)->sceneSolid->Cast();
	PHSolidIf* soPointer = iPointer->GetPointerSolid();
	CDConvexIf* a = DCAST(CDConvexIf, phSolid->GetShape(0));				///< ���̂����ʌ`��
	CDConvexIf* b = DCAST(CDConvexIf, soPointer->GetShape(0));				///< �͊o�|�C���^�̓ʌ`��
	Posed a2w = phSolid->GetPose();											///< ���̂̎p��
	Posed b2w = soPointer->GetPose();										///< �͊o�|�C���^�̎p��
	Vec3d dir = -1.0 * iInfo->neighborInfo.face_normal;
	Vec3d cp = phSolid->GetCenterPosition();								///< ���̂̏d�S
	Vec3d normal;															///< ���̂���͊o�|�C���^�ւ̖@��(���[���h���W)
	Vec3d pa, pb;															///< pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j

	/// GJK���g�����ߖT�_�T��
	double r = FindNearestPoint(a, b, a2w, b2w, dir, cp, normal, pa, pb);	

	/// �ߖT�_�܂ł̒�������ߖT���̂��i��
	if(r < iPointer->GetLocalRange()){
		/// ���߂čŋߖT���̂ɂȂ����ꍇ
		if(!iInfo->flag.blocal){																
			iInfo->flag.bfirstlocal = true;													
			iInfo->neighborInfo.face_normal = normal;	// ���߂ċߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D										
			#ifdef _DEBUG
				if (iInfo->neighborInfo.face_normal * normal < 0.8){
					DSTR << "Too big change on normal = " << normal << std::endl;
				}
			#endif
		}
		/// ���߂ċߖT�܂��͌p�����ċߖT�������ꍇ
		iInfo->flag.blocal = true;								// �ߖT���̂Ȃ̂�blocal��true�ɂ���
		iInfo->neighborInfo.closest_point = pa;					// ���̋ߖT�_�̃��[�J�����W
		iInfo->neighborInfo.pointer_point = pb;					// �͊o�|�C���^�ߖT�_�̃��[�J�����W
		iInfo->neighborInfo.last_face_normal = iInfo->neighborInfo.face_normal;		// �O��̖@��(�@���̕�ԂɎg��)�C���߂ċߖT�ɂȂ������͍���ł����@��
		iInfo->neighborInfo.face_normal = normal;				// ���̂���͊o�|�C���^�ւ̖@��
	}else{
		/// �ߖT���̂ł͂Ȃ��̂�false�ɂ���
		iInfo->flag.bneighbor = false;
		iInfo->flag.bfirstlocal = false;						
		iInfo->flag.blocal = false;																
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
	/// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�
	if(a2b.norm() < 0.01){									
		pa = pb = Vec3d(0.0, 0.0, 0.0);
		/// dir���ׂ�Ă��܂��Ă���ꍇ�͍��̏d�S����ߖT�_�ւ̃x�N�g���Ƃ���
		if(dir == Vec3d(0.0, 0.0, 0.0) ){
			dir = wa - pc;
		}
		double dist = 0.0;
		/// CCDGJK�̎��s
		int cp = ContFindCommonPoint(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
		/// CCDGJK�����s�����ꍇ�̏���
		if(cp != 1){
			ContFindCommonPointSaveParam(ca, cb, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
			DSTR << "ContFindCommonPoint do not find contact point" << std::endl;
		}
	}
	return a2b.norm();
}


}