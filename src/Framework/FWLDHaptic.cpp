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
FWLDHapticLoop::FWLDHapticLoop(){
	proK = 1000;
	proD = 0.05;
	proM = 7.5*10e-5;
	bPic = false;
	picNum = 0;
	for(int i=0;i<2;i++){
//		DisplayProxy[i] = Vec3d(5,0,0);
		DisplayForce[i] = Vec3d(0,0,0);
	}
}
void FWLDHapticLoop::Step(){
	UpdateInterface();
	switch(hmode){
		case PENALTY:
			HapticRendering();
			break;
		case PENALTY6D:
			HapticRendering6D();
			break;
		case CONSTRAINT:
			ConstraintBasedRendering();
			break;
		case PROXY:
			Proxy();
			break;
		case PROXYSIMULATION:
			ProxySimulation();
			break;
		default:
			Proxy();
			break;
	}
	LocalDynamics();
}

void FWLDHapticLoop::UpdateInterface(){
	int N = NIAPointers();
	for(int i = 0; i < N; i++){
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		double s = iPointer->GetWorldScale() * iPointer->GetPosScale();
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			//6���R�x�C���^�t�F�[�X�̏ꍇ
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
			hif->Update((float)hdt);
			PHSolid* hiSolid = &iPointer->hiSolid;
			hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
			hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
			Posed hifPose;
			hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
			hifPose.Ori()=hif->GetOrientation();
			hiSolid->SetPose(GetIAPointer(i)->GetDefaultPosition() * hifPose);
			//static bool frag = true;
			//if(frag){					
			//	hiSolid->SetMass(iPointer->GetPointerSolid()->GetMass());
			//	hiSolid->SetInertia(iPointer->GetPointerSolid()->GetInertia());
			//	DSTR << hiSolid->GetMass() << endl;
			//	DSTR << hiSolid->GetInertia() << endl;
			//	frag = false;
			//}
		}else{
			//3���R�x�C���^�t�F�[�X�̏ꍇ
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
			hif->Update((float)hdt);
			PHSolid* hiSolid = &iPointer->hiSolid;
			hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
			Posed hifPose;
			hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
			hiSolid->SetPose(GetIAPointer(i)->GetDefaultPosition() * hifPose);
		}
	}
}

void FWLDHapticLoop::HapticRendering(){
	bPic = false;
	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		if((int)iPointer->bContact.size() < NIASolids()){
			for(int i=(int)iPointer->bContact.size(); i<NIASolids(); i++){
				iPointer->bContact.push_back(false);
				contactFlag[0].push_back(false);
				contactFlag[1].push_back(false);
				oVibForce.push_back(Vec3d(0,0,0));
			}
		}		
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();
		PicVibration(NIASolids());

		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			if((int)proxy[j].size() <= NIASolids()){
				for(int k=(int)proxy[j].size(); k<NIASolids(); k++){
					proxy[j].push_back(poseSolid.Inv()*pPoint);
					proVel[j].push_back(Vec3d(0,0,0));
					pLastPoint[j].push_back(Vec3d(0,0,0));
				}
			}

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;

			double f = force_dir * interpolation_normal;		// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			if(f < 0.0){										// ���ς����Ȃ�͂��v�Z
				contactFlag[j][i] = true;
				if(contactFlag[0][i]&&contactFlag[1][i]) bPic = true;	// ���̂�c�����Ă��邩�ǂ���
				Vec3d pVibForce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->correctionSpringK;
				double D = iPointer->correctionDamperD;
				Vec3d addforce = -1 * (K * ortho + D * dvortho);
				Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;

				///�U���̌v�Z
				if(iPointer->bVibration){
					pVibForce = Vibration(iSolid, iPointer, i);
					if(bPic){ pVibForce += oVibForce[i]; }
				}

				outForce.v() += addforce + pVibForce;	
				//outForce.w() += addtorque;

				DisplayForce[j] = outForce.v();

				/// �v�Z�����͂����̂ɉ�����//
				iPointer->interactInfo[i].mobility.force = -1 * addforce;	
				nInfo->test_force_norm = addforce.norm();
				nInfo->test_force = addforce;
				//if(iPointer->bForce)	DSTR << vibforce << endl;
			}else{
				iPointer->bContact[i] = false;
				contactFlag[j][i] = false;
			}
		}
		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());
	}
	for(int k=0;k<NIASolids();k++){
		if(contactFlag[0][k]&&contactFlag[1][k]){
			picNum = k;
			bPic = true;
		}
	}
}

