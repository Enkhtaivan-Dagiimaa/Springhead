/*
 *  Copyright (c) 2003-2011, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/*	SPIDARG6�̏o�͂��m�F���邽�߂̃v���O����
	�}���`���f�B�A�^�C�}�̊��荞�݂��g�����ƂŁC
	��1ms�̍X�V���\�D
*/

#include <conio.h>
#include <Springhead.h>
#include <windows.h>

using namespace Spr;

// �e��ϐ�
UTRef<HISpidarGIf> spg;
float dt = 0.001f;		// �X�V����
double K = 5000;		// �o�l�W��
double D = 5;			// �_���p�W��
double fy = -0.009;		// �͊o�񎦈ʒu

// �}���`���f�B�A�^�C�}�̃R�[���o�b�N�֐�
void SPR_CDECL CallBackLoop(int id, void* arg){
	spg->Update(dt);

	double py  = spg->GetPosition().y;	//�|�C���^�̈ʒu
	double pvy = spg->GetVelocity().y;	//�|�C���^�̑��x
	double dy  = py-fy;					//���ƃ|�C���^�̈ʒu�̍�
	double dv  = pvy - 0;				//���ƃ|�C���^�̑��x��

	Vec3d f = Vec3d();
#if 1
	if(dy < 0)	f.y= -K * dy - D * dv;		//�͂̌v�Z
#else	
	if(dy< 0)	f.y = 5;
#endif
	spg->SetForce(f, Vec3d());

	//std::cout << spg->GetPosition() << std::endl;
}

void __cdecl main(){
	// �͊o�C���^�t�F�[�X�Ƃ̐ڑ��ݒ�
	UTRef<HISdkIf> hiSdk = HISdkIf::CreateSdk();
	// win32
	DRUsb20SimpleDesc usbSimpleDesc;
	hiSdk->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
	DRUsb20Sh4Desc usb20Sh4Desc;
	for(int i=0; i<10; ++i){
		usb20Sh4Desc.channel = i;
		hiSdk->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
	}
	// win64
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

	// �}���`���f�B�A�^�C�}�[�̐ݒ�
	UTTimerIf* timer = UTTimerIf::Create();				
	timer->SetMode(UTTimerIf::MULTIMEDIA);		// �^�C�}�̃��[�h�ݒ�(MULTIMEDIA or THREAD)
	timer->SetResolution(1);					// ����\(ms)
	timer->SetInterval((unsigned int)dt*1000);	// ����(ms)
	timer->SetCallback(CallBackLoop, NULL);		// �Ăт����֐�
	timer->Start();								// �^�C�}�X�^�[�g
	
	std::cout << "Start the application. " << std::endl;
	std::cout << "Press any key to exit." << std::endl;
	while(!_kbhit()){}
	timer->Stop();
	exit(0);
}
