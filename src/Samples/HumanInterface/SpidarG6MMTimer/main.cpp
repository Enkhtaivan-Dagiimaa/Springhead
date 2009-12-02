#include <conio.h>
#include <Springhead.h>
#include <HumanInterface/SprHIDRUsb.h>
#include <iomanip>
#include "Foundation/UTMMTimer.h"
using namespace Spr;

UTRef<HISpidarGIf> spg;
void CallBackHoge(void* arg){
	spg->Update(0.001f);
	Vec3f spgpos = spg->GetPosition();
	std::cout << std::setprecision(2) << spgpos << std::endl;
	Vec3f f(0.0, 0.0, 0.0);
	if(spgpos.y < -0.015){
		f[1] = 10.0;
	}
	spg->SetForce(f, Vec3f());
}

int __cdecl main(){
	UTRef<HISdkIf> sdk = HISdkIf::CreateSdk();
	DRUsb20SimpleDesc usbSimpleDesc;
	sdk->AddRealDevice(DRUsb20SimpleIf::GetIfInfoStatic(), &usbSimpleDesc);
	DRUsb20Sh4Desc usb20Sh4Desc;
	for(int i=0; i<10; ++i){
		usb20Sh4Desc.number = i;
		sdk->AddRealDevice(DRUsb20Sh4If::GetIfInfoStatic(), &usb20Sh4Desc);
	}
	sdk->AddRealDevice(DRKeyMouseWin32If::GetIfInfoStatic());

	sdk->Init();
	sdk->Print(DSTR);
	spg = sdk->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
	spg->Init(&HISpidarGDesc("SpidarG6X3R"));
	int t = 0;

	Spr::UTMMTimer timer1;				/// �}���`���f�B�A�^�C�}�̐錾
	timer1.Resolution(1);					///	 �Ăт�������\
	timer1.Interval(1);					/// �Ăт����p�x
	timer1.Set(CallBackHoge, NULL);	/// �Ăт����֐�

	timer1.Create();							/// �}���`���f�B�A�^�C�}�X�^�[�g
	
	std::cout << "�I������ɂ͉����L�[�������Ă�������" << std::endl;
	while(!_kbhit()){}
	return 0;
}