void FWLDHapticLoop::HapticRendering6D(){
	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		SpatialVector outForce = SpatialVector();

		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;

			/// ���̖̂ʂ̖@�����
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			Vec3d interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			// �J�E���^���K��̓����J�E���g������ꍇ�͌��݂̖@���ɂ���
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;
			
			std::vector < Vec3d > psection = nInfo->pointer_section;
			std::vector < Vec3d > ssection = nInfo->solid_section;
			int pSecSize = (int)psection.size();
			for(int k = 0; k < pSecSize; k++){
				Vec3d pPoint = iPointer->hiSolid.GetPose() * psection[k];	// �͊o�|�C���^�̐ڐG�_(���[���h���W)
				Vec3d cPoint = cSolid->GetPose() * ssection[k];				// ���̂̐ڐG�_(���[���h���W)
				Vec3d force_dir = pPoint - cPoint;
				// ���̂̐ڐG�_����͊o�|�C���^�̐ڐG�_�ւ̃x�N�g��(���[���h�n)
				double f = force_dir * interpolation_normal;				// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
				if(f < 0.0){												// ���ς����Ȃ�͂��v�Z
					Vec3d ortho = f * interpolation_normal;					// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
					Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
					Vec3d dvortho = dv.norm() * interpolation_normal;

					/// �R�͂̌v�Z
					double K = iPointer->correctionSpringK / psection.size();
					double D = iPointer->correctionDamperD / psection.size();
					double s = iPointer->GetWorldScale() * iPointer->GetPosScale();
					Vec3d addforce = -1 * (K * ortho + D * dvortho);
					Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce / s;

					outForce.v() += addforce;	
					outForce.w() += addtorque;

					/// �v�Z�����͂����̂ɉ�����//
					iPointer->interactInfo[i].mobility.force = -1 * addforce;	
					nInfo->test_force_norm = addforce.norm();
					nInfo->test_force = addforce;
				}
			}
		}
		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());
	}
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
	//DSTR << "Could not convergent the variable. << std::endl; 
	//DTR << "Abort the iteration." << std::endl;
}

struct SolidVertex{
	int iaSolidID;
	Vec3d normal;
	Vec3d r;
	double d;
	Vec3d force;
};
typedef std::vector< SolidVertex > SolidVertices;

//#define SIMPLEX
#ifdef SIMPLEX 
#include "C:\Project\Experiments\HapticInteraction\FWLDHapticTest\Simplex.h"
#endif
void FWLDHapticLoop::ConstraintBasedRendering(){
	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		SpatialVector outForce = SpatialVector();
		SolidVertices sv;
		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
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
			std::vector < Vec3d > psection = nInfo->pointer_section;
			std::vector < Vec3d > ssection = nInfo->solid_section;
			double ptemp;
			double ctemp;
			for(int k = 0; k < psection.size(); k++){
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
		}
		/// �A���s�������v�Z���邽�߂̍s����쐬
		if(sv.size() > 0){
			int l = sv.size();
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
			// �|�C���^�ɍS����^����
			Vec3d dr = Vec3d();
			Vec3d dtheta = Vec3d();
			for(int m = 0; m < l; m++){
				if(f[m] < 0) f[m] = 0.0;
				dr += f[m] * sv[m].normal / iPointer->hiSolid.GetMass();
				dtheta += f[m] * iPointer->hiSolid.GetInertia().inv() * (sv[m].r % sv[m].normal);
			}
			// �͊o�C���^�t�F�[�X�ɏo�͂���͂̌v�Z
			outForce.v() = iPointer->correctionSpringK * dr - iPointer->correctionDamperD * (dr/hdt);
			outForce.w() = iPointer->correctionSpringK * dtheta / 20;
//			CSVOUT << outForce.v().x << "," << outForce.v().y << "," << outForce.v().z << "," << outForce.w().x << "," << outForce.w().y << "," << outForce.w().z << endl;
		}
		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());
	}
}

void FWLDHapticLoop::LocalDynamics(){
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
			vel += (iInfo->mobility.A * iInfo->mobility.force) * hdt;			// �͊o�|�C���^����̗͂ɂ�鑬�x�ω�
			iInfo->mobility.force = Vec3d();
		}
		vel += iSolid->b * hdt;
		iSolid->copiedSolid.SetVelocity(vel.v());		
		iSolid->copiedSolid.SetAngularVelocity(vel.w());
		iSolid->copiedSolid.SetCenterPosition(iSolid->copiedSolid.GetCenterPosition() + vel.v() * hdt);
		iSolid->copiedSolid.SetOrientation(( Quaterniond::Rot(vel.w() * hdt) * iSolid->copiedSolid.GetOrientation()).unit());

		//Debug
		//DSTR<<"Vel:"<<iSolid->copiedSolid.GetVelocity()<<std::endl;
		//DSTR<<"AngVel:"<<iSolid->copiedSolid.GetAngularVelocity()<<std::endl;
		//DSTR<<"Pos:"<<iSolid->copiedSolid.GetCenterPosition()<<std::endl;
		//DSTR<<"Ori:"<<iSolid->copiedSolid.GetOrientation()<<std::endl;
		//DSTR<<"----------------------------"<<std::endl;

 		iSolid->copiedSolid.SetUpdated(true);
		iSolid->copiedSolid.Step();
	}
}

