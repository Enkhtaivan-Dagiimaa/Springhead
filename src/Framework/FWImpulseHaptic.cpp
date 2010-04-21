/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include "FWImpulseHaptic.h"

// FWImpulseHaptic�̎���
///////////////////////////////////////////////
FWImpulseHapticLoop::FWImpulseHapticLoop(){}
void FWImpulseHapticLoop::Step(){
	UpdateInterface();
	switch(hmode){
		case PENALTY3D:
			HapticRendering();
			break;
		case CONSTRAINT:
			ConstraintBasedRendering();
			break;
		default:
			HapticRendering();
			break;
	}
}

void FWImpulseHapticLoop::HapticRendering(){
	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();

		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			ToHaptic* th = &iInfo->toHaptic;
			ToPhysic* tp = &iInfo->toPhysic;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Vec3d cPoint = cSolid->GetPose() * th->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * th->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;										// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			Vec3d interpolation_cPoint;

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * th->face_normal + 
				(syncCount - (double)loopCount) * th->last_face_normal) / syncCount;															

			// �����J�E���g���z�����猻�݂̖@���C�ڐG�_���g��
			if(loopCount > syncCount){
				interpolation_normal = th->face_normal;
				force_dir = pPoint - cPoint;
			}

			double f = force_dir * interpolation_normal;		// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			if(f < 0.0){										// ���ς����Ȃ�͂��v�Z
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->correctionSpringK;
				double D = iPointer->correctionDamperD;
				Vec3d addforce = -1 * (K * ortho + D * dvortho);
		
				double ws4 = pow(iPointer->GetWorldScale(), 4);
				outForce.v() += addforce / ws4;	
				outForce.w() = Vec3d();

				/// �v�Z�����͂����̂ɉ�����//
				tp->impulse = -1 * addforce;	
			}
		}
		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());
	}
}

void FWImpulseHapticLoop::ConstraintBasedRendering(){}


// FWImpulseHaptic�̎���
///////////////////////////////////////////////
FWImpulseHaptic::FWImpulseHaptic(){
	hapticLoop = &impulseLoop;
}

void FWImpulseHaptic::Init(){
	FWMultiRateHaptic::Init();
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	InitIAAdaptee();
	hapticLoop->Init(pdt, hdt);
}

void FWImpulseHaptic::Clear(){
	FWMultiRateHaptic::Clear();
}

void FWImpulseHaptic::CallBackHapticLoop(){
	GetHapticLoop()->Step();
	Sync();
}

void FWImpulseHaptic::SyncHaptic2Physic(){
	/// HapticLoop--->PhysicsLoop
	double hdt = GetHapticLoop()->hdt;
	int lc = GetHapticLoop()->GetLoopCount(); 
	for(int i = 0; i < GetHapticLoop()->NIAPointers(); i++){
		FWInteractPointer* hip = GetHapticLoop()->GetIAPointer(i);

		FWInteractSolids* hiss = GetHapticLoop()->GetIASolids();
		for(int j = 0; j < (int)hiss->size(); j++){
			GetIAPointer(i)->interactInfo[j].toPhysic = hip->interactInfo[j].toPhysic;

			FWInteractInfo* iInfo = &hip->interactInfo[j];
			// bSim = ture ���� bfirstSim = false�Ȃ猋�ʂ𔽉f������
			if(!iInfo->flag.blocal || iInfo->flag.bfirstlocal) continue;
			Vec3d cPoint = iInfo->toHaptic.pose * iInfo->toHaptic.closest_point;	// �ߖT���̂̐ڐG�_
			Vec3d force = iInfo->toPhysic.impulse / (lc * hdt);							// �ߖT���̂ɉ������
			DSTR << force << std::endl;
			iInfo->toPhysic.impulse = Vec3d();											// �ߖT���̂ɉ����͐ς̏�����
			PHSolid* ps = GetIASolid(j)->sceneSolid;
			ps->AddForce(force, cPoint);						// �ߖT���̂̐ڐG�_�ɗ͊o�|�C���^����̗͂�������
		}
	}
}

void FWImpulseHaptic::SyncPhysic2Haptic(){
	/// PhysicsLoop--->HapticLoop ///
	// 1. �V�[���ŐV�����������ꂽ�����g��
	// 1.1. �͊o�|�C���^�̑�����
	std::vector<FWInteractPointer>* hips= GetHapticLoop()->GetIAPointers();
	for(int i = (int)hips->size(); i < NIAPointers(); i++){
		hips->resize(i+1);
		hips->back() = *GetIAPointer(i);
		hips->back().Sync();
	}
	// 1.2. Solid�̑�����
	FWInteractSolids* hiss = GetHapticLoop()->GetIASolids();
	for(int i = (int)hiss->size(); i < (int)NIASolids(); i++){
		hiss->resize(i + 1);
		hiss->back() = *GetIASolid(i);
		/// �|�C���^�������ɂ��Ă��g��
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* hip = GetHapticLoop()->GetIAPointer(j);
			hip->interactInfo.resize(i + 1);
			hip->interactInfo.back() = GetIAPointer(j)->interactInfo[i];
		}
	}
	// 2. ���̓���
	for(unsigned i = 0; i < hiss->size(); i++){
		FWInteractSolid* pis = GetIASolid(i);
		FWInteractSolid* his = GetHapticLoop()->GetIASolid(i);

		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* hip = GetHapticLoop()->GetIAPointer(j)->Cast();
			hip->interactInfo[i].toHaptic = GetIAPointer(j)->interactInfo[i].toHaptic;
			hip->interactInfo[i].flag = GetIAPointer(j)->interactInfo[i].flag;
			hip->bForce = GetIAPointer(j)->bForce;
			hip->bVibration = GetIAPointer(j)->bVibration;
		}
	}
}

void FWImpulseHaptic::Step(){
	if (bSync) return;
	if (bCalcPhys){
		UpdatePointer();
		GetPHScene()->Step();
		UpdateSolidList();
		NeighborObjectFromPointer();
		bCalcPhys = false;
	}
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;	
}

void FWImpulseHaptic::UpdatePointer(){
	for(int i = 0; i < NIAPointers(); i++){	
		if(GetHapticLoop()->NIAPointers() == 0) return; 
		PHSolidIf* soPointer = GetIAPointer(i)->pointerSolid;
		FWInteractPointer* hiPointer = GetHapticLoop()->GetIAPointer(i)->Cast();
		PHSolid* hiSolid = &hiPointer->hiSolid;
		soPointer->SetVelocity(hiSolid->GetVelocity());
		soPointer->SetAngularVelocity(hiSolid->GetAngularVelocity());
		soPointer->SetFramePosition(hiSolid->GetFramePosition());
		soPointer->SetOrientation(hiSolid->GetOrientation());
		soPointer->SetDynamical(false);
	}
}

void FWImpulseHaptic::BeginKeyboard(){}
void FWImpulseHaptic::EndKeyboard(){}