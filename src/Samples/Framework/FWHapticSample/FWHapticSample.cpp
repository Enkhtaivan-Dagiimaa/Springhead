/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

#include "FWHapticSample.h"	

using namespace Spr;

FWHapticSample::FWHapticSample(){
	pdt = 0.02;
	hdt = 0.001;
	engineType = LD;
	humanInterface = XBOX;
}

void FWHapticSample::BuildScene(){
		PHSdkIf* phSdk = GetSdk()->GetPHSdk();				// �V�F�C�v�쐬�̂��߂�PHSdk�ւ̃|�C���^���Ƃ��Ă���
		phscene = GetSdk()->GetScene()->GetPHScene();		// ���̍쐬�̂��߂�PHScene�ւ̃|�C���^���Ƃ��Ă���
		
		Vec3d pos = Vec3d(0, 0, 1.21825);					// �J���������ʒu
		GetCurrentWin()->GetTrackball()->SetPosition(pos);	// �J���������ʒu�̐ݒ�
		GetSdk()->SetDebugMode(true);						// �f�o�b�N�\���̗L����
			
		// �����쐬
		CDBoxDesc bd;
		bd.boxsize = Vec3f(5.0f, 1.0f, 5.0f);
		bd.material.mu = 0.5;
		bd.material.mu0 = 0.6;
		PHSolidIf* floor = phscene->CreateSolid();
		floor->AddShape(phSdk->CreateShape(bd));
		floor->SetFramePosition(Vec3d(0, -1.0, 0.0));
		floor->SetDynamical(false);
	
		// �����쐬
		PHSolidIf* soBox = phscene->CreateSolid();
		soBox->SetMass(0.3f);
		bd.boxsize.clear(0.4f);
		soBox->AddShape(phSdk->CreateShape(bd));
		soBox->SetInertia(soBox->GetShape(0)->CalcMomentOfInertia() * soBox->GetMass());
		soBox->SetFramePosition(Vec3d(-0.5 , -0.35, 0.0));

		// �͊o�|�C���^�̍쐬
		pointer = phscene->CreateHapticPointer();	// �͊o�|�C���^�̍쐬
		CDSphereDesc cd;
		cd.radius = 0.1f;
		cd.material.mu = 0.4;
		bd.boxsize = Vec3f(0.2f, 0.2f, 0.2f);
		//pointer->AddShape(phSdk->CreateShape(bd));	// �V�F�C�v�̒ǉ�
		pointer->AddShape(phSdk->CreateShape(cd));	// �V�F�C�v�̒ǉ�
		Posed defaultPose;
		defaultPose.Pos() = Vec3d(0.0, -0.35, 0.0);	
		pointer->SetDefaultPose(defaultPose);		// �͊o�|�C���^�����p���̐ݒ�
		pointer->SetInertia(pointer->GetShape(0)->CalcMomentOfInertia());	// �����e���\���̐ݒ�
		pointer->SetLocalRange(0.1);				// �Ǐ��V�~�����[�V�����͈͂̐ݒ�
		pointer->SetPosScale(50);					// �͊o�|�C���^�̈ړ��X�P�[���̐ݒ�
		pointer->SetReflexSpring(5000);				// �o�l�W���̐ݒ�
		pointer->SetReflexDamper(0.1 * 0.0);		// �_���p�W���̐ݒ�
		pointer->EnableFriction(false);				// ���C��L���ɂ��邩�ǂ���
		//pointer->EnableDebugControl(true);		// �L�[�{�[�h����͊o�|�C���^�𓮂����@�\�J�[�\����x�Ay�����Ɉړ��\
		FWHapticPointerIf* fwPointer = GetSdk()->GetScene()->CreateHapticPointer();	// HumanInterface�Ɛڑ����邽�߂̃I�u�W�F�N�g���쐬
		fwPointer->SetHumanInterface(spg);		// HumanInterface�̐ݒ�
		fwPointer->SetPHHapticPointer(pointer); // PHHapticPointerIf��s���Ă�
}