Vec3d FWLDHapticLoop::Vibration(FWInteractSolid* iSolid, FWInteractPointer* iPointer, int n){
	Vec3d vibforce = Vec3d(0,0,0);

	if(!iPointer->bContact[n]){	// ���̋ߖT�_�Ə��ڐG���ǂ���
		vibV = iPointer->hiSolid.GetVelocity() - iSolid->copiedSolid.GetVelocity();
		iSolid->sceneSolid->GetShape(0)->SetVibT(0);
	}
	vibT = iSolid->sceneSolid->GetShape(0)->GetVibT();		// material.vibT�ւ̃|�C���^
	iPointer->bContact[n] = true;

	double vibA = iSolid->copiedSolid.GetShape(0)->GetVibA();
	double vibB = iSolid->copiedSolid.GetShape(0)->GetVibB();
	double vibW = iSolid->copiedSolid.GetShape(0)->GetVibW();

	vibforce = vibA * (vibV*0.03) * exp(-vibB * vibT) * sin(2 * M_PI * vibW * vibT);		//�U���v�Z
	iSolid->sceneSolid->GetShape(0)->SetVibT((float)(vibT+hdt));		// �ڐG���Ԃ̍X�V
	return vibforce;
}

#if 1
void FWLDHapticLoop::PicVibration(int nSolids){
	PHSceneIf* phScene = GetIAAdaptee()->GetPHScene();
	
	int sceneCnt = phScene->GetCount();
	static std::vector<Vec3d> vel;
	if((int)vel.size() < nSolids){
		for(int i=(int)vel.size(); i<nSolids; i++){
			vel.push_back(Vec3d(0,0,0));
		}
	}

	PHSolidPairForLCPIf* solidPair;
	PHShapePairForLCPIf* shapePair;
	for(int i=0; i<nSolids; i++){ oVibForce[i] = Vec3d(0,0,0); }

	for (int i=0; i<nSolids; ++i) {
		for (int j=i+1; j<nSolids; ++j) {
			PHSolidIf *so1, *so2;
			so1 = phScene->GetSolids()[i];
			so2 = phScene->GetSolids()[j];

			solidPair = phScene->GetSolidPair(i, j);
			//PHConstraintIf* constraint = phScene->GetConstraintEngine()->GetContactPoints()->FindBySolidPair(so1, so2);

			if (!solidPair)  { continue; }
			//if (!constraint) { continue; }

			for (int s=0; s<so1->NShape(); ++s) {
				for (int t=0; t<so2->NShape(); ++t) {

					// ���̕��@���Ɠ������̂̈قȂ�Shape�ɂ��Ă̐ڐG��
					// �قȂ�Contact�ƂȂ�B
					// ��������̂��Ƃɂ܂Ƃ߂�ɂ͔畆���o�̉��Z�ɂ��Ă̒m�����K�v�B
					// �Ƃ肠�������Ƃ܂킵 (mitake, 09/02/07)

					shapePair = solidPair->GetShapePair(s, t);

					if (!shapePair) { continue; }

					int			contactStat	= solidPair->GetContactState(s, t);
					unsigned	lastContCnt	= solidPair->GetLastContactCount(s, t);

					if (contactStat == 1 || (contactStat == 2 && (lastContCnt == sceneCnt-1))) {
						if(contactStat ==1){
							vel[i] = so1->GetVelocity() - so2->GetVelocity();
							vel[j] = so2->GetVelocity() - so1->GetVelocity();
							so1->GetShape(0)->SetVibT(0.0);
							so2->GetShape(0)->SetVibT(0.0);
						}

						double vibT1 = so1->GetShape(0)->GetVibT();
						double vibA1 = so1->GetShape(0)->GetVibA();
						double vibB1 = so1->GetShape(0)->GetVibB();
						double vibW1 = so1->GetShape(0)->GetVibW();

						double vibT2 = so2->GetShape(0)->GetVibT();
						double vibA2 = so2->GetShape(0)->GetVibA();
						double vibB2 = so2->GetShape(0)->GetVibB();
						double vibW2 = so2->GetShape(0)->GetVibW();

						oVibForce[i] += vibA1 * (vel[i]*0.03) * exp(-vibB1 * vibT1) * sin(2 * M_PI * vibW1 * vibT1);		//�U���v�Z
						oVibForce[j] += vibA2 * (vel[j]*0.03) * exp(-vibB2 * vibT2) * sin(2 * M_PI * vibW2 * vibT2);		//�U���v�Z

						so1->GetShape(0)->SetVibT(vibT1+hdt);
						so2->GetShape(0)->SetVibT(vibT2+hdt);
					}
				}
			}
		}
	}
}
#endif

