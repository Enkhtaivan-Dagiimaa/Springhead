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
	proK = 50;
	proD = 0.005;
	proM = 7.5*10e-7;
}
void FWLDHapticLoop::Step(){
	UpdateInterface();
	switch(hmode){
		case PENALTY:
			HapticRendering();
			break;
		case PROXY:
			Proxy();
			break;
		case PROXYSIMULATION:
			ProxySimulation();
			break;
		default:
			HapticRendering();
			break;
	}
	LocalDynamics();
}

void FWLDHapticLoop::UpdateInterface(){
	int N = NINPointers();
	for(int i = 0; i < N; i++){
		FWInteractPointer* iPointer = GetINPointer(i)->Cast();
		double s = iPointer->GetPosScale();
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
			Posed hiSolidPose = hifPose * GetINPointer(i)->GetDefaultPosition()*GetINPointer(i)->GetPointersCalibPosition();
			hiSolid->SetPose(hiSolidPose);
		}else{
			//3���R�x�C���^�t�F�[�X�̏ꍇ
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
			hif->Update((float)hdt);
			PHSolid* hiSolid = &iPointer->hiSolid;
			hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
			Posed hifPose;
			hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
			Posed hiSolidPose = hifPose * GetINPointer(i)->GetDefaultPosition()*GetINPointer(i)->GetPointersCalibPosition();
			hiSolid->SetPose(hiSolidPose);
		}
	}
}

