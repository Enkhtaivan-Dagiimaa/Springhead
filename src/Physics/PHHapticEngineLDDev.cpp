#include <Physics/PHHapticEngineLDDev.h>

namespace Spr{;
//----------------------------------------------------------------------------
// PHHapticLoopLDDev
void PHHapticLoopLDDev::Step(){
	UpdateInterface();
	HapticRendering();
	LocalDynamics6D();
}
void PHHapticLoopLDDev::HapticRendering(){
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


void PHHapticLoopLDDev::LocalDynamics6D(){
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
			SpatialVector force;
			force.v() = sp->force;
			force.w() = sp->torque;
			vel += (sp->A6D * force) * hdt;			// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
			//CSVOUT << force[0] << "," << force[1] << "," << force[2] << "," << force[3] << "," << force[4] << "," << force[5] << "," <<std::endl;
			//DSTR << force << std::endl;
			//DSTR << sp->A6D << std::endl;
		}
		vel += hsolid->b * hdt;
		//DSTR << vel << std::endl;
		//CSVOUT << vel.w().y << std::endl;
		localSolid->SetVelocity(vel.v());		
		localSolid->SetAngularVelocity(vel.w());
		localSolid->SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * localSolid->GetOrientation()).unit());
		//localSolid->SetOrientation(( localSolid->GetOrientation() * Quaterniond::Rot(vel.w() * hdt)).unit());
		localSolid->SetCenterPosition(localSolid->GetCenterPosition() + vel.v() * hdt);

 		localSolid->SetUpdated(true);
		localSolid->Step();
	}
}

//----------------------------------------------------------------------------
// PHHapticEngineLDDev
PHHapticEngineLDDev::PHHapticEngineLDDev(){
	hapticLoop = &hapticLoopLD;
	hapticLoop->engineImp = this;
	states = ObjectStatesIf::Create();
}

void PHHapticEngineLDDev::Step1(){
	lastvels.clear();
	for(int i = 0; i < NHapticSolids(); i++){
		SpatialVector vel;
		vel.v() = GetHapticSolid(i)->sceneSolid->GetVelocity();
		vel.w() = GetHapticSolid(i)->sceneSolid->GetAngularVelocity();
		lastvels.push_back(vel);
	}
}
void PHHapticEngineLDDev::Step2(){
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
	PredictSimulation6D();
}


