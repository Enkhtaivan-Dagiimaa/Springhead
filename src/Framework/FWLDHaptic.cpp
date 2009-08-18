/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWLDHaptic.h>
#include <Framework/SprFWApp.h>

// FWLDHapticLoop�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHapticLoop::FWLDHapticLoop(){}
void FWLDHapticLoop::Step(){
	UpdateInterface();
	HapticRendering();
	LocalDynamics();
}

void FWLDHapticLoop::UpdateInterface(){
	int N = NInteractPointers();
	for(int i = 0; i < N; i++){
		FWInteractPointer* iPointer =GetInteractPointer(i)->Cast();
		HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		double s = iPointer->GetPosScale();
		hif->Update((float)hdt);
		PHSolid* hiSolid = &iPointer->hiSolid;
		hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
		hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
		hiSolid->SetFramePosition((Vec3d)hif->GetPosition() * s);
		hiSolid->SetOrientation(hif->GetOrientation());
	}
}


void FWLDHapticLoop::HapticRendering(){
	for(int j = 0; j < NInteractPointers(); j++){
		FWInteractPointer* iPointer = GetInteractPointer(j)->Cast();
		HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		static double vibT = 0;
		static bool vibFlag = false;
		Vec3d vibV = (Vec3d)hif->GetVelocity() * iPointer->GetPosScale();
		static Vec3d vibVo = vibV;
		double vibforce = 0;
		static Vec3d proxy[100];
		bool noContact = true;

		SpatialVector outForce = SpatialVector();
		for(int i = 0; i < NInteractSolids(); i++){
			FWInteractSolid* iSolid = GetInteractSolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;

			double f = force_dir * interpolation_normal;		// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			if(f < 0.0){										// ���ς����Ȃ�͂��v�Z
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv = cSolid->GetPointVelocity(cPoint) - iPointer->hiSolid.GetPointVelocity(pPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;
				
				static Vec3d addforce = Vec3d(0,0,0);
				double K = iPointer->springK;
				double D = iPointer->damperD;
	//			if(!bproxy){
					addforce = -K * ortho + D * dvortho;// * ortho.norm();								// �񎦗͌v�Z (*�_���p�̍���ortho�̃m�����������Ă݂�)
	//			}else{
	//				addforce = -K * (pPoint - (proxy[i]+phSolid->GetCenterPosition())) + D * dvortho;	// �񎦗͌v�Z(proxy)
	//			}
				//Vec3d addtorque = (pPoint - hpointer.GetCenterPosition()) % addforce ;

				if(!vibFlag){
					vibT = 0;
					vibVo = vibV - cSolid->GetVelocity() ;
				}
				vibFlag = true;
	//			if(vhaptic){
					double vibA = cSolid->GetShape(0)->GetVibA();
					double vibB = cSolid->GetShape(0)->GetVibB();
					double vibW = cSolid->GetShape(0)->GetVibW();
					vibforce = vibA * (vibVo * 0.003 * addforce.unit()) * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT);	//�U���v�Z
	//			}			

				// proxy�@�ł̖��C�̌v�Z
				Vec3d posVec = pPoint - (proxy[i] + cSolid->GetCenterPosition());
				double posDot = dot(nInfo->face_normal,posVec);
				Vec3d tVec = posDot * nInfo->face_normal;
				Vec3d tanjent = posVec - tVec;
				double mu0 = cSolid->GetShape(0)->GetStaticFriction();
				double mu1 = cSolid->GetShape(0)->GetDynamicFriction();
				if(tanjent.norm() > abs(mu0 * posDot)){
					proxy[i] += (tanjent.norm() - abs(mu1 * posDot)) * tanjent.unit();
	//				proxyPos += (tanjent.norm() - abs(1.0 * posDot)) * tanjent.unit();
				}
				outForce.v() += addforce + (vibforce * addforce.unit());	// ���[�U�ւ̒񎦗�		
	//			outForce.w.() += addtorque;										 
				iPointer->interactInfo[i].mobility.force = -1 * addforce;						// �v�Z�����͂����̂ɉ�����
				nInfo->test_force_norm = addforce.norm();
				noContact = false;
			}
		}
		if (noContact) vibFlag = false;
		vibT += hdt;
		#ifdef TORQUE
		if(bDisplayforce) fInterface->SetForce(displayforce, displaytorque);
		#else
	//	if(bDisplayforce) 
//			hif->SetForce(Vec3f(), Vec3f());			// ���U���|���̂łƂ肠�����o�͂Ȃ��ŁC���ƂŃt���O������܂�
			hif->SetForce(outForce.v(), Vec3d());					
		#endif
	}
}