void FWLDHapticLoop::Proxy(){
	static bool friFlag = false;
	bPic = false;

	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		if((int)iPointer->bContact.size() < NIASolids()){
			for(int i=(int)iPointer->bContact.size(); i<NIASolids(); i++){
				iPointer->bContact.push_back(false);
				contactFlag[0].push_back(false);
				contactFlag[1].push_back(false);
				oVibForce.push_back(Vec3d(0,0,0));
			}
		}
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();
		PicVibration(NIASolids());

		for(int i = 0; i < NIASolids(); i++){
			FWInteractSolid* iSolid = GetIASolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			if((int)proxy[j].size() <= NIASolids()){
				for(int k=(int)proxy[j].size(); k<NIASolids(); k++){
					proxy[j].push_back(poseSolid.Inv()*pPoint);
					proVel[j].push_back(Vec3d(0,0,0));
					pLastPoint[j].push_back(Vec3d(0,0,0));
				}
			}

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;

			double f = force_dir * interpolation_normal;		// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			if(f < 0.0){										// ���ς����Ȃ�͂��v�Z
				contactFlag[j][i] = true;
				if(contactFlag[0][i]&&contactFlag[1][i]) bPic = true;	// ���̂�c�����Ă��邩�ǂ���
				Vec3d pVibForce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->correctionSpringK;
				double D = iPointer->correctionDamperD;
				Vec3d addforce = (-K * (pPoint - (poseSolid * proxy[j][i])) - D * dvortho);
				Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;

				///�U���̌v�Z
				if(iPointer->bVibration){
					pVibForce = Vibration(iSolid, iPointer, i);
					if(bPic)	pVibForce += oVibForce[i];		// ���̂�c�����Ă����炻�̍��̂̐U������ɓ`����
				}


				// ���C�̌v�Z
				Vec3d wproxy = poseSolid * proxy[j][i];
				Vec3d posVec = pPoint - (wproxy);
				double posDot = dot(interpolation_normal,posVec);
				Vec3d tVec = posDot * interpolation_normal;
				Vec3d tanjent = posVec - tVec;
				double mu0 = cSolid->GetShape(0)->GetStaticFriction();
				double mu1 = cSolid->GetShape(0)->GetDynamicFriction();
				if(!friFlag){
					if(tanjent.norm() > abs(mu0 * posDot)) friFlag = true;
				}
				if(friFlag){
					if(tanjent.norm() > abs(mu1 * posDot)){
						dproxy = (tanjent.norm() - abs(mu1 * posDot)) * tanjent.unit();
						wproxy += dproxy;
						proxy[j][i] = poseSolid.Inv() * wproxy;
					}
				}

				double ws = iPointer->GetWorldScale();
				double ws4 = ws *ws*ws*ws;
				outForce.v() += addforce/ws4 + pVibForce;	
				outForce.w() += addtorque;

				DSTR<<outForce.v()<<std::endl;

				DisplayForce[j] = outForce.v();

				/// �v�Z�����͂����̂ɉ�����
				iPointer->interactInfo[i].mobility.force = -1 * addforce;						
				nInfo->test_force_norm = addforce.norm();
				nInfo->test_force = addforce;

			}else{
				iPointer->bContact[i] = false;
				contactFlag[j][i] = false;
				proxy[j][i] = poseSolid.Inv() * pPoint;
			}
//			DisplayProxy[0] = poseSolid * proxy[0][i];
//			DisplayProxy[1] = poseSolid * proxy[1][i];
		}

		/// �C���^�t�F�[�X�֗͂��o��
		if(iPointer->bForce){
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v(), Vec3d());
				#ifdef TORQUE
					hif->SetForce(outForce.v(), outForce.w());
				#endif
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v());
			}
		}else{
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d(), Vec3d());
				#ifdef TORQUE
					hif->SetForce(Vec3d(), Vec3d());
				#endif
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d());
			}		
		}
	}
	for(int k=0;k<NIASolids();k++){
		if(contactFlag[0][k]&&contactFlag[1][k]){
			picNum = k;
			bPic = true;
		}
	}
}