void PHHapticEngineLDDev::PredictSimulation6D(){
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
		SpatialVector preb = (nextvel - curvel)/pdt;
		hsolid->b = preb;


		states->LoadState(phScene);					// ���݂̏�Ԃɖ߂�

		//if(i == 1){
		//	DSTR << "-------" << std::endl;
		//	//DSTR << dot << std::endl;
		//	//DSTR << hsolid->sceneSolid->GetLastVelocity().y << "," << "curvel" << curvel.v().y << "    " << "nextvel" << nextvel.v().y << std::endl;  
		//	DSTR << hsolid->curb << "," << hsolid->b << "," << hsolid->bimpact << std::endl;
		//	//CSVOUT << hsolid->sceneSolid->GetFramePosition().y << "," << curvel.v().y << std::endl;
		//}

		/// HapticPointer�̐������͂�������\���V�~�����[�V����
		for(int j = 0; j < NHapticPointers(); j++){
			PHHapticPointer* pointer = GetHapticPointer(j);
			PHSolidPairForHaptic* solidPair = GetSolidPairForHaptic(i, pointer->GetPointerID());
			if(solidPair->inLocal == 0) continue;
			PHShapePairForHaptic* sp = solidPair->shapePairs.item(0, 0);	// 1�`��̂ݑΉ�
			Vec3d cPoint = sp->shapePoseW[0] * sp->closestPoint[0];		// �͂�������_(���[���h���W)
			Vec3d normal = -1 * sp->normal;

			float minTestForce = 0.5;		// �ŏ��e�X�g��
			float minTestTorque = 0.5;

			// �e�X�g�́A�e�X�g�g���N�̍쐬
			SpatialVector testForce;
			testForce.v() = solidPair->force;
			testForce.w() = solidPair->torque;
			solidPair->force = Vec3d();
			solidPair->torque = Vec3d();

			/// �e�X�g�͂�0�̏ꍇ�̏���
			//�e�X�g�g���N���K��0�ɂȂ�
			//�ڐG���Ă��Ȃ��̂ŁC1�_�����ɗ͂�������悤�ɂ���
			if(testForce.v().norm() < 1e-5){
				testForce.v() = minTestForce * normal;
				Vec3d cPoint = sp->shapePoseW[0] * sp->closestPoint[0];		// �͂�������_(���[���h���W)
				Vec3d center = phSolid->GetCenterPosition();
				testForce.w() = (cPoint - center) % testForce.v();
				//DSTR << testForce.w() << std::endl;
			}
			///// �e�X�g�g���N��0�̏ꍇ�̏���
			//if(testForce.w().norm() < 1e-5){
			//	testForce.w() = minTestTorque * Vec3d(1, 0, 0);	// �Ƃ肠�����K���ȃx�N�g�������Ă���
			//	DSTR << "test torque is Zero" << std::endl;
			//}

			SpatialVector f[6];
#if 1
			/// ��f[0].v()�Ɛ����ȗ�2�{
			f[0].v() = testForce.v();
			Vec3d base1 = f[0].v().unit();
			Vec3d base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if(base2.norm() > 0.1)	base2.unitize();
			else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			Vec3d base3 = base1^base2;
			f[1].v() = f[0].v().norm() * (base1 + base2).unit();
			f[2].v() = f[0].v().norm() * (base1 + base3).unit();

			/// �g���Nf[0].w()�Ɛ����ȃg���N2�{
			f[3].w() = testForce.w();
			base1 = f[3].w().unit();
			base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if(base2.norm() > 0.1)	base2.unitize();
			else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			base3 = base1^base2;
			f[4].v() = f[5].v() = f[0].v();
			f[4].w() = f[3].w().norm() * (base1 + base2).unit();
			f[5].w() = f[3].w().norm() * (base1 + base3).unit();

#else
			/// �e�X�g�͂��ׂĂ�f[0]�ɂ���
			for(int k = 1; k < 6; k++) f[k] = f[0];

			/// ��f[0].v()�Ɛ����ȗ�2�{
			f[0].v() = testForce.v();
			Vec3d base1 = f[0].v().unit();
			Vec3d base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if(base2.norm() > 0.1)	base2.unitize();
			else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			Vec3d base3 = base1^base2;
			f[1].v() = f[0].v().norm() * (base1 + base2);
			f[2].v() = f[0].v().norm() * (base1 + base3);

			/// �g���Nf[0].w()�Ɛ����ȃg���N2�{
			base1 = f[0].w().unit();
			base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if(base2.norm() > 0.1)	base2.unitize();
			else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			base3 = base1^base2;
			f[3].w() = f[0].w().norm() * (base1 + base2);
			f[4].w() = f[0].w().norm() * (base1 + base3);

			/// f[0]�Ɛ����ɂȂ�x�N�g�� svbase2 = (a * f[0].v(), b*f[0].w())^{t}
			/// a*f[0].v()*f[0].v() + b*f[0].w()*f[0].w() = 0�ƂȂ�a, b���݂���
			SpatialVector svbase1 = SpatialVector();
			svbase1.v() = f[0].v();
			svbase1.w() = f[0].w();
			svbase1.unitize();
			double f_ip = f[0].v()*f[0].v();
			double t_ip = f[0].w()*f[0].w();
			double c = f_ip / t_ip;
			double a = 1;
			double b = - a * c;
			//DSTR << c << std::endl;
			SpatialVector svbase2 = SpatialVector();
			svbase2.v() = a * f[0].v();
			svbase2.w() = b * f[0].w();
			svbase2.unitize();
			//f[5] = f[0].w().norm() * (svbase1 + svbase2);
			f[5] = svbase1 + svbase2;
			f[5].v() = f[0].v().norm() * f[5].v();
			f[5].w() = f[0].w().norm() * f[5].w();

#endif

			TMatrixRow<6,6,double> u = TMatrixRow<6,6,double>();		// ���̂̋@�B�C���s�[�_���X
			TMatrixRow<6,6,double> F = TMatrixRow<6,6,double>();		// �������,�g���N�s��
			for(int k = 0; k < 6; k++)	F.col(k) = f[k];				// �e�X�g�́C�e�X�g�g���N���s��ɋl�߂�

			/// �e�X�g�́C�e�X�g�g���N�������ăe�X�g�V�~�����[�V�������s
			for(int k = 0; k < 6; k++){
				phSolid->AddForce(f[k].v()); 
				phSolid->AddTorque(f[k].w());
				#ifdef DIVIDE_STEP
				phScene->IntegratePart2();
				#else
				phScene->Step();
				#endif
				nextvel.v() = phSolid->GetVelocity();
				nextvel.w() = phSolid->GetAngularVelocity();
				u.col(k) = (nextvel - curvel) /pdt - hsolid->b;
				states->LoadState(phScene);

				//DSTR << "force" << std::endl;
				//DSTR << f[k] << std::endl;
				//DSTR << "nextvel" << std::endl;
				//DSTR << nextvel << std::endl;
			}
			//DSTR << F.det() << std::endl; 
			//DSTR << u << std::endl; 
			solidPair->A6D = u  * F.inv();			// ���r���e�BA�̌v�Z
#if 0
			DSTR << "------------------------" << std::endl;
			DSTR << "u" << std::endl; DSTR << u << std::endl;
			DSTR << "b:" << std::endl;	DSTR << hsolid->b << std::endl;
			DSTR << "F:" << std::endl;	DSTR << F << std::endl;
			DSTR << "Minv:" << std::endl;	DSTR << solidPair->A6D << std::endl;
#if 1
			TMatrixRow<6, 6, double> M = TMatrixRow<6, 6, double>();
			if(det(u) == 0)	M = F * u; 
			else			M = F * u.inv(); 
			DSTR << "M:" << std::endl;	DSTR << M << std::endl;
#endif
#endif
		}
	}
	///--------�e�X�g�V�~�����[�V�����I��--------
