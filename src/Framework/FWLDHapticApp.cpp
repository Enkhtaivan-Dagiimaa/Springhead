/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWLDHapticApp.h>

void FWLDHapticProcess::Step(){}

void FWLDHapticProcess::LocalDynamics(){
	FWExpandedPHSolid** esolids = GetFWExpandedPHSoilds();
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

FWExpandedPHSolid** FWLDHapticProcess::GetFWExpandedPHSoilds(){
	return expandedPHSolids.empty() ? NULL : (FWExpandedPHSolid**)&*expandedPHSolids.begin();
}

int FWLDHapticProcess::GetNExpandedPHSolids(){
	return expandedPHSolids.size();
}

void FWLDHapticApp::CallBack(){
	if(hprocess.GetLoopCount() > 300) return;
	hprocess.Step();
	SyncHapticProcess();
}
/*
void FWLDHapticApp::Step(){
	if (bsync) return;
	if (calcPhys){
		UpdateHapticPointer();
		vector<SpatialVector> lastvel;
		for(unsigned int i = 0; i < expandedObjects.size(); i++){
			if(!expandedObjects[i].flag.blocal) continue;
			lastvel.resize(i + 1);
			lastvel.back().v() = expandedObjects[i].phSolidIf->GetVelocity();
			lastvel.back().w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		}
		if(bStep) GetPHScene()->Step();
		else if (bOneStep){
			GetPHScene()->Step();
			bOneStep = false;
		}

		for(unsigned i = 0; i < expandedObjects.size(); i++){
			if(!expandedObjects[i].flag.blocal) continue;
			SpatialVector curvel;
			curvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
			curvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
			expandedObjects[i].syncInfo.motionCoeff.curb = (curvel - lastvel[i]) / pdt;
		}
		ExpandSolidInfo();
		FindNearestObject();	// �ߖT���̂̎擾
		PredictSimulation();
		calcPhys = false;
	}
	if (hapticcount < pdt/hdt) return;
	hapticcount -= pdt/hdt;
	bsync = true;
	calcPhys = true;
}*/
/*
void TFWLDHapticApp::TestSimulation(){
	// neighborObjets��blocal��true�̕��̂ɑ΂��ĒP�ʗ͂������C�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC�^���W�����v�Z����
#ifdef DIVIDE_STEP
	states2->SaveState(GetPHScene());			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	//	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	GetPHScene()->ClearForce();
	GetPHScene()->GenerateForce();
	GetPHScene()->IntegratePart1();
#endif
	states->SaveState(GetPHScene());			// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����

	for(unsigned i = 0; i < expandedObjects.size(); i++){
		if(!expandedObjects[i].flag.blocal) continue;
		
		// ���݂̑��x��ۑ�
		SpatialVector currentvel, nextvel; 
		currentvel.v() = expandedObjects[i].phSolidIf->GetVelocity();											// ���݂̑��x
		currentvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();									// ���݂̊p���x									
		Vec3d cPoint = expandedObjects[i].phSolidIf->GetPose() * expandedObjects[i].syncInfo.neighborPoint.closestPoint;	// �͂�������_
		const float minTestForce = 0.5;
		if(expandedObjects[i].syncInfo.neighborPoint.test_force_norm < minTestForce){
			expandedObjects[i].syncInfo.neighborPoint.test_force_norm = minTestForce;		// �e�X�g�͂�0�Ȃ�1�ɂ��� 
		}

		// �S�����W�n����邽�߂̏���
		Vec3d rpjabs, vpjabs;
		rpjabs = cPoint - soPointer->GetCenterPosition();																							//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
		vpjabs = soPointer->GetVelocity() + soPointer->GetAngularVelocity() % rpjabs;													//�ڐG�_�ł̑��x
		Vec3d rjabs, vjabs;
		rjabs = cPoint - expandedObjects[i].phSolidIf->GetCenterPosition();																	//���̂̒��S����ڐG�_�܂ł̃x�N�g��
		vjabs = expandedObjects[i].phSolidIf->GetVelocity() + expandedObjects[i].phSolidIf->GetAngularVelocity() % rjabs;	//�ڐG�_�ł̑��x

		//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
		Vec3d n, t[2], vjrel, vjrelproj;
		n = -expandedObjects[i].syncInfo.neighborPoint.face_normal;
		vjrel = vjabs - vpjabs;										// ���Α��x
		vjrelproj = vjrel - (n * vjrel) * n;						// ���Α��x�x�N�g����@���ɒ������镽�ʂɎˉe�����x�N�g��
		double vjrelproj_norm = vjrelproj.norm();
		if(vjrelproj_norm < 1.0e-10){							// �ˉe�x�N�g���̃m�������������Ƃ�
			t[0] = n % Vec3d(1.0, 0.0, 0.0);					// t[0]��@����Vec3d(1.0, 0.0, 0.0)�̊O�ςƂ���
			if(t[0].norm() < 1.0e-10)								// ����ł��m������������������
				t[0] = n % Vec3d(0.0, 1.0, 0.0);				// t[0]��@����Vec3d(0.0, 1.0, 0.0)�̊O�ςƂ���
			t[0].unitize();												// t[0]��P�ʃx�N�g���ɂ���
		}
		else{
			t[0] = vjrelproj / vjrelproj_norm;					// �m�������������Ȃ�������C�ˉe�x�N�g���̂܂�
		}
		t[1] = n % t[0];												// t[1]�͖@����t[0]�̊O�ςł��܂�

		// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
#ifdef DIVIDE_STEP
		GetPHScene()->IntegratePart2();
#else
		GetPHScene()->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		expandedObjects[i].syncInfo.motionCoeff.lastb = expandedObjects[i].syncInfo.motionCoeff.b;
		expandedObjects[i].syncInfo.motionCoeff.b = (nextvel - currentvel) / pdt;

		TMatrixRow<6, 3, double> u;
		TMatrixRow<3, 3, double> force;
		// �@�������ɗ͂�������
		states->LoadState(GetPHScene());
		force.col(0) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * n;
		expandedObjects[i].phSolidIf->AddForce(force.col(0), cPoint);
#ifdef DIVIDE_STEP
		GetPHScene()->IntegratePart2();
#else
		GetPHScene()->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(0) = (nextvel - currentvel) /pdt - expandedObjects[i].syncInfo.motionCoeff.b;

		// n + t[0]�����ɗ͂�������
		states->LoadState(GetPHScene());
		force.col(1) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * (n + t[0]);
		expandedObjects[i].phSolidIf->AddForce(force.col(1), cPoint);
		GetPHScene()->IntegratePart2();
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(1) = (nextvel - currentvel) /pdt - expandedObjects[i].syncInfo.motionCoeff.b;

		// n+t[1]�����͂�������
		states->LoadState(GetPHScene());
		force.col(2) = expandedObjects[i].syncInfo.neighborPoint.test_force_norm * (n + t[1]);
		expandedObjects[i].phSolidIf->AddForce(force.col(2), cPoint);
#ifdef DIVIDE_STEP
		GetPHScene()->IntegratePart2();
#else
		GetPHScene()->Step();
#endif
		nextvel.v() = expandedObjects[i].phSolidIf->GetVelocity();
		nextvel.w() = expandedObjects[i].phSolidIf->GetAngularVelocity();
		u.col(2) = (nextvel - currentvel) /pdt - expandedObjects[i].syncInfo.motionCoeff.b;
		
		expandedObjects[i].syncInfo.motionCoeff.A = u  * force.inv();				// �^���W��A�̌v�Z
		states->LoadState(GetPHScene());								// ����state�ɖ߂��V�~�����[�V������i�߂�
	}
#ifdef DIVIDE_STEP
		states2->LoadState(GetPHScene());								// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
}*/