void FWLDHapticLoop::ProxySimulation(){
	static bool friFlag = false;
	Vec3d dproVel = Vec3d(0,0,0);
	bPic = false;

	for(int j = 0; j < NIAPointers(); j++){
		FWInteractPointer* iPointer = GetIAPointer(j)->Cast();
		if((int)iPointer->bContact.size() < NIASolids()){
			for(int i=(int)iPointer->bContact.size(); i<NIASolids(); i++){
				iPointer->bContact.push_back(false);
				contactFlag[0].push_back(false);
				contactFlag[1].push_back(false);
			}
		}
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
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			if((int)proxy[j].size() < NIASolids()){
				for(int k=(int)proxy[j].size(); k<NIASolids(); k++){
					proxy[j].push_back(poseSolid.Inv()*pPoint);
					proVel[j].push_back(Vec3d(0,0,0));
					pLastPoint[j].push_back(Vec3d(0,0,0));
				}
			}

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * nInfo->face_normal + 
				(syncCount - (double)loopCount) * nInfo->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = nInfo->face_normal;

			double f = force_dir * interpolation_normal;		// ���̖̂ʂ̖@���Ɠ��ς��Ƃ�
			if(f < 0.0){										// ���ς����Ȃ�͂��v�Z
				contactFlag[j][i] = true;
				Vec3d vibforce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->correctionSpringK;
				double D = iPointer->correctionDamperD;
				Vec3d addforce;
				Vec3d addtorque;
				if(dvortho.norm() < 0){
					addforce = -K * (pPoint - (poseSolid * proxy[j][i])) - D * dvortho;
					addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;
				}else{
					addforce = -K * (pPoint - (poseSolid * proxy[j][i]));
					addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;
				}

				///�U���̌v�Z
				if(iPointer->bVibration){
					vibforce = Vibration(iSolid, iPointer, i);
				}

				// ���C�̌v�Z
				Vec3d pvel = iPointer->hiSolid.GetPointVelocity(pPoint);
				Vec3d wproxy = poseSolid * proxy[j][i];
				Vec3d posVec = pLastPoint[j][i] - (wproxy);
				double posDot = dot(interpolation_normal,posVec);
				Vec3d tVec = posDot * interpolation_normal;
				Vec3d tanjent = posVec - tVec;
				Vec3d velVec = pvel - proVel[j][i];
				double velDot = dot(interpolation_normal,velVec);
				Vec3d velTan = velVec - (velDot * interpolation_normal);

				double mu0 = cSolid->GetShape(0)->GetStaticFriction();
				double mu1 = cSolid->GetShape(0)->GetDynamicFriction();
				if(!friFlag){
					if(tanjent.norm() >= abs(mu0 * posDot))		friFlag = true;
				}
				if(friFlag){
					Vec3d dPOS;
					Vec3d dVEL;
					Vec3d dFRI;
					if((tanjent.norm() == 0)&&(velTan.norm() == 0)){
						dPOS = Vec3d(0,0,0);
						dVEL = Vec3d(0,0,0);
						dFRI = Vec3d(0,0,0);
					}else if(tanjent.norm() == 0){
						dPOS = - hdt * proVel[j][i];
						dVEL = dot((pvel - proVel[j][i]),velTan.unit()) * velTan.unit();
						dFRI = Vec3d(0,0,0);
					}else if(velTan.norm() == 0){
						dPOS = dot(pPoint-wproxy,tanjent.unit()) * tanjent.unit();
						dVEL = Vec3d(0,0,0);
						dFRI = abs(posDot*proK * mu1)*tanjent.unit();
					}else{
						dPOS = dot(pPoint-wproxy,tanjent.unit()) * tanjent.unit() - hdt * proVel[j][i];
						dVEL = dot((pvel - proVel[j][i]),velTan.unit()) * velTan.unit();
						dFRI = abs(posDot*proK * mu1)*tanjent.unit();
					}
					if(proVel[j][i].norm() < 0){
						dproVel = (proK * dPOS + proD * dVEL + dFRI) * (hdt/(proM + proD*hdt + proK*hdt*hdt));
					}else{
						dproVel = (proK * dPOS + proD * dVEL - dFRI) * (hdt/(proM + proD*hdt + proK*hdt*hdt));
					}
					proVel[j][i] += dproVel;
					wproxy += proVel[j][i] * hdt;
					proxy[j][i] = poseSolid.Inv() * wproxy;

					if(proVel[j][i].norm() < 1){
						proVel[j][i] = Vec3d(0,0,0);
						friFlag = false;
					}
				}

				outForce.v() += addforce + vibforce;	
				outForce.w() += addtorque;

				DisplayForce[j] = outForce.v();

				/// �v�Z�����͂����̂ɉ�����
				iPointer->interactInfo[i].mobility.force = -1 * addforce* iPointer->GetForceScale();;						
				nInfo->test_force_norm = addforce.norm()* iPointer->GetForceScale();;
				nInfo->test_force = addforce* iPointer->GetForceScale();;
			}else{
				iPointer->bContact[i] = false;
				contactFlag[j][i] = false;
				proxy[j][i] = poseSolid.Inv() * pPoint;
				proVel[j][i] = Vec3d(0,0,0);
			}
//			DisplayProxy[0] = poseSolid * proxy[0][i];
//			DisplayProxy[1] = poseSolid * proxy[1][i];
			pLastPoint[j][i] = pPoint;

			/// �C���^�t�F�[�X�֗͂��o��
			if(iPointer->bForce){
				if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
					HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
					hif->SetForce(outForce.v(), Vec3d());
					#ifdef TORQUE
						hif->SetForce(outForce.v(), outForce.w());
					#endif
				}else{
					HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
					hif->SetForce(outForce.v());
				}
			}else{
				if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
					HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
					hif->SetForce(Vec3d(), Vec3d());
					#ifdef TORQUE
					hif->SetForce(Vec3d(), Vec3d());
					#endif
				}else{
					HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
					hif->SetForce(Vec3d());
				}		
			}
		}
	}
	for(int k=0;k<NIASolids();k++){
		if(contactFlag[0][k]&&contactFlag[1][k]){
			picNum = k;
			bPic = true;
		}
	}
}

