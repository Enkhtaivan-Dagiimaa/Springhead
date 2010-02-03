/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWGrabCoupling.h>
#include <Framework/SprFWApp.h>
#include <Physics/PHConstraintEngine.h>
#include <sstream>

namespace Spr{;

FWGrabCoupling::FWGrabCoupling(){
	cluchFlag = false;
	cluchTrans = Posed();
}
void FWGrabCoupling::CreatePointerSolid(){
	bool grabFlag = true;

	if(grabFlag){
		if(vcSolid.size()== 0 ){
			int N = NIAPointers();
			for(int i = 0; i < N; i++){
				PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
				pSolid->SetMass(1e30f);
				pSolid->SetInertia(1e30f*Matrix3d().Unit());
				pSolid->SetGravity(false);
				pSolid->SetIntegrate(false);
				pSolid->SetDynamical(false);
				GetPHScene()->SetContactMode(pSolid,PHSceneDesc::MODE_NONE);

			}
		}
	}else{
		if(vcSolid.size()== 0 ){
			int N = NIAPointers();
			for(int i = 0; i < N; i++){
				PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
				
				//vcSolid�̍쐬(�C���^�t�F�[�X�ɓ������ē���shape�̂Ȃ��|�C���^)
				PHSolidDesc desc;
				desc.mass = 1e20f;
				desc.inertia *= 1e30f;
				vcSolid.push_back( GetPHScene()->CreateSolid(desc) );
				vcSolid[i]->SetGravity(false);
				vcSolid[i]->SetIntegrate(false);
				vcSolid[i]->SetDynamical(false);

				//vcJoint�̍쐬
				PHSpringDesc jointDesc;
				{
					jointDesc.poseSocket.Pos()	= Vec3f(0.0f, 0.0f, 0.0f);
					jointDesc.posePlug.Pos()	= Vec3f(0.0f, 0.0f, 0.0f);
					jointDesc.spring			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->springK;
					jointDesc.damper			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->damperD;
					jointDesc.springOri			= GetIAPointer(i)->springOriK;
					jointDesc.damperOri			= GetIAPointer(i)->damperOriD;
				}
				vcJoint.push_back( GetPHScene()->CreateJoint(vcSolid[i], pSolid, jointDesc) );
			}
		}
	}
}
void FWGrabCoupling::GrabSolid(){
	//�|�C���^�̐ڐG���肩�画�肷����@
	int N = NIAPointers();
	for(int i = 0; i < N; i++){
		if(GetIAPointer(i)->GetGrabFlag()==1){
			PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
			int NSolids = GetPHScene()->NSolids();
			////Pointer�ɐڐG���Ă��鍄�̂̎擾
			PHSolidPairForLCPIf* pair;						// ���̂̃y�A�������ϐ�pair
			int contact;									// �ڐG����̌���0�����傫���ꍇ�ڐG
			PHSolidIf* grabSolid;							// �ڐG���Ă��鍄��
			Vec3d cPoint;									// ���̂��ڐG���Ă���ʒu(World���W�n)
			Vec3d cPointG;									// �͂ލ��̍��W�n�ł̐ڐG�_
			Vec3d cPointP;									// �|�C���^���W�n�ł̐ڐG�_
			PHConstraintIf* param;
			bool createdFlag =false;

			////�|�C���^���̂ƐڐG���Ă��鍄�̂̃y�A������
			for(int m=0; m< NSolids&&!createdFlag; m++){
				for(int n=m+1; n<NSolids&&!createdFlag; n++){
					// (m, n)�Ԗڂ̍��̃y�A�ւ̃|�C���^���擾���āC�����ō����pair�ɑ��
					pair =GetPHScene()->GetSolidPair(m, n);
					// �|�C���^�ƍ��̂̐ڐG�𔻒�
					for(int pairN = 0; pairN<2&&!createdFlag ;pairN++){     //�|�C���^��m�Ԗ�or n�Ԗڂɂɂ���̂�2�񔻒肷��.
						if(pSolid == pair->GetSolid(pairN)){  //m�Ԗ�,n�Ԗڂ̂ǂ��炩�̍��̂��|�C���^�̏ꍇ����������Ȃ�
							// (m, n)�Ԗڂ̍��̃y�A�̐ڐG����@contact>0�̂Ƃ��ڐG
							for(int si=0; si<pair->GetSolid(0)->NShape()&&!createdFlag; si++){
								for(int sl=0; sl<pair->GetSolid(1)->NShape() &&!createdFlag; sl++){
									contact = pair->GetContactState(si, sl);
									param = GetPHScene()->GetConstraintEngine()->GetContactPoints()->FindBySolidPair(pair->GetSolid(0), pair->GetSolid(1));
									if(contact>0&&param){
										DSTR<<"CreateGrabPointer---------------------------"<<std::endl;
										DSTR<<pair->GetSolid(0)->GetName()<<"::"<<pair->GetSolid(1)->GetName()<<std::endl;
										cPoint = pair->GetCommonPoint(si, sl);
										DSTR<<"cPoint:"<<cPoint<<std::endl;
										grabSolid = pair->GetSolid(1-pairN);
										if(pairN==0){ //1�Ԗڂ��|�C���^�̏ꍇ
											cPointG = grabSolid->GetShapePose(sl)* Vec3d(0,0,0);//�͂ލ��̍��W�n�ł�Plug�̈ʒu
											cPointP = pSolid->GetShapePose(si) * Vec3d(0,0,0);  //�|�C���^���W�n�ł�Soket�̈ʒu
										}
										if(pairN==1){ //2�Ԗڂ��|�C���^�̏ꍇ
											cPointG = grabSolid->GetShapePose(si)* Vec3d(0,0,0);//�͂ލ��̍��W�n�ł�Plug�̈ʒu
											cPointP = pSolid->GetShapePose(sl) * Vec3d(0,0,0);  //�|�C���^���W�n�ł�Soket�̈ʒu
										}
										DSTR<<"cPointG"<<cPointG<<std::endl;
										//vcJoint�̍쐬
										PHSpringDesc jointDesc;
										double coefficient = 1; //�o�l��ڑ������u�Ԃ͌W���������ݒ肵�Ȃ��Ɣ��U���Ă��܂�
										{
											jointDesc.poseSocket.Pos()	= Vec3d(0,0,0);
											jointDesc.poseSocket.Ori()	= grabSolid->GetPose().Ori();
											jointDesc.posePlug.Pos()	= cPointG;

											jointDesc.spring			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->springK * coefficient;
											jointDesc.damper			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->damperD* coefficient;
											jointDesc.springOri			= GetIAPointer(i)->springOriK* coefficient;
											jointDesc.damperOri			= GetIAPointer(i)->damperOriD* coefficient;
										}
										grabJoint.push_back( GetPHScene()->CreateJoint(vcSolid[i], grabSolid, jointDesc) );
										grabJoint[0]->SetName("grabJoint");
										//�͂񂾍��̂̈ʒu���|�C���^�̈ʒu�Ɉړ�
										grabSolid->SetPose(pSolid->GetPose());
										//Pointer���̂̐ڐG�������
										GetPHScene()->SetContactMode(GetIAPointer(i)->GetPointerSolid(),PHSceneDesc::MODE_NONE);
										createdFlag = true;
										continue;
									}
								}
							}
						}
					}
				}
			}
			//Joint�̍쐬�I��
			if(createdFlag)GetIAPointer(i)->SetGrabFlag(2);
			else GetIAPointer(i)->SetGrabFlag(0);

		}else if(GetIAPointer(i)->GetGrabFlag()==4){
			//Pointer���̂̐ڐG����𕜊�
			GetPHScene()->SetContactMode(GetIAPointer(i)->GetPointerSolid(),PHSceneDesc::MODE_LCP);
			for(size_t i=0;i<grabJoint.size() ;i++){
				grabJoint[i]->Enable(false);
				grabJoint[i]->Clear();
			}
			grabJoint.clear();
			GetIAPointer(i)->SetGrabFlag(0);
		}

	}
}

void FWGrabCoupling::GrabSolid2(){
	//�|�C���^�̈ʒu��񂩂画�肷����@
	int N = NIAPointers();
	for(int i = 0; i < N; i++){
		if(GetIAPointer(i)->GetGrabFlag()==1){
			PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
			int NSolids = GetPHScene()->NSolids();
			////Pointer�ɐڐG���Ă��鍄�̂̎擾
			PHSolidIf* grabSolid = NULL;							// �ڐG���Ă��鍄��
			Posed pose = pSolid->GetPose();
			bool createdFlag =false;

			double length =1e30;
			////�|�C���^�̃����W���̍��̂�����
			PHSolidIf** solids = GetPHScene()->GetSolids();
			for(int n=0; n< NSolids&&!createdFlag; n++){
				//---
				std::string centerName;
				centerName= "soCenter";
				if(solids[n]->GetName() == centerName)continue;
				//---
				//if(!solids[n]->IsDynamical())continue;
				Posed shapePose = solids[n]->GetShapePose(0);
				Posed spose = solids[n]->GetPose() * shapePose;
				Vec3d lPose = spose.Pos() - pose.Pos();
				if(length > lPose.norm()) {
					length = lPose.norm();
					if(length <GetIAPointer(i)->localRange && solids[n]!=pSolid){
						grabSolid =solids[n]; //�|�C���^�ߖT�̍���
					}
				}
			}
			
			///�W���C���g�̍쐬
			if(grabSolid){
				vcSolid.push_back(grabSolid);
				DSTR<<"grabSolid: "<<grabSolid->GetName()<<std::endl;
				grabSolid->GetDesc(&grabSolidDesc);
				grabSolid->SetDynamical(true);
				grabSolid->SetIntegrate(true);
				

				//Axis�\���p�̍��W�ϊ�Pose��ۑ�
				grabPointerPose = pSolid->GetPose().Inv() * grabSolid->GetPose();
				Posed shapePose = grabSolid->GetShapePose(0);
				PHSpringDesc jointDesc;
				double coefficient = 1; //�o�l��ڑ������u�Ԃ͌W���������ݒ肵�Ȃ��Ɣ��U���Ă��܂�
				{
					jointDesc.poseSocket.Pos()	= Vec3d(0,0,0);
					jointDesc.poseSocket.Pos()	= Vec3d(0,0,0);
					jointDesc.poseSocket.Ori()	= pSolid->GetPose().Ori().Inv();
					//jointDesc.posePlug.Pos()	= shapePose * Vec3d(0,0,0);
					Posed plug =grabSolid->GetPose().Inv()*pSolid->GetPose();
					jointDesc.posePlug.Pos()	= plug.Pos();
					jointDesc.posePlug.Ori()	= grabSolid->GetShapePose(0).Ori().Inv() * grabSolid->GetPose().Ori().Inv();
					jointDesc.spring			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->springK * coefficient;
					jointDesc.damper			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->damperD * coefficient;
					jointDesc.springOri			= GetIAPointer(i)->springOriK* coefficient;
					jointDesc.damperOri			= GetIAPointer(i)->damperOriD* coefficient;
					jointDesc.fMax				= 15.0;
				}
				//DSTR<<grabSolid->GetName()<<std::endl;
				std::string name[8];
				name[0]= "soRightUpperArm";
				name[1]= "soLeftUpperArm";
				name[2]= "soRightUpperKnee";
				name[3]= "soLeftUpperKnee";
				name[4]= "soWaist";
				name[5]= "soBreast";
				name[6]= "soHead";
				for(int i=0; i<6; i++){
					if(grabSolid->GetName() == name[i]){
						jointDesc.springOri			= 0.0;
						jointDesc.damperOri			= 0.0;
					}
				}
				grabJoint.push_back( GetPHScene()->CreateJoint(GetIAPointer(i)->GetPointerSolid(), grabSolid, jointDesc) );
				//�͂񂾍��̂̈ʒu���|�C���^�̈ʒu�Ɉړ�
				pSolid->SetPose(grabSolid->GetPose());

				
				//Pointer���̂̐ڐG�������
				GetPHScene()->SetContactMode(GetIAPointer(i)->GetPointerSolid(),PHSceneDesc::MODE_NONE);
				createdFlag = true;
				//�|�C���^��Shape���폜
				//pointerShape = pSolid->GetShape(0);
				//pSolid->ClearShape();
				//continue;
			}

			//Joint�̍쐬�I��
			if(createdFlag)GetIAPointer(i)->SetGrabFlag(2);
			else GetIAPointer(i)->SetGrabFlag(0);

		}else if(GetIAPointer(i)->GetGrabFlag()==4){
			for(int i=0;i<grabJoint.size() ;i++){
				grabJoint[i]->Enable(false);
				grabJoint[i]->Clear();
			}
			for(int i=0;i<vcSolid.size() ;i++){
				//vcSolid[i]->SetDynamical(grabSolidDesc.dynamical);
				//vcSolid[i]->SetIntegrate(grabSolidDesc.integrate);
				vcSolid[i]->SetVelocity(Vec3d(0.0,0.0,0.0));
				vcSolid[i]->SetAngularVelocity(Vec3d(0.0,0.0,0.0));
			}
			vcSolid.clear();
			grabJoint.clear();
			GetIAPointer(i)->SetGrabFlag(0);
			//GetIAPointer(i)->GetPointerSolid()->AddShape(pointerShape);
		}

	}
}
void FWGrabCoupling::AdjustSpring(double n){
///// ���̂ƃ|�C���^�ԂŃW���C���g���쐬�����u�Ԃ̓o�l�W���������ݒ肵�Ȃ��Ɣ��U���Ă��܂��D
///// �����ŁC�ڑ���ɐݒ肵���o�l�W���ɕύX���s��
///// ���݂͒P��̃|�C���^�݂̂ɑΉ�
//
//	for(int i =0; i < grabJoint.size(); i++){
//		PHSpringIf* j = grabJoint[i]->Cast();
//		j->SetSpring(Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(0)->springK * n);
//		j->SetDamper(Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(0)->damperD * n);
//		j->SetSpringOri(GetIAPointer(0)->springOriK * n);
//		j->SetDamperOri(GetIAPointer(0)->damperOriD * n);
//	}
//
//	for(int i =0; i < vcSolid.size(); i++){
//		PHSolidIf* s = vcSolid[i]->Cast();
//		s->SetDynamical(true);
//	}
//
//	
}

void FWGrabCoupling::UpdateInterface(){

	
	int N = NIAPointers();
		double pdt = GetPHScene()->GetTimeStep();
		for(int i = 0; i < N; i++){
			FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
			double s = iPointer->GetWorldScale() * iPointer->GetPosScale();
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				//6���R�x�C���^�t�F�[�X�̏ꍇ
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->Update((float)pdt);
				PHSolid* hiSolid = &iPointer->hiSolid;
				Posed hifPose;

				hifPose.Pos() = (Vec3d)hif->GetPosition() * s;
				hifPose.Ori() = hif->GetOrientation();
				Posed newCameraPose; 
				newCameraPose.Ori() = GetIAPointer(i)->GetCameraOri();

				//Posed hiSolidPose = GetIAPointer(i)->GetDefaultPosition()*hifPose;
				Posed origin = GetIAPointer(i)->GetCameraOrigin();
				
				Posed hiSolidPose = GetIAPointer(i)->GetDefaultPosition() * GetIAPointer(i)->cameraPose * hifPose * origin;
				origin  = hifPose.Inv() * newCameraPose.Inv() * GetIAPointer(i)->cameraPose * hifPose * origin;
				GetIAPointer(i)->SetCameraOrigin(origin);
				GetIAPointer(i)->cameraPose = newCameraPose;

				//hiSolidPose =	cameraPose*hiSolidPose; //�J�����̈ړ��ɑ΂��Ĉړ�������ύX
				if(!cluchFlag){
					hiSolid->SetPose(cluchTrans * hiSolidPose);
					hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
					hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
					cluchPose[0] = cluchTrans * hiSolidPose;
				}else{
					//�N���b�`��
					cluchPose[1] = hiSolidPose;
					cluchTrans = cluchPose[0] * cluchPose[1].Inv();
				}
			}else{
				//3���R�x�C���^�t�F�[�X�̏ꍇ
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->Update((float)pdt);
				PHSolid* hiSolid = &iPointer->hiSolid;
				hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
				Posed hifPose;
				hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
				Posed hiSolidPose = GetIAPointer(i)->GetDefaultPosition()*GetIAPointer(i)->GetPointersCalibPosition() * hifPose;
				hiSolid->SetPose(hiSolidPose);
			}
		}
}
void FWGrabCoupling::UpdatePointerDirect(){
	//���ڃ|�C���^���X�V����
	int N = NIAPointers();
	
	for(int i = 0; i<N ; i++){
		PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		PHSolid* hiSolid = &iPointer->hiSolid;					//�C���^�t�F�[�X�̈ʒu���i�[����PHSolid

		pSolid->SetPose(hiSolid->GetPose());
			
	}
}

