/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWLDHaptic6D.h>
#include <Framework/SprFWApp.h>


// FWLDHaptic6DLoop�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic6DLoop::FWLDHaptic6DLoop(){}

void FWLDHaptic6DLoop::Step(){
	UpdateInterface();
	//switch(hmode){
	//	case CONSTRAINT:
			{
				ConstraintBasedRendering();
				LocalDynamics6D();
			}
	//		break;
	//}
}

// �K�E�X�U�C�f���@���g��Ax+b>0������
template <class AD, class XD, class BD>
void GaussSeidel(MatrixImp<AD>& a, VectorImp<XD>& x, const VectorImp<BD>& b){
	int nIter = 20;					// �����񐔂̏��
	double errorRange = 10e-8;		// ���e�덷
	int n = (int)a.height();		// �A���������̐�(�s��a�̍s��)
	std::vector< double > lastx;
	for(int i = 0; i < n; i++){
		lastx.push_back(x[i]);
		x[i] = 0;
	}

	for(int k = 0; k < nIter; k++){		
		for(int i = 0; i < n; i++){
			double term1 = 0.0;
			double term2 = 0.0;
			for(int j = 0; j < i; j++){
				term1 += a[i][j] * x[j];
			}
			for(int j = i+1; j < n; j++){
				term2 += a[i][j] * lastx[j];
			}
			// x�̍X�V(�J��Ԃ��v�Z�̎����g�p)
			x[i] =  (-b[i] - term1 - term2) / a[i][i];
			if(x[i] < 0) x[i] = 0.0;
		}

		// (lastx - x)��2��̑��a�ƌ덷�͈͂��r
		double error = 0.0;
		for(int i = 0; i < n; i++){
			error += pow(x[i] - lastx[i], 2);
		}
		if(error < errorRange){
			//DSTR <<�@"Finish the iteration in admissible error. " << std::endl;
			return;
		}

		// �J��Ԃ��v�Z�̂��߂ɍX�V���x��lastx�ɕۑ�
		for(int i = 0; i < n; i++) lastx[i] = x[i];
	}
	// nIter�Ōv�Z���I���Ȃ������̂őł��؂�
	static int iterError = 0;
	iterError += 1;
	DSTR << iterError << " Could not convergent the variable." << std::endl; 
}

struct SolidVertex{
	int iaSolidID;
	Vec3d normal;
	Vec3d r;
	double d;
	Vec3d force;
};
typedef std::vector< SolidVertex > SolidVertices;


std::vector<string> savePointerForce;
std::vector<string> savePointerPosition;

