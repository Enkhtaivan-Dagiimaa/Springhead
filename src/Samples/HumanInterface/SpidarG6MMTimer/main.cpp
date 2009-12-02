#include <conio.h>
#include <Springhead.h>
#include <HumanInterface/SprHIDRUsb.h>
#include <iomanip>
#include "Foundation/UTMMTimer.h"
using namespace Spr;

UTRef<HISpidarGIf> spg;

Vec3d ForceRenderring(){
	double K = 5000;
	double D = 0.0;

	Vec3d F = Vec3d(0.0, 0.0, 0.0);
	double fY  = -0.01; //���̈ʒu
	double pY  = spg->GetPosition().y; //�|�C���^�̈ʒu
	double pvY = spg->GetVelocity().y; //�|�C���^�̑��x
	double dy  = pY-fY;		//���ƃ|�C���^�̈ʒu�̍�
	double dv  = pvY - 0;   //���ƃ|�C���^�̑��x��
	
	if(dy<0){
		F.y= -K*dy - D*dv;		//�͂̌v�Z
	}
	/*
	-0.05<F.y<0.05�͈̔͂ŕω�



	*/
	//if(dy<0)F.y = 5;
	DSTR<<"F:"<<F<<"dy:"<<dy<<"dv"<<dv<<std::endl;
	return F;
}

void CallBackLoop(void* arg){
	spg->Update(0.001f);
	Vec3f spgpos = spg->GetPosition();
	//std::cout << std::setprecision(2) << spgpos << std::endl;
	Vec3f f(0.0, 0.0, 0.0);
	f = ForceRenderring();
	
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
	spg->Calibration();

	Spr::UTMMTimer timer1;				/// �}���`���f�B�A�^�C�}�̐錾
	timer1.Resolution(1);				///	 �Ăт�������\
	timer1.Interval(1);					/// �Ăт����p�x
	timer1.Set(CallBackLoop, NULL);		/// �Ăт����֐�
	timer1.Create();					/// �}���`���f�B�A�^�C�}�X�^�[�g

	while(!_kbhit()){}
	return 0;
}
