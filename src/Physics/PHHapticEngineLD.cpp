#include <Physics/PHHapticEngineLD.h>

namespace Spr{;
//----------------------------------------------------------------------------
// PHHapticLoopLD
void PHHapticLoopLD::Step(){
	UpdateInterface();
	HapticRendering();
	LocalDynamics();
}
void PHHapticLoopLD::HapticRendering(){
	PHHapticRenderInfo info;
	info.pointers = GetHapticPointers();
	info.hsolids = GetHapticSolids();
	info.sps = GetSolidPairsForHaptic();
	info.hdt = GetHapticTimeStep();
	info.pdt = GetPhysicsTimeStep();
	info.loopCount = loopCount;
	info.bInterpolatePose = false;
	GetHapticRender()->HapticRendering(info);
}

void PHHapticLoopLD::LocalDynamics(){
	double pdt = GetPhysicsTimeStep();
	double hdt = GetHapticTimeStep();
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* hsolid = GetHapticSolid(i);
		if(hsolid->doSim == 0) continue;
		if(hsolid->GetLocalSolid()->IsDynamical() == false) continue;
		PHSolid* localSolid = &hsolid->localSolid;
		SpatialVector vel;
		vel.v() = localSolid->GetVelocity();
		vel.w() = localSolid->GetAngularVelocity();
		if(loopCount == 1){
			vel += (hsolid->curb - hsolid->lastb) *  pdt;	// �Փ˂̉e���𔽉f
		}
		for(int j = 0; j < NHapticPointers(); j++){
			PHHapticPointer* pointer = GetHapticPointer(j);
			PHSolidPairForHaptic* sp = GetSolidPairForHaptic(i, pointer->GetPointerID());
			if(sp->inLocal == 0) continue;
			vel += (sp->A * sp->force) * hdt;			// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
		}
		vel += hsolid->b * hdt;
		localSolid->SetVelocity(vel.v());		
		localSolid->SetAngularVelocity(vel.w());
		localSolid->SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * localSolid->GetOrientation()).unit());
		localSolid->SetCenterPosition(localSolid->GetCenterPosition() + vel.v() * hdt);

 		localSolid->SetUpdated(true);
		localSolid->Step();
	}
}

//----------------------------------------------------------------------------
// PHHapticEngineLD
PHHapticEngineLD::PHHapticEngineLD(){
	hapticLoop = &hapticLoopLD;
	hapticLoop->engineImp = this;
	states = ObjectStatesIf::Create();
}

void PHHapticEngineLD::Step1(){
	lastvels.clear();
	for(int i = 0; i < NHapticSolids(); i++){
		SpatialVector vel;
		vel.v() = GetHapticSolid(i)->sceneSolid->GetVelocity();
		vel.w() = GetHapticSolid(i)->sceneSolid->GetAngularVelocity();
		lastvels.push_back(vel);
	}
}
void PHHapticEngineLD::Step2(){
	// �X�V��̑��x�A�O��̑��x������萔�����v�Z
	for(int i = 0; i < NHapticSolids(); i++){
		// �ߖT�̍��̂̂�
		if(GetHapticSolid(i)->doSim == 0) continue;
		PHSolid* solid = GetHapticSolid(i)->sceneSolid;
		SpatialVector dvel;
		dvel.v() = solid->GetVelocity();
		dvel.w() = solid->GetAngularVelocity();
		GetHapticSolid(i)->curb = (dvel - lastvels[i]) / GetPhysicsTimeStep();
	}

	engine->StartDetection();
	PredictSimulation3D();
}