//#define SIMPLEX
#ifdef SIMPLEX 
#include "C:\Project\Experiments\HapticInteraction\FWLDHapticTest\Simplex.h"
#endif
void FWLDHaptic6DLoop::ConstraintBasedRendering(){
	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		SpatialVector outForce = SpatialVector();
		SolidVertices sv;
		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			iInfo->mobility.f.clear();
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;

			/// ���̖̂ʂ̖@�����
			double syncCount = pdt / hdt;									// �v���Z�X�̍��ݎ��Ԃ̔�
			// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			Vec3d interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			// �J�E���^���K��̓����J�E���g������ꍇ�͌��݂̖@���ɂ���
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;			
#if 0
			std::vector < Vec3d > psection = nInfo->pointer_section;
			std::vector < Vec3d > ssection = nInfo->solid_section;
			double ptemp;
			double ctemp;
			for(size_t k = 0; k < psection.size(); k++){
				Vec3d pPoint = iPointer->hiSolid.GetPose() * psection[k];	// �͊o�|�C���^�̐ڐG�_(���[���h���W)
				Vec3d cPoint = cSolid->GetPose() * ssection[k];				// ���̂̐ڐG�_(���[���h���W)
				ptemp = pPoint.y;
				ctemp = cPoint.y;
				Vec3d force_dir =  pPoint - cPoint;							// ���̂̐ڐG�_����͊o�|�C���^�̐ڐG�_�ւ̃x�N�g��(���[���h�n)
				double f = force_dir * interpolation_normal;				// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
				if(f < 0.0){												// ���ς����Ȃ�͂��v�Z
					Vec3d ortho = f * interpolation_normal;					// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
					sv.resize(sv.size() + 1);
					sv.back().iaSolidID = i;
					sv.back().normal = interpolation_normal;
					sv.back().r = pPoint - iPointer->hiSolid.GetCenterPosition();
					sv.back().d = ortho.norm();
				}
			}
#else
			nInfo->solid_section.clear();
			std::vector < Vec3d > ivs = nInfo->intersection_vertices;
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̐ڐG�_(���[���h���W)
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̐ڐG�_(���[���h���W)
			Vec3d force_dir =  pPoint - cPoint;									// ���̂̐ڐG�_����͊o�|�C���^�̐ڐG�_�ւ̃x�N�g��(���[���h�n)
			double f = force_dir * interpolation_normal;						// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			//DSTR << "--------------------" << std::endl;
			if(f < 0.0){														// ���ς����Ȃ�͂��v�Z
				// �ڐG�_���ɐڐG�_�����ԕ��ʂ܂ł̋������v�Z
				for(size_t k = 0; k < ivs.size(); k++){
					Vec3d wivs = iPointer->hiSolid.GetPose() * ivs[k];
					double dot = (wivs - cPoint) * interpolation_normal;
					if(dot >= 0)	continue;
					Vec3d ortho = dot * interpolation_normal;		// ���̂̋ߖT�_����ڐG�_�܂ł̃x�N�g����ʖ@���֎ˉe
					sv.resize(sv.size() + 1);
					sv.back().iaSolidID = i;
					sv.back().normal = interpolation_normal;
					sv.back().r = wivs - iPointer->hiSolid.GetCenterPosition();
					sv.back().d = ortho.norm();
					Vec3d onPlane = wivs - ortho;
					nInfo->solid_section.push_back(cSolid->GetPose().Inv() * onPlane);
				}
				//DSTR << nInfo->solid_section.size() << std::endl;
			}
	
#endif
		}
		/// �A���s�������v�Z���邽�߂̍s����쐬
		if(sv.size() > 0){
			int l = (int)sv.size();	// �ڐG�_�̐�
			VMatrixRow<double> a;
			a.resize(l, l);
			VVector<double> d;
			d.resize(l);
			VVector<double> fnorm;
			fnorm.resize(l+1);
			for(int m = 0; m <= l; m++){
				if(m == 0)	fnorm[m] = 0.0;
				else		fnorm[m] = 1.0;
			}
			for(int m = 0; m < l; m++){
				for(int n = 0; n < l; n++){
					Vec3d rm = sv[m].r;
					Vec3d nm = sv[m].normal;
					Vec3d rn = sv[n].r; 
					Vec3d nn = sv[n].normal;
					a[m][n] = (rm % nm) * iPointer->hiSolid.GetInertia().inv() * (rn % nn) + nm * nn / iPointer->hiSolid.GetMass();
				}
				d[m] = sv[m].d;
			}
#ifdef SIMPLEX
			Inequality ineq;
			ineq.resize(l);
			for(int m = 0; m < l; m++){
				ineq[m] = '>';
			}
			Simplex simplex;
			simplex.SetTargetFunction(fnorm);
			simplex.SetLinearInequality(a, d, ineq);
			VVector<double> f = simplex.Start();
			double ftemp = 0;
#else
			VVector<double> f = VVector<double>();
			f.resize(l);
			GaussSeidel(a,f,-d);
#endif
			// �|�C���^�ƍ��̂ɍS����^����
			Vec3d dr = Vec3d();
			Vec3d dtheta = Vec3d();
			for(int m = 0; m < l; m++){
				if(f[m] < 0) f[m] = 0.0;
				// �|�C���^�ւ̍S��
				dr += f[m] * sv[m].normal / iPointer->hiSolid.GetMass();							// �ʒu�S��
				dtheta += f[m] * iPointer->hiSolid.GetInertia().inv() * (sv[m].r % sv[m].normal);	// ��]�S��
			}

			/// �͊o�C���^�t�F�[�X�ɏo�͂���͂̌v�Z
			double ws4 = pow(iPointer->GetWorldScale(), 4);
			outForce.v() = (iPointer->correctionSpringK * dr - iPointer->correctionDamperD * (dr/hdt)) /ws4;
			outForce.w() = (iPointer->correctionSpringK * dtheta) / ws4 / iPointer->GetPosScale();

			/// ���̂֗͂�������
			for(int m = 0; m < l; m++){
				Vec3d addforce = -1 * f[m] * sv[m].normal * iPointer->correctionSpringK /ws4;
				iPointer->interactInfo[sv[m].iaSolidID].mobility.f.push_back(addforce);	
			}
		}

		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());

#if 0
		//CSVOUT << l << "," << sv[0].d << "," << outForce.v().y << endl;
		//CSVOUT << outForce.v().x << "," << outForce.v().y << "," << outForce.v().z << "," << outForce.w().x << "," << outForce.w().y << "," << outForce.w().z << endl;


		//�͂̃t�@�C���Z�[�u
		std::stringstream str;
		str << outForce.v().x << "\t" << outForce.v().y << "\t" << outForce.v().z << "\t" << outForce.w().x << "\t" << outForce.w().y << "\t" << outForce.w().z;
		savePointerForce.push_back(str.str());
		str.str("");
		str.clear();

		//�ʒu�̃t�@�C���Z�[�u
		Vec3d pos = iPointer->hiSolid.GetCenterPosition();
		Vec3d ori, temp;
		iPointer->hiSolid.GetOrientation().ToEuler(temp);
		ori.x = Deg(temp.z);
		ori.y = Deg(temp.x);
		ori.z = Deg(temp.y);
		str << pos.x << "\t" << pos.y << "\t" << pos.z << "\t" << ori.x << "\t" << ori.y << "\t" << ori.z; 
		savePointerPosition.push_back(str.str());
#endif
	}
}


