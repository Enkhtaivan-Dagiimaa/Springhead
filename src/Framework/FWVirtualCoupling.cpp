/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include <Framework/FWVirtualCoupling.h>
#include <Framework/SprFWApp.h>
#include <Physics/PHConstraintEngine.h>
#include <sstream>

namespace Spr{;

FWVirtualCoupling::FWVirtualCoupling(){
}
void FWVirtualCoupling::CreateVCPointer(){
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
void FWVirtualCoupling::GrabSolid(){
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
										{
											jointDesc.poseSocket.Pos()	= Vec3d(0,0,0);
											jointDesc.poseSocket.Ori()	= grabSolid->GetPose().Ori();
											jointDesc.posePlug.Pos()	= cPointG;
											jointDesc.spring			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->springK;
											jointDesc.damper			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->damperD;
											jointDesc.springOri			= GetIAPointer(i)->springOriK;
											jointDesc.damperOri			= GetIAPointer(i)->damperOriD;
										}
										grabJoint.push_back( GetPHScene()->CreateJoint(vcSolid[i], grabSolid, jointDesc) );
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

		}else if(GetIAPointer(i)->GetGrabFlag()==3){
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
void FWVirtualCoupling::UpdateInterface(){
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
				hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
				hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
				Posed hifPose;
				hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
				hifPose.Ori()=hif->GetOrientation(); //naga
				Posed hiSolidPose = hifPose * GetIAPointer(i)->GetDefaultPosition()*GetIAPointer(i)->GetPointersCalibPosition();
				hiSolid->SetPose(hiSolidPose);
			}else{
				//3���R�x�C���^�t�F�[�X�̏ꍇ
				HIForceInterface3DIf* hif = iPointer->GetHI()->Cast();
				hif->Update((float)pdt);
				PHSolid* hiSolid = &iPointer->hiSolid;
				hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
				Posed hifPose;
				hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
				Posed hiSolidPose = hifPose * GetIAPointer(i)->GetDefaultPosition()*GetIAPointer(i)->GetPointersCalibPosition();
				hiSolid->SetPose(hiSolidPose);
			}
		}
}
void FWVirtualCoupling::UpdatePointerDirect(){
	//���ڃ|�C���^���X�V����
	int N = NIAPointers();
	for(int i = 0; i<N ; i++){
		PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		PHSolid* hiSolid = &iPointer->hiSolid;					//�C���^�t�F�[�X�̈ʒu���i�[����PHSolid
		pSolid->SetPose(hiSolid->GetPose());

	}
}

void FWVirtualCoupling::UpdatePointer(){
	//�o�[�`�����J�b�v�����O�Ń|�C���^���X�V����
	CreateVCPointer(); //VC�|�C���^���쐬����Ă��Ȃ��ꍇ�쐬����D
	int N = NIAPointers();
	for(int i = 0; i<N ; i++){
		PHSolidIf* pSolid = GetIAPointer(i)->GetPointerSolid(); //�����V�~�����[�V�����̃|�C���^
		FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
		PHSolid* hiSolid = &iPointer->hiSolid;					//�C���^�t�F�[�X�̈ʒu���i�[����PHSolid
		vcSolid[i]->SetPose(hiSolid->GetPose());				//vcSolid�̈ʒu���C���^�t�F�[�X�̈ʒu�ɂ���

		//�W���C���g�ɓ�����(world���W�n)���擾
		SpatialVector outForce = SpatialVector();
		Posed jPose; vcJoint[i]->GetSocketPose(jPose);
		vcJoint[i]->GetConstraintForce(outForce.v(),outForce.w());
		double s = iPointer->GetForceScale();
		outForce.v() = jPose * outForce.v() * (-s);
		outForce.w() = jPose * outForce.w() * (-s);

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

void FWVirtualCoupling::Step(){
	UpdateInterface();
	UpdatePointer();
	//UpdatePointerDirect();
	//GetPHScene()->Step();�@//LDHaptic�ƕ��p�ł���悤�ɂ��邽�߁CStep()�͎����ŌĂԂ��Ƃɂ���
}

void FWVirtualCoupling::Clear(){
	bSync = false;
	bCalcPhys = true;
	hapticcount = 1;
	vcSolid.clear();
	vcJoint.clear();
}


}
