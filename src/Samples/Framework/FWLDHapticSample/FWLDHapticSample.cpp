#include "FWLDHapticSample.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

using namespace std;

FWLDHapticSample::FWLDHapticSample(){
}
void FWLDHapticSample::InitCameraView(){
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWLDHapticSample::InitHumanInterface(){
	/// HISdk�̍쐬
	CreateHISdk();

	DRUsb20SimpleDesc usbSimpleDesc;
	GetHISdk()->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
	DRUsb20Sh4Desc usb20Sh4Desc;
	for(int i=0; i<10; ++i){
		usb20Sh4Desc.number = i;
		GetHISdk()->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
	}
	GetHISdk()->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());
	GetHISdk()->Init();
	GetHISdk()->Print(DSTR);
<<<<<<< .mine
#if 0
=======

>>>>>>> .r4330
	/// SPIDARG6��2��g���ꍇ
	UTRef<HISpidarGIf> spg[2];
	for(size_t i = 0; i < 2; i++){
		spg[i] = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
		if(i == 0) spg[i]->Init(&HISpidarGDesc("SpidarG6X3R"));
		if(i == 1) spg[i]->Init(&HISpidarGDesc("SpidarG6X3L"));
		AddHI(spg[i]);
	}
}
void FWLDHapticSample::BuildPointer(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	/// �|�C���^
	{	
		for(int i= 0; i < 2; i++){
			PHSolidIf* soPointer = phscene->CreateSolid(desc);
			CDSphereDesc sd;
			sd.radius = 0.5;//1.0;
			CDSphereIf* shapePointer = DCAST(CDSphereIf,  GetSdk()->GetPHSdk()->CreateShape(sd));
			soPointer->AddShape(shapePointer);
			soPointer->SetDynamical(false);
			soPointer->GetShape(0)->SetStaticFriction(1.0);
			soPointer->GetShape(0)->SetDynamicFriction(1.0);
			GetSdk()->GetScene()->GetPHScene()->SetContactMode(soPointer, PHSceneDesc::MODE_NONE);
			FWInteractPointerDesc idesc;			// interactpointer�̃f�B�X�N���v�^
			idesc.pointerSolid = soPointer;			// soPointer��ݒ�
			idesc.humanInterface = GetHI(i);		// humaninterface��ݒ�
			idesc.springK = 10;//0.8						// haptic rendering�̃o�l�W��
			idesc.damperD = 0.1;//0.01					// haptic rendering�̃_���p�W��
			idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
			idesc.localRange = 1.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
			if(i==0) idesc.position =Posed(1,0,0,0,5,0,0);	// �����ʒu�̐ݒ�
			if(i==1) idesc.position =Posed(1,0,0,0,-5,0,0);
			GetINScene()->CreateINPointer(idesc);	// interactpointer�̍쐬
		}
	}
}

void FWLDHapticSample::BuildScene(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	/// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
	{
		/// ����(soFloor)�̍쐬
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		/// �`��(shapeFloor)�̍쐬
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		/// ���̂Ɍ`���t������
		soFloor->AddShape(shapeFloor);
		soFloor->GetShape(0)->SetVibration(10,80,100);
		soFloor->SetFramePosition(Vec3d(0, -10, 0));
	}

	/// ��(�����@���ɏ]���C�^�����ω�)
	{
		/// ����(soBox)�̍쐬
		desc.mass = 0.05;
		desc.inertia = 0.333 * Matrix3d::Unit();
		PHSolidIf* soBox = phscene->CreateSolid(desc);
		/// �`��(shapeBox)�̍쐬
		bd.boxsize = Vec3f(4,4,4);
		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
		/// ���̂Ɍ`���t��
		soBox->AddShape(shapeBox);
		soBox->GetShape(0)->SetStaticFriction(2.0);
		soBox->GetShape(0)->SetDynamicFriction(1.0);
		soBox->GetShape(0)->SetVibration(10,80,300);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
	}
}


void FWLDHapticSample::Keyboard(int key, int x, int y){
<<<<<<< .mine
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		case 'c':
			{
				MTimerRelease();
				for(int i = 0; i < GetINScene()->NINPointers(); i++){
					GetINScene()->GetINPointer(i)->Calibration();
				}
				MTimerStart();
			}
			break;
		case 'f':
			{
				static bool bf = false;
				bf = !bf;
				for(int i = 0; i < GetINScene()->NINPointers(); i++){
					GetINScene()->GetINPointer(i)->EnableForce(bf);
				}
				if(bf){
					DSTR << "Enable Force Feedback" << std::endl;
				}else{
					DSTR << "Disable Force Feedback" << std::endl;
				}
			}
			break;
		//case 'o':
		//	{
		//		static bool bv = false;
		//		bv = !bv;
		//		for(int i = 0; i < GetINScene()->NINPointers(); i++){
		//			GetINScene()->GetINPointer(i)->EnableVibration(bv);
		//		}
		//		if(bv){
		//			DSTR << "Enable Vibration Feedback" << std::endl;
		//		}else{
		//			DSTR << "Disable Vibration Feedback" << std::endl;
		//		}
		//	}
		//	break;
		default:
			break;
	}
=======
	FWAppHaptic::Keyboard(key , x, y);
>>>>>>> .r4330
}