Vec3d* FWLDHapticLoop::GetProxyPoint(){
	return DisplayProxy;
};

Vec3d* FWLDHapticLoop::GetForce(){
	return DisplayForce;
}

bool FWLDHapticLoop::GetContactFlag(){
	return bPic;
};

Vec3d FWLDHapticLoop::GetPicCenterPosition(){
	return GetIASolid(picNum)->copiedSolid.GetCenterPosition();
}
#if 0
int* FWLDHapticLoop::ContactStat(int nPic){
	PHSceneIf* phScene = GetPHScene();
	int	contactStat[phScene->NSolids()];

	PHSolidPairForLCPIf* solidPair;
	PHShapePairForLCPIf* shapePair;

	for(int i=0; i<phScene->NSolids(); ++i){
		PHSolidIf *so1, *so2;
		so1	= phScene->GetSolids()[i];
		so2	= phScene->GetSolids()[nPic];
		solidPair = phScene->GetSolidPair(i,nPic);
		PHConstraintIf* constraint = phScene->GetConstraintEngine()->GetContactPoints()->FindBySolidPair(so1, so2);

		if (!solidPair){
			int contactStat[i] = 0;
			continue;
		}
		if (!constraint){
			int contactStat[i] = 0;
			continue;
		}

		for (int s=0; s<so1->NShape(); ++s){
			for (int t=0; t<so2->NShape(); ++t){
				shapePair = solidPair->GetShapePair(s, t);

				if (!shapePair){ 
					int contactStat[i] = 0;
					continue;
				}
				int	contactStat[i]	= solidPair->GetContactState(s, t);
			}
		}
	}
	return contactStat;
}
#endif
/*
int GetPicTime();
*/
// FWLDHapticApp�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic::FWLDHaptic(){}
void FWLDHaptic::Init(){
	FWMultiRateHaptic::Init();
	hapticLoop = &LDHapticLoop;
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	InitIAAdaptee();
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
#if 0
		TestSimulation6D();
#else
		TestSimulation();
#endif
		bCalcPhys = false;
	}
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}
void FWLDHaptic::PhysicsStep(){
	std::vector<SpatialVector> lastvel;
	for(int i = 0; i < NIASolids(); i++){
		if(!GetIASolid(i)->bSim) continue;
		lastvel.resize(i + 1);
		PHSolid* phSolid = GetIASolid(i)->sceneSolid;
		lastvel.back().v() = phSolid->GetVelocity();
		lastvel.back().w() = phSolid->GetAngularVelocity();
	}
	/// �V�~�����[�V������i�߂�

	//DSTR<<"Physics�V�~�����[�V����"<<std::endl;
	GetPHScene()->Step();
	for(int i = 0; i < NIASolids(); i++){
		if(!GetIASolid(i)->bSim) continue;
		SpatialVector curvel;
		PHSolid* phSolid = GetIASolid(i)->sceneSolid;
		curvel.v() = phSolid->GetVelocity();
		curvel.w() = phSolid->GetAngularVelocity();
		double pdt = GetPHScene()->GetTimeStep();
		GetIASolid(i)->curb = (curvel - lastvel[i]) / pdt;
		//DSTR << "naga"<<curvel.w() << std::endl;
//		DSTR << GetIASolid(i)->curb << std::endl;
	}
}

