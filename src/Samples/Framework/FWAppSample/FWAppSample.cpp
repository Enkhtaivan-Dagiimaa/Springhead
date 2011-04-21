#include "FWAppSample.h"
#include "SampleModel.h"

#define ESC 27

FWAppSample::FWAppSample(){
	bDrawInfo = false;
}

void FWAppSample::Init(int argc, char* argv[]){
	SetGRAdaptee(TypeGLUT);									// CG��OpenGL(GLUT)�ŕ`��w��
	GRInit(argc, argv);										// GLUT�̏�����

	CreateSdk();											// Sdk�̍쐬
	GetSdk()->CreateScene();								// Scene�̍쐬
	GetSdk()->GetScene()->GetPHScene()->SetTimeStep(0.05);	// �V�~�����[�V�����̍��ݎ��Ԃ�ݒ�

	FWWinDesc windowDesc;					// GL�̃E�B���h�E�f�B�X�N���v�^
	windowDesc.title = "Springhead2";		// �E�B���h�E�̃^�C�g��
	CreateWin(windowDesc);					// �E�B���h�E�̍쐬�ƃV�[���̊��蓖��
	InitWindow();							// �E�B���h�E�̏�����(�`�悷��scene��ݒ�)
	InitCameraView();						// �J�����r���[�̏�����

	CreateObject();		// ���̂��쐬
	CreateTimer();		// �^�C�}�[�̐���
}

void FWAppSample::TimerFunc(int id){	
	GetSdk()->Step();
	PostRedisplay();
}

void FWAppSample::InitCameraView(){
	//	Affinef�^�����A�X�g���[������s���ǂݏo���@�\�𗘗p���Ď��_�s���������
	std::istringstream issView(
		"((0.9996 0.0107463 -0.0261432 -0.389004)"
		"(-6.55577e-010 0.924909 0.380188 5.65711)"
		"(0.0282657 -0.380037 0.92454 13.7569)"
		"(     0      0      0      1))"
	);
	issView >> cameraInfo.view;
}

void FWAppSample::CreateObject(){
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

void FWAppSample::Display(){
	// �e��A����\�����邩�ǂ���
	FWSceneIf* fwScene = GetSdk()->GetScene();
	fwScene->EnableRenderAxis(bDrawInfo);		// ���W���̕\��
	fwScene->EnableRenderForce(bDrawInfo);		// �S���͂̕\��
	fwScene->EnableRenderContact(bDrawInfo);	// �ڐG��Ԃ̕\��

	// �V�[���̕`��
	GetSdk()->SetDebugMode(true);
	GetSdk()->GetRender()->SetViewMatrix(cameraInfo.view.inv());
	GetSdk()->Draw();
	GetSdk()->GetRender()->SwapBuffers();
}

void FWAppSample::Reset(){
	GetSdk()->GetScene()->GetPHScene()->Clear();
	CreateObject();
}

void FWAppSample::Keyboard(int key, int x, int y){
	switch (key) {
		case ESC:
		case 'q':
			FWApp::Clear();
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
