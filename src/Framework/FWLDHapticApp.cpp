/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWLDHapticApp.h>

// FWLDHapticProcess�̎���
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void FWLDHapticProcess::Step(){}

void FWLDHapticProcess::LocalDynamics(){
	FWExpandedPHSolid** esolids = GetFWExpandedPHSolids();
	int Nesolids = GetNExpandedPHSolids();
	double hdt = GetHapticTimeStep();
	double pdt = GetPhysicTimeStep();

	for(int i = 0; i < Nesolids; i++){
		if(!esolids[i]->flag.blocal) continue;
		SpatialVector vel;																																					// ���̂̑��x�i���[���h���W�n�j
		vel.v() = esolids[i]->haSolid.GetVelocity();
		vel.w() = esolids[i]->haSolid.GetAngularVelocity();
		if(GetLoopCount() == 1) vel += (esolids[i]->syncInfo.motionCoeff.curb - esolids[i]->syncInfo.motionCoeff.lastb) *  pdt;	// �Փ˂̉e���𔽉f
		vel += (esolids[i]->syncInfo.motionCoeff.A * esolids[i]->haSolid.nextForce + esolids[i]->syncInfo.motionCoeff.b) * hdt;	// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
		esolids[i]->haSolid.SetVelocity(vel.v());																		
		esolids[i]->haSolid.SetAngularVelocity(vel.w());
		esolids[i]->haSolid.SetCenterPosition(esolids[i]->haSolid.GetCenterPosition() + vel.v() * hdt);
		esolids[i]->haSolid.SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * esolids[i]->haSolid.GetOrientation()).unit());
 		esolids[i]->haSolid.SetUpdated(true);
		esolids[i]->haSolid.Step();
	}
}


// FWLDHapticApp�̎���
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

FWLDHapticApp::FWLDHapticApp(){
	hapticProcess = &hprocess;
}

void FWLDHapticApp::CallBack(){
	if(hprocess.GetLoopCount() > 300) return;
	hprocess.Step();
	SyncHapticProcess();
}

void FWLDHapticApp::ResetScene(){
	expandedPHSolids.clear();
	hprocess.expandedPHSolids.clear();
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
	hprocess.ResetHapticProcess();
	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();
	PHSceneIf* phscene = GetScene()->GetPHScene();
	phscene->Clear();
	phscene->SetTimeStep(pdt);
	phscene->SetNumIteration(nIter);
	BuildScene();
}

