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
			vcSolid[i]->SetDynamical(false);

			//vcJoint�̍쐬
			PHSpringDesc jointDesc;
			{
				jointDesc.poseSocket.Pos()	= Vec3f(0.0f, 0.0f, 0.0f);
				jointDesc.posePlug.Pos()	= Vec3f(0.0f, 0.0f, 0.0f);
				jointDesc.spring			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->correctionSpringK;
				jointDesc.damper			= Vec3f(1.0f, 1.0f, 1.0f) * GetIAPointer(i)->correctionDamperD;
				jointDesc.springOri			= GetIAPointer(i)->springK *1000;
				jointDesc.damperOri			= GetIAPointer(i)->damperD *1000;
			}
			vcJoint.push_back( GetPHScene()->CreateJoint(vcSolid[i], pSolid, jointDesc) );
		}
	}
}
void FWVirtualCoupling::UpdateInterface(){
	int N = NIAPointers();
		double pdt = GetPHScene()->GetTimeStep();
		for(int i = 0; i < N; i++){
			FWInteractPointer* iPointer = GetIAPointer(i)->Cast();
			double s = iPointer->GetPosScale();
			if(DCAST(HIForceInterface6DIf, iPointer->GetHI())){
				//6���R�x�C���^�t�F�[�X�̏ꍇ
				HIForceInterface6DIf* hif = iPointer->GetHI()->Cast();
				hif->Update((float)pdt);
				PHSolid* hiSolid = &iPointer->hiSolid;
				hiSolid->SetVelocity((Vec3d)hif->GetVelocity() * s);
				hiSolid->SetAngularVelocity((Vec3d)hif->GetAngularVelocity());
				Posed hifPose;
				hifPose.Pos()=(Vec3d)hif->GetPosition() * s;
				hifPose.Ori()=hif->GetOrientation();
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
		Posed jPose; vcJoint[i]->GetPlugPose(jPose);
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
