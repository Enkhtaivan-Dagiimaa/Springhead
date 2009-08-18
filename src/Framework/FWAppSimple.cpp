#include "Framework/SprFWAppSimple.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>

using namespace std;

FWAppSimple::FWAppSimple(){
	bDrawInfo=false;
}

void FWAppSimple::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUT);
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.01);

	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "Springhead2";						// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();
	InitCameraView();										// �J�����r���[�̏�����

	BuildObject();											// ���̂��쐬

}

void FWAppSimple::Timer(){
	GTimer* timer0 = CreateTimerFunc();			// �^�C�}�[�̐���
	GetTimerFunc(0)->Set(TimerFunc);			// �Ăт����֐�
	GetTimerFunc(0)->Create(GetGRAdaptee());	// GLUT�^�Ń^�C�}�[���쐬
}
void FWAppSimple::TimerFunc(int id){
	((FWAppSimple*)instance)->GetTimerFunc(0)->Loop();
	((FWAppSimple*)instance)->CallStep();
	((FWAppSimple*)instance)->GetGRAdaptee()->PostRedisplay();
}

void FWAppSimple::CallStep(){
	if(!vfBridge || !vfBridge->Step())
		Step();
}

void FWAppSimple::InitCameraView(){
	//	Affinef �^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWAppSimple::Step(){
	GetSdk()->Step();
}

void FWAppSimple::Display(){
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

void FWAppSimple::Reset(){
	GetSdk()->GetScene()->GetPHScene()->Clear();
	BuildObject();
}
