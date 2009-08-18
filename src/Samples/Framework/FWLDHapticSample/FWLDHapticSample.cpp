#include "FWLDHapticSample.h"
//#include "SampleModel.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>
#include "Foundation/UTMMTimer.h"

#define ESC 27

using namespace std;

FWLDHapticSample::FWLDHapticSample(){
	bDrawInfo = false;
}

void FWLDHapticSample::Init(int argc, char* argv[]){
	SetGRAdaptee(grAdapteeType::TypeGLUT);
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.02);

	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWLDHapticSample";					// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();
	InitCameraView();										// �J�����r���[�̏�����

	InitHumanInterface();
	FWInteractSceneDesc desc;
	desc.fwScene = GetSdk()->GetScene();
	desc.mode = LOCAL_DYNAMICS;
	desc.hdt = 0.001;
	CreateInteractScene(desc);

	BuildScene();											// ���̂��쐬

	UTMMTimer* mtimer = CreateMMTimerFunc();
	mtimer->Resolution(1);
	mtimer->Interval(1);
	mtimer->Set(CallBackHapticLoop, NULL);
	mtimer->Create();
}

void FWLDHapticSample::InitCameraView(){
	//	Affinef �^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWLDHapticSample::InitHumanInterface(){
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
	UTRef<HISpidarGIf> spg = GetHISdk()->CreateHumanInterface(HISpidarGIf::GetIfInfoStatic())->Cast();
	spg->Init(&HISpidarGDesc("SpidarG6X3R"));
	AddHI(spg);
}

void FWLDHapticSample::IdleFunc(){
	CallBackPhysicsLoop();
}

void FWLDHapticSample::CallBackHapticLoop(void* arg){	
//	FWLDHapticSample::instance->GetInteractAdaptee()->CallBackHapticLoop();
	((FWLDHapticSample*)instance)->GetInteractScene()->CallBackHapticLoop();

}
void FWLDHapticSample::CallBackPhysicsLoop(){
	FWLDHapticSample::instance->GetInteractScene()->Step();
	glutPostRedisplay();
}

void FWLDHapticSample::Display(){
	// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(true);
	GRDebugRenderIf* render = GetCurrentWin()->render->Cast();
	render->SetRenderMode(true, false);
	render->EnableRenderAxis(bDrawInfo);
	render->EnableRenderForce(bDrawInfo);
	render->EnableRenderContact(bDrawInfo);

	// �J�������W�̎w��
	GRCameraIf* cam = GetCurrentWin()->scene->GetGRScene()->GetCamera();
	if (cam && cam->GetFrame()){
		cam->GetFrame()->SetTransform(cameraInfo.view);
	}else{
		GetCurrentWin()->render->SetViewMatrix(cameraInfo.view.inv());
	}

	// �`��̎��s
	if(!GetCurrentWin()) return;
	GetSdk()->SwitchScene(GetCurrentWin()->GetScene());
	GetSdk()->SwitchRender(GetCurrentWin()->GetRender());
	GetSdk()->Draw();
	glutSwapBuffers();
}

void FWLDHapticSample::Reset(){
	UTMMTimer* mtimer = GetMMTimerFunc(0);
	mtimer->Release();
	GetInteractScene()->Clear();
	GetSdk()->GetScene()->GetPHScene()->Clear();
	BuildScene();
	mtimer->Create();
}

void FWLDHapticSample::BuildScene(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	PHSolidDesc desc;
	CDBoxDesc bd;

	// ��(�����@���ɏ]��Ȃ��C�^�����ω����Ȃ�)
	{
		// ����(soFloor)�̍쐬
		desc.mass = 1e20f;
		desc.inertia *= 1e30f;
		PHSolidIf* soFloor = phscene->CreateSolid(desc);		// ���̂�desc�Ɋ�Â��č쐬
		soFloor->SetDynamical(false);
		soFloor->SetGravity(false);
		// �`��(shapeFloor)�̍쐬
		bd.boxsize = Vec3f(50, 10, 50);
		CDShapeIf* shapeFloor = GetSdk()->GetPHSdk()->CreateShape(bd);
		// ���̂Ɍ`���t������
		soFloor->AddShape(shapeFloor);
		soFloor->SetFramePosition(Vec3d(0, -10, 0));
	}

	// ��(�����@���ɏ]���C�^�����ω�)
	{
		// ����(soBox)�̍쐬
		desc.mass = 0.5;
		desc.inertia *= 0.033;
		PHSolidIf* soBox = phscene->CreateSolid(desc);
		// �`��(shapeBox)�̍쐬
		bd.boxsize = Vec3f(2,2,2);
		CDShapeIf* shapeBox = GetSdk()->GetPHSdk()->CreateShape(bd);
		// ���̂Ɍ`���t��
		soBox->AddShape(shapeBox);
		soBox->SetFramePosition(Vec3d(0, 10, 0));
	
		// �|�C���^
		soBox = phscene->CreateSolid(desc);
		soBox->AddShape(shapeBox);
		soBox->SetDynamical(false);
		GetSdk()->GetScene()->GetPHScene()->SetContactMode(soBox, PHSceneDesc::MODE_NONE);
		FWInteractPointerDesc idesc;
		idesc.pointerSolid = soBox;;
		idesc.humanInterface = GetHI(0); 
		idesc.springK = 10;
		idesc.damperD = 0.0;
		idesc.posScale = 300;
		idesc.localRange = 0.7;
		GetInteractScene()->CreateInteractPointer(idesc);
	}
}

void FWLDHapticSample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			Exit();
			break;
		case 'r':
			Reset();
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		//case '1':
		//	DSTR << "box" << std::endl;
		//	CreateBox(GetSdk());
		//	break;
		//case '2':
		//	DSTR << "sphere" << std::endl;
		//	CreateSphere(GetSdk());
		//	break;
		//case '3':
		//	DSTR << "capsule" << std::endl;
		//	CreateCapsule(GetSdk());
		//	break;
		//case '4':
		//	DSTR << "roundcone" << std::endl;
		//	CreateRoundCone(GetSdk());
		//	break;
		default:
			break;
	}
}