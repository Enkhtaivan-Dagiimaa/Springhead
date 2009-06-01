#include <Framework/FWMultiRateHapticApp.h>
#include <Physics/PHConstraintEngine.h>
#include <sstream>

#ifdef USE_HDRSTOP
#pragma  hdrstop
#endif

namespace Spr{;



FWMultiRateHapticApp::FWMultiRateHapticApp(){
	bSync = false;
	bCalcPhys = true;
	pdt = 50;
	hdt = 1;
}

void FWMultiRateHapticApp::SetHapticProcess(FWHapticProcessBase* process){ hapticProcess = process; }
FWHapticProcessBase* FWMultiRateHapticApp::GetHapticProcess(){ return hapticProcess; }
void FWMultiRateHapticApp::SetPhysicTimeStep(double dt){ pdt = dt; }
double FWMultiRateHapticApp::GetPhysicTimeStep(){ return pdt; }
void FWMultiRateHapticApp::SetHapticTimeStep(double dt){ hdt = dt; }
double FWMultiRateHapticApp::GetHapticTimeStep(){ return hdt; }
void FWMultiRateHapticApp::SetHapticPointer(PHSolidIf* pointer){ 
	hapticPointer = pointer; 
	// �͊o�|�C���^��springhead2�̐ڐG�v�Z����O��
	// ���̂������邽�т�ContactMode��NONE�ɂ��Ȃ���΂����Ȃ�(���������̂ƐڐG�v�Z�����Ă��܂�����)
	GetScene()->GetPHScene()->SetContactMode(hapticPointer, PHSceneDesc::MODE_NONE);
}
PHSolidIf* FWMultiRateHapticApp::GetHapticPointer(){ return hapticPointer; }
int FWMultiRateHapticApp::GetNExpandedPHSolids(){ return expandedPHSolids.size(); }
FWExpandedPHSolid** FWMultiRateHapticApp::GetFWExpandedPHSolids(){ 
	return expandedPHSolids.empty() ? NULL : (FWExpandedPHSolid**)&*expandedPHSolids.begin();
}


void FWMultiRateHapticApp::Init(){	
	GetHapticProcess()->SetHapticInterface(GetHapticInterface());
	SetScene(GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc()));		// Scene�̍쐬�C�ݒ�
	ResetScene();
}

void FWMultiRateHapticApp::ResetScene(){
	expandedPHSolids.clear();
	GetHapticProcess()->expandedPHSolids.clear();
	bSync = false;
	bCalcPhys=true;
	hapticcount = 1;
	GetHapticProcess()->loopCounter = 1;
	
	PHSceneIf* phscene = GetScene()->GetPHScene();
	phscene->Clear();	
	phscene->SetGravity(gravity);				
	phscene->SetTimeStep(pdt);
	phscene->SetNumIteration(nIter);
	SetHapticTimeStep(GetHapticProcess()->GetHapticTimeStep());
	GetHapticProcess()->SetPhysicTimeStep(GetPhysicTimeStep()); 
	BuildScene();																// ���̂��쐬
}

void FWMultiRateHapticApp::Idle(){
	Step();
}

FWExpandedPHSolid** FWMultiRateHapticApp::ExpandPHSolidInfo(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidIf** solids = phscene->GetSolids();
	for(int i = (int)expandedPHSolids.size(); i < phscene->NSolids(); i++){
		expandedPHSolids.resize(i + 1);
		expandedPHSolids.back().phSolidIf = solids[i];
	}
	return expandedPHSolids.empty() ? NULL : (FWExpandedPHSolid**)&*expandedPHSolids.begin();
}

