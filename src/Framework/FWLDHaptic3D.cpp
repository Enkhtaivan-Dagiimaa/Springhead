/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWLDHaptic3D.h>
#include <Framework/SprFWApp.h>


// FWLDHaptic3DLoop�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic3DLoop::FWLDHaptic3DLoop(){
	proK = 10;
	proD = 0.001;
	proM = 7.5*10e-6;
	bPic = false;
	picNum = 0;
	for(int i=0;i<2;i++){
//		DisplayProxy[i] = Vec3d(5,0,0);
		DisplayForce[i] = Vec3d(0,0,0);
	}
}
void FWLDHaptic3DLoop::Step(){
	UpdateInterface();
	switch(hmode){
		case PENALTY3D:
			HapticRendering3D();
			LocalDynamics();
			break;
		case PENALTY:
			HapticRendering();
			LocalDynamics();
			break;
		case PROXY:
			Proxy();
			LocalDynamics();
			break;
		case PROXYSIMULATION:
			ProxySimulation();
			LocalDynamics();
			break;
		default:
			Proxy();
			LocalDynamics();
			break;
	}
}


void FWLDHaptic3DLoop::HapticRendering3D(){
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
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * th->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * th->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j

			// ���̖̂ʂ̖@�����
			// �O��̖@���ƌ��݂̖@���̊Ԃ��Ԃ��Ȃ���X�V
			double syncCount = pdt / hdt;						// �v���Z�X�̍��ݎ��Ԃ̔�
			interpolation_normal = (loopCount * th->face_normal + 
				(syncCount - (double)loopCount) * th->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = th->face_normal;

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
				Vec3d tf = -1 * addforce;
				iPointer->interactInfo[i].mobility.force = tf;
				iPointer->proxy_pose = iPointer->hiSolid.GetPose() + ortho;
				tp->test_force = tf;
			}
		}
		/// �C���^�t�F�[�X�֗͂��o��
		SetRenderedForce(iPointer->GetHI(), iPointer->bForce, outForce * iPointer->GetForceScale());
	}
}

void FWLDHaptic3DLoop::HapticRendering(){
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
			ToHaptic* th = &iInfo->toHaptic;
			ToPhysic* tp = &iInfo->toPhysic;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * th->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * th->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
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
			interpolation_normal = (loopCount * th->face_normal + 
				(syncCount - (double)loopCount) * th->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = th->face_normal;

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
				iPointer->interactInfo[i].mobility.force = tp->test_force = -1 * addforce;	
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


void FWLDHaptic3DLoop::LocalDynamics(){
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


Vec3d FWLDHaptic3DLoop::Vibration(FWInteractSolid* iSolid, FWInteractPointer* iPointer, int n){
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
void FWLDHaptic3DLoop::PicVibration(int nSolids){
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

void FWLDHaptic3DLoop::Proxy(){
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
			ToHaptic* th = &iInfo->toHaptic;
			ToPhysic* tp = &iInfo->toPhysic;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * th->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * th->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
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
			interpolation_normal = (loopCount * th->face_normal + 
				(syncCount - (double)loopCount) * th->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = th->face_normal;

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
				double ws4 = ws * ws * ws * ws;
				outForce.v() += addforce/ws4 + pVibForce;	
				outForce.w() += addtorque;

				DisplayForce[j] = outForce.v();

				/// �v�Z�����͂����̂ɉ�����
				iPointer->interactInfo[i].mobility.force = -1 * addforce;						
				tp->test_force = -1 * addforce;

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
		if((*contactFlag).size()>0){
			if(contactFlag[0][k]&&contactFlag[1][k]){
				picNum = k;
				bPic = true;
			}
		}
	}
}

void FWLDHaptic3DLoop::ProxySimulation(){
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
			ToHaptic* th = &iInfo->toHaptic;
			ToPhysic* tp = &iInfo->toPhysic;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * th->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * th->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
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
			interpolation_normal = (loopCount * th->face_normal + 
				(syncCount - (double)loopCount) * th->last_face_normal) / syncCount;															
			if(loopCount > syncCount)	interpolation_normal = th->face_normal;

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
				tp->test_force = -1 * addforce* iPointer->GetForceScale();;
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
		if((*contactFlag).size()>0){
			if(contactFlag[0][k]&&contactFlag[1][k]){
				picNum = k;
				bPic = true;
			}
		}
	}
}

Vec3d* FWLDHaptic3DLoop::GetProxyPoint(){
	return DisplayProxy;
};

Vec3d* FWLDHaptic3DLoop::GetForce(){
	return DisplayForce;
}

bool FWLDHaptic3DLoop::GetContactFlag(){
	return bPic;
};

Vec3d FWLDHaptic3DLoop::GetPicCenterPosition(){
	return GetIASolid(picNum)->copiedSolid.GetCenterPosition();
}
#if 0
int* FWLDHaptic3DLoop::ContactStat(int nPic){
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
FWLDHaptic3D::FWLDHaptic3D(){
	hapticLoop = &ldLoop3D;
}

void FWLDHaptic3D::Init(){
	FWMultiRateHaptic::Init();
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	InitIAAdaptee();
	hapticLoop->Init(pdt, hdt);
	states = ObjectStatesIf::Create();
	states2 = ObjectStatesIf::Create();
}

void FWLDHaptic3D::Clear(){
	FWMultiRateHaptic::Clear();
	states->Clear();
	states2->Clear();
}

void FWLDHaptic3D::CallBackHapticLoop(){
	GetHapticLoop()->Step();
	Sync();
}

void FWLDHaptic3D::SyncHaptic2Physic(){
	/// HapticLoop--->PhysicsLoop
	FWInteractSolids* hiss = GetHapticLoop()->GetIASolids();
	for(unsigned i = 0; i < (int)hiss->size(); i++){
		FWInteractSolid* his = GetHapticLoop()->GetIASolid(i);
		// bSim = ture ���� bfirstSim = false�Ȃ猋�ʂ𔽉f������
		if(!his->bSim || his->bfirstSim) continue;
		FWInteractSolid* pis = GetIASolid(i);

		//1. ���r���e�B�萔���Ŕ��f���x�����
		double pdt = GetPHScene()->GetTimeStep();						// physics�̍���
		SpatialVector b = (pis->b + (pis->curb - pis->lastb)) * pdt;	// ���r���e�B�萔��
		Vec3d v = his->copiedSolid.GetVelocity() + b.v();				// ���f���x
		Vec3d w = his->copiedSolid.GetAngularVelocity() + b.w();		// ���f�p���x

		//2. solid�̏�Ԃ̍X�V�iPhysics���j
		his->sceneSolid->SetVelocity(v);
		his->sceneSolid->SetAngularVelocity(w);
		his->sceneSolid->SetCenterPosition(his->copiedSolid.GetCenterPosition());
		his->sceneSolid->SetOrientation(his->copiedSolid.GetOrientation());

		//3. �e�|�C���^�������(�e�X�g��)�𓯊�
		// �Ȃ񂩉�����(10/4/13 susa)
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* hip = GetHapticLoop()->GetIAPointer(j)->Cast();
			GetIAPointer(j)->interactInfo[i].toPhysic = hip->interactInfo[i].toPhysic;
		}
	}
}

void FWLDHaptic3D::SyncPhysic2Haptic(){
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

		// 2.1. ���[�J���V�~�����[�V�����̎��s�t���O����
		his->bSim = pis->bSim;
		his->bfirstSim = pis->bfirstSim;
		if(pis->bfirstSim){
			// ���߂ăV�~�����[�V�����ΏۂɂȂ����Ƃ��̏���
			his->copiedSolid = pis->copiedSolid;	// �ŐV�̏����R�s�[
			pis->bfirstSim = false;					// �t���O���Ċ���
		}

		// 2.2. ���r���e�B�萔���̓���
		his->b = pis->b;
		his->curb = pis->curb;
		his->lastb = pis->lastb;

		// 2.3. �|�C���^���ƂɎ����̓���
		for(int j = 0; j < NIAPointers(); j++){
			FWInteractPointer* hip = GetHapticLoop()->GetIAPointer(j)->Cast();
			hip->interactInfo[i].toHaptic = GetIAPointer(j)->interactInfo[i].toHaptic;
			hip->interactInfo[i].mobility = GetIAPointer(j)->interactInfo[i].mobility;
			hip->interactInfo[i].flag = GetIAPointer(j)->interactInfo[i].flag;
			hip->bForce = GetIAPointer(j)->bForce;
			hip->bVibration = GetIAPointer(j)->bVibration;
		}
	}
}

void FWLDHaptic3D::Step(){
	if (bSync) return;
	if (bCalcPhys){
		UpdatePointer();
		PhysicsStep();
		UpdateSolidList();
		NeighborObjectFromPointer();
		TestSimulation();
		bCalcPhys = false;
	}
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetIAScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}

void FWLDHaptic3D::PhysicsStep(){
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
	}
}

void FWLDHaptic3D::UpdatePointer(){
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

		GetIAPointer(i)->proxy_pose = hiPointer->proxy_pose;
	}
}

void FWLDHaptic3D::TestSimulation(){
	/** FWInteractSolids��blocal��true�̕��̂ɑ΂��ăe�X�g�͂������C
		�ڐG���Ă��邷�ׂĂ̕��̂ɂ��āC���r���e�B���v�Z���� */
	PHSceneIf* phScene = GetPHScene();

	//#define DIVIDE_STEP
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
			Vec3d cPoint = phSolid->GetPose() * iInfo->toHaptic.closest_point;		// �͂�������_(���[���h���W)
			Vec3d n = -iInfo->toHaptic.face_normal;

			TMatrixRow<6, 3, double> u;			// ���̂̋@�B�C���s�[�_���X
			TMatrixRow<3, 3, double> force;		// �������
			for(int k = 0; k < 6; k++){
				for(int l = 0; l < 3; l++){
					u[k][l] = 0;
					if(k < 3) force[k][l] = 0;
				}
			}

			float minTestForce = 0.5;		// �ŏ��e�X�g��

//#define NORMAL
#ifdef NORMAL
			Matrix3d local = CalcConstraintCoordinateSystem(soPointer, phSolid, 
			iInfo->toHaptic.pointer_point, iInfo->toHaptic.closest_point, n);
			/// �ʏ�e�X�g�͂��ŏ��e�X�g�͂������ꍇ
			if(iInfo->toHaptic.test_force.norm() < minTestForce){
				force.col(0) = minTestForce * n;
			}else{
				force.col(0) = iInfo->toHaptic.test_force.norm() * n;			
			}
			force.col(1) = force.col(0).norm() * (n + local.col(1));
			force.col(2) = force.col(0).norm() * (n + local.col(2));
#else
			if(iInfo->toPhysic.test_force.norm() == 0){
				force.col(0) = minTestForce * n;
			}else{
				force.col(0) = iInfo->toPhysic.test_force;
				iInfo->toPhysic.test_force = Vec3d();
			}
			Vec3d base1 = force.col(0).unit();
			Vec3d base2 = Vec3d(1,0,0) - (Vec3d(1,0,0)*base1)*base1;
			if (base2.norm() > 0.1){
				base2.unitize();
			}else{
				base2 = Vec3d(0,1,0) - (Vec3d(0,1,0)*base1)*base1;
				base2.unitize();
			}
			Vec3d base3 = base1^base2;
			force.col(1) = force.col(0).norm() * (base1 + base2);
			force.col(2) = force.col(0).norm() * (base1 + base3);
#endif
		
			/// �e�X�g�͂�3�����ɉ�����	
			for(int k = 0; k < 3; k++){
				phSolid->AddForce(force.col(k), cPoint);
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

			iInfo->mobility.A = u  * force.inv();			// ���r���e�BA�̌v�Z
		}
	}
	///--------�e�X�g�V�~�����[�V�����I��--------
	#ifdef DIVIDE_STEP
		states2->LoadState(phScene);							// ����state�ɖ߂��V�~�����[�V������i�߂�
	#endif
}

void FWLDHaptic3D::BeginKeyboard(){
	PHSceneIf* phScene = GetIAScene()->GetScene()->GetPHScene();
	states->ReleaseState(phScene);
	states2->ReleaseState(phScene);
}

void FWLDHaptic3D::EndKeyboard(){
	PHSceneIf* phScene = GetIAScene()->GetScene()->GetPHScene();
	for(int i = 0; i < NIAPointers(); i++){
		PHSolidIf* pointer = GetIAPointer(i)->pointerSolid;
		phScene->SetContactMode(pointer,PHSceneDesc::MODE_NONE);
	}
}


Vec3d* FWLDHaptic3D::GetProxyPoint(){
	FWLDHaptic3DLoop* l = (FWLDHaptic3DLoop*)GetHapticLoop();  
	return l->GetProxyPoint();
}

Vec3d* FWLDHaptic3D::GetForce(){
	FWLDHaptic3DLoop* l = (FWLDHaptic3DLoop*)GetHapticLoop();
	return l->GetForce();
}

bool FWLDHaptic3D::GetContactFlag(){
	FWLDHaptic3DLoop* l = (FWLDHaptic3DLoop*)GetHapticLoop();
	return l->GetContactFlag();
}

Vec3d FWLDHaptic3D::GetPicCenterPosition(){
	FWLDHaptic3DLoop* l = (FWLDHaptic3DLoop*)GetHapticLoop();
	return l->GetPicCenterPosition();
}