#ifdef DIVIDE_STEP
	states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
#endif
	engine->bPhysicStep = true;
}


void PHHapticEngineLDDev::SyncHaptic2Physic(){
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
		//b = SpatialVector();
		Vec3d v = localSolid->GetVelocity() + b.v();			// ���f���x
		Vec3d w = localSolid->GetAngularVelocity() + b.w();		// ���f�p���x

		// ��Ԃ̔��f
		PHSolid* sceneSolid = hsolid->sceneSolid;
		Vec3d svel = sceneSolid->GetVelocity();
		Vec3d spos = sceneSolid->GetFramePosition();
		sceneSolid->SetVelocity(v);
		sceneSolid->SetAngularVelocity(w);
		sceneSolid->SetPose(localSolid->GetPose());
		//DSTR << phvel << ", " << v << std::endl;
		//if(i == 1){
		//	DSTR << "physicsvel" << svel.y << "," << "hapticvel" << localSolid->GetVelocity().y << std::endl;
		//	DSTR << "mergevel" << sceneSolid->GetVelocity().y << std::endl;
		//	DSTR << "nextb" << psolid->b << std::endl;
		//	DSTR << b << std::endl;
		//	CSVOUT << svel.y << "," << spos.y << "," <<  localSolid->GetVelocity().y << "," << localSolid->GetFramePosition().y << "," << v.y << std::endl;
		//}

		PHSolidForHapticSt* hst = (PHSolidForHapticSt*)hsolid;
		PHSolidForHapticSt* pst = (PHSolidForHapticSt*)GetHapticSolid(i);
		*pst = *hst;
	}
#endif
}

void PHHapticEngineLDDev::SyncPhysic2Haptic(){
#if 1
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
			//DSTR << hpair->A << std::endl;
		}
	}
#endif
}

}