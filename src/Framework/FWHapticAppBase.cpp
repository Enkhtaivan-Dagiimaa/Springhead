/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWHapticAppBase.h>
#include <Framework/FWExpandedPHSolid.h>
#include <Physics/PHConstraintEngine.h>
#include <GL/glut.h>
#include <sstream>

#ifdef USE_HDRSTOP
#pragma  hdrstop
#endif

namespace Spr{;

FWHapticAppBase::FWHapticAppBase(){
	localRange = 0.5;
	bStep = true;
	bOneStep = false;
	bDebug = false;
}

void FWHapticAppBase::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);
	GetSdk()->Clear();

	FWWinDesc windowDesc;
	windowDesc.title = "(c)Springhead2    FWHapticApp";
	FWWin* window = CreateWin(windowDesc);
	window->scene = GetSdk()->GetScene();
}

void FWHapticAppBase::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWHapticAppBase::Start(){
	instance = this;
	if (!NWin()){
		CreateWin();
		wins.back()->SetScene(GetSdk()->GetScene());
	}
//	glutTimerFunc(1, FWAppGLUT::GlutTimerFunc, 0);
	glutIdleFunc(FWAppGLUT::GlutIdleFunc);
	glutMainLoop();
}

void FWHapticAppBase::Display(){
	FWWin* window = GetCurrentWin();
	window->SetScene(GetSdk()->GetScene());
	GRCameraIf* cam = window->scene->GetGRScene()->GetCamera();
	GRDebugRenderIf* render = window->render->Cast();

	// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);	
	render->SetRenderMode(true, false);
//	render->EnableRenderAxis(bDebug);
	render->EnableRenderForce(bDebug);
	render->EnableRenderContact(false);			// �ڐG��Ԃ͊��荞�ݏ����œ��I�ɕω�����̂ŏ��false

	// �J�������W�̎w��
	if(cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		window->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!window) return;
	GRRenderIf*curRender =  window->GetRender();
	FWSceneIf* curScene = window->GetScene();
	GetSdk()->SwitchRender(curRender);
	GetSdk()->SwitchScene(curScene);
	if(!curRender) return;
	curRender->ClearBuffer();
	curRender->BeginScene();
	if (curScene) curScene->Draw(curRender, GetSdk()->GetDebugMode());	// �V�[����`��
	DebugDisplay(render);																			// �f�o�b�N�\�����[�h
	curRender->EndScene();
 
	glutSwapBuffers(); 
}

void FWHapticAppBase::DebugDisplay(GRDebugRenderIf* render){
	if(!bDebug) return;
	GRLightDesc ld;
	ld.diffuse = Vec4f(1,1,1,1) * 0.8f;
	ld.specular = Vec4f(1,1,1,1) * 0.8f;
	ld.ambient = Vec4f(1,1,1,1) * 0.4f;
	ld.position = Vec4f(1,1,1,0);
	render->PushLight(ld);// �����̒ǉ�
	// �����Ƀf�o�b�N�\�����[�h�ŕ`�悵�������̂�����
	//
	render->PopLight();	//	�����̍폜
}

FWExpandedPHSolid** FWHapticAppBase::ExpandPHSolidInfo(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidIf** solids = phscene->GetSolids();
	for(int i = (int)expandedPHSolids.size(); i < phscene->NSolids(); i++){
		expandedPHSolids.resize(i + 1);
		expandedPHSolids.back().phSolidIf = solids[i];
	}
	return expandedPHSolids.empty() ? NULL : (FWExpandedPHSolid**)&*expandedPHSolids.begin();
}

void FWHapticAppBase::FindNearestObjectFromHapticPointer(PHSolidIf* hPointer){
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
		DCAST(PHSolid, esolids[i]->phSolidIf)->GetBBoxSupport(dir, eit[0].edge, eit[1].edge);
		Vec3d dPos = esolids[i]->phSolidIf->GetDeltaPosition();
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

void FWHapticAppBase::UpdateHapticPointer(PHSolidIf* hPointer, PHSolid hInterface){
	hPointer->SetFramePosition(hInterface.GetFramePosition());				
//	hPointer->SetFramePosition(cameraInfo.view.Rot() * phpointer.GetFramePosition());		// (������)cameraInfo.view.Rot()�������ė͊o�|�C���^�̑�����J��������]�ɂ��킹��(*�͊o���l���Ȃ��Ƃ�����)			
	hPointer->SetOrientation(hInterface.GetOrientation());					
	hPointer->SetVelocity(hInterface.GetVelocity());
	hPointer->SetAngularVelocity(hInterface.GetAngularVelocity());	
	hPointer->SetDynamical(false);
}	

// protected�ϐ��ւ̃A�N�Z�X
double FWHapticAppBase::GetHapticTimeStep(){
	return hapticTimeStep;
}

double FWHapticAppBase::GetPhysicTimeStep(){
	return physicTimeStep;
}

PHSolid FWHapticAppBase::GetHapticInterface(){
	return hapticInterface;
}

PHSolidIf* FWHapticAppBase::GetHapticPointer(){
	return hapticPointer;
}

FWExpandedPHSolid** FWHapticAppBase::GetFWExpandedPHSolids(){
	return expandedPHSolids.empty() ? NULL : (FWExpandedPHSolid**)&*expandedPHSolids.begin();
}

int FWHapticAppBase::GetNExpandedPHSolids(){
	return (int)expandedPHSolids.size();
}

// �t���O��؂�ւ��邽�߂̊֐�
void FWHapticAppBase::SetDebugMode(bool bD){
	bDebug = bD;
}

bool FWHapticAppBase::GetDebugMode(){
	return bDebug;
}

}