#include "FWGLUISample.h"
#include "SampleModel.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>
#include <GL/glui.h>

#define ESC 27

FWGLUISample::FWGLUISample(){
	bDrawInfo = false;
}

void FWGLUISample::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUI);
	GetGRAdaptee()->Init(argc, argv);						// Sdk�̍쐬
	CreateSdk();
	GetSdk()->Clear();										// SDK�̏�����
	GetSdk()->CreateScene(PHSceneDesc(), GRSceneDesc());	// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.05);

	FWWinDesc windowDesc;									// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "FWAppSample";						// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);									// �E�B���h�E�̍쐬
	InitWindow();
	
	FWGLUIDesc uiDesc;
	{
		uiDesc.fromLeft = 510;	uiDesc.fromTop	=  30;
	}
	// UI�����ꍇ�̏����iif(glui)�Ɠ��`�j
	if(glui = ((FWGLUI*)GetGRAdaptee())->CreateGUI(GetWin(0)->GetID(), uiDesc)){
		DesignGUI();
	}

	InitCameraView();										// �J�����r���[�̏�����
	BuildObject();											// ���̂��쐬
	CreateTimer(FWTimer::GLUT)->SetInterval(10);							// �^�C�}���쐬�C����10ms
}

void FWGLUISample::InitCameraView(){
	//	Affinef �^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWGLUISample::BuildObject(){
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
		soFloor->SetFramePosition(Vec3d(0, -5, 0));
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
	}
}

void FWGLUISample::TimerFunc(int id){
	GetSdk()->Step();
	GetGRAdaptee()->PostRedisplay();
}

void FWGLUISample::Display(){
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
	std::cout << NWin() << std::endl;
	GetSdk()->Draw();
	glutSwapBuffers();
	glutPostRedisplay();
}

void FWGLUISample::Reset(){
	GetSdk()->GetScene()->GetPHScene()->Clear();
	BuildObject();
}

void FWGLUISample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			exit(0);
			break;
		case 'r':
			Reset();
			break;
		case 'd':
			bDrawInfo = !bDrawInfo;
			break;
		case '1':
			DSTR << "box" << std::endl;
			CreateBox(GetSdk());
			break;
		case '2':
			DSTR << "sphere" << std::endl;
			CreateSphere(GetSdk());
			break;
		case '3':
			DSTR << "capsule" << std::endl;
			CreateCapsule(GetSdk());
			break;
		case '4':
			DSTR << "roundcone" << std::endl;
			CreateRoundCone(GetSdk());
			break;
		default:
			break;
	}
}


void FWGLUISample::DesignGUI(){
	panel = glui->add_panel("Sample", true);
	button1 = glui->add_button_to_panel(panel, "Create Box", 1, GLUI_CB(CallButton1));
}

void FWGLUISample::CallButton1(int control){
	((FWGLUISample*)FWApp::instance)->Button1(control);
}

void FWGLUISample::Button1(int control){
	Keyboard('1', 0, 0);
}