void FWGrabCoupling::UpdateGrabPointer(){
	//�o�[�`�����J�b�v�����O�Ń|�C���^���X�V����
	int N = NIAPointers();
	SpatialVector outForce;
	FWInteractPointer* iPointer;
	for(int i = 0; i<N ; i++){
			PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
			iPointer = GetIAPointer(i)->Cast();
			PHSolid* hiSolid = &iPointer->hiSolid;					//�C���^�t�F�[�X�̈ʒu���i�[����PHSolid
			hiSolid->SetPose(hiSolid->GetPose());				//vcSolid�̈ʒu���C���^�t�F�[�X�̈ʒu�ɂ���

			//�W���C���g�ɓ�����(world���W�n)���擾
			outForce = SpatialVector();
			Posed jPose;
			Posed cPose = iPointer->GetCameraOrigin();
		if(GetIAPointer(i)->GetGrabFlag()==3){
			//GrabSolid�̏ꍇ
			grabJoint[i]->GetSocketPose(jPose);
			grabJoint[i]->GetConstraintForce(outForce.v(),outForce.w());

			double fs = iPointer->GetForceScale();
			double ts = iPointer->GetTorqueScale();
			outForce.v() = jPose * cPose* outForce.v() * (-fs);
			outForce.w() = jPose * cPose*  outForce.w() * (-ts);
		}else{
			outForce.v() = Vec3d(0.0,0.0,0.0);
			outForce.w() = Vec3d(0.0,0.0,0.0);
		}

		double fRange = 1.0;

		if(outForce.v().norm() > fRange){
			for(int i=0; i<3 ; i++){
				outForce.v()[i] = outForce.v()[i] * fRange / outForce.v().norm();
			}
		}

		double fRotRange = 1.0;
		if(outForce.w().norm() > fRotRange){
			for(int i=0; i<3 ; i++){
				outForce.w()[i] = outForce.w()[i] * fRotRange / outForce.w().norm();
			}
		}
		//outForce.v() = Vec3d(0.0,0.0,0.0);

		//DSTR<<outForce<<std::endl;

		/// �C���^�t�F�[�X�֗͂��o��
		if(iPointer->bForce){
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v(), outForce.w());
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v());
			}
		}else{
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d(), Vec3d());
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d());
			}		
		}
	}
	UpdatePointerDirect();
}