void FWHapticSample::InitInterface(){
	HISdkIf* hiSdk = GetSdk()->GetHISdk();

	if(humanInterface == SPIDAR){
		// x86
		DRUsb20SimpleDesc usbSimpleDesc;
		hiSdk->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
		DRUsb20Sh4Desc usb20Sh4Desc;
		for(int i=0; i< 10; ++i){
			usb20Sh4Desc.channel = i;
			hiSdk->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
		}
		// x64
		DRCyUsb20Sh4Desc cyDesc;
		for(int i=0; i<10; ++i){
			cyDesc.channel = i;
			hiSdk->AddRealDevice(DRCyUsb20Sh4If::GetIfInfoStatic(), &cyDesc);
		}
		hiSdk->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());
		hiSdk->Print(DSTR);
		hiSdk->Print(std::cout);

		spg = hiSdk->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
		spg->Init(&HISpidarGDesc("SpidarG6X3R"));
		spg->Calibration();
	}else if(humanInterface == XBOX){
		spg = hiSdk->CreateHumanInterface(HIXbox360ControllerIf::GetIfInfoStatic())->Cast();
	}else if(humanInterface == FALCON){
		spg = hiSdk->CreateHumanInterface(HINovintFalconIf::GetIfInfoStatic())->Cast();
		spg->Init(NULL);
	}
}

void FWHapticSample::Init(int argc, char* argv[]){
	FWApp::Init(argc, argv);							// �A�v���P�[�V�����̏�����
	InitInterface();									// �C���^�t�F�[�X�̏�����
	BuildScene();										// �I�u�W�F�N�g�̍쐬
	PHHapticEngineIf* he = phscene->GetHapticEngine();	// �͊o�G���W�����Ƃ��Ă���
	he->EnableHapticEngine(true);						// �͊o�G���W���̗L����

	if(engineType == SINGLE){
		// �V���O���X���b�h���[�h
		he->SetHapticEngineMode(PHHapticEngineDesc::SINGLE_THREAD);
		phscene->SetTimeStep(hdt);
	}else if(engineType == MULTI){
		// �}���`�X���b�h���[�h
		he->SetHapticEngineMode(PHHapticEngineDesc::MULTI_THREAD);
		phscene->SetTimeStep(pdt);
	}else if(engineType == LD){
		// �Ǐ��V�~�����[�V�������[�h
		he->SetHapticEngineMode(PHHapticEngineDesc::LOCAL_DYNAMICS);
		phscene->SetTimeStep(pdt);
	}
		physicsTimerID = GetTimer(0)->GetID();					// �����X���b�h�̃^�C�}ID�̎擾
		GetTimer(0)->SetMode(UTTimerIf::IDLE);					// �����X���b�h�̃^�C�}��IDLE���[�h�ɐݒ�
		UTTimerIf* timer = CreateTimer(UTTimerIf::MULTIMEDIA);	// �͊o�X���b�h�p�̃}���`���f�B�A�^�C�}���쐬
		timer->SetResolution(1);			// ����\(ms)
		timer->SetInterval(unsigned int(hdt * 1000));		// ����(ms)h
		hapticTimerID = timer->GetID();		// �͊o�X���b�h�̃^�C�}ID�̎擾
		timer->Start();						// �^�C�}�X�^�[�g
}


void FWHapticSample::TimerFunc(int id){
	if(engineType == 0){
		if(hapticTimerID == id){
			GetSdk()->GetScene()->UpdateHapticPointers();
			phscene->Step();
		}else if(physicsTimerID == id){
			PostRedisplay();
		}
	}else if(engineType > 0){
		if(hapticTimerID == id){
			GetSdk()->GetScene()->UpdateHapticPointers();
			phscene->StepHapticLoop();
		}else{
			PHHapticEngineIf* he = phscene->GetHapticEngine();
			he->StepPhysicsSimulation();
			PostRedisplay();
		}
	}
}