void FWLDHaptic::UpdatePointer(){
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

void FWLDHaptic::TestSimulation(){
	/** FWInteractSolids��blocal��true�̕��̂ɑ΂��ăe�X�g�͂������C
		�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC���r���e�B���v�Z���� */
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
			if(iPointer->interactInfo[i].flag.blocal==false) continue;
			PHSolid* soPointer = iPointer->pointerSolid->Cast();
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			Vec3d cPoint = phSolid->GetPose() * iInfo->neighborInfo.closest_point;		// �͂�������_(���[���h���W)

			Vec3d n = -iInfo->neighborInfo.face_normal;
			Matrix3d local = CalcConstraintCoordinateSystem(soPointer, phSolid, 
				iInfo->neighborInfo.pointer_point, iInfo->neighborInfo.closest_point, n);

			/// �ڐG�_�ɉ������
			const float minTestForce = 0.5;										// �ŏ��e�X�g��
			/// �ʏ�e�X�g�͂��ŏ��e�X�g�͂������ꍇ
			if(iInfo->neighborInfo.test_force_norm < minTestForce){
				iInfo->neighborInfo.test_force_norm = minTestForce;					 
			}
			#ifdef FORCE_ELEMENT
			for(int i = 0; i < 3; i++){
				if(iInfo->neighborInfo.test_force[i] < minTestForce)
					iInfo->neighborInfo.test_force[i] = minTestForce;
			}
			#endif
			TMatrixRow<6, 3, double> u;			// ���̂̋@�B�C���s�[�_���X
			TMatrixRow<3, 3, double> force;		// �������

			#ifdef FORCE_ELEMENT
			force.col(0) = Vec3d(iInfo->neighborInfo.test_force[0], 0, 0);
			force.col(1) = Vec3d(0, iInfo->neighborInfo.test_force[1], 0);
			force.col(2) = Vec3d(0, 0, iInfo->neighborInfo.test_force[2]);
			#else
			force.col(0) = iInfo->neighborInfo.test_force_norm * n;
			force.col(1) = iInfo->neighborInfo.test_force_norm * (n + local.col(1));
			force.col(2) = iInfo->neighborInfo.test_force_norm * (n + local.col(2));
			#endif
			//DSTR<<force<<std::endl;

			//DSTR<<" �@��--------------------"<<std::endl;
			/// �@�������ɗ͂�������
			phSolid->AddForce(force.col(0), cPoint); 
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(0) = (nextvel - curvel) /pdt - inSolid->b;
			states->LoadState(phScene);
			
			//DSTR<<" n + t[0]--------------------"<<std::endl;
			/// n + t[0]�����ɗ͂�������
			phSolid->AddForce(force.col(1), cPoint);
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(1) = (nextvel - curvel) /pdt - inSolid->b;
			states->LoadState(phScene);

			//DSTR<<" n + t[1]--------------------"<<std::endl;
			/// n+t[1]�����͂�������
			phSolid->AddForce(force.col(2), cPoint);
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(2) = (nextvel - curvel) /pdt - inSolid->b;
			
			iInfo->mobility.A = u  * force.inv();			// ���r���e�BA�̌v�Z
//			DSTR <<  iInfo->mobility.A << std::endl;
			states->LoadState(phScene);						// ����state�ɖ߂��V�~�����[�V������i�߂�
		}
	}
	///--------�e�X�g�V�~�����[�V�����I��--------
	#ifdef DIVIDE_STEP
		states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
	#endif
}

void FWLDHaptic::TestSimulation6D(){
	/* 6DoF Haptic Rendering �̂��߂̑��_�ڐG�e�X�g�V�~�����[�V����*/
	/*	�͊o�|�C���^�����̂ɉ�����͂̓_������
		�_��FWInteractAdaptee::FindSectionVertex()�Ŏ擾���C
		vector< Vec3d > FWInteractInfo::neighborInfo::section�ɕۑ�����Ă���
	*/
	
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
			if(iPointer->interactInfo[i].flag.blocal==false) continue;
			PHSolid* soPointer = iPointer->pointerSolid->Cast();
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			Vec3d cPoint = phSolid->GetPose() * iInfo->neighborInfo.closest_point;		// �͂�������_(���[���h���W)
			
			/// �S�����W�n�̍쐬
			Vec3d n = -iInfo->neighborInfo.face_normal;
			Matrix3d local = CalcConstraintCoordinateSystem(soPointer, phSolid, 
				iInfo->neighborInfo.pointer_point, iInfo->neighborInfo.closest_point, n);

			/// �ڐG�_�ɉ������
			const float minTestForce = 0.5;										// �ŏ��e�X�g��
			/// �ʏ�e�X�g�͂��ŏ��e�X�g�͂������ꍇ
			if(iInfo->neighborInfo.test_force_norm < minTestForce){
				iInfo->neighborInfo.test_force_norm = minTestForce;					 
			}
			TMatrixRow<6, 3, double> u;			// ���̂̋@�B�C���s�[�_���X
			TMatrixRow<3, 3, double> force;		// �������

			force.col(0) = iInfo->neighborInfo.test_force_norm * n;
			force.col(1) = iInfo->neighborInfo.test_force_norm * (n + local.col(1));
			force.col(2) = iInfo->neighborInfo.test_force_norm * (n + local.col(2));

			/// �@�������ɗ͂�������
			std::vector< Vec3d > section;
			int neibSSize = (int)iInfo->neighborInfo.solid_section.size();
			for(int k = 0; k < neibSSize; k++){
				section.push_back(iInfo->neighborInfo.solid_section[k]);			
				phSolid->AddForce(force.col(0), section[k]); 
			}
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(0) = (nextvel - curvel) /pdt - inSolid->b;
			states->LoadState(phScene);
			
			/// n + t[0]�����ɗ͂�������
			int secSize = (int)section.size();
			for(int k = 0; k < secSize; k++){
				phSolid->AddForce(force.col(1), section[k]); 
			}
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(1) = (nextvel - curvel) /pdt - inSolid->b;
			states->LoadState(phScene);

			/// n+t[1]�����͂�������
			for(int k = 0; k < secSize; k++){
				phSolid->AddForce(force.col(2), section[k]); 
			}
			#ifdef DIVIDE_STEP
			phScene->IntegratePart2();
			#else
			phScene->Step();
			#endif
			nextvel.v() = phSolid->GetVelocity();
			nextvel.w() = phSolid->GetAngularVelocity();
			u.col(2) = (nextvel - curvel) /pdt - inSolid->b;
			
			iInfo->mobility.A = u  * force.inv();			// ���r���e�BA�̌v�Z
			states->LoadState(phScene);						// ����state�ɖ߂��V�~�����[�V������i�߂�
		}
	}
	///--------�e�X�g�V�~�����[�V�����I��--------
	#ifdef DIVIDE_STEP
		states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
	#endif
}

