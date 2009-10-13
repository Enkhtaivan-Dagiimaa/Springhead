/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include "FWLDHapticSample.h"
#include <iostream>
#include <sstream>
#include <Framework/FWInteractScene.h>

using namespace std;

FWLDHapticSample::FWLDHapticSample(){
	bStep = true;
	bDrawInfo = false;
}

void FWLDHapticSample::Init(int argc, char* argv[]){
	/// Sdk�̏������C�V�[���̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�

	/// �`�惂�[�h�̐ݒ�
	SetGRAdaptee(TypeGLUT);									// GLUT���[�h�ɐݒ�
	GRInit(argc, argv);						// Sdk�̍쐬

	/// �`��Window�̍쐬�C������
	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWLDHapticSample";					// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();											// �E�B���h�E�̏�����
	InitCameraView();										// �J�����r���[�̏�����

	/// HumanInterface�̏�����
	InitHumanInterface();

	/// InteractScene�̍쐬
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	desc.iaMode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateIAScene(desc);									// interactScene�̍쐬

	/// �����V�~�����[�V�������鍄�̂��쐬
	BuildScene();
	BuildPointer();

	/// �^�C�}�̍쐬�C�ݒ�
	int timerId = CreateTimer(FWTimer::MM);
	SetInterval(timerId , 1);
	SetResolution(timerId , 1);
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

#if 1
	/// SPIDARG6��2��g���ꍇ
	UTRef<HISpidarGIf> spg[2];
	for(size_t i = 0; i < 2; i++){
		spg[i] = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
		if(i == 0) spg[i]->Init(&HISpidarGDesc("SpidarG6X3R"));
		if(i == 1) spg[i]->Init(&HISpidarGDesc("SpidarG6X3L"));
		AddHI(spg[i]);
	}
#else
	/// SPIDAR4D���g���ꍇ
	UTRef<HISpidar4If> spg = GetHISdk()->CreateHumanInterface(HISpidar4If::GetIfInfoStatic())->Cast();
	spg->Init(&HISpidar4Desc("SpidarR",Vec4i(1,2,3,4)));
	UTRef<HISpidar4If> spg2 = GetHISdk()->CreateHumanInterface(HISpidar4If::GetIfInfoStatic())->Cast();
	spg2->Init(&HISpidar4Desc("SpidarL",Vec4i(5,6,7,8)));
	AddHI(spg);
	AddHI(spg2);
#endif
}

void FWLDHapticSample::Reset(){
	ReleaseAllTimer();
	GetSdk()->Clear();
	ClearIAScenes();
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);	// ���݂̐ݒ�
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();					// fwScene�ɑ΂���interactscene�����
	desc.iaMode = LOCAL_DYNAMICS;								// humaninterface�̃����_�����O���[�h�̐ݒ�
	desc.hdt = 0.001;										// �}���`���[�g�̏ꍇ�̍X�V[s]
	CreateIAScene(desc);									// interactScene�̍쐬
	BuildScene();
	BuildPointer();
	GetCurrentWin()->SetScene(GetSdk()->GetScene());
	CreateAllTimer();
}

void FWLDHapticSample::TimerFunc(int id){	
	switch(id){
		case 0:{
			GetIAScene()->CallBackHapticLoop();
			break;
			   }
	}
}

void FWLDHapticSample::IdleFunc(){
	/// �V�~�����[�V������i�߂�(interactscene������ꍇ�͂��������Ă�)
	if(bStep) 	GetIAScene()->Step();
	else if (bOneStep){
			GetIAScene()->Step();
			bOneStep = false;
	}
	PostRedisplay();
}

void FWLDHapticSample::Display(){
	/// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	render->EnableRenderAxis(bDrawInfo);
	render->EnableRenderForce(bDrawInfo);
	render->EnableRenderContact(bDrawInfo);

	/// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	/// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();

	glutSwapBuffers();
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
		soFloor->GetShape(0)->SetVibration(5,80,100);
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
		soBox->GetShape(0)->SetVibration(5,80,300);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
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
			idesc.damperD = 0.01;//0.01					// haptic rendering�̃_���p�W��
			idesc.posScale = 300;					// soPointer�̉���̐ݒ�(�`�{)
			idesc.localRange = 1.0;					// LocalDynamics���g���ꍇ�̋ߖT�͈�
			if(i==0) idesc.defaultPosition =Posed(1,0,0,0,5,0,0);	// �����ʒu�̐ݒ�
			if(i==1) idesc.defaultPosition =Posed(1,0,0,0,-5,0,0);
			GetIAScene()->CreateIAPointer(idesc);	// interactpointer�̍쐬
		}
	}
}

void FWLDHapticSample::Keyboard(int key, int x, int y){
	BeginKeyboard();
	switch (key) {
		case  27: //ESC
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		case 's':
			bStep = false;
			bOneStep = true;
			DSTR << "Stepwise Execution" << endl;
			break;
		case 'a':
			bStep = true;
			DSTR << "Play" << endl;
			break;
		case 'c':
			{
				ReleaseAllTimer();
				for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
					GetIAScene()->GetIAPointer(i)->Calibration();
				}
				CreateAllTimer();
			}
			break;
		case 'f':
			{
				static bool bf = false;
				bf = !bf;
				for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
					GetIAScene()->GetIAPointer(i)->EnableForce(bf);
					if(!bf){
						HIBaseIf* hib = GetIAScene()->GetIAPointer(i)->GetHI();
						if(DCAST(HIForceInterface6DIf, hib)){
							HIForceInterface6DIf* hif = hib->Cast();
							hif->SetForce(Vec3d(), Vec3d());
						}
						if(DCAST(HIForceInterface3DIf, hib)){
							HIForceInterface3DIf* hif = hib->Cast();
							hif->SetForce(Vec3d());
						}
					}
				}
				if(bf){
					DSTR << "Enable Force Feedback" << std::endl;
				}else{
					DSTR << "Disable Force Feedback" << std::endl;
				}
			}
			break;
		case 'o':
			{
				static bool bv = false;
				bv = !bv;
				for(int i = 0; i < GetIAScene()->NIAPointers(); i++){
					GetIAScene()->GetIAPointer(i)->EnableVibration(bv);
				}
				if(bv){
					DSTR << "Enable Vibration Feedback" << std::endl;
				}else{
					DSTR << "Disable Vibration Feedback" << std::endl;
				}
			}
			break;
		case 'j':
			{
					GetIAScene()->SetHMode(PENALTY);
					DSTR << "PENALTY MODE" << std::endl;
					break;
			}
		case 'k':
			{
					GetIAScene()->SetHMode(PROXY);
					DSTR << "PROXY MODE" << std::endl;
					break;
			}
		case 'l':
			{
					GetIAScene()->SetHMode(PROXYSIMULATION);
					DSTR << "PROXYSIMULATION MODE" << std::endl;
					break;
			}
		default:
			break;
	}
	EndKeyboard();
}
