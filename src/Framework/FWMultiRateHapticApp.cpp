#include <Framework/FWMultiRateHapticApp.h>
#include <sstream>

#ifdef USE_HDRSTOP
#pragma  hdrstop
#endif

namespace Spr{;

FWMultiRateHapticApp::FWMultiRateHapticApp(){
	bSync = false;
	bCalcPhys = true;
}

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

//void FWMultiRateHapticApp::SyncHapticProcess(FWHapticBase* hprocess){
//	if(bSync){
//		// �͊o�|�C���^�̓���
//		UpdateHapticPointer(GetHapticPointer(), GetHapticInterface());
//
//		// �͊o�v���Z�X->�����v���Z�X
//		// �͊o�v���Z�X�ł̋ߖT���̂̃V�~�����[�V�������ʂ𕨗��v���Z�X�ɔ��f������
//		for(unsigned i = 0; i < hprocess.expandedObjects.size(); i++){
//		// blocal��true�Cblocal���ȏꍇ�͌��ʂ𔽉f������
//			if(expandedObjects[i].flag.blocal && !expandedObjects[i].flag.bfirstlocal){
//				SpatialVector b = (expandedObjects[i].syncInfo.motionCoeff.b + 
//					(expandedObjects[i].syncInfo.motionCoeff.curb - expandedObjects[i].syncInfo.motionCoeff.lastb)) *  pdt;
//				Vec3d v = hprocess.expandedObjects[i].phSolid.GetVelocity() + b.v();
//				hprocess.expandedObjects[i].phSolidIf->SetVelocity(v);
//				Vec3d w = hprocess.expandedObjects[i].phSolid.GetAngularVelocity() + b.w();
//				hprocess.expandedObjects[i].phSolidIf->SetAngularVelocity(w);
//				hprocess.expandedObjects[i].phSolidIf->SetCenterPosition(hprocess.expandedObjects[i].phSolid.GetCenterPosition());
//				hprocess.expandedObjects[i].phSolidIf->SetOrientation(hprocess.expandedObjects[i].phSolid.GetOrientation());
//				expandedObjects[i].syncInfo.neighborPoint.test_force_norm = hprocess.expandedObjects[i].syncInfo.neighborPoint.test_force_norm;
//			}
//		}
//
//		// �����v���Z�X->�͊o�v���Z�X
//		// �V�[���ŐV�����������ꂽ���̂��i�[
//		for(size_t i = hprocess.expandedObjects.size(); i < expandedObjects.size(); i++){
//			hprocess.expandedObjects.resize(hprocess.expandedObjects.size() + 1);
//			hprocess.expandedObjects.back() = expandedObjects[i];
//		}
//		for(unsigned i = 0; i < hprocess.expandedObjects.size(); i++){
//			// ���߂Ď擾�����ߖT���̂̂ݍs��
//			if(expandedObjects[i].flag.bfirstlocal){
//				hprocess.expandedObjects[i].phSolid = expandedObjects[i].phSolid;
//				expandedObjects[i].flag.bfirstlocal = false;
//			}
//			hprocess.expandedObjects[i].flag.blocal = expandedObjects[i].flag.blocal;
//			hprocess.expandedObjects[i].syncInfo = expandedObjects[i].syncInfo;
//		}
//
//		// �����v���Z�X�Ŏg�p���鍏�ݎ���
//		//GetPHScene()->SetTimeStep(0.001f * (float)hcount);
//		//dt = 0.001f * (float)hcount;
//		hprocess.hcount = 0;
//
//		// �����I���̃t���O
//		bsync = false;
//	}
//	if(hprocess.hcount > COUNT_MAX) {
//		DSTR << "too many step" << endl;
//		hprocess.hcount = 0;
//		bsync = false;
//	}
//	hprocess.hcount++;
//	hapticcount++;
//}



}