void FWLDHapticLoop::LocalDynamics(){
	for(int i = 0; i < NInteractSolids(); i++){
		FWInteractSolid* iSolid = FWHapticLoopBase::GetInteractSolid(i);
		if(!iSolid->bSim) continue;
		SpatialVector vel;
		vel.v() = iSolid->copiedSolid.GetVelocity();
		vel.w() = iSolid->copiedSolid.GetAngularVelocity();
		if(GetLoopCount() == 1) vel += (iSolid->curb - iSolid->lastb) *  pdt;	// �Փ˂̉e���𔽉f
		for(int j = 0; j < NInteractPointers(); j++){
			FWInteractPointer* iPointer = GetInteractPointer(j);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			vel += (iInfo->mobility.A * iInfo->mobility.force) * hdt;			// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�

			iInfo->mobility.force = Vec3d();
		}
		vel += iSolid->b * hdt;
		iSolid->copiedSolid.SetVelocity(vel.v());																		
		iSolid->copiedSolid.SetAngularVelocity(vel.w());
		iSolid->copiedSolid.SetCenterPosition(iSolid->copiedSolid.GetCenterPosition() + vel.v() * hdt);
		iSolid->copiedSolid.SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * iSolid->copiedSolid.GetOrientation()).unit());
 		iSolid->copiedSolid.SetUpdated(true);
		iSolid->copiedSolid.Step();
	}
}

// FWLDHapticApp�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic::FWLDHaptic(){}
void FWLDHaptic::Init(){
	FWMultiRateHaptic::Init();
	hapticLoop = &LDHapticLoop;
	double pdt = GetInteractScene()->GetScene()->GetPHScene()->GetTimeStep();
	double hdt = GetInteractScene()->hdt;
	hapticLoop->Init(pdt, hdt);
	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();
}
void FWLDHaptic::Clear(){
	FWMultiRateHaptic::Clear();
	states->Clear();
	states2->Clear();
}
void FWLDHaptic::CallBackHapticLoop(){
	GetHapticLoop()->Step();
	Sync();
}
void FWLDHaptic::Step(){
	if (bSync) return;
	if (bCalcPhys){
		UpdatePointer();
		PhysicsStep();
		UpdateSolidList();
		NeighborObjectFromPointer();
		TestSimulation();
		bCalcPhys = false;
	}
	double pdt = GetInteractScene()->GetScene()->GetPHScene()->GetTimeStep();
	double hdt = GetInteractScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}