/// 1��1��shape�ŁA1�_�̐ڐG�̂ݑΉ�
void PHHapticEngineLD::PredictSimulation3D(){
	engine->bPhysicStep = false;
	/** PHSolidForHaptic��dosim > 0�̕��̂ɑ΂��ăe�X�g�͂������C
		���ׂĂ̋ߖT���̂ɂ��āC�A�N�Z�������X���v�Z���� */
	PHSceneIf* phScene = engine->GetScene();

	//#define DIVIDE_STEP
	#ifdef DIVIDE_STEP
	/// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	states2->SaveState(phScene);		
	///	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phScene->ClearForce();
	phScene->GenerateForce();
	phScene->IntegratePart1();
	#endif
	/// �\���V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	phScene->GetConstraintEngine()->SetBSaveConstraints(true);
	states->Clear();
	states->SaveState(phScene);	
#if 1
	/// �e�X�g�V�~�����[�V�������s
	for(int i = 0; i < NHapticSolids(); i++){
		if(GetHapticSolid(i)->doSim == 0) continue;
		PHSolidForHaptic* hsolid = GetHapticSolid(i);
		PHSolid* phSolid = hsolid->sceneSolid;
		/// ���݂̑��x��ۑ�
		SpatialVector curvel, nextvel; 
		curvel.v() = phSolid->GetVelocity();			// ���݂̑��x
		curvel.w() = phSolid->GetAngularVelocity();		// ���݂̊p���x		

		//DSTR<<" �͂������Ȃ���1�X�e�b�v�i�߂�--------------------"<<std::endl;
		/// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
		#ifdef DIVIDE_STEP
		phScene->IntegratePart2();
		#else
		phScene->Step();
		#endif 
		nextvel.v() = phSolid->GetVelocity();
		nextvel.w() = phSolid->GetAngularVelocity();
		/// �A�N�Z�������Xb�̎Z�o

		hsolid->lastb = hsolid->b;
		double pdt = phScene->GetTimeStep();
		hsolid->b = (nextvel - curvel)/pdt;

		states->LoadState(phScene);						// ���݂̏�Ԃɖ߂�

		/// HapticPointer�̐������͂�������\���V�~�����[�V����
		for(int j = 0; j < NHapticPointers(); j++){
			PHHapticPointer* pointer = GetHapticPointer(j);
			PHSolidPairForHaptic* solidPair = GetSolidPairForHaptic(i, pointer->GetPointerID());
			if(solidPair->inLocal == 0) continue;
			PHShapePairForHaptic* sp = solidPair->shapePairs.item(0, 0);	// 1�`��̂ݑΉ�
			Vec3d cPoint = sp->shapePoseW[0] * sp->closestPoint[0];		// �͂�������_(���[���h���W)
			Vec3d normal = -1 * sp->normal;

			TMatrixRow<6, 3, double> u;			// ���̂̋@�B�C���s�[�_���X?
			TMatrixRow<3, 3, double> force;		// �������
			u.clear(0.0);
			force.clear(0.0);

			float minTestForce = 0.5;		// �ŏ��e�X�g��

			// 3�����̃e�X�g�͂�����
			Vec3d testForce;
			if(solidPair->force.norm() == 0){
				testForce = minTestForce * normal;
			}else{
				testForce = solidPair->force;
				solidPair->force = Vec3d();
			}
			// �e�X�g�͂ɑ΂��Đ��������̃x�N�g��2�{���v�Z
			Vec3d base1 = testForce.unit();
			Vec3d base2 = Vec3d(1, 0, 0) - (Vec3d(1, 0, 0) * base1) * base1;
			if (base2.norm() > 0.1){
				base2.unitize();
			}else{
				base2 = Vec3d(0, 1, 0) - (Vec3d(0, 1, 0) * base1) * base1;
				base2.unitize();
			}
			Vec3d base3 = base1 ^ base2;
#if 1
			// �����ȃx�N�g�������Ƃ߃u�����h
			force.col(0) = testForce;
			force.col(1) = force.col(0).norm() * (base1 + base2).unit();
			force.col(2) = force.col(0).norm() * (base1 + base3).unit();
#else
			// testForce���܂�3�����̃x�N�g��(�e���g�^�Ɂj
			// ���肵�Ă��Ȃ�
			Vec3d base12 = (base1 + base2).unit();	// base1, base2�Ԃ̃x�N�g��
			Vec3d base23 = (-base2 - base3).unit();	// -base2, -base3�Ԃ̃x�N�g��
			Vec3d base32 = (base3 - base2).unit();	// base3, -base2�Ԃ̃x�N�g��
			double a = testForce.norm() / ( base12 * testForce.unit());
			//DSTR << (a * base12) * testForce.unit() << "," << testForce.norm() <<  std::endl;
			force.col(0) = a * base12;
			force.col(1) = a * (base1 + base23).unit();
			force.col(2) = a * (base1 + base32).unit();
			//DSTR << force << std::endl;
			//DSTR << base12 * base23 << "," << base12 * base32 << std::endl;
			//DSTR << (base12 + base23 + base32).unit() << std::endl;

#endif

			/// �e�X�g�͂�3�����ɉ�����	
			for(int m = 0; m < 3; m++){
				phSolid->AddForce(force.col(m), cPoint);
				#ifdef DIVIDE_STEP
				phScene->IntegratePart2();
				#else
				phScene->Step();
				#endif
				nextvel.v() = phSolid->GetVelocity();
				nextvel.w() = phSolid->GetAngularVelocity();
				u.col(m) = (nextvel - curvel) / pdt - hsolid->b;
				states->LoadState(phScene);			
				//DSTR << "force.col" << m << " " << force.col(m) << force.col(m).norm() << std::endl;
			}

			solidPair->A = u  * force.inv();	// m/(Ns2)
#if 0
			DSTR << "i = " << i << std::endl;
			DSTR << "j = " << j << std::endl;
			DSTR << "f" << std::endl;
			DSTR << force << std::endl;
			DSTR << "u" << std::endl;
			DSTR << u << std::endl;
			DSTR << "b" << std::endl;
			DSTR << hsolid->b << std::endl;
			DSTR << "A" << std::endl;
			DSTR << solidPair->A << std::endl;
#endif
		}
	}
#endif
	///--------�e�X�g�V�~�����[�V�����I��--------
#ifdef DIVIDE_STEP
	states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
	engine->bPhysicStep = true;
}