void FWHapticSample::Keyboard(int key, int x, int y){
	// �e�X���b�h�̋��L�������̃A�N�Z�X�ᔽ����̂��߂ɑS�Ẵ^�C�}���Ƃ߂�
	float dr = 0.01;
	for(int i = 0; i < NTimers(); i++)	GetTimer(i)->Stop();
	switch(key){
		case 'q':
			// �A�v���P�[�V�����̏I��
			exit(0);
			break;
		case '1':
			{
				// �����_�����O���[�h��Penalty��
				DSTR << "Penalty mode" << std::endl;
				pointer->SetHapticRenderMode(PHHapticPointerDesc::PENALTY);
				break;
			}
		case '2':
			{
				// �����_�����O���[�h��Constraint��
				DSTR << "Constraint mode" << std::endl;
				pointer->SetHapticRenderMode(PHHapticPointerDesc::CONSTRAINT);
				break;
			}
		case 'c':
			{
				// �C���^�t�F�[�X�̃L�����u���[�V����
				spg->Calibration();
				DSTR << "CameraPosition" << std::endl;
				DSTR << GetCurrentWin()->GetTrackball()->GetPosition() << std::endl;
			}
			break;
		case 'f':
			{
				// �͊o�񎦂�ON/OFF
				if(pointer){
					static bool bForce = false;
					if(!bForce){
						bForce = true;
						pointer->EnableForce(bForce);
						DSTR << "Force: ON" << std::endl;
					}else{
						bForce = false;
						pointer->EnableForce(bForce);					
						DSTR << "Force: OFF" << std::endl;
					}
				}
			}
			break;
		case 'v':
			{
				// �U���񎦂�ON/OFF
				if(pointer){
					static bool bVibration = false;
					if(!bVibration){
						bVibration = true;
						pointer->EnableVibration(bVibration);
						DSTR << "Vibration: ON" << std::endl;
					}else{
						bVibration = false;
						pointer->EnableVibration(bVibration);					
						DSTR << "Vibration: ON" << std::endl;
					}
				}			
			}
			break;
		case 'd':
			{
				// �f�o�b�N�\����ON/OFF
				static bool bDebug = false;
				if(!bDebug){
					bDebug = true;
					GetSdk()->GetScene()->EnableRenderHaptic(bDebug);
					DSTR << "Debug Mode: ON" << std::endl;
				}else{
					bDebug = false;
					GetSdk()->GetScene()->EnableRenderHaptic(bDebug);				
					DSTR << "Debug Mode: OFF" << std::endl;
				}
			}
			break;
		case'a':
			{
				// �o�l�W����100���₷
				if(pointer){
					float spring = pointer->GetReflexSpring();
					spring += 100.0;
					pointer->SetReflexSpring(spring);
					DSTR << "Spring: " << spring << std::endl;
				}
			}
			break;
		case'z':
			{
				// �o�l�W����100���炷
				if(pointer){
					float spring = pointer->GetReflexSpring();
					spring -= 100.0;
					pointer->SetReflexSpring(spring);
					DSTR << "Spring: " << spring << std::endl;
				}
			}
			break;
		case ' ':
			{
				// �V���ɍ��̂𐶐�����
				GetSdk()->GetScene()->GetPHScene()->GetHapticEngine()->ReleaseState();
				CDBoxDesc bd;
				bd.boxsize.clear(0.4f);
				PHSolidIf* box = phscene->CreateSolid();
				box->SetMass(0.3f);
				box->AddShape(GetSdk()->GetPHSdk()->CreateShape(bd));
				box->SetInertia(box->GetShape(0)->CalcMomentOfInertia() * box->GetMass());
				box->SetFramePosition(Vec3d(-0.5, 1.0, 0.0));
			}
		case 356: // left
			{
				Vec3d pos = pointer->GetFramePosition();
				pos.x -= dr;
				pointer->SetFramePosition(pos);
			}
			break;
		case 358: // right
			{
				Vec3d pos = pointer->GetFramePosition();
				pos.x += dr;
				pointer->SetFramePosition(pos);
			}
			break;		
		case 357: // up
			{
				Vec3d pos = pointer->GetFramePosition();
				pos.y += dr;
				pointer->SetFramePosition(pos);
			}
			break;			
		case 359: // down
			{
				Vec3d pos = pointer->GetFramePosition();
				pos.y -= dr;
				pointer->SetFramePosition(pos);
			}
			break;	
		default:
			break;
	}
	// �S�Ẵ^�C�}���n��
	for(int i = 0; i < NTimers(); i++)	GetTimer(i)->Start();
}