void FWMultiRateHapticApp::FindNearestObjectFromHapticPointer(PHSolidIf* hPointer){
	/*
		GJK���g���ċߖT���̂ƋߖT���̂̍ŋߓ_���擾
		��������ׂĂ�shape�������̂ɂ��Ă��
		AABB�ŗ͊o�|�C���^�ߖT�̕��̂��i��
		�����ōi�������̂ɂ���GJK���s���D�����ōi�邱�Ƃ�GJK������񐔂����Ȃ��ł���D
		Solid��BBox���x���ł̌�������(z���\�[�g)�D�����̂�����̖����g�����O
	*/

	//1. BBox���x���̏Փ˔���
	FWExpandedPHSolid** esolids = GetFWExpandedPHSolids();
	int Nesolids = GetNExpandedPHSolids();
	Vec3f dir(0,0,1);
	Edges edges;
	edges.resize(2 * Nesolids);
	Edges::iterator eit = edges.begin();
	for(int i = 0; i <	Nesolids; ++i){
		// ���[�J����������ׂ�false�ɂ���
		esolids[i]->flag.bneighbor = false;
		((PHSolid*)esolids[i]->phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
//		DCAST(PHSolid, esolids[i]->phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		cout << esolids[i]->phSolidIf->GetFramePosition() << endl;
		FWExpandedPHSolid esolid;
		esolid.phSolidIf = GetHapticPointer();
		Vec3d d = esolid.phSolidIf->GetDeltaPosition();
		cout << d << endl;


		Vec3d dPos = ((PHSolid*)esolids[i]->phSolidIf)->GetDeltaPosition();
//		Vec3d dPos = esolids[i]->phSolidIf->GetDeltaPosition();
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
	std::sort(edges.begin(), edges.end());
	//�[���猩�Ă����āC�ڐG�̉\��������m�[�h�̔��������D
	typedef std::set<int> SolidSet;
	SolidSet cur;																											//	���݂�Solid�̃Z�b�g
	bool found = false;

	for(Edges::iterator it = edges.begin(); it != edges.end(); ++it){
		if (it->bMin){																										//	���[��������C���X�g���̕��̂Ɣ���
			for(SolidSet::iterator itf=cur.begin(); itf != cur.end(); ++itf){
				int f1 = it->index;
				int f2 = *itf;
				if (f1 > f2) std::swap(f1, f2);
				// �ߖT���̂̉�����
				if(esolids[f1]->phSolidIf == hPointer){
					esolids[f2]->flag.bneighbor = true;
				}else if(esolids[f2]->phSolidIf == hPointer){
					esolids[f1]->flag.bneighbor = true;
				}
			}
			cur.insert(it->index);
		}else{
			cur.erase(it->index);																						//	�I�[�Ȃ̂ō폜�D
		}
	}

	// GJK�ŋߖT�_�����߁C�͊o�|�C���^�ߖT�̕��̂����肷��
	// �ߖT���̂�������blocal��true�ɂ��CphSolid��phSolidIf���R�s�[����
	// blocal�����ł�true��������R�s�[�ς݂Ȃ̂ŋߖT�_�����R�s�[����
	for(int i = 0; i < Nesolids; i++){
		if(!esolids[i]->flag.bneighbor){
			esolids[i]->flag.bfirstlocal = false;																	//�ߖT���̂łȂ��̂�false�ɂ���
			esolids[i]->flag.blocal = false;
			continue;																									// �ߖT�łȂ���Ύ���
		}

		CDConvex* a = DCAST(CDConvex, esolids[i]->phSolidIf->GetShape(0));					// ���̂����ʌ`��
		CDConvex* b = DCAST(CDConvex, hPointer->GetShape(0));									// �͊o�|�C���^�̓ʌ`��
		Posed a2w, b2w;																								// ���̂̃��[���h���W
		/*	if(esolids[i]->flag.blocal) 	a2w = esolids[i]->phSolidIf.GetPose();						// blocal��true�Ȃ�ŐV�̏��ł��
		else */								a2w = esolids[i]->phSolidIf->GetPose();
		b2w = hPointer->GetPose();																				// �͊o�|�C���^�̃��[���h���W																									// pa:���̂̋ߖT�_�Cpb:�͊o�|�C���^�̋ߖT�_�i���[�J�����W�j
		Vec3d pa = Vec3d(0.0, 0.0, 0.0);																		// PHSolid�̋ߖT�_
		Vec3d pb = Vec3d(0.0, 0.0, 0.0);																		// HapticPointer�̋ߖT�_
		FindClosestPoints(a, b, a2w, b2w, pa, pb);															// GJK�ŋߖT�_�̎Z�o
		Vec3d wa = a2w * pa;																						// ���̋ߖT�_�̃��[���h���W
		Vec3d wb = b2w * pb;																						// �͊o�|�C���^�ߖT�_�̃��[���h���W
		Vec3d a2b = wb - wa;																						// ���̂���͊o�|�C���^�ւ̃x�N�g��
		Vec3d normal = a2b.unit();
		if(a2b.norm() < localRange){																			// �ߖT�_�ԋ�������ߖT���̂��i��
			if(a2b.norm() < 0.01){																				// �͊o�|�C���^�ƍ��̂����łɐڐG���Ă�����CCDGJK�Ŗ@�������߂�		
				pa = pb = Vec3d(0.0, 0.0, 0.0);																// �ϐ��̃��Z�b�g
				Vec3d dir = -1.0 * esolids[i]->syncInfo.neighborPoint.face_normal;				
				if(dir == Vec3d(0.0, 0.0, 0.0) ){																
					dir = -(hPointer->GetCenterPosition() - wa);											// dir��(0 , 0, 0�Ȃ�PHSolid�̋ߖT�_����HapticPointer�̏d�S�܂łɂ���
				}
				double dist = 0.0;
				int cp = ContFindCommonPoint(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
				if(cp != 1){
					ContFindCommonPointSaveParam(a, b, a2w, b2w, dir, -DBL_MAX, 1, normal, pa, pb, dist);
					DSTR << "ContFindCommonPoint do not find contact point" << std::endl;
				}
			}
			if(!esolids[i]->flag.blocal){																			// ���߂čŋߖT���̂ɂȂ�����
				esolids[i]->flag.bfirstlocal = true;
				esolids[i]->haSolid = *DCAST(PHSolid, esolids[i]->phSolidIf);						// �V�[���������̂̒��g��͊o�v���Z�X�Ŏg�����́i���́j�Ƃ��ăR�s�[����
				esolids[i]->syncInfo.neighborPoint.face_normal = normal;							// ���߂ċߖT���̂ɂȂ����̂ŁC�O��̖@���ɍ���ł����@�����㏑������D										
#ifdef _DEBUG
				if (esolids[i]->syncInfo.neighborPoint.face_normal * normal < 0.8){
					DSTR << "Too big change on normal" << normal << std::endl;
				}
#endif
			}
			esolids[i]->flag.blocal = true;																		// �ߖT���̂Ȃ̂�blocal��true�ɂ���
			esolids[i]->syncInfo.neighborPoint.closestPoint = pa;										// ���̋ߖT�_�̃��[�J�����W
			esolids[i]->syncInfo.neighborPoint.pointerPoint = pb;										// �͊o�|�C���^�ߖT�_�̃��[�J�����W
			esolids[i]->syncInfo.neighborPoint.last_face_normal											// �@����Ԃ̂��߂ɑO��̖@��������Ă���
				= esolids[i]->syncInfo.neighborPoint.face_normal;										// ���߂ċߖT�ɂȂ������͍���ł����@��
			esolids[i]->syncInfo.neighborPoint.face_normal = normal;								// ���̂���͊o�|�C���^�ւ̖@��
		}else{
			//GJK�̌��ʁC�ߖT�_������localRange�����傫���ꍇ
			esolids[i]->flag.blocal = false;																		// �ߖT���̂ł͂Ȃ��̂�blocal��false�ɂ���
			esolids[i]->flag.bfirstlocal = false;
		}
	}
}

void FWMultiRateHapticApp::UpdateHapticPointer(){	
	PHSolid reproHapticPointer = GetHapticProcess()->GetHapticPointer();
	GetHapticPointer()->SetFramePosition(reproHapticPointer.GetFramePosition());				
//	GetHapticPointer()->SetFramePosition(cameraInfo.view.Rot() * reproHapticPointer.GetFramePosition());		// (������)cameraInfo.view.Rot()�������ė͊o�|�C���^�̑�����J��������]�ɂ��킹��(*�͊o���l���Ȃ��Ƃ�����)			
	GetHapticPointer()->SetOrientation(reproHapticPointer.GetOrientation());					
	GetHapticPointer()->SetVelocity(reproHapticPointer.GetVelocity());
	GetHapticPointer()->SetAngularVelocity(reproHapticPointer.GetAngularVelocity());	
	GetHapticPointer()->SetDynamical(false);
}

void FWMultiRateHapticApp::SyncHapticProcess(){
		FWHapticProcessBase* hprocess = GetHapticProcess();
	if(bSync){
		// �͊o�|�C���^�̓���
		UpdateHapticPointer();

		// �͊o�v���Z�X->�����v���Z�X
		// �͊o�v���Z�X�ł̋ߖT���̂̃V�~�����[�V�������ʂ𕨗��v���Z�X�ɔ��f������
		for(unsigned i = 0; i < hprocess->expandedPHSolids.size(); i++){
		// blocal��true�Cblocal���ȏꍇ�͌��ʂ𔽉f������
			if(expandedPHSolids[i].flag.blocal && !expandedPHSolids[i].flag.bfirstlocal){
				SpatialVector b = (expandedPHSolids[i].syncInfo.motionCoeff.b + 
					(expandedPHSolids[i].syncInfo.motionCoeff.curb - expandedPHSolids[i].syncInfo.motionCoeff.lastb)) *  GetPhysicTimeStep();
				Vec3d v = hprocess->expandedPHSolids[i].haSolid.GetVelocity() + b.v();
				hprocess->expandedPHSolids[i].phSolidIf->SetVelocity(v);
				Vec3d w = hprocess->expandedPHSolids[i].haSolid.GetAngularVelocity() + b.w();
				hprocess->expandedPHSolids[i].phSolidIf->SetAngularVelocity(w);
				hprocess->expandedPHSolids[i].phSolidIf->SetCenterPosition(hprocess->expandedPHSolids[i].haSolid.GetCenterPosition());
				hprocess->expandedPHSolids[i].phSolidIf->SetOrientation(hprocess->expandedPHSolids[i].haSolid.GetOrientation());
				expandedPHSolids[i].syncInfo.neighborPoint.test_force_norm = hprocess->expandedPHSolids[i].syncInfo.neighborPoint.test_force_norm;
			}
		}

		// �����v���Z�X->�͊o�v���Z�X
		// �V�[���ŐV�����������ꂽ���̂��i�[
		for(int i = hprocess->expandedPHSolids.size(); i < expandedPHSolids.size(); i++){
			hprocess->expandedPHSolids.resize(hprocess->expandedPHSolids.size() + 1);
			hprocess->expandedPHSolids.back() = expandedPHSolids[i];
		}
		for(unsigned i = 0; i < hprocess->expandedPHSolids.size(); i++){
			// ���߂Ď擾�����ߖT���̂̂ݍs��
			if(expandedPHSolids[i].flag.bfirstlocal){
				hprocess->expandedPHSolids[i].haSolid = expandedPHSolids[i].haSolid;
				expandedPHSolids[i].flag.bfirstlocal = false;
			}
			hprocess->expandedPHSolids[i].flag.blocal = expandedPHSolids[i].flag.blocal;
			hprocess->expandedPHSolids[i].syncInfo = expandedPHSolids[i].syncInfo;
		}

		// �����v���Z�X�Ŏg�p���鍏�ݎ���
		//GetPHScene()->SetTimeStep(0.001f * (float)hcount);
		//dt = 0.001f * (float)hcount;
		hprocess->loopCounter = 0;

		// �����I���̃t���O
		bSync = false;
	}
#define COUNT_MAX 300
	if(hprocess->loopCounter > COUNT_MAX) {
		DSTR << "too many step" << std::endl;
		hprocess->loopCounter = 0;
		bSync = false;
	}
	hprocess->loopCounter++;
	hapticcount++;
}

}