std::vector<string> saveSolidForce;
std::vector<string> saveSolidPosition;
void FWLDHaptic6DLoop::LocalDynamics6D(){
	for(int i = 0; i < NIASolids(); i++){
		FWInteractSolid* iSolid = FWHapticLoopBase::GetIASolid(i);
		if(!iSolid->bSim) continue;
		SpatialVector vel;
		vel.v() = iSolid->copiedSolid.GetVelocity();
		vel.w() = iSolid->copiedSolid.GetAngularVelocity();
		if(GetLoopCount() == 1){
			vel += (iSolid->curb - iSolid->lastb) *  pdt;	// �Փ˂̉e���𔽉f
		}
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* iPointer = GetIAPointer(j);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			SpatialVector f = SpatialVector();
			for(int k = 0; k < (int)iInfo->mobility.f.size(); k++){
				f.v() += iInfo->mobility.f[k];
				Vec3d r = (iSolid->copiedSolid.GetPose() * iInfo->neighborInfo.solid_section[k]) - iSolid->copiedSolid.GetCenterPosition(); 
				f.w() += r % iInfo->mobility.f[k];
			}
			vel += (iInfo->mobility.Minv * f) * hdt;				// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
			//DSTR << iInfo->mobility.Minv * f << std::endl;
			iInfo->neighborInfo.test_force = f.v();					// �e�X�g�͂̕ۑ�
			iInfo->neighborInfo.test_torque = f.w();				// �e�X�g�g���N�̕ۑ�
			iInfo->neighborInfo.solid_section.clear();
			iInfo->mobility.f.clear();
		}
		vel += iSolid->b * hdt;

		//solid�@force���Z�[�u���邽�߂̌v�Z
		SpatialVector dvel = SpatialVector();
		dvel.v() = vel.v() - iSolid->copiedSolid.GetVelocity();
		dvel.w() = vel.w() - iSolid->copiedSolid.GetAngularVelocity();
		SpatialVector accel = dvel / hdt;		// �����x
		SpatialVector sf = SpatialVector();	// ��
		sf.v() = iSolid->copiedSolid.GetMass() * accel.v() ;
		sf.w() = iSolid->copiedSolid.GetInertia() * accel.w();

		/// ��Ԃ̍X�V
		iSolid->copiedSolid.SetVelocity(vel.v());		
		iSolid->copiedSolid.SetAngularVelocity(vel.w());
		iSolid->copiedSolid.SetCenterPosition(iSolid->copiedSolid.GetCenterPosition() + vel.v() * hdt);
		iSolid->copiedSolid.SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * iSolid->copiedSolid.GetOrientation()).unit());
		/// �X�V�����̃t���O�𗧂Ă�
 		iSolid->copiedSolid.SetUpdated(true);
		iSolid->copiedSolid.Step();

#if 0
		if(i != 1) continue;
		//Solid Position, Force�̃Z�[�u
		std::stringstream str;
		str << sf.v().x << "\t" << sf.v().y << "\t" << sf.v().z << "\t" << sf.w().x << "\t" << sf.w().y << "\t" << sf.w().z;
		saveSolidForce.push_back(str.str());
		str.str("");
		str.clear();

		//�ʒu�̃t�@�C���Z�[�u
		Vec3d sVel = iSolid->copiedSolid.GetVelocity();
		Vec3d sAVel = iSolid->copiedSolid.GetAngularVelocity();
		Vec3d pos = iSolid->copiedSolid.GetCenterPosition();
		Vec3d ori, temp;
		iSolid->copiedSolid.GetOrientation().ToEuler(temp);
		ori.x = Deg(temp.z);
		ori.y = Deg(temp.x);
		ori.z = Deg(temp.y);
		str << sVel.x << "\t" << sVel.y << "\t" << sVel.z << "\t" << sAVel.x << "\t" << sAVel.y << "\t" << sAVel.z << "\t";
		str << pos.x << "\t" << pos.y << "\t" << pos.z << "\t" << ori.x << "\t" << ori.y << "\t" << ori.z;
		//DSTR << str.str() << std::endl;
		saveSolidPosition.push_back(str.str());
#endif
	}
}



// FWLDHapticApp�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic6D::FWLDHaptic6D(){
	hapticLoop = &ldLoop6D;
}

void FWLDHaptic6D::Step(){
	if (bSync) return;
	if (bCalcPhys){
		UpdatePointer();
		PhysicsStep();
		UpdateSolidList();
		NeighborObjectFromPointer();
		TestSimulation6D();
		bCalcPhys = false;
	}
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}