void FWGrabCoupling::UpdatePointer(){
	//�o�[�`�����J�b�v�����O�Ń|�C���^���X�V����
	//CreateVCPointer(); //VC�|�C���^���쐬����Ă��Ȃ��ꍇ�쐬����D
	int N = NIAPointers();
	for(int i = 0; i<N ; i++){
		PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		PHSolid* hiSolid = &iPointer->hiSolid;					//�C���^�t�F�[�X�̈ʒu���i�[����PHSolid
		vcSolid[i]->SetPose(hiSolid->GetPose());				//vcSolid�̈ʒu���C���^�t�F�[�X�̈ʒu�ɂ���

		//�W���C���g�ɓ�����(world���W�n)���擾
		SpatialVector outForce = SpatialVector();
		Posed jPose;
		Posed cPose ;cPose.Ori()= iPointer->GetCameraOri();

		if(GetIAPointer(i)->GetGrabFlag()==3){
			//GrabSolid�̏ꍇ
			grabJoint[i]->GetSocketPose(jPose);
			grabJoint[i]->GetConstraintForce(outForce.v(),outForce.w());
		}else{
			vcJoint[i]->GetSocketPose(jPose);
			vcJoint[i]->GetConstraintForce(outForce.v(),outForce.w());
		}
		double s = iPointer->GetForceScale();
		outForce.v() = cPose.Inv() *jPose * outForce.v() * (-s);
		outForce.w() = cPose.Inv() *jPose * outForce.w() * (-s);

		DSTR<<jPose * outForce.v() * (-s)<<" �ϊ��� "<<cPose *jPose * outForce.v() * (-s)<<std::endl;

		/// �C���^�t�F�[�X�֗͂��o��
		if(iPointer->bForce){
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v(), outForce.w());
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(outForce.v());
			}
		}else{
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d(), Vec3d());
			}else{
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->SetForce(Vec3d());
			}		
		}

	}
}

void FWGrabCoupling::Step(){
	UpdateInterface();
	//UpdatePointer();
	UpdateGrabPointer();
	//UpdatePointerDirect();
	//GetPHScene()->Step();�@//LDHaptic�ƕ��p�ł���悤�ɂ��邽�߁CStep()�͎����ŌĂԂ��Ƃɂ���
}

void FWGrabCoupling::Clear(){
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
	vcSolid.clear();
	vcJoint.clear();
}


}