void FWLDHaptic::PhysicsStep(){
	FWInteractSolids* iSolids = GetInteractSolids();
	std::vector<SpatialVector> lastvel;
	for(int i = 0; i < (int)iSolids->size(); i++){
		FWInteractSolid* iSolid = &iSolids->at(i);
		if(!iSolid->bSim) continue;
		lastvel.resize(i + 1);
		lastvel.back().v() = iSolid->sceneSolid->GetVelocity();
		lastvel.back().w() = iSolid->sceneSolid->GetAngularVelocity();
	}
//	if(bStep) GetFWApp()->GeSdk()->GetScene()->GetPHScene()->Step();
//	else if (bOneStep){
		GetInteractScene()->GetScene()->GetPHScene()->Step();
//		bOneStep = false;
//	}
	for(int i = 0; i < (int)iSolids->size(); i++){
		FWInteractSolid* iSolid = &iSolids->at(i);
		if(!iSolid->bSim) continue;
		SpatialVector curvel;
		curvel.v() = iSolid->sceneSolid->GetVelocity();
		curvel.w() = iSolid->sceneSolid->GetAngularVelocity();
		double pdt = GetInteractScene()->GetScene()->GetPHScene()->GetTimeStep();
		iSolid->curb = (curvel - lastvel[i]) / pdt;
	}
}
void FWLDHaptic::UpdatePointer(){
	int N = GetInteractScene()->NInteractPointers();
	for(int i = 0; i < N; i++){	
		if(GetHapticLoop()->NInteractPointers() == 0) return; 
		FWInteractPointer* piPointer = GetInteractScene()->GetInteractPointer(i)->Cast();
		FWInteractPointer* hiPointer = GetHapticLoop()->GetInteractPointer(i)->Cast();
		PHSolidIf* soPointer = piPointer->pointerSolid;
		PHSolid* hiSolid = &hiPointer->hiSolid;
		soPointer->SetVelocity(hiSolid->GetVelocity());
		soPointer->SetAngularVelocity(hiSolid->GetAngularVelocity());
		soPointer->SetFramePosition(hiSolid->GetFramePosition());
		soPointer->SetOrientation(hiSolid->GetOrientation());
		soPointer->SetDynamical(false);
	}
}
void FWLDHaptic::TestSimulation(){
	/** FWInteractSolids��blocal��true�̕��̂ɑ΂��ăe�X�g�͂������C
		�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC���r���e�B���v�Z���� */
	PHSceneIf* phScene = GetInteractScene()->GetScene()->GetPHScene();

	#ifdef DIVIDE_STEP
	/// �e�X�g�V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	states2->SaveState(phScene);		
	///	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phScene->ClearForce();
	phScene->GenerateForce();
	phScene->IntegratePart1();
	#endif
	/// �e�X�g�V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	states->SaveState(phScene);			

	/// �e�X�g�V�~�����[�V�������s
	FWInteractSolids* iSolids = GetInteractSolids();
	for(int i = 0; i < (int)iSolids->size(); i++){
		FWInteractSolid* iSolid = GetInteractSolid(i);
		PHSolid* phSolid = iSolid->sceneSolid;
		if(!iSolid->bSim) continue;

		/// ���݂̑��x��ۑ�
		SpatialVector curvel, nextvel; 
		curvel.v() = phSolid->GetVelocity();			// ���݂̑��x
		curvel.w() = phSolid->GetAngularVelocity();		// ���݂̊p���x									

		/// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
		#ifdef DIVIDE_STEP
		phScene->IntegratePart2();
		#else
		phScene->Step();
		#endif
		nextvel.v() = phSolid->GetVelocity();
		nextvel.w() = phSolid->GetAngularVelocity();
		/// ���r���e�Bb�̎Z�o
		iSolid->lastb = iSolid->b;
		double pdt = phScene->GetTimeStep();
		iSolid->b = (nextvel - curvel) / pdt;
		states->LoadState(phScene);						// ���݂̏�Ԃɖ߂�

		/// InteractPointer�̐������͂�������e�X�g�V�~�����[�V�������s��
		int N = GetInteractScene()->NInteractPointers();
		for(int j = 0; j < N; j++){
			FWInteractPointer* iPointer = GetInteractScene()->GetInteractPointer(j)->Cast();
			PHSolidIf* soPointer = iPointer->pointerSolid;
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			Vec3d cPoint = phSolid->GetPose() * iInfo->neighborInfo.closest_point;		// �͂�������_(���[���h���W)

			/// �S�����W�n����邽�߂̏���
			Vec3d rpjabs, vpjabs;
			rpjabs = cPoint - soPointer->GetCenterPosition();									//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
			vpjabs = soPointer->GetVelocity() + soPointer->GetAngularVelocity() % rpjabs;		//�ڐG�_�ł̑��x
			Vec3d rjabs, vjabs;
			rjabs = cPoint - phSolid->GetCenterPosition();										//���̂̒��S����ڐG�_�܂ł̃x�N�g��
			vjabs = phSolid->GetVelocity() + phSolid->GetAngularVelocity() % rjabs;				//�ڐG�_�ł̑��x

			/// �S�����W�ϊ��s��̌v�Z
			Vec3d n, t[2], vjrel, vjrelproj;						//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
			n = -1* iInfo->neighborInfo.face_normal;
			vjrel = vjabs - vpjabs;									// ���Α��x
			vjrelproj = vjrel - (n * vjrel) * n;					// ���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
			double vjrelproj_norm = vjrelproj.norm();
			if(vjrelproj_norm < 1.0e-10){							// �ˉe�x�N�g���̃m�������������Ƃ�
				t[0] = n % Vec3d(1.0, 0.0, 0.0);					// t[0]��@����Vec3d(1.0, 0.0, 0.0)�̊O�ςƂ���
				if(t[0].norm() < 1.0e-10)							// ����ł��m������������������
					t[0] = n % Vec3d(0.0, 1.0, 0.0);				// t[0]��@����Vec3d(0.0, 1.0, 0.0)�̊O�ςƂ���
				t[0].unitize();										// t[0]��P�ʃx�N�g���ɂ���
			}
			else{
				t[0] = vjrelproj / vjrelproj_norm;					// �m�������������Ȃ�������C�ˉe�x�N�g���̂܂�
			}
			t[1] = n % t[0];										// t[1]�͖@����t[0]�̊O�ςł��܂�

			/// �ڐG�_�ɉ������
			const float minTestForce = 0.5;										// �ŏ��e�X�g��
			/// �ʏ�e�X�g�͂��ŏ��e�X�g�͂������ꍇ
			if(iInfo->neighborInfo.test_force_norm < minTestForce){
				iInfo->neighborInfo.test_force_norm = minTestForce;					 
			}

			TMatrixRow<6, 3, double> u;			// ���̂̋@�B�C���s�[�_���X
			TMatrixRow<3, 3, double> force;		// �������

			/// �@�������ɗ͂�������
			force.col(0) = iInfo->neighborInfo.test_force_norm * n;
			phSolid->AddForce(force.col(0), cPoint);
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(0) = (nextvel - curvel) /pdt - iSolid->b;
			states->LoadState(phScene);

			/// n + t[0]�����ɗ͂�������
			force.col(1) = iInfo->neighborInfo.test_force_norm * (n + t[0]);
			phSolid->AddForce(force.col(1), cPoint);
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(1) = (nextvel - curvel) /pdt - iSolid->b;
			states->LoadState(phScene);

			/// n+t[1]�����͂�������
			force.col(2) = iInfo->neighborInfo.test_force_norm * (n + t[1]);
			phSolid->AddForce(force.col(2), cPoint);
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(2) = (nextvel - curvel) /pdt - iSolid->b;
			
			iInfo->mobility.A = u  * force.inv();			// ���r���e�BA�̌v�Z
			states->LoadState(phScene);								// ����state�ɖ߂��V�~�����[�V������i�߂�
		}
	}
	///--------�e�X�g�V�~�����[�V�����I��--------
	#ifdef DIVIDE_STEP
		states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
	#endif
}