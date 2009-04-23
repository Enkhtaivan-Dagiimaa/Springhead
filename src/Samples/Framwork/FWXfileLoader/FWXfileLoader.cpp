#include "FWXfileLoader.h"
#include <iostream>
#include <sstream>
#include <GL/glut.h>


#define ESC 27

FWAppSample::FWAppSample(){
	bDrawInfo	= true;
	bDebug		= true;
}

void FWAppSample::Init(int argc, char* argv[]){
	FWAppGLUT::Init(argc, argv);

	GetSdk()->Clear();										// SDK�̍쐬
	GetSdk()->LoadScene("./xFiles/sceneSample.x");			//Xfile����Scene�̍쐬
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->SetStateMode(true);
	//BuildObject();											// ���̂��쐬

	FWWinDesc windowDesc;
	windowDesc.title = "FWAppFWAppSample";
	CreateWin(windowDesc);
	GetCurrentWin()->scene = GetSdk()->GetScene();

	InitCameraView();
}

void FWAppSample::InitCameraView(){
	//	Affinef �^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWAppSample::BuildObject(){
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
		desc.mass = 0.05;
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

void FWAppSample::Step(){
	PHSceneIf* phscene = GetSdk()->GetScene()->GetPHScene();
	phscene->Step();
	glutPostRedisplay();
}

void FWAppSample::Display(){
	// �`�惂�[�h�̐ݒ�
	GetSdk()->SetDebugMode(bDebug);
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

void FWAppSample::Keyboard(int key, int x, int y){
	switch (key) {
		case 'i':
			bDrawInfo = !bDrawInfo;
			break;
		case 'd':
			if(bDebug==true){
				bDebug=false;
			}else{
				bDebug=true;
			}
			break;
		case ESC:
		case 'q':
			exit(0);
			break;
		default:
			break;
	}
}