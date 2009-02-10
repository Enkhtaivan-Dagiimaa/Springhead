#include <Framework/FWMultiRateHapticApp.h>
#include <sstream>

#ifdef USE_HDRSTOP
#pragma  hdrstop
#endif

namespace Spr{;

FWMultiRateHapticApp::FWMultiRateHapticApp(){}

void FWMultiRateHapticApp::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWMultiRateHapticApp::Idle(){
	Step();
}

void FWMultiRateHapticApp::SyncHapticProcess(){}
void FWMultiRateHapticApp::ExpandPHSolidInfo(){}

void FWMultiRateHapticApp::FindNearestObjectFromHapticPointer(){
	///*
	//	GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
	//	��������ׂĂ�shape�������̂ɂ��Ă��
	//	AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
	//	�����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
	//	Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
	//*/
	//
	////1. BBox���x���̏Փ˔���
	//std::vector<ExpandedPHSolid>* esolids = GetExpandedPHSolids();
	//esolids->[0];
//	size_t N = expandedObjects.size();
//	Vec3f dir(0,0,1);
//	Edges edges;
//	edges.resize(2 * N);
//	Edges::iterator eit = edges.begin();
//	for(int i = 0; i < (int)N; ++i){
//		// ���[�J����������ׂ�false�ɂ���
//		expandedObjects[i].flag.bneighbor = false;
//		DCAST(PHSolid, expandedObjects[i].phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
//		Vec3d dPos = expandedObjects[i].phSolidIf->GetDeltaPosition();
//		float dLen = (float) (dPos * dir);
//		if (dLen < 0){
//			eit[0].edge += dLen;
//		}else{
//			eit[1].edge += dLen;
//		}
//		eit[0].index = i; eit[0].bMin = true;
//		eit[1].index = i; eit[1].bMin = false;
//		eit += 2;
//	}
//	sort(edges.begin(), edges.end());
//	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
//	typedef set<int> SolidSet;
//	SolidSet cur;							//	���݂�Solid�̃Z�b�g
//	bool found = false;
//
//	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
//		if (it->bMin){						//	���[��������C���X�g���̕��̂Ɣ���
//			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
//				int f1 = it->index;
//				int f2 = *itf;
//				if (f1 > f2) std::swap(f1, f2);
//				// �ߖT���̂Ƃ��Č���
//				if(expandedObjects[f1].phSolidIf == soPointer){
//					expandedObjects[f2].flag.bneighbor = true;
//				}else if(expandedObjects[f2].phSolidIf == soPointer){
//					expandedObjects[f1].flag.bneighbor = true;
//				}
//			}
//			cur.insert(it->index);
//		}else{
//			cur.erase(it->index);			//	�I�[�Ȃ̂ō폜�D
//		}
//	}
//
//	for(size_t i = 0; i < expandedObjects.size(); i++){
//		if(expandedObjects[i].flag.bneighbor) continue;
//		expandedObjects[i].flag.bfirstlocal = false;			//�ߖT���̂łȂ��̂�false�ɂ���
//		expandedObjects[i].flag.blocal = false;
//	}
//
//	// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ŋߖT�̕��̂����肷��
//	// �ŋߖT���̂�������blocal��true�ɂ��CphSolid��phSolidIf���R�s�[����
//	// blocal�����ł�true��������R�s�[�ς݂Ȃ̂ŋߖT�_�����R�s�[����
//	for(unsigned i = 0; i < (int)expandedObjects.size(); i++){
//		if(!expandedObjects[i].flag.bneighbor) continue;															// �ߖT�łȂ���Ύ���
//		CDConvex* a = DCAST(CDConvex, expandedObjects[i].phSolidIf->GetShape(0));		// ���̂����ʌ`��
//		CDConvex* b = DCAST(CDConvex, soPointer->GetShape(0));									// �͊o�|�C���^�̓ʌ`��
//		Posed a2w, b2w;																								// ���̂̃��[���h���W
//*		if(expandedObjects[i].blocal) 	a2w = expandedObjects[i].phSolid.GetPose();				// blocal��true�Ȃ�ŐV�̏��ł��
//		else */								a2w = expandedObjects[i].phSolidIf->GetPose();
//		b2w = soPointer->GetPose();																			// �͊o�|�C���^�̃��[���h���W
//		Vec3d pa ,pb;																									// pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j
//		pa = pb = Vec3d(0.0, 0.0, 0.0);
//		FindClosestPoints(a, b, a2w, b2w, pa, pb);															// GJK�ŋߖT�_�̎Z�o
//		Vec3d wa = a2w * pa;																						// ���̋ߖT�_�̃��[���h���W
//		Vec3d wb = b2w * pb;																						// �͊o�|�C���^�ߖT�_�̃��[���h���W
//		Vec3d a2b = wb - wa;																						// ���̂���͊o�|�C���^�ւ̃x�N�g��
//		Vec3d normal = a2b.unit();
//		if(a2b.norm() < range){																					// �ߖT�_�܂ł̒�������ߖT���̂��i��
//			if(a2b.norm() < 0.01){																				// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�		
//				pa = pb = Vec3d(0.0, 0.0, 0.0);
//				Vec3d dir = -1.0 * expandedObjects[i].syncInfo.neighborPoint.face_normal;
//				if(dir == Vec3d(0.0, 0.0, 0.0) ){
//					dir = -(soPointer->GetCenterPosition() - wa);
//				}
//				double dist = 0.0;
//				int cp = ContFindCommonPoint(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
//				if(cp != 1){
//					ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
//					DSTR << "ContFindCommonPoint do not find contact point" << endl;
//				}
//			}
//			if(!expandedObjects[i].flag.blocal){																			// ���߂čŋߖT���̂ɂȂ�����
//				expandedObjects[i].flag.bfirstlocal = true;
//				expandedObjects[i].phSolid = *DCAST(PHSolid, expandedObjects[i].phSolidIf);			// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
//				expandedObjects[i].syncInfo.neighborPoint.face_normal = normal;							// ���߂čŋߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D										
//#ifdef _DEBUG
//				if (expandedObjects[i].syncInfo.neighborPoint.face_normal * normal < 0.8){
//					DSTR << "Too big change on normal" << normal << std::endl;
//				}
//#endif
//			}
//			expandedObjects[i].flag.blocal = true;																		// �ŋߖT���̂Ȃ̂�blocal��true�ɂ���
//			expandedObjects[i].syncInfo.neighborPoint.closestPoint = pa;										// ���̋ߖT�_�̃��[�J�����W
//			expandedObjects[i].syncInfo.neighborPoint.pointerPoint = pb;										// �͊o�|�C���^�ߖT�_�̃��[�J�����W
//			expandedObjects[i].syncInfo.neighborPoint.last_face_normal = expandedObjects[i].syncInfo.neighborPoint.face_normal;					// �O��̖@��(�@���̕�ԂɎg��)�C���߂ċߖT�ɂȂ������͍���ł����@��
//			expandedObjects[i].syncInfo.neighborPoint.face_normal = normal;								// ���̂���͊o�|�C���^�ւ̖@��
//		}else{
//			expandedObjects[i].flag.blocal = false;																		// �ŋߖT���̂ł͂Ȃ��̂�blocal��false�ɂ���
//			expandedObjects[i].flag.bfirstlocal = false;
//		}
//	}
}
}