Matrix3d FWLDHaptic::CalcConstraintCoordinateSystem(PHSolid* pha, PHSolid* phb, Vec3d pa, Vec3d pb, Vec3d normal){
			/// �S�����W�n����邽�߂̏���
			Vec3d rajabs, vajabs;
			rajabs = pha->GetPose() * pa - pha->GetCenterPosition();				//���̂̒��S����ڐG�_�܂ł̃x�N�g��
			vajabs = pha->GetVelocity() + pha->GetAngularVelocity() % rajabs;		//�ڐG�_�ł̑��x
			Vec3d rbjabs, vbjabs;
			rbjabs = phb->GetPose() * pb - phb->GetCenterPosition();				//�͊o�|�C���^�̒��S����ڐG�_�܂ł̃x�N�g��
			vbjabs = phb->GetVelocity() + phb->GetAngularVelocity() % rbjabs;		//�ڐG�_�ł̑��x

			/// �S�����W�ϊ��s��̌v�Z
			Vec3d n, t[2], vjrel, vjrelproj;						//�ڐ��x�N�g��t[0], t[1] (t[0]�͑��Α��x�x�N�g���ɕ��s�ɂȂ�悤�ɂ���)
			n = normal;
			vjrel = vbjabs - vajabs;								// ���Α��x(����a->����b)
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
			Matrix3d local;
			local.col(0) = n;
			local.col(1) = t[0];
			local.col(2) = t[1];
			return local;

			/* ���ǔŁi�������j			
			/// �S�����W�n�̍쐬
			Vec3d v0 = phSolid->GetPointVelocity(cPoint);
			Vec3d v1 = soPointer->GetPointVelocity(pPoint);
			Matrix3d local;	//	contact coodinate system �ڐG�̍��W�n
			Vec3d normal = iInfo->neighborInfo.face_normal; 
			local.Ex() = normal;
			local.Ey() = v1-v0;
			local.Ey() -= local.Ey() * normal * normal;
			if (local.Ey().square() > 1e-6){
				local.Ey().unitize(); 
			}else{
				if (Square(normal.x) < 0.5) local.Ey()= (normal ^ Vec3f(1,0,0)).unit();
				else local.Ey() = (normal ^ Vec3f(0,1,0)).unit();
			}
			local.Ez() =  local.Ex() ^ local.Ey();
			*/
}

void FWLDHaptic::BeginKeyboard(){
	PHSceneIf* phScene = GetIAScene()->GetScene()->GetPHScene();
	states->ReleaseState(phScene);
	states2->ReleaseState(phScene);
}
void FWLDHaptic::EndKeyboard(){
	PHSceneIf* phScene = GetIAScene()->GetScene()->GetPHScene();
	for(int i = 0; i < NIAPointers(); i++){
		PHSolidIf* pointer = GetIAPointer(i)->pointerSolid;
		phScene->SetContactMode(pointer,PHSceneDesc::MODE_NONE);
	}
}

void FWLDHaptic::ReleaseState(PHSceneIf* p){
	states->ReleaseState(p);
	states2->ReleaseState(p);
}

Vec3d* FWLDHaptic::GetProxyPoint(){
	return LDHapticLoop.GetProxyPoint();
}

Vec3d* FWLDHaptic::GetForce(){
	return LDHapticLoop.GetForce();
}

bool FWLDHaptic::GetContactFlag(){
	return LDHapticLoop.GetContactFlag();
}

Vec3d FWLDHaptic::GetPicCenterPosition(){
	return LDHapticLoop.GetPicCenterPosition();
}