void FWLDHaptic6D::TestSimulation6D(){
	/* 6DoF Haptic Rendering �̂��߂̃e�X�g�V�~�����[�V����*/
	PHSceneIf* phScene = GetPHScene();

	#ifdef DIVIDE_STEP
	/// �e�X�g�V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	states2->SaveState(phScene);		
	///	LCP�̒��O�܂ŃV�~�����[�V�������Ă��̏�Ԃ�ۑ�
	phScene->ClearForce();
	phScene->GenerateForce();
	phScene->IntegratePart1();
	#endif
	/// �e�X�g�V�~�����[�V�����̂��߂Ɍ��݂̍��̂̏�Ԃ�ۑ�����
	phScene->GetConstraintEngine()->SetBSaveConstraints(true);
	states->SaveState(phScene);	

	/// �e�X�g�V�~�����[�V�������s
	for(int i = 0; i < NIASolids(); i++){
		if(!GetIASolid(i)->bSim) continue;
		FWInteractSolid* inSolid = GetIASolid(i);
		PHSolid* phSolid = GetIASolid(i)->sceneSolid;
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
		/// ���r���e�Bb�̎Z�o
		inSolid->lastb = inSolid->b;
		double pdt = phScene->GetTimeStep();
		inSolid->b = (nextvel - curvel) / pdt;
		states->LoadState(phScene);						// ���݂̏�Ԃɖ߂�

		/// InteractPointer�̐������͂�������e�X�g�V�~�����[�V�������s��
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* iPointer = GetIAPointer(j);
			if(iPointer->interactInfo[i].flag.blocal == false) continue;
			PHSolid* soPointer = iPointer->pointerSolid->Cast();
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			Vec3d n = -iInfo->neighborInfo.face_normal;

			const float minTestForce = 1;		// �ŏ��e�X�g��
			const float minTestTorque = 0.;

			SpatialVector f[6];
			for(int k = 0; k < 5; k++)	f[k] = SpatialVector();
			f[0].v() = iInfo->neighborInfo.test_force;
			f[0].w() = iInfo->neighborInfo.test_torque;
			iInfo->neighborInfo.test_force = Vec3d();
			iInfo->neighborInfo.test_torque = Vec3d();
			/// �e�X�g�͂�0�̏ꍇ�̏���
			//�e�X�g�g���N���K��0�ɂȂ�
			//�ڐG���Ă��Ȃ��̂ŁC1�_�����ɗ͂�������悤�ɂ���
			if(f[0].v().norm() == 0){
				f[0].v() = minTestForce * n;
				Vec3d cPoint = phSolid->GetPose() * iInfo->neighborInfo.closest_point;		// �͂�������_(���[���h���W)
				Vec3d center = phSolid->GetCenterPosition();
				f[0].w() = (cPoint - center) % f[0].v();
			}
			/// �e�X�g�g���N��0�̏ꍇ�̏���
			if(f[0].w().norm() == 0){
				f[0].w() = minTestTorque * Vec3d(1, 0, 0);	// �Ƃ肠�����K���ȃx�N�g�������Ă���
				DSTR << "test torque is Zero" << std::endl;
			}

#if 1
			f[3].w() = f[0].w();
			f[0].w() = Vec3d();
			/// ��f[0].v()�Ɛ����ȗ�2�{
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
			base1 = f[3].w().unit();
			base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if(base2.norm() > 0.1)	base2.unitize();
			else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			base3 = base1^base2;
			f[4].v() = f[5].v() = f[0].v();
			f[4].w() = f[3].w().norm() * (base1 + base2);
			f[5].w() = f[3].w().norm() * (base1 + base3);
#else
			/// �e�X�g�͂��ׂĂ�f[0]�ɂ���
			for(int k = 1; k < 6; k++) f[k] = f[0];

			/// ��f[0].v()�Ɛ����ȗ�2�{
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

			/// f[0]�Ɛ����ɂȂ�x�N�g�� svbase2 = (a * f[0].v() b*f[0].w())^{t}
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
				u.col(k) = (nextvel - curvel) /pdt - inSolid->b;
				states->LoadState(phScene);
			}
			//DSTR << F.det() << std::endl; 
			//DSTR << u << std::endl; 
			iInfo->mobility.Minv = u  * F.inv();			// ���r���e�BA�̌v�Z
#if 1
			//DEBUG
			//DSTR << "------------------------" << std::endl;
			//DSTR << "b:" << std::endl;	DSTR << inSolid->b << std::endl;
			//DSTR << "F:" << std::endl;	DSTR << F << std::endl;
			//DSTR << "Minv:" << std::endl;	DSTR << iInfo->mobility.Minv << std::endl;
#if 0
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
}