void PHHapticEngineLD::SyncHaptic2Physic(){
#if 1
	// physics <------ haptic
	// PHSolidForHaptic�̓���
	// PHSolidPairForHaptic(�͊o�|�C���^�ƋߖT�̕���)�̊e����̓���
	for(int i = 0; i < hapticLoop->NHapticPointers(); i++){
		PHHapticPointer* hpointer = hapticLoop->GetHapticPointer(i);
		int hpointerID = hpointer->GetPointerID();
		int nNeighbors = hpointer->neighborSolidIDs.size();
		// �ߖT���̂ł���y�A��������
		for(int j = 0; j < nNeighbors; j++){
			int solidID = hpointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, hpointerID);
			PHSolidPairForHapticSt* hst = (PHSolidPairForHapticSt*)hpair;
			PHSolidPairForHapticSt* pst = (PHSolidPairForHapticSt*)ppair;
			*pst = *hst;	// haptic���ŕێ����Ă����ׂ����𓯊�
		}
	}
	// LocalDynamicsSimulation�̌��ʂ��V�[���ɔ��f
	for(int i = 0; i < (int)hapticLoop->NHapticSolids(); i++){
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		if(hsolid->bPointer) continue;		// �|�C���^�̏ꍇ
		if(hsolid->doSim <= 1) continue;	// �Ǐ��V�~�����[�V�����ΏۂłȂ��ꍇ

		//�A�N�Z�������X�萔���Ŕ��f���x�����
		double pdt = GetPhysicsTimeStep();
		PHSolid* localSolid = hsolid->GetLocalSolid();
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		SpatialVector b = (psolid->b + (psolid->curb - psolid->lastb)) * pdt;	// ���r���e�B�萔��
		Vec3d v = localSolid->GetVelocity() + b.v();			// ���f���x
		Vec3d w = localSolid->GetAngularVelocity() + b.w();		// ���f�p���x

		// ��Ԃ̔��f
		PHSolid* sceneSolid = hsolid->sceneSolid;
		Vec3d svel = sceneSolid->GetVelocity();
		Vec3d spos = sceneSolid->GetFramePosition();
		sceneSolid->SetVelocity(v);
		sceneSolid->SetAngularVelocity(w);
		sceneSolid->SetPose(localSolid->GetPose());

		PHSolidForHapticSt* hst = (PHSolidForHapticSt*)hsolid;
		PHSolidForHapticSt* pst = (PHSolidForHapticSt*)GetHapticSolid(i);
		*pst = *hst;
	}
#endif
}

void PHHapticEngineLD::SyncPhysic2Haptic(){
	// haptic <------ physics
	// PHSolidForHaptic�̓���
	for(int i = 0; i < NHapticSolids(); i++){
		PHSolidForHaptic* psolid = GetHapticSolid(i);
		if(psolid->bPointer) continue;
		PHSolidForHaptic* hsolid = hapticLoop->GetHapticSolid(i);
		PHSolidForHapticSt2* pst2 = (PHSolidForHapticSt2*)psolid;
		PHSolidForHapticSt2* hst2 = (PHSolidForHapticSt2*)hsolid;
		*hst2 = *pst2;
		if(psolid->doSim == 1) hsolid->localSolid = psolid->localSolid;		
	}
	// solidpair, shapepair�̓���
	// �ߖT���̂̂ݓ���������
	for(int i = 0; i < NHapticPointers(); i++){
		PHHapticPointer* ppointer = GetHapticPointer(i);
		const int ppointerID = ppointer->GetPointerID();
		const int nNeighbors = ppointer->neighborSolidIDs.size();
		for(int j = 0; j < nNeighbors; j++){
			const int solidID = ppointer->neighborSolidIDs[j];
			PHSolidPairForHaptic* hpair = hapticLoop->GetSolidPairForHaptic(solidID, ppointerID);
			PHSolidPairForHaptic* ppair = GetSolidPairForHaptic(solidID, ppointerID);
			*hpair = PHSolidPairForHaptic(*ppair);
		}
	}
}

}