void FWLDHapticLoop::HapticRendering(){
	for(int j = 0; j < NINPointers(); j++){
		FWInteractPointer* iPointer = GetINPointer(j)->Cast();
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();
		for(int i = 0; i < NINSolids(); i++){
			FWInteractSolid* iSolid = GetINSolid(i);
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
				Vec3d vibforce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				Vec3d friction_dir = -1 * (force_dir - ortho);	// ���C�͂��|�C���^�ɉ�������
				Vec3d friction_normal = friction_dir.unit();
				double friction_rate = friction_dir.norm() / ortho.norm();
				PTM::TMatrixRow<1,3,double> fdtrans;
				fdtrans[0][0] = friction_normal[0];
				fdtrans[0][1] = friction_normal[1];
				fdtrans[0][2] = friction_normal[2];
				Vec3d dvortho2 = dvortho - (fdtrans * dvortho)[0] * friction_normal;

				/// �R�͂̌v�Z
				double K = iPointer->springK;
				double D = iPointer->damperD;
				Vec3d addforce = -1 * (K * ortho + D * dvortho2);
				Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;

				///�U���̌v�Z
				if(iPointer->bVibration)	vibforce = Vibration(iSolid, iPointer, i);

				outForce.v() += addforce + vibforce;	
				outForce.w() += addtorque;

				/// �v�Z�����͂����̂ɉ�����//
				iPointer->interactInfo[i].mobility.force = -1 * addforce * iPointer->GetForceScale();	
				nInfo->test_force_norm = addforce.norm() * iPointer->GetForceScale();
				nInfo->test_force = addforce * iPointer->GetForceScale();
				//if(iPointer->bForce)	DSTR << vibforce << endl;
			}else{
				iSolid->sceneSolid->GetShape(0)->SetVibContact(true); 
			}
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
}

void FWLDHapticLoop::LocalDynamics(){
	for(int i = 0; i < NINSolids(); i++){
		FWInteractSolid* iSolid = FWHapticLoopBase::GetINSolid(i);
		if(!iSolid->bSim) continue;
		SpatialVector vel;
		vel.v() = iSolid->copiedSolid.GetVelocity();
		vel.w() = iSolid->copiedSolid.GetAngularVelocity();
		if(GetLoopCount() == 1){
			vel += (iSolid->curb - iSolid->lastb) *  pdt;	// �Փ˂̉e���𔽉f

		}
		for(int j = 0; j < NINPointers(); j++){
			FWInteractPointer* iPointer = GetINPointer(j);
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
	iSolid->sceneSolid->GetShape(0)->SetVibT(vibT+hdt);		// �ڐG���Ԃ̍X�V
	return vibforce;
}

void FWLDHapticLoop::Proxy(){
	static bool friFlag = false;

	for(int j = 0; j < NINPointers(); j++){
		FWInteractPointer* iPointer = GetINPointer(j)->Cast();
		if((int)iPointer->bContact.size() < NINSolids()){
			for(int i=(int)iPointer->bContact.size(); i<NINSolids(); i++){
				iPointer->bContact.push_back(false);
			}
		}
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();
		for(int i = 0; i < NINSolids(); i++){
			FWInteractSolid* iSolid = GetINSolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			if((int)proxy[j].size() <= NINSolids()){
				for(int k=(int)proxy[j].size(); k<NINSolids(); k++){
					proxy[j].push_back(poseSolid.Inv()*pPoint);
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
				Vec3d vibforce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->springK;
				double D = iPointer->damperD;
				Vec3d addforce = -K * (pPoint - (poseSolid * proxy[j][i])) - D * dvortho;
				Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;

				///�U���̌v�Z
				if(iPointer->bVibration)	vibforce = Vibration(iSolid, iPointer, i);

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

				outForce.v() += addforce + vibforce;	
				outForce.w() += addtorque;

				/// �v�Z�����͂����̂ɉ�����
				iPointer->interactInfo[i].mobility.force = -1 * addforce* iPointer->GetForceScale();						
				nInfo->test_force_norm = addforce.norm()* iPointer->GetForceScale();;
				nInfo->test_force = addforce* iPointer->GetForceScale();;

			}else{
				iPointer->bContact[i] = false;
				proxy[j][i] = poseSolid.Inv() * pPoint;
			}
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
}

void FWLDHapticLoop::ProxySimulation(){
	static bool friFlag = false;
	Vec3d dproVel = Vec3d(0,0,0);

	for(int j = 0; j < NINPointers(); j++){
		FWInteractPointer* iPointer = GetINPointer(j)->Cast();
		if((int)iPointer->bContact.size() < NINSolids()){
			for(int i=(int)iPointer->bContact.size(); i<NINSolids(); i++){
				iPointer->bContact.push_back(false);
			}
		}
		if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
			HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
		}else{
			HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
		}

		SpatialVector outForce = SpatialVector();
		for(int i = 0; i < NINSolids(); i++){
			FWInteractSolid* iSolid = GetINSolid(i);
			FWInteractInfo* iInfo = &iPointer->interactInfo[i];
			if(!iInfo->flag.blocal) continue;
			NeighborInfo* nInfo = &iInfo->neighborInfo;
			PHSolid* cSolid = &iSolid->copiedSolid;
			Posed poseSolid = cSolid->GetPose();
			Vec3d cPoint = cSolid->GetPose() * nInfo->closest_point;			// ���̂̋ߖT�_�̃��[���h���W�n
			Vec3d pPoint = iPointer->hiSolid.GetPose() * nInfo->pointer_point;	// �͊o�|�C���^�̋ߖT�_�̃��[���h���W�n
			Vec3d force_dir = pPoint - cPoint;
			Vec3d interpolation_normal;											// �񎦗͌v�Z�ɂ��悤����@���i�O��̖@���Ƃ̊Ԃ��Ԃ���j
			if((int)proxy[j].size() < NINSolids()){
				for(int k=(int)proxy[j].size(); k<NINSolids(); k++){
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
				Vec3d vibforce = Vec3d(0,0,0);
				Vec3d ortho = f * interpolation_normal;			// �ߖT�_����͊o�|�C���^�ւ̃x�N�g���̖ʂ̖@���ւ̐��ˉe
				Vec3d dv =  iPointer->hiSolid.GetPointVelocity(pPoint) - cSolid->GetPointVelocity(cPoint);
				Vec3d dvortho = dv.norm() * interpolation_normal;

				/// �R�͂̌v�Z
				double K = iPointer->springK;
				double D = iPointer->damperD;
				Vec3d addforce = -K * (pPoint - (poseSolid * proxy[j][i])) - D * dvortho;
				Vec3d addtorque = (pPoint - cSolid->GetCenterPosition()) % addforce ;


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

				/// �v�Z�����͂����̂ɉ�����
				iPointer->interactInfo[i].mobility.force = -1 * addforce* iPointer->GetForceScale();;						
				nInfo->test_force_norm = addforce.norm()* iPointer->GetForceScale();;
				nInfo->test_force = addforce* iPointer->GetForceScale();;
			}else{
				iPointer->bContact[i] = false;
				proxy[j][i] = poseSolid.Inv() * pPoint;
				proVel[j][i] = Vec3d(0,0,0);
			}
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
}

// FWLDHapticApp�̎���
//////////////////////////////////////////////////////////////////////////////////////////////
FWLDHaptic::FWLDHaptic(){}
void FWLDHaptic::Init(){
	FWMultiRateHaptic::Init();
	hapticLoop = &LDHapticLoop;
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetINScene()->hdt;
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
	double pdt = GetPHScene()->GetTimeStep();
	double hdt = GetINScene()->hdt;
	if (hapticcount < pdt / hdt) return;
	hapticcount -= (int)(pdt/hdt);
	bSync = true;
	bCalcPhys = true;
}
void FWLDHaptic::PhysicsStep(){
	std::vector<SpatialVector> lastvel;
	for(int i = 0; i < NINSolids(); i++){
		if(!GetINSolid(i)->bSim) continue;
		lastvel.resize(i + 1);
		PHSolid* phSolid = GetINSolid(i)->sceneSolid;
		lastvel.back().v() = phSolid->GetVelocity();
		lastvel.back().w() = phSolid->GetAngularVelocity();
	}
	/// �V�~�����[�V������i�߂�

	//DSTR<<"Physics�V�~�����[�V����"<<std::endl;
	GetPHScene()->Step();
	for(int i = 0; i < NINSolids(); i++){
		if(!GetINSolid(i)->bSim) continue;
		SpatialVector curvel;
		PHSolid* phSolid = GetINSolid(i)->sceneSolid;
		curvel.v() = phSolid->GetVelocity();
		curvel.w() = phSolid->GetAngularVelocity();
		double pdt = GetPHScene()->GetTimeStep();
		GetINSolid(i)->curb = (curvel - lastvel[i]) / pdt;
		//DSTR << "naga"<<curvel.w() << std::endl;
//		DSTR << GetINSolid(i)->curb << std::endl;
	}
}

void FWLDHaptic::UpdatePointer(){
	for(int i = 0; i < NINPointers(); i++){	
		if(GetHapticLoop()->NINPointers() == 0) return; 
		PHSolidIf* soPointer = GetINPointer(i)->pointerSolid;
		FWInteractPointer* hiPointer = GetHapticLoop()->GetINPointer(i)->Cast();
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
	for(int i = 0; i < NINSolids(); i++){
		if(!GetINSolid(i)->bSim) continue;
		FWInteractSolid* inSolid = GetINSolid(i);
		PHSolid* phSolid = GetINSolid(i)->sceneSolid;
		/// ���݂̑��x��ۑ�
		SpatialVector curvel, nextvel; 
		curvel.v() = phSolid->GetVelocity();			// ���݂̑��x
		curvel.w() = phSolid->GetAngularVelocity();		// ���݂̊p���x		


		//if(i==2){
		//	DSTR<<"1:"<<phSolid->GetVelocity().y<<std::endl; //naga
		//}

		//DSTR<<" �͂������Ȃ���1�X�e�b�v�i�߂�--------------------"<<std::endl;
		/// �����͂������Ȃ��ŃV�~�����[�V������1�X�e�b�v�i�߂�
		#ifdef DIVIDE_STEP
		phScene->IntegratePart2();
		#else
		//DSTR <<"V1"<< phSolid->GetVelocity() << std::endl;	//naga
		phScene->Step();
		//DSTR <<"V2"<< phSolid->GetVelocity() << std::endl;	//naga
		#endif
		nextvel.v() = phSolid->GetVelocity();
		nextvel.w() = phSolid->GetAngularVelocity();
		/// ���r���e�Bb�̎Z�o
		inSolid->lastb = inSolid->b;
		double pdt = phScene->GetTimeStep();
		inSolid->b = (nextvel - curvel) / pdt;
		//DSTR <<  inSolid->b << std::endl;	//naga
		states->LoadState(phScene);						// ���݂̏�Ԃɖ߂�

		/// InteractPointer�̐������͂�������e�X�g�V�~�����[�V�������s��
		for(int j = 0; j < NINPointers(); j++){
			FWInteractPointer* inPointer = GetINPointer(j);
			if(inPointer->interactInfo[i].flag.blocal==false)continue;
			PHSolidIf* soPointer = inPointer->pointerSolid;
			FWInteractInfo* iInfo = &inPointer->interactInfo[i];
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
				//if (!FPCK_FINITE(t[0]));//naga
				
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
			force.col(1) = iInfo->neighborInfo.test_force_norm * (n + t[0]);
			force.col(2) = iInfo->neighborInfo.test_force_norm * (n + t[1]);
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

void FWLDHaptic::BeginKeyboard(){
	PHSceneIf* phScene = GetINScene()->GetScene()->GetPHScene();
	states->ReleaseState(phScene);
	states2->ReleaseState(phScene);
}
void FWLDHaptic::EndKeyboard(){
	PHSceneIf* phScene = GetINScene()->GetScene()->GetPHScene();
	for(int i = 0; i < NINPointers(); i++){
		PHSolidIf* pointer = GetINPointer(i)->pointerSolid;
		phScene->SetContactMode(pointer,PHSceneDesc::MODE_NONE);
	}
}

void FWLDHaptic::ReleaseState(PHSceneIf* p){
	states->ReleaseState(p);
	states2->ReleaseState(p);
}