void FWLDHapticApp::Step(){
	if (bSync) return;
	if (bCalcPhys){
		UpdateHapticPointer();
		int Nesolids = GetNExpandedPHSolids();
		FWExpandedPHSolid** esolids = GetFWExpandedPHSolids();
		std::vector<SpatialVector> lastvel;
		for(int i = 0; i < Nesolids; i++){
			if(!esolids[i]->flag.blocal) continue;
			lastvel.resize(i + 1);
			lastvel.back().v() = esolids[i]->phSolidIf->GetVelocity();
			lastvel.back().w() = esolids[i]->phSolidIf->GetAngularVelocity();
		}
		if(bStep) GetSdk()->GetScene()->GetPHScene()->Step();
		else if (bOneStep){
			GetSdk()->GetScene()->GetPHScene()->Step();
			bOneStep = false;
		}
		for(int i = 0; i < Nesolids; i++){
			if(!esolids[i]->flag.blocal) continue;
			SpatialVector curvel;
			curvel.v() = esolids[i]->phSolidIf->GetVelocity();
			curvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();
			esolids[i]->syncInfo.motionCoeff.curb = (curvel - lastvel[i]) / pdt;
		}
		ExpandPHSolidInfo();
		FindNearestObjectFromHapticPointer(GetHapticPointer());	// �ߖT���̂̎擾
		TestSimulation();
		bCalcPhys = false;
	}
	if (hapticcount < pdt/hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}


void FWLDHapticApp::TestSimulation(){
	// expandedPHSolids��blocal��true�̕��̂ɑ΂��ĒP�ʗ͂������C�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC�^���W�����v�Z����
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidIf* hpointer = GetHapticPointer();
	FWExpandedPHSolid** esolids = GetFWExpandedPHSolids(); 
	int Nesolids = GetNExpandedPHSolids();

#ifdef DIVIDE_STEP
	states2->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	//	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phscene->ClearForce();
	phscene->GenerateForce();
	phscene->IntegratePart1();
#endif
	states->SaveState(phscene);			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����

	for(int i = 0; i < Nesolids; i++){
		if(!esolids[i]->flag.blocal) continue;
		// ���݂̑��x��ۑ�
		SpatialVector currentvel, nextvel; 
		currentvel.v() = esolids[i]->phSolidIf->GetVelocity();																	// ���݂̑��x
		currentvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();														// ���݂̊p���x									
		Vec3d cPoint = esolids[i]->phSolidIf->GetPose() * esolids[i]->syncInfo.neighborPoint.closestPoint;	// �͂�������_
		const float minTestForce = 0.5;																									// �ŏ��e�X�g��
		if(esolids[i]->syncInfo.neighborPoint.test_force_norm < minTestForce){
			esolids[i]->syncInfo.neighborPoint.test_force_norm = minTestForce;											// �e�X�g�͂�0�Ȃ�1�ɂ��� 
		}

		// �S�����W�n����邽�߂̏���
		Vec3d rpjabs, vpjabs;
		rpjabs = cPoint - hpointer->GetCenterPosition();																				//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
		vpjabs = hpointer->GetVelocity() + hpointer->GetAngularVelocity() % rpjabs;										//�ڐG�_�ł̑��x
		Vec3d rjabs, vjabs;
		rjabs = cPoint - esolids[i]->phSolidIf->GetCenterPosition();																//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs = esolids[i]->phSolidIf->GetVelocity() + esolids[i]->phSolidIf->GetAngularVelocity() % rjabs;		//�ڐG�_�ł̑��x

		//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
		Vec3d n, t[2], vjrel, vjrelproj;
		n = -esolids[i]->syncInfo.neighborPoint.face_normal;
		vjrel = vjabs - vpjabs;															// ���Α��x
		vjrelproj = vjrel - (n * vjrel) * n;											// ���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
		double vjrelproj_norm = vjrelproj.norm();
		if(vjrelproj_norm < 1.0e-10){												// �ˉe�x�N�g���̃m�������������Ƃ�
			t[0] = n % Vec3d(1.0, 0.0, 0.0);										// t[0]��@����Vec3d(1.0, 0.0, 0.0)�̊O�ςƂ���
			if(t[0].norm() < 1.0e-10)													// ����ł��m������������������
				t[0] = n % Vec3d(0.0, 1.0, 0.0);									// t[0]��@����Vec3d(0.0, 1.0, 0.0)�̊O�ςƂ���
			t[0].unitize();																	// t[0]��P�ʃx�N�g���ɂ���
		}
		else{
			t[0] = vjrelproj / vjrelproj_norm;										// �m�������������Ȃ�������C�ˉe�x�N�g���̂܂�
		}
		t[1] = n % t[0];																	// t[1]�͖@����t[0]�̊O�ςł��܂�

		// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
#ifdef DIVIDE_STEP
		phscene->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = esolids[i]->phSolidIf->GetVelocity();
		nextvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();
		esolids[i]->syncInfo.motionCoeff.lastb = esolids[i]->syncInfo.motionCoeff.b;
		esolids[i]->syncInfo.motionCoeff.b = (nextvel - currentvel) / GetPhysicTimeStep();

		TMatrixRow<6, 3, double> u;
		TMatrixRow<3, 3, double> force;
		// �@�������ɗ͂�������
		states->LoadState(phscene
			);
		force.col(0) = esolids[i]->syncInfo.neighborPoint.test_force_norm * n;
		esolids[i]->phSolidIf->AddForce(force.col(0), cPoint);
#ifdef DIVIDE_STEP
		GetPHScene()->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = esolids[i]->phSolidIf->GetVelocity();
		nextvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();
		u.col(0) = (nextvel - currentvel) /GetPhysicTimeStep() - esolids[i]->syncInfo.motionCoeff.b;

		// n + t[0]�����ɗ͂�������
		states->LoadState(phscene);
		force.col(1) = esolids[i]->syncInfo.neighborPoint.test_force_norm * (n + t[0]);
		esolids[i]->phSolidIf->AddForce(force.col(1), cPoint);
		phscene->IntegratePart2();
		nextvel.v() = esolids[i]->phSolidIf->GetVelocity();
		nextvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();
		u.col(1) = (nextvel - currentvel) /GetPhysicTimeStep() - esolids[i]->syncInfo.motionCoeff.b;

		// n+t[1]�����͂�������
		states->LoadState(phscene);
		force.col(2) = esolids[i]->syncInfo.neighborPoint.test_force_norm * (n + t[1]);
		esolids[i]->phSolidIf->AddForce(force.col(2), cPoint);
#ifdef DIVIDE_STEP
		GetPHScene()->IntegratePart2();
#else
		phscene->Step();
#endif
		nextvel.v() = esolids[i]->phSolidIf->GetVelocity();
		nextvel.w() = esolids[i]->phSolidIf->GetAngularVelocity();
		u.col(2) = (nextvel - currentvel) /GetPhysicTimeStep() - esolids[i]->syncInfo.motionCoeff.b;
		
		esolids[i]->syncInfo.motionCoeff.A = u  * force.inv();				// �^���W��A�̌v�Z
		states->LoadState(phscene);													// ����state�ɖ߂��V�~�����[�V������i�߂�
	}
#ifdef DIVIDE_STEP
		states2->LoadState(GetPHScene